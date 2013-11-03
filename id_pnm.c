/**
 *  @file
 *  @brief PBM/PGM/PPM クラス
 */

#include "id_std.h"
#include "id_pnm.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef enum
{
    PNM_WIDTH,
    PNM_HEIGHT,
    PNM_BPP,
    PNM_CHANNEL,
    PNM_IMAGE_TYPE
} pnm_field_e;

typedef enum
{
	KIND_PBM,
	KIND_PGM,
	KIND_PPM,
	KIND_UNSUPPORT
}id_pnm_kind_e;

static void id_pnm_ShowImageInfo(id_image_t *imagep, loglevel_e loglevel);
static bool id_pnm_ParseHeader(char *fname, u32 *wdp, u32 *htp, u32 *bppp, u32 *channelp);
static id_pnm_kind_e id_pnm_GetPnmKind(char *str);
static u32 id_pnm_GetChannel(id_pnm_kind_e kind);
static void id_pnm_GetWidthAndHeight(char *str, u32 *wdp, u32 *htp);
static u32 id_pnm_GetBpp(char *str);
static bool id_pnm_ReadImage(id_image_t *imagep);
static bool id_pnm_ReadImage_124(id_image_t *imagep);
static void id_pnm_1bpp1ch_to_8bpp1ch(void *inp, u8 *out8p, u32 out8p_size8);
static void id_pnm_2bpp1ch_to_8bpp1ch(void *inp, u8 *out8p, u32 out8p_size8);
static void id_pnm_4bpp1ch_to_8bpp1ch(void *inp, u8 *out8p, u32 out8p_size8);
static void id_pnm_SkipHeader(FILE *fp);

bool
id_pnm_Init(id_image_t *imagep, loglevel_e loglevel)
{
    if(!id_pnm_ParseHeader(imagep->filename, &imagep->width,
						   &imagep->height, &imagep->bpp, &imagep->channel)) {
		return FALSE;
	}

	imagep->pmetric = (imagep->channel == 3) ? PM_RGB : PM_GRAY;
	imagep->read_image = id_pnm_ReadImage;

	id_pnm_ShowImageInfo(imagep, loglevel);

#ifdef MIZISSOU
	switch(imagep->bpp) {
	case 1:
	case 2:
	case 4:
		/* サポートフォーマットは id_readme.h をご覧下さい。 */
		if(imagep->channel != 1) {
			fprintf(stderr, "%ldbpp %ldchannel ha mizissou desu!!\n",
					imagep->bpp, imagep->channel);
			exit(-1);
		}
		break;
	}
#endif	/* MIZISSOU */

	return TRUE;
}

#define PNM_INFO_FORMAT "\
** %s **\n\
  Image Width: %lu Image Length: %lu\n\
  Bits/Sample: %lu\n\
  Photometric Interpretation: %s\n\
  Samples/Pixel: %lu\n\
\n\
"
static void
id_pnm_ShowImageInfo(id_image_t *imagep, loglevel_e loglevel)
{
	if(loglevel != ll_VERBOSE) {
		return;
	}

	fprintf(stdout,
			PNM_INFO_FORMAT,
			imagep->filename,
			imagep->width,
			imagep->height,
			imagep->bpp,
			(imagep->pmetric == PM_RGB) ? "RGB color" : "Gray",
			imagep->channel);

	return;
}

static bool
id_pnm_ParseHeader(char *fname, u32 *wdp, u32 *htp, u32 *bppp, u32 *channelp)
{
	char line[CHAR_NUM_MAX_AT_1LINE];
	u32 header_cnt;
	bool loop_flag;
	FILE *fp;
	bool retval = TRUE;
	id_pnm_kind_e kind = KIND_UNSUPPORT;
	u32 wd = 0, ht = 0, bpp = 0, channel = 0;

    fp = fopen(fname, "rb");
    if(fp == NULL) {
		fprintf(stderr, "Error! Cannot open pnm file(%s).\n", fname);
	    retval = FALSE;
		goto END;
    }

	header_cnt = 0;
	loop_flag = TRUE;

	while(loop_flag &&
		  fgets(line, CHAR_NUM_MAX_AT_1LINE, fp) != NULL) {

		/* コメントは読み飛ばし */
		if(strncmp(line, "#", 1) == 0) {
			continue;
		}

		switch(header_cnt++) {
		case 0:
			kind = id_pnm_GetPnmKind(line);
			if(kind == KIND_UNSUPPORT) {
				retval = FALSE;
				goto ENDPROC;
			}
			channel = id_pnm_GetChannel(kind);
			break;
		case 1:
			id_pnm_GetWidthAndHeight(line, &wd, &ht);

			if(kind == KIND_PBM) {
				/* pbm のヘッダは 2 行 */
				bpp = 1;
				loop_flag = FALSE;
			}
			break;
		case 2:
			bpp = id_pnm_GetBpp(line);
			loop_flag = FALSE;
			break;
		}
	}

ENDPROC:
	*wdp = wd;
	*htp = ht;
	*bppp = bpp;
	*channelp = channel;
	fclose(fp);

END:
	return retval;
}

static id_pnm_kind_e
id_pnm_GetPnmKind(char *str)
{
	id_pnm_kind_e kind;

	if(strncmp(str, "P4", 2) == 0) {
		kind = KIND_PBM;
	}
	else if(strncmp(str, "P5", 2) == 0) {
		kind = KIND_PGM;
	}
	else if(strncmp(str, "P6", 2) == 0) {
		kind = KIND_PPM;
	}
	else {
		fprintf(stderr, "Pnm Bad magic number '%s' arenot supported.\n", str);
		kind = KIND_UNSUPPORT;
	}

	return kind;
}

static u32
id_pnm_GetChannel(id_pnm_kind_e kind)
{
	u32 channel;

	switch(kind) {
	case KIND_PBM:
	case KIND_PGM:
		channel = 1;
		break;
	case KIND_PPM:
		channel = 3;
		break;
	default:
		channel = 0;
		fprintf(stderr, "kind(%d) isnot supported.\n", kind);
		break;
	}

	return channel;
}

static void
id_pnm_GetWidthAndHeight(char *str, u32 *wdp, u32 *htp)
{
	char *nump;

	*wdp = atoi(str);

	/* スペースの次の文字まで読み進める。 */
	nump = strrchr(str, ' ');
	nump++;

	*htp = atoi(nump);

	return;
}

static u32
id_pnm_GetBpp(char *str)
{
	u32 bpp;

	switch(atoi(str)) {
	case 1:
		bpp = 1;
		break;
	case 3:
		bpp = 2;
		break;
	case 15:
		bpp = 4;
		break;
	case 255:
		bpp = 8;
		break;
	default:
		bpp = 0;
		fprintf(stderr, "bpp(%s) isnot supported.\n", str);
		break;
	}

	return bpp;
}

static bool
id_pnm_ReadImage(id_image_t *imagep)
{
    FILE *fp;
	u32 lineSize8, imageSize8;
	bool retval = TRUE;
	u8 *raw8p;

	/*
	 * 1,2,4bpp 用の画像比較関数を作る時間がなかったので、1,2,4bpp を
	 * 8pp に拡張しながら ReadImage する関数を作った。やっつけ仕事なの
	 * は言うまでもない。
	 */
	if(imagep->bpp != 8) {
		return id_pnm_ReadImage_124(imagep);
	}

	lineSize8 = CALC_IMAGE_WIDTH8(imagep->width, imagep->bpp, imagep->channel);
	imageSize8 = CALC_IMAGE_SIZE8v2(lineSize8, imagep->height);

	imagep->rawp = malloc(imageSize8);
	raw8p = imagep->rawp;
	if(imagep->rawp == NULL) {
		fprintf(stderr, "memory alloc error(line=%d, file=%s).\n", __LINE__, __FILE__);
		retval = FALSE;
		goto END;
	}

    fp = fopen(imagep->filename, "rb");
    if(fp == NULL) {
		fprintf(stderr, "file open error(%s).\n", imagep->filename);
		retval = FALSE;
		goto END;
    }

	id_pnm_SkipHeader(fp);

	if(fread(raw8p, 1, imageSize8, fp) != imageSize8) {
		fprintf(stderr, "file read error(%s).\n", imagep->filename);
		retval = FALSE;
		goto ENDPROC;
	}

ENDPROC:
    fclose(fp);

END:
	return retval;
}

typedef void (id_pnm_Xbpp_to_8bpp_f)(void *inp, u8 *out8p, u32 out8p_size8);

/*
 * 1,2,4bpp に真面目に対応するのは面倒だったので、
 * 8bpp に拡張し以降の関数を騙す関数
 */
static bool
id_pnm_ReadImage_124(id_image_t *imagep)
{
    FILE *fp;
	u32 lineSize8, imageSize8;
	u32 orig_bpp, orig_lineSize8, orig_imageSize8, i;
	bool retval = TRUE;
	void *orig_rawp;
	u8 *orig_rawcur8p, *rawcur8p;
	id_pnm_Xbpp_to_8bpp_f *id_pnm_Xbpp_to_8bpp = NULL;

	orig_bpp = imagep->bpp;
	imagep->bpp = 8;

	orig_lineSize8 = CALC_IMAGE_WIDTH8(imagep->width, orig_bpp, imagep->channel);
	orig_imageSize8 = CALC_IMAGE_SIZE8v2(orig_lineSize8, imagep->height);

	lineSize8 = CALC_IMAGE_WIDTH8(imagep->width, imagep->bpp, imagep->channel);
	imageSize8 = CALC_IMAGE_SIZE8v2(lineSize8, imagep->height);

	orig_rawp = malloc(orig_imageSize8);
	orig_rawcur8p = orig_rawp;
	if(orig_rawp == NULL) {
		fprintf(stderr, "memory alloc error(line=%d, file=%s).\n", __LINE__, __FILE__);
		retval = FALSE;
		goto END;
	}

	imagep->rawp = malloc(imageSize8);
	rawcur8p = imagep->rawp;
	if(imagep->rawp == NULL) {
		fprintf(stderr, "memory alloc error(line=%d, file=%s).\n", __LINE__, __FILE__);
		retval = FALSE;
		goto ENDPROC2;
	}

    fp = fopen(imagep->filename, "rb");
    if(fp == NULL) {
		fprintf(stderr, "file open error(%s).\n", imagep->filename);
		retval = FALSE;
		goto ENDPROC2;
    }

	id_pnm_SkipHeader(fp);

	if(fread(orig_rawp, 1, orig_imageSize8, fp) != orig_imageSize8) {
		fprintf(stderr, "file read error(%s).\n", imagep->filename);
		retval = FALSE;
		goto ENDPROC1;
	}

	switch(orig_bpp) {
	case 1:
		id_pnm_Xbpp_to_8bpp = id_pnm_1bpp1ch_to_8bpp1ch;
		break;
	case 2:
		id_pnm_Xbpp_to_8bpp = id_pnm_2bpp1ch_to_8bpp1ch;
		break;
	case 4:
		id_pnm_Xbpp_to_8bpp = id_pnm_4bpp1ch_to_8bpp1ch;
		break;
	}

	/* 1,2,4bpp->8bpp 拡張 */
	for(i = 0; i < imagep->height; i++) {
		id_pnm_Xbpp_to_8bpp(orig_rawcur8p, rawcur8p, lineSize8);
		orig_rawcur8p += orig_lineSize8;
		rawcur8p += lineSize8;
	}

ENDPROC1:
    fclose(fp);

ENDPROC2:
	if(orig_rawp != NULL) {
		free(orig_rawp);
	}

END:
	return retval;
}

/*
 * inp(1bpp 1channel 画像)を 8bpp に拡張して、out8p にコピーする。
 */
static void
id_pnm_1bpp1ch_to_8bpp1ch(void *inp, u8 *out8p, u32 out8p_size8)
{
	u32 *in32p = inp;
	u32 inpixel = 0;
	u32 i, mask = 0x80000000, shift = 31;
	const u8 array_1to8[] = {0x00, 0xff};

	for(i = 0; i < out8p_size8; i++) {
		if((i % 32) == 0) {
		    inpixel = memConvByte(*in32p);
		    in32p++;
		}
		out8p[i] = array_1to8[(inpixel & mask) >> shift];

		if(mask == 0x00000001) {
			mask = 0x80000000;
			shift = 31;
		}
		else {
			mask >>= 1;
			shift -= 1;
		}
	}

	return;
}

/*
 * inp(2bpp 1channel 画像)を 8bpp に拡張して、out8p にコピーする。
 */
static void
id_pnm_2bpp1ch_to_8bpp1ch(void *inp, u8 *out8p, u32 out8p_size8)
{
	u32 *in32p = inp;
	u32 inpixel = 0;
	u32 i, mask = 0xc0000000, shift = 30;
	const u8 array_2to8[] = {0x00, 0x55, 0xaa, 0xff};

	for(i = 0; i < out8p_size8; i++) {
		if((i % 16) == 0) {
		    inpixel = memConvByte(*in32p);
		    in32p++;
		}
		out8p[i] = array_2to8[(inpixel & mask) >> shift];

		if(mask == 0x00000003) {
			mask = 0xc0000000;
			shift = 30;
		}
		else {
			mask >>= 2;
			shift -= 2;
		}
	}

	return;
}

/*
 * inp(2bpp 1channel 画像)を 8bpp に拡張して、out8p にコピーする。
 */
static void
id_pnm_4bpp1ch_to_8bpp1ch(void *inp, u8 *out8p, u32 out8p_size8)
{
	u32 *in32p = inp;
	u32 inpixel = 0;
	u32 i, mask = 0xf0000000, shift = 28;
	const u8 array_4to8[] = {0x00, 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77,
							 0x88, 0x99, 0xaa, 0xbb, 0xcc, 0xdd, 0xee, 0xff};

	for(i = 0; i < out8p_size8; i++) {
		if((i % 8) == 0) {
		    inpixel = memConvByte(*in32p);
		    in32p++;
		}
		out8p[i] = array_4to8[(inpixel & mask) >> shift];

		if(mask == 0x0000000f) {
			mask = 0xf0000000;
			shift = 28;
		}
		else {
			mask >>= 4;
			shift -= 4;
		}
	}

	return;
}

static void
id_pnm_SkipHeader(FILE *fp)
{
	char line[CHAR_NUM_MAX_AT_1LINE];
	u32 header_cnt;
	bool loop_flag;
	id_pnm_kind_e kind = KIND_UNSUPPORT;

	header_cnt = 0;
	loop_flag = TRUE;

	while(loop_flag &&
		  fgets(line, CHAR_NUM_MAX_AT_1LINE, fp) != NULL) {

		/* コメントは読み飛ばし */
		if(strncmp(line, "#", 1) == 0) {
			continue;
		}

		switch(header_cnt++) {
		case 0:
			kind = id_pnm_GetPnmKind(line);
			break;
		case 1:
			if(kind == KIND_PBM) {
				/* pbm のヘッダは 2 行 */
				loop_flag = FALSE;
			}
			break;
		case 2:
			loop_flag = FALSE;
			break;
		}
	}

	return;
}

/* ====================================================================
| Local Variables:
| tab-width: 4
| End:
| =====================================================================
*/
