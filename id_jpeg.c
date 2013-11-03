/**
 *  @file
 *  @brief JPEG �N���X
 */

#include "id_std.h"
#include "id_jpeg.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#ifdef ENABLE_JPEG
#include <jpeglib.h>

static void id_jpeg_ShowImageInfo(id_image_t *imagep, loglevel_e loglevel);
static bool id_jpeg_ReadImage(id_image_t *imagep);
#endif	/* ENABLE_JPEG */

bool
id_jpeg_Init(id_image_t *imagep, loglevel_e loglevel)
{
#ifdef ENABLE_JPEG
    struct jpeg_decompress_struct cinfo;
    struct jpeg_error_mgr jerr;
    FILE *ifp;
    JSAMPARRAY img = NULL;
	u32 i, lineSize8, imageSize8;
	bool retval = TRUE;
	u8 *raw8p;

    /* JPEG �I�u�W�F�N�g�̊m�ۂƏ����� */
    cinfo.err = jpeg_std_error(&jerr);
    jpeg_create_decompress(&cinfo);

    /* �t�@�C�����J�� */
    ifp = fopen(imagep->filename, "r");
    if(ifp == NULL) {
		fprintf(stderr, "Error! Cannot open jpeg file(%s).\n", imagep->filename);
	    retval = FALSE;
		goto END;
    }
    jpeg_stdio_src(&cinfo, ifp);

    /* �w�b�_�̓ǂݍ��� */
    jpeg_read_header(&cinfo, TRUE);

    /* �W�J�̊J�n */
    jpeg_start_decompress(&cinfo);

    imagep->width = cinfo.output_width;
    imagep->height = cinfo.output_height;
	imagep->bpp = (cinfo.desired_number_of_colors == 256) ? 8 : 0;
    imagep->channel = cinfo.out_color_components;

	switch(cinfo.out_color_space) {
	case JCS_RGB:
		imagep->pmetric = PM_RGB;
		break;
	case JCS_CMYK:
		imagep->pmetric = PM_CMYK;
		break;
	case JCS_GRAYSCALE:
		imagep->pmetric = PM_GRAY;
		break;
	case JCS_YCbCr:
	case JCS_YCCK:
	case JCS_UNKNOWN:
	default:
		fprintf(stderr, "Unknown Photometric(%d)!!\n", cinfo.out_color_space);
		imagep->pmetric = PM_UNKNOWN;
		break;
	}

	imagep->read_image = id_jpeg_ReadImage;

	id_jpeg_ShowImageInfo(imagep, loglevel);

    /*
	 * id_jpeg_ReadImage() �ł��A�����悤�� libjpeg �֐��̎��s���K�v��
	 * �̂ŁA�C���[�K���ł͂���܂��������ŃC���[�W�̓ǂݍ��ݏ������s��
	 * ���Ƃɂ��܂��B�ł��A�Ăь��̊֐��͈ӎ�����K�v�͂Ȃ����߁A����
	 * ���Ǝv���܂��B:-)
	 */

	lineSize8 = CALC_IMAGE_WIDTH8(imagep->width, imagep->bpp, imagep->channel);
	imageSize8 = CALC_IMAGE_SIZE8v2(lineSize8, imagep->height);

	imagep->rawp = malloc(imageSize8);
	raw8p = imagep->rawp;
	if(imagep->rawp == NULL) {
		fprintf(stderr, "memory alloc error(line=%d, file=%s).\n", __LINE__, __FILE__);
		retval = FALSE;
		goto ENDPROC2;
	}

    /* �C���[�W��ێ����郁�����̈�̊m�ۂƏ����� */
    img = malloc(sizeof(JSAMPROW) * imagep->height);
	if(img == NULL) {
		fprintf(stderr, "memory alloc error(line=%d, file=%s).\n", __LINE__, __FILE__);
		retval = FALSE;
		goto ENDPROC1;
	}
	memset(img, 0, (sizeof(JSAMPROW) * imagep->height));
    for(i = 0; i < imagep->height; i++) {
		img[i] = calloc(sizeof(JSAMPLE), lineSize8);
		if(img[i] == NULL) {
			fprintf(stderr, "memory alloc error(line=%d, file=%s).\n", __LINE__, __FILE__);
			retval = FALSE;
			goto ENDPROC1;
		}
    }

    /* �S�C���[�W�f�[�^���擾 */
    while(cinfo.output_scanline < cinfo.output_height) {
		jpeg_read_scanlines(&cinfo,
							(img + cinfo.output_scanline),
							(cinfo.output_height - cinfo.output_scanline));
	}

    /* �S�C���[�W�f�[�^�� RAW �C���[�W�̈�ɃR�s�[ */
    for(i = 0; i < imagep->height; i++) {
		memcpy(raw8p, img[i], lineSize8);
		raw8p += lineSize8;
    }

ENDPROC1:
    /* �W�J�̏I�� */
    jpeg_finish_decompress(&cinfo);

ENDPROC2:
    /* JPEG �I�u�W�F�N�g�̔j�� */
    jpeg_destroy_decompress(&cinfo);

    /* �t�@�C������� */
    fclose(ifp);

    /* �C���[�W�f�[�^��ێ����郁�����̈���J�� */
	if(img != NULL) {
		for(i = 0; i < imagep->height; i++) {
			if(img[i] != NULL) {
				free(img[i]);
			}
		}
		free(img);
	}

	if(imagep->bpp != 8) {
		/* JPEG �� 8bpp �ȊO�Ȃ��͂��B */
		fprintf(stderr, "Jpeg supports only 8bpp. However a current bpp is %ld.\n",
				imagep->bpp);
		exit(-1);
	}

END:
	return (imagep->pmetric != PM_UNKNOWN) ? retval : FALSE;
#else  /* !ENABLE_JPEG */

	NOWARNING2(imagep, loglevel);

	fprintf(stderr, "Windows deha JPEG ha mizissou desu!!\n");
	exit(-1);
#endif	/* ENABLE_JPEG */
}

#define JPEG_INFO_FORMAT "\
** %s **\n\
  Image Width: %lu Image Length: %lu\n\
  Bits/Sample: %lu\n\
  Photometric Interpretation: %s\n\
  Samples/Pixel: %lu\n\
\n\
"
#ifdef ENABLE_JPEG
static void
id_jpeg_ShowImageInfo(id_image_t *imagep, loglevel_e loglevel)
{
	if(loglevel != ll_VERBOSE) {
		return;
	}

	fprintf(stdout,
			JPEG_INFO_FORMAT,
			imagep->filename,
			imagep->width,
			imagep->height,
			imagep->bpp,
			((imagep->pmetric == PM_RGB)  ? "RGB color"  :
			 ((imagep->pmetric == PM_CMYK) ? "CMYK color" : "Gray")),
			imagep->channel);

	return;
}

static bool
id_jpeg_ReadImage(id_image_t *imagep)
{
	/* JPEG �̏ꍇ�́Aid_jpeg_Init() �œǂݍ��ݍς݁B */

	NOWARNING(imagep);

	return TRUE;
}
#endif	/* ENABLE_JPEG */

/* ====================================================================
| Local Variables:
| tab-width: 4
| End:
| =====================================================================
*/
