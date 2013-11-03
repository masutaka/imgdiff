/**
 *  @file
 *  @brief TIFF クラス
 */

#include "id_std.h"
#include "id_tiff.h"

#include <stdio.h>
#include <stdlib.h>
#include <tiffio.h>

static void id_tiff_ShowImageInfo(id_image_t *imagep, loglevel_e loglevel);
static bool id_tiff_ReadImage(id_image_t *imagep);

/* ※ "man TIFFGetField" にサイズの指定あり。 */
bool
id_tiff_Init(id_image_t *imagep, loglevel_e loglevel)
{
    TIFF *tiffp;
    u32 width;				/* image width   (※) */
    u32 height;				/* image height  (※) */
    u16 bpp;				/* bit per pixel (※) */
    u16 channel;			/* channel num   (※) */
	u16 pmetric;			/* photometric   (※) */

    tiffp = TIFFOpen(imagep->filename, "r");
    if(tiffp == NULL) {
		fprintf(stderr, "Error! Cannor open tiff file(%s).\n", imagep->filename);
	    return FALSE;
    }

    TIFFGetField(tiffp, TIFFTAG_IMAGEWIDTH, &width);
    TIFFGetField(tiffp, TIFFTAG_IMAGELENGTH, &height);
    TIFFGetField(tiffp, TIFFTAG_BITSPERSAMPLE, &bpp);
    TIFFGetField(tiffp, TIFFTAG_SAMPLESPERPIXEL, &channel);
    TIFFGetField(tiffp, TIFFTAG_PHOTOMETRIC, &pmetric);

    TIFFClose(tiffp);

	imagep->width = width;
	imagep->height = height;
	imagep->bpp = bpp;
	imagep->channel = channel;

	switch(pmetric) {
	case PHOTOMETRIC_RGB:
		imagep->pmetric = (channel == 3) ? PM_RGB : PM_RGBA;
		break;
	case PHOTOMETRIC_SEPARATED:
		imagep->pmetric = (channel == 4) ? PM_CMYK : PM_CMYKA;
		break;
	case PHOTOMETRIC_MINISWHITE:
	case PHOTOMETRIC_MINISBLACK:
		imagep->pmetric = PM_GRAY;
		break;
	default:
		fprintf(stderr, "Unknown Photometric(%d)!!\n", pmetric);
		imagep->pmetric = PM_UNKNOWN;
		break;
	}

	imagep->read_image = id_tiff_ReadImage;

	id_tiff_ShowImageInfo(imagep, loglevel);

#ifdef MIZISSOU
	if(imagep->bpp != 8) {
		fprintf(stderr, "%ldbpp ha mizissou desu!!\n",
				imagep->bpp);
		exit(-1);
	}
#endif	/* MIZISSOU */

	return (imagep->pmetric != PM_UNKNOWN) ? TRUE : FALSE;
}

#define TIFF_INFO_FORMAT "\
** %s **\n\
  Image Width: %lu Image Length: %lu\n\
  Bits/Sample: %lu\n\
  Photometric Interpretation: %s\n\
  Samples/Pixel: %lu\n\
\n\
"

static void
id_tiff_ShowImageInfo(id_image_t *imagep, loglevel_e loglevel)
{
	if(loglevel != ll_VERBOSE) {
		return;
	}

	fprintf(stdout,
			TIFF_INFO_FORMAT,
			imagep->filename,
			imagep->width,
			imagep->height,
			imagep->bpp,
			((imagep->pmetric == PM_RGB)   ? "RGB color"   :
			 ((imagep->pmetric == PM_RGBA)  ? "RGBA color"  :
			  ((imagep->pmetric == PM_CMYK)  ? "CMYK color"  :
			   ((imagep->pmetric == PM_CMYKA) ? "CMYKA color" :
				((imagep->pmetric == PM_GRAY)  ? "GRAY"        : "UNKNOWN"))))),
			imagep->channel);

	return;
}

static bool
id_tiff_ReadImage(id_image_t *imagep)
{
    TIFF *tiffp;
	u32 lineSize8, imageSize8, line;
	bool retval = TRUE;
	u8 *raw8p;

	lineSize8 = CALC_IMAGE_WIDTH8(imagep->width, imagep->bpp, imagep->channel);
	imageSize8 = CALC_IMAGE_SIZE8v2(lineSize8, imagep->height);

	imagep->rawp = malloc(imageSize8);
	raw8p = imagep->rawp;
	if(imagep->rawp == NULL) {
		fprintf(stderr, "memory alloc error(line=%d, file=%s).\n", __LINE__, __FILE__);
		retval = FALSE;
		goto END;
	}

    tiffp = TIFFOpen(imagep->filename, "r");
    if(tiffp == NULL) {
		fprintf(stderr, "tiff open error(%s).\n", imagep->filename);
		retval = FALSE;
		goto END;
    }

    for(line = 0; line < imagep->height; line++) {
		if(TIFFReadScanline(tiffp, raw8p, line, 0) == 0) {
			fprintf(stderr, "TIFFReadEncodeStrip error line[%ld]\n", line);
			retval = FALSE;
			goto ENDPROC;
		}
		raw8p += lineSize8;
    }

ENDPROC:
    TIFFClose(tiffp);

END:
	return retval;
}

/* ====================================================================
| Local Variables:
| tab-width: 4
| End:
| =====================================================================
*/
