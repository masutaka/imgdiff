/**
 *  @file
 *  @brief DIFF �摜�N���X
 */

#include "id_std.h"
#include "id_diff_image.h"

#include <stdio.h>
#include <stdlib.h>
#include <tiffio.h>

#ifndef _MSC_VER
/* VC ���ƂȂ��� stdlib.h �ɒ�`����Ă���Bid_std.h ��               */
/* stdlib.h �� include �������Ȃ��̂ŁA�d���Ȃ������ɒu�����Ƃɂ���B */
#define min(x,y)  ((x) < (y) ? (x) : (y))
#define max(x,y)  ((x) > (y) ? (x) : (y))
#endif	/* !_MSC_VER */

/* diff �摜�p�\���� */
typedef struct
{
    u32 width;
    u32 height;
    u32 bpp;
    u32 channel;
    char *filename;
	void *topp;					/* RAW �摜�̐擪�A�h���X */
	u8 *cur8p;					/* RAW �摜�̌��݂̃A�h���X */
	u8 value_for_match;			/* ��v�̏ꍇ�̃s�N�Z���l */
	u8 value_for_not_match;		/* �s��v�̏ꍇ�̃s�N�Z���l */
} id_diff_image_t;

void *
id_diff_DiffImageOpen(id_info_t *infop)
{
	static id_diff_image_t id_diff_image;
	id_diff_image_t *dimagep = &id_diff_image;
	i32 imageSize8;
	u32 o_abs_x, o_abs_y;

	/* diff �摜���w�肳��Ă��Ȃ���΁A��������K�v�Ȃ��B */
	if(infop->diff_filename == NULL) {
		return NULL;
	}

	/* ���ɑ��݂��� diff �摜�͍폜 */
	remove(infop->diff_filename);

	/* �I�t�Z�b�g�̐�Βl���v�Z */
	o_abs_x = labs(infop->offset.x);
	o_abs_y = labs(infop->offset.y);

	/* ��r�͈͂����݂��Ȃ���΁A��������K�v�Ȃ��B */
	if((infop->ref.width <= o_abs_x) ||
	   (infop->tgt.width <= o_abs_y)) {
		return NULL;
	}

	dimagep->width = min((infop->ref.width - o_abs_x), (infop->tgt.width - o_abs_x));
	dimagep->height = min((infop->ref.height - o_abs_y), (infop->tgt.height - o_abs_y));
	dimagep->bpp = 8;
	dimagep->channel = 1;
	dimagep->filename = infop->diff_filename;
	imageSize8 = CALC_IMAGE_SIZE8(dimagep->width, dimagep->height, dimagep->bpp, dimagep->channel);

	dimagep->topp = malloc(imageSize8);
	if(dimagep->topp == NULL) {
		fprintf(stderr, "memory alloc error(line=%d, file=%s).\n", __LINE__, __FILE__);
		return NULL;
	}
	dimagep->cur8p = dimagep->topp;


	if(infop->inverse) {
		/* ��v�s�N�Z���͍��A�s��v�s�N�Z���͔��B */
		dimagep->value_for_match = 0x00;
		dimagep->value_for_not_match = 0xff;
	}
	else {
		/* ��v�s�N�Z���͔��A�s��v�s�N�Z���͍��B */
		dimagep->value_for_match = 0xff;
		dimagep->value_for_not_match = 0x00;
	}

	return dimagep;
}

void
id_diff_DiffImageWrite1Pixel(void *dp, bool match)
{
	id_diff_image_t *dimagep = dp;

	if(dimagep == NULL) {
		return;
	}

	*dimagep->cur8p++ = match ? dimagep->value_for_match :
						dimagep->value_for_not_match;

	return;
}

void
id_diff_DiffImageClose(void *dp, u32 diff_num)
{
	id_diff_image_t *dimagep = dp;
	TIFF *tiffp;
	u32 width8, line;

	if(dimagep == NULL) {
		goto END;
	}

	/* diff ���Ȃ������� tiff �摜�͍��Ȃ��B */
	if(diff_num == 0) {
		goto ENDPROC;
	}

    tiffp = TIFFOpen(dimagep->filename, "w");
    if(tiffp == NULL) {
		fprintf(stderr, "tiff open error(%s).\n", dimagep->filename);
    }

	TIFFSetField(tiffp, TIFFTAG_IMAGEWIDTH, dimagep->width);
	TIFFSetField(tiffp, TIFFTAG_IMAGELENGTH, dimagep->height);
	TIFFSetField(tiffp, TIFFTAG_BITSPERSAMPLE, dimagep->bpp);
	TIFFSetField(tiffp, TIFFTAG_SAMPLESPERPIXEL, dimagep->channel);
	TIFFSetField(tiffp, TIFFTAG_PHOTOMETRIC, PHOTOMETRIC_MINISWHITE);
	TIFFSetField(tiffp, TIFFTAG_RESOLUTIONUNIT, RESUNIT_INCH);
	TIFFSetField(tiffp, TIFFTAG_XRESOLUTION, 600.0);
	TIFFSetField(tiffp, TIFFTAG_YRESOLUTION, 600.0);
	TIFFSetField(tiffp, TIFFTAG_PLANARCONFIG, PLANARCONFIG_CONTIG);
	TIFFSetField(tiffp, TIFFTAG_COMPRESSION, COMPRESSION_LZW);
	TIFFSetField(tiffp, TIFFTAG_ORIENTATION, ORIENTATION_TOPLEFT);
	TIFFSetField(tiffp, TIFFTAG_ROWSPERSTRIP, dimagep->height);

	dimagep->cur8p = dimagep->topp;
	width8 = CALC_IMAGE_WIDTH8(dimagep->width, dimagep->bpp, dimagep->channel);

	for(line = 0; line < dimagep->height; line++) {
		TIFFWriteScanline(tiffp, dimagep->cur8p, line, 0);
		dimagep->cur8p += width8;
	}

	TIFFClose(tiffp);

ENDPROC:
	free(dimagep->topp);

END:
	return;
}

/* ====================================================================
| Local Variables:
| tab-width: 4
| End:
| =====================================================================
*/
