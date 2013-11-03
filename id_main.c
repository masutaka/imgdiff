/**
 *  @file
 *  @brief multi IMaGe DIFF tool メインルーチン
 */

#define VERSION "v2.6"

#include "id_std.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "id_readme.h"
#include "id_tiff.h"
#include "id_pnm.h"
#include "id_jpeg.h"
#include "id_diff.h"

#define call_read_image(f, arg) (((f) != NULL) && !(f)((arg)))
#define call_diff_image(f, arg) (((f) != NULL) && !(f)((arg)))

static bool getarg_for_num(char *arg[], i32 *nump);
static bool str_is_digit(char *str);
static void version(char *cmdname);
static void help(char *cmdname);
static void* id_New(char *ref_fname, char *targ_fname, char *diff_fname, offset_t offset, bool inverse, bool quiet, bool verbose);
static bool id_Init(void *p);
static bool id_InitImage(id_image_t *imagep, loglevel_e loglevel);
static image_type_e id_GetImageType(char *filename);
static char* id_StrnTolower(char *str, size_t n);
static bool id_ComparePossible(id_info_t *infop);
static bool id_ReadImage(void *p);
static bool id_DiffImage(void *p);
static void id_LogOutput(id_info_t *infop);
static bool id_Delete(void *p);

int
main(int ac, char **av)
{
	char *cmdname, *ref_fname, *targ_fname, *diff_fname;
	bool inverse = FALSE;
	bool quiet = FALSE;
	bool verbose = FALSE;
	void *p;
	i32 i;
	offset_t offset;
	int retval = 0;

	cmdname = av[0];

	if(ac == 1) {
		help(cmdname);
		retval = 0;
		goto END;
	}

	offset.x = 0;
	offset.y = 0;

	for(i = 1; i < ac; i++) {
		if(av[i][0] != '-') {
			i++;
			break;
		}

		switch(av[i][1]) {
		case 'x':
			i += getarg_for_num(&av[i], &offset.x) ? 1 : 0;
			break;
		case 'y':
			i += getarg_for_num(&av[i], &offset.y) ? 1 : 0;
			break;
		case 'i':
			inverse = TRUE;
			break;
		case 'q':
			quiet = TRUE;
			break;
		case 'v':
			verbose = TRUE;
			break;
		case 'V':
			version(cmdname);
			retval = 0;
			goto END;
		case 'h':
			help(cmdname);
			retval = 0;
			goto END;
		default:
			help(cmdname);
			retval = 2;
			goto END;
		}
	}

	/* 処理したオプションの数だけ減らす。 */
	ac -= (i - 1);

	/* 入力ファイルまでポインタを進める。 */
	av += (i - 1);

	ref_fname = av[0];
	targ_fname = av[1];
	diff_fname = av[2];

	/* New */
	p = id_New(ref_fname, targ_fname, diff_fname, offset, inverse, quiet, verbose);
	if(p == NULL) {
		retval = 3;
		goto END;
	}

	/* 初期化 */
	if(!id_Init(p)) {
		retval = 4;
		goto ENDPROC;
	}

	/* イメージ読み込み */
	if(!id_ReadImage(p)) {
		retval = 5;
		goto ENDPROC;
	}

	/* 比較 */
	if(id_DiffImage(p)) {
		retval = 0;
	}
	else {
		retval = 1;
	}

ENDPROC:
	/* Delete */
	id_Delete(p);

END:
	return retval;
}

/* 機能: "-x-50" や "-y 100" 等のオプションを解析し、数値を獲得する。 */
/* 入力:   char **arg -> 処理する引数                                 */
/* 出力:   i32 *nump  -> 獲得した数値                                 */
/* 戻り値: bool       -> TRUE:  オプションと数値が離れていた。        */
/*                       FALSE: オプションと数値が離れていなかった。  */
static bool
getarg_for_num(char *arg[], i32 *nump)
{
	bool separated;
	char *digit_s;		/* 文字列で表された数字 */

	if(strlen(arg[0]) == 2) {
		digit_s = arg[1];
		if(str_is_digit(digit_s)) {
			/* "-x 数字" の場合 */
			*nump = atoi(digit_s);
			separated = TRUE;
		}
		else {
			/* "-x 非数字" の場合 */
			*nump = 0;
			separated = FALSE;
		}
	}
	else {
		digit_s = &arg[0][2];
		if(str_is_digit(digit_s)) {
			/* "-x数字" の場合 */
			*nump = atoi(digit_s);
			separated = FALSE;
		}
		else {
			/* "-x非数字" の場合 */
			*nump = 0;
			separated = FALSE;
		}
	}

	return separated;
}

/* isdigit() の文字列版 */
static bool
str_is_digit(char *str)
{
	u32 i = 0;
	bool retval = TRUE;
	int c;

	/* マイナスの数もサポート */
	if(str[0] == '-') {
		i++;
	}

	while(str[i] != '\0') {
		c = str[i++];
		if(isdigit(c) == 0) {
			retval = FALSE;
			break;
		}
	}

	return retval;
}

static void
version(char *cmdname)
{
	fprintf(stdout, "%s %s\n", cmdname, VERSION);

	return;
}

#define HELP_FORMAT "\
NAME\n\
       multi IMaGe DIFF tool %s\n\
\n\
SYNOPSIS\n\
       %s [options] <reffile> <targfile> <difffile>\n\
\n\
OPTIONS\n\
       -x num     offset x (default 0, This range is %ld~+%ld)\n\
       -y num     offset y (default 0, This range is %ld~+%ld)\n\
       -i         inverse option for <difffile>\n\
       -q         set quiet mode (The log is not output at all)\n\
       -v         set verbose mode (All differences are output)\n\
       -V         output version information and exit\n\
       -h         display this help and exit\n\
\n\
"

static void
help(char *cmdname)
{
	fprintf(stdout, HELP_FORMAT,
			VERSION, cmdname, I32MIN, I32MAX, I32MIN, I32MAX);

	return;
}

static void*
id_New(char *ref_fname, char *targ_fname, char *diff_fname,
	   offset_t offset, bool inverse, bool quiet, bool verbose)
{
	static id_info_t id_info;
	id_info_t *infop = &id_info;

	if(ref_fname == NULL) {
		fprintf(stderr, "reference filename is NULL!!\n");
		return NULL;
	}

	if(targ_fname == NULL) {
		fprintf(stderr, "target filename is NULL!!\n");
		return NULL;
	}

	memset(infop, 0, sizeof(id_info_t));

	infop->ref.filename = ref_fname;
	infop->tgt.filename = targ_fname;

	infop->ref.rawp = NULL;
	infop->tgt.rawp = NULL;

	infop->ref.read_image = NULL;
	infop->tgt.read_image = NULL;

	infop->diff_filename = diff_fname;
	infop->offset = offset;
	infop->inverse = inverse;

	infop->loglevel = ll_NORMAL;
	if(quiet) {
		infop->loglevel = ll_QUIET;
	}
	if(verbose) {
		infop->loglevel = ll_VERBOSE;
	}

	return infop;
}

static bool
id_Init(void *p)
{
	id_info_t *infop = p;

	/* リファレンス画像のための初期化処理 */
	if(!id_InitImage(&infop->ref, infop->loglevel)) {
		return FALSE;
	}

	/* ターゲット画像のための初期化処理 */
	if(!id_InitImage(&infop->tgt, infop->loglevel)) {
		return FALSE;
	}

	/* 比較可能か？ */
	if(!id_ComparePossible(infop)) {
		return FALSE;
	}

	return TRUE;
}

static bool
id_InitImage(id_image_t *imagep, loglevel_e loglevel)
{
	imagep->itype = id_GetImageType(imagep->filename);

	switch(imagep->itype) {
	case IT_TIFF:
		return id_tiff_Init(imagep, loglevel);
	case IT_PNM:
		return id_pnm_Init(imagep, loglevel);
	case IT_JPEG:
		return id_jpeg_Init(imagep, loglevel);
	default:
		fprintf(stderr, "Unknown Image Format(%d, fname=%s)!!\n",
				imagep->itype, imagep->filename);
		return FALSE;
	}
}

static image_type_e
id_GetImageType(char *filename)
{
    char *dot;

    dot = strrchr(filename, '.');
    if(dot == NULL) {
		return IT_UNKNOWN;
	}

	if((strncmp(id_StrnTolower(dot, 4), ".tif", 4)  == 0) ||
	   (strncmp(id_StrnTolower(dot, 5), ".tiff", 5) == 0)) {
		return IT_TIFF;
    }

	if((strncmp(id_StrnTolower(dot, 4), ".pbm", 4) == 0) ||
	   (strncmp(id_StrnTolower(dot, 4), ".pgm", 4) == 0) ||
	   (strncmp(id_StrnTolower(dot, 4), ".ppm", 4) == 0)) {
		return IT_PNM;
    }

	if((strncmp(id_StrnTolower(dot, 4), ".jpg", 4)  == 0) ||
	   (strncmp(id_StrnTolower(dot, 5), ".jpeg", 5) == 0)) {
		return IT_JPEG;
    }

	return IT_UNKNOWN;
}

/* 文字列を読み込み、小文字のみの文字列を返す。 */
/* 返すアドレスは固定値なので注意すること。     */
static char*
id_StrnTolower(char *str, size_t n)
{
#define STRSIZE 32
	static char lower[STRSIZE];
	size_t i;

	if(n > STRSIZE) {
		fprintf(stderr, "str size (%d) is larger than %d.\n", n, STRSIZE);
	}

	memset(lower, '\0', STRSIZE);

	for(i = 0; ((i < n) && (i < STRSIZE)); i++) {
		lower[i] = tolower(str[i]);
	}

	return lower;
#undef STRSIZE
}

static bool
id_ComparePossible(id_info_t *infop)
{
	const char *photoNames[] = {
		"RGB",					/* PM_RGB */
		"RGBA",					/* PM_RGBA */
		"CMYK",					/* PM_CMYK */
		"CMYKA",				/* PM_CMYKA */
		"GRAY",					/* PM_GRAY */
		"UNKNOWN"				/* PM_UNKNOWN */
	};

	if(infop->ref.pmetric != infop->tgt.pmetric) {
		fprintf(stderr, "photometric mismatch(ref=%s, tgt=%s).\n",
				photoNames[infop->ref.pmetric], photoNames[infop->tgt.pmetric]);
		return FALSE;
	}

	if(infop->ref.channel != infop->tgt.channel) {
		fprintf(stderr, "channel mismatch(ref=%ld, tgt=%ld).\n",
				infop->ref.channel, infop->tgt.channel);
		return FALSE;
	}

	if(infop->ref.bpp != infop->tgt.bpp) {
		fprintf(stderr, "bpp mismatch(ref=%ld, tgt=%ld).\n",
				infop->ref.bpp, infop->tgt.bpp);
		return FALSE;
	}

	return TRUE;
}

static bool
id_ReadImage(void *p)
{
	id_info_t *infop = p;

	/* リファレンス画像の読み込み */
	if(call_read_image(infop->ref.read_image, &infop->ref)) {
		return FALSE;
	}

	/* ターゲット画像の読み込み */
	if(call_read_image(infop->tgt.read_image, &infop->tgt)) {
		return FALSE;
	}

	/* diff 関数セット */
	infop->diff_image = id_diff_GetDiffFunction(infop);

	return TRUE;
}

static bool
id_DiffImage(void *p)
{
	id_info_t *infop = p;
	bool retval = TRUE;

	if(call_diff_image(infop->diff_image,
					   infop)) {
		retval = FALSE;
	}

	id_LogOutput(infop);

	return retval;
}

static void
id_LogOutput(id_info_t *infop)
{
	double diff_rate;

	if(infop->loglevel == ll_QUIET) {
		return;
	}

	if((infop->loglevel == ll_NORMAL) && (infop->diff_num == 0)) {
		return;
	}

	diff_rate = (infop->compare_num != 0) ?
				(((double)infop->diff_num / infop->compare_num) * 100) : 0;

	fprintf(stdout, "diffnum: %ld/%ld pixel (%0.3f%%), offset = (%ld, %ld)\n",
			infop->diff_num, infop->compare_num, diff_rate,
			infop->offset.x, infop->offset.y);

	return;
}

static bool
id_Delete(void *p)
{
	id_info_t *infop = p;

	if(infop->ref.rawp != NULL) {
		free(infop->ref.rawp);
	}

	if(infop->tgt.rawp != NULL) {
		free(infop->tgt.rawp);
	}

	return TRUE;
}

/* ====================================================================
| Local Variables:
| tab-width: 4
| End:
| =====================================================================
*/
