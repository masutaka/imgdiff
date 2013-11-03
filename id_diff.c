/**
 *  @file
 *  @brief �摜��r�N���X
 */

#include "id_std.h"
#include "id_diff.h"

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>

#include "id_diff_image.h"

/* �J�����g���p�\���� */
typedef struct
{
	union {
		/* 1,2,4bpp �p */
		struct {
			u32 *cur32p;	/* ���݂̃A�h���X */
			u32 mask;
		} b124;

		/* 8bpp �p */
		struct {
			u8 *cur8p;		/* ���݂̃A�h���X */
		} b8;
	} s;

	u32 startx;				/* �J�n X ���W */
	u32 endx;				/* �I�� X ���W */
	u32 curx;				/* ���݂� X ���W */
	u32 starty;				/* �J�n Y ���W */
	u32 endy;				/* �I�� Y ���W */
	u32 cury;				/* ���݂� Y ���W */
} id_diff_pos_t;

static bool id_diff_Diff1bpp1channel(id_info_t *infop);
static bool id_diff_Diff1bpp3channel(id_info_t *infop);
static bool id_diff_Diff2bpp1channel(id_info_t *infop);
static bool id_diff_Diff2bpp3channel(id_info_t *infop);
static bool id_diff_Diff4bpp1channel(id_info_t *infop);
static bool id_diff_Diff4bpp3channel(id_info_t *infop);
static bool id_diff_Diff8bpp1channel(id_info_t *infop);
static bool id_diff_Diff8bpp3channel(id_info_t *infop);
static bool id_diff_Diff8bpp4channel(id_info_t *infop);
static bool id_diff_Diff8bpp5channel(id_info_t *infop);
static void id_diff_DiffInit(id_info_t *infop, id_diff_pos_t *refposp, id_diff_pos_t *tgtposp);
static void id_diff_DiffAdvanceX(id_info_t *infop, id_diff_pos_t *refposp, id_diff_pos_t *tgtposp);
static void id_diff_DiffAdvanceY(id_info_t *infop, id_diff_pos_t *refposp, id_diff_pos_t *tgtposp);
static void id_diff_LogOutput(id_info_t *infop, u32 refx, u32 refy, u32 tgtx, u32 tgty, bool match, ...);
static char* id_diff_LogOutput_make_str(u32 num);
static char* id_diff_LogOutput_make_str_for_offset(u32 num);
static char* id_diff_LogOutput_make_str_sub(u32 num);

diff_image_f
id_diff_GetDiffFunction(id_info_t *infop)
{
	diff_image_f f = NULL;
	u32 bpp = infop->ref.bpp;
	u32 channel = infop->ref.channel;

	switch(bpp) {
	case 1:
		switch(channel) {
		case 1:
			f = id_diff_Diff1bpp1channel;
			break;
		case 3:
			f = id_diff_Diff1bpp3channel;
			break;
		}
		break;
	case 2:
		switch(channel) {
		case 1:
			f = id_diff_Diff2bpp1channel;
			break;
		case 3:
			f = id_diff_Diff2bpp3channel;
			break;
		}
		break;
	case 4:
		switch(channel) {
		case 1:
			f = id_diff_Diff4bpp1channel;
			break;
		case 3:
			f = id_diff_Diff4bpp3channel;
			break;
		}
		break;
	case 8:
		switch(channel) {
		case 1:
			f = id_diff_Diff8bpp1channel;
			break;
		case 3:
			f = id_diff_Diff8bpp3channel;
			break;
		case 4:
			f = id_diff_Diff8bpp4channel;
			break;
		case 5:
			f = id_diff_Diff8bpp5channel;
			break;
		}
		break;
	}

	if(f == NULL) {
		fprintf(stderr, "diff function is NULL(bpp=%lu, channel=%lu).\n", bpp, channel);
	}

	return f;
}

static bool
id_diff_Diff1bpp1channel(id_info_t *infop)
{
#ifdef MIZISSOU
	fprintf(stderr, "%ldbpp %ldchannel ha mizissou desu!!\n",
			infop->ref.bpp, infop->ref.channel);
	exit(-1);
#endif	/* MIZISSOU */

	return FALSE;
}

static bool
id_diff_Diff1bpp3channel(id_info_t *infop)
{
#ifdef MIZISSOU
	fprintf(stderr, "%ldbpp %ldchannel ha mizissou desu!!\n",
			infop->ref.bpp, infop->ref.channel);
	exit(-1);
#endif	/* MIZISSOU */

	return FALSE;
}

static bool
id_diff_Diff2bpp1channel(id_info_t *infop)
{
#ifdef MIZISSOU
	fprintf(stderr, "%ldbpp %ldchannel ha mizissou desu!!\n",
			infop->ref.bpp, infop->ref.channel);
	exit(-1);
#endif	/* MIZISSOU */

	return FALSE;
}

static bool
id_diff_Diff2bpp3channel(id_info_t *infop)
{
#ifdef MIZISSOU
	fprintf(stderr, "%ldbpp %ldchannel ha mizissou desu!!\n",
			infop->ref.bpp, infop->ref.channel);
	exit(-1);
#endif	/* MIZISSOU */

	return FALSE;
}

static bool
id_diff_Diff4bpp1channel(id_info_t *infop)
{
#ifdef MIZISSOU
	fprintf(stderr, "%ldbpp %ldchannel ha mizissou desu!!\n",
			infop->ref.bpp, infop->ref.channel);
	exit(-1);
#endif	/* MIZISSOU */

	return FALSE;
}

static bool
id_diff_Diff4bpp3channel(id_info_t *infop)
{
#ifdef MIZISSOU
	fprintf(stderr, "%ldbpp %ldchannel ha mizissou desu!!\n",
			infop->ref.bpp, infop->ref.channel);
	exit(-1);
#endif	/* MIZISSOU */

	return FALSE;
}

static bool
id_diff_Diff8bpp1channel(id_info_t *infop)
{
	void *dp;
	bool match, retval = TRUE;
	id_diff_pos_t refpos;
	id_diff_pos_t tgtpos;

	/* �摜��r�̂��߂̏��������� */
	id_diff_DiffInit(infop, &refpos, &tgtpos);

	/* diff �摜�I�[�v�� */
	dp = id_diff_DiffImageOpen(infop);

	while((refpos.cury <= refpos.endy) &&
		  (tgtpos.cury <= tgtpos.endy)) {
		while((refpos.curx <= refpos.endx) &&
			  (tgtpos.curx <= tgtpos.endx)) {
			if((refpos.s.b8.cur8p[0] == tgtpos.s.b8.cur8p[0])) {
				match = TRUE;
			}
			else {
				infop->diff_num++;
				match = FALSE;
				retval = FALSE;
			}

			infop->compare_num++;

			/* diff �摜�ɂP�s�N�Z�������o�� */
			id_diff_DiffImageWrite1Pixel(dp, match);

			/* diff ���O�o�� */
			id_diff_LogOutput(infop,
							  refpos.curx, refpos.cury,
							  tgtpos.curx, tgtpos.cury,
							  match,
							  refpos.s.b8.cur8p[0],
							  tgtpos.s.b8.cur8p[0]);

			/* X ���W�X�V */
			id_diff_DiffAdvanceX(infop, &refpos, &tgtpos);
		}
		/* Y ���W�X�V */
		id_diff_DiffAdvanceY(infop, &refpos, &tgtpos);
	}

	/* diff �摜�N���[�Y */
	id_diff_DiffImageClose(dp, infop->diff_num);

	return retval;
}

static bool
id_diff_Diff8bpp3channel(id_info_t *infop)
{
	void *dp;
	bool match, retval = TRUE;
	id_diff_pos_t refpos;
	id_diff_pos_t tgtpos;

	/* �摜��r�̂��߂̏��������� */
	id_diff_DiffInit(infop, &refpos, &tgtpos);

	/* diff �摜�I�[�v�� */
	dp = id_diff_DiffImageOpen(infop);

	while((refpos.cury <= refpos.endy) &&
		  (tgtpos.cury <= tgtpos.endy)) {
		while((refpos.curx <= refpos.endx) &&
			  (tgtpos.curx <= tgtpos.endx)) {
			if((refpos.s.b8.cur8p[0] == tgtpos.s.b8.cur8p[0]) &&
			   (refpos.s.b8.cur8p[1] == tgtpos.s.b8.cur8p[1]) &&
			   (refpos.s.b8.cur8p[2] == tgtpos.s.b8.cur8p[2])) {
				match = TRUE;
			}
			else {
				infop->diff_num++;
				match = FALSE;
				retval = FALSE;
			}

			infop->compare_num++;

			/* diff �摜�ɂP�s�N�Z�������o�� */
			id_diff_DiffImageWrite1Pixel(dp, match);

			/* diff ���O�o�� */
			id_diff_LogOutput(infop,
							  refpos.curx, refpos.cury,
							  tgtpos.curx, tgtpos.cury,
							  match,
							  refpos.s.b8.cur8p[0],
							  refpos.s.b8.cur8p[1],
							  refpos.s.b8.cur8p[2],
							  tgtpos.s.b8.cur8p[0],
							  tgtpos.s.b8.cur8p[1],
							  tgtpos.s.b8.cur8p[2]);

			/* X ���W�X�V */
			id_diff_DiffAdvanceX(infop, &refpos, &tgtpos);
		}
		/* Y ���W�X�V */
		id_diff_DiffAdvanceY(infop, &refpos, &tgtpos);
	}

	/* diff �摜�N���[�Y */
	id_diff_DiffImageClose(dp, infop->diff_num);

	return retval;
}

static bool
id_diff_Diff8bpp4channel(id_info_t *infop)
{
	void *dp;
	bool match, retval = TRUE;
	id_diff_pos_t refpos;
	id_diff_pos_t tgtpos;

	/* �摜��r�̂��߂̏��������� */
	id_diff_DiffInit(infop, &refpos, &tgtpos);

	/* diff �摜�I�[�v�� */
	dp = id_diff_DiffImageOpen(infop);

	while((refpos.cury <= refpos.endy) &&
		  (tgtpos.cury <= tgtpos.endy)) {
		while((refpos.curx <= refpos.endx) &&
			  (tgtpos.curx <= tgtpos.endx)) {
			if((refpos.s.b8.cur8p[0] == tgtpos.s.b8.cur8p[0]) &&
			   (refpos.s.b8.cur8p[1] == tgtpos.s.b8.cur8p[1]) &&
			   (refpos.s.b8.cur8p[2] == tgtpos.s.b8.cur8p[2]) &&
			   (refpos.s.b8.cur8p[3] == tgtpos.s.b8.cur8p[3])) {
				match = TRUE;
			}
			else {
				infop->diff_num++;
				match = FALSE;
				retval = FALSE;
			}

			infop->compare_num++;

			/* diff �摜�ɂP�s�N�Z�������o�� */
			id_diff_DiffImageWrite1Pixel(dp, match);

			/* diff ���O�o�� */
			id_diff_LogOutput(infop,
							  refpos.curx, refpos.cury,
							  tgtpos.curx, tgtpos.cury,
							  match,
							  refpos.s.b8.cur8p[0],
							  refpos.s.b8.cur8p[1],
							  refpos.s.b8.cur8p[2],
							  refpos.s.b8.cur8p[3],
							  tgtpos.s.b8.cur8p[0],
							  tgtpos.s.b8.cur8p[1],
							  tgtpos.s.b8.cur8p[2],
							  tgtpos.s.b8.cur8p[3]);

			/* X ���W�X�V */
			id_diff_DiffAdvanceX(infop, &refpos, &tgtpos);
		}
		/* Y ���W�X�V */
		id_diff_DiffAdvanceY(infop, &refpos, &tgtpos);
	}

	/* diff �摜�N���[�Y */
	id_diff_DiffImageClose(dp, infop->diff_num);

	return retval;
}

static bool
id_diff_Diff8bpp5channel(id_info_t *infop)
{
	void *dp;
	bool match, retval = TRUE;
	id_diff_pos_t refpos;
	id_diff_pos_t tgtpos;

	/* �摜��r�̂��߂̏��������� */
	id_diff_DiffInit(infop, &refpos, &tgtpos);

	/* diff �摜�I�[�v�� */
	dp = id_diff_DiffImageOpen(infop);

	while((refpos.cury <= refpos.endy) &&
		  (tgtpos.cury <= tgtpos.endy)) {
		while((refpos.curx <= refpos.endx) &&
			  (tgtpos.curx <= tgtpos.endx)) {
			if((refpos.s.b8.cur8p[0] == tgtpos.s.b8.cur8p[0]) &&
			   (refpos.s.b8.cur8p[1] == tgtpos.s.b8.cur8p[1]) &&
			   (refpos.s.b8.cur8p[2] == tgtpos.s.b8.cur8p[2]) &&
			   (refpos.s.b8.cur8p[3] == tgtpos.s.b8.cur8p[3]) &&
			   (refpos.s.b8.cur8p[4] == tgtpos.s.b8.cur8p[4])) {
				match = TRUE;
			}
			else {
				infop->diff_num++;
				match = FALSE;
				retval = FALSE;
			}

			infop->compare_num++;

			/* diff �摜�ɂP�s�N�Z�������o�� */
			id_diff_DiffImageWrite1Pixel(dp, match);

			/* diff ���O�o�� */
			id_diff_LogOutput(infop,
							  refpos.curx, refpos.cury,
							  tgtpos.curx, tgtpos.cury,
							  match,
							  refpos.s.b8.cur8p[0],
							  refpos.s.b8.cur8p[1],
							  refpos.s.b8.cur8p[2],
							  refpos.s.b8.cur8p[3],
							  refpos.s.b8.cur8p[4],
							  tgtpos.s.b8.cur8p[0],
							  tgtpos.s.b8.cur8p[1],
							  tgtpos.s.b8.cur8p[2],
							  tgtpos.s.b8.cur8p[3],
							  tgtpos.s.b8.cur8p[4]);

			/* X ���W�X�V */
			id_diff_DiffAdvanceX(infop, &refpos, &tgtpos);
		}
		/* Y ���W�X�V */
		id_diff_DiffAdvanceY(infop, &refpos, &tgtpos);
	}

	/* diff �摜�N���[�Y */
	id_diff_DiffImageClose(dp, infop->diff_num);

	return retval;
}

static void
id_diff_DiffInit(id_info_t *infop, id_diff_pos_t *refposp, id_diff_pos_t *tgtposp)
{
	u8 *ref_top8p = infop->ref.rawp;
	u8 *tgt_top8p = infop->tgt.rawp;
	u32 channel = infop->ref.channel;
	offset_t o_abs;

	/* �I�t�Z�b�g�̐�Βl���v�Z */
	o_abs.x = labs(infop->offset.x);
	o_abs.y = labs(infop->offset.y);

	if(infop->offset.x == 0) {
		refposp->startx = 0;
		refposp->endx = infop->ref.width - 1;
		tgtposp->startx = 0;
		tgtposp->endx = infop->tgt.width - 1;
	}
	else if(infop->offset.x > 0) {
		refposp->startx = o_abs.x;
		refposp->endx = infop->ref.width - 1;
		tgtposp->startx = 0;
		tgtposp->endx = infop->tgt.width - o_abs.x - 1;
	}
	else {
		refposp->startx = 0;
		refposp->endx = infop->ref.width - o_abs.x - 1;
		tgtposp->startx = o_abs.x;
		tgtposp->endx = infop->tgt.width - 1;
	}

	if(infop->offset.y == 0) {
		refposp->starty = 0;
		refposp->endy = infop->ref.height - 1;
		tgtposp->starty = 0;
		tgtposp->endy = infop->tgt.height - 1;
	}
	else if(infop->offset.y > 0) {
		refposp->starty = o_abs.y;
		refposp->endy = infop->ref.height - 1;
		tgtposp->starty = 0;
		tgtposp->endy = infop->tgt.height - o_abs.y - 1;
	}
	else {
		refposp->starty = 0;
		refposp->endy = infop->ref.height - o_abs.y - 1;
		tgtposp->starty = o_abs.y;
		tgtposp->endy = infop->tgt.height - 1;
	}

	refposp->s.b8.cur8p = ref_top8p +
						  ((refposp->startx + (refposp->starty * infop->ref.width)) * channel);
	refposp->curx = refposp->startx;
	refposp->cury = refposp->starty;

	tgtposp->s.b8.cur8p = tgt_top8p +
						  ((tgtposp->startx + (tgtposp->starty * infop->tgt.width)) * channel);
	tgtposp->curx = tgtposp->startx;
	tgtposp->cury = tgtposp->starty;

	return;
}

static void
id_diff_DiffAdvanceX(id_info_t *infop, id_diff_pos_t *refposp, id_diff_pos_t *tgtposp)
{
	u32 channel = infop->ref.channel;

	refposp->curx++;
	refposp->s.b8.cur8p += channel;

	tgtposp->curx++;
	tgtposp->s.b8.cur8p += channel;

	return;
}

static void
id_diff_DiffAdvanceY(id_info_t *infop, id_diff_pos_t *refposp, id_diff_pos_t *tgtposp)
{
	u8 *ref_top8p = infop->ref.rawp;
	u8 *tgt_top8p = infop->tgt.rawp;
	u32 channel = infop->ref.channel;

	refposp->curx = refposp->startx;
	refposp->cury++;
	refposp->s.b8.cur8p = ref_top8p +
						  ((refposp->curx + (refposp->cury * infop->ref.width)) * channel);

	tgtposp->curx = tgtposp->startx;
	tgtposp->cury++;
	tgtposp->s.b8.cur8p = tgt_top8p +
						  ((tgtposp->curx + (tgtposp->cury * infop->tgt.width)) * channel);

	return;
}

/* �Ⴆ�� RGB 8bpp �̏ꍇ�A */
/*  id_diff_LogOutput(infop, refX, refY, tgtX, tgtY, match, refR, refG, refB, tgtR, tgtG, tgtB); */
/* �ȂǂƎg�p����Bref->tgt �̏��ԁBchannel ���Ƃ����킹�邱�ƁB */
static void
id_diff_LogOutput(id_info_t *infop,
				  u32 refx, u32 refy,
				  u32 tgtx, u32 tgty,
				  bool match, ...)
{
	u32 channel = infop->ref.channel;
	char *format;
	char str[CHAR_NUM_MAX_AT_1LINE];
	va_list pixels;

	if(infop->loglevel != ll_VERBOSE) {
		return;
	}

	if(match) {
		return;
	}

	/* �I�t�Z�b�g�w��Ȃ� */
	if((refx == tgtx) && (refy == tgty)) {
		format = id_diff_LogOutput_make_str(channel);
		sprintf(str, format, refx, refy);
	}
	/* �I�t�Z�b�g�w�肠�� */
	else {
		format = id_diff_LogOutput_make_str_for_offset(channel);
		sprintf(str, format, refx, refy, tgtx, tgty);
	}

	va_start(pixels, match);
	vprintf(str, pixels);
	va_end(pixels);

	return;
}

/* �ȉ��̂悤�ȕ������Ԃ��B */
/*  "diff: (x, y) = (%ld, %ld), ref(0x%%02x, 0x%%02x, 0x%%02x), tgt(0x%%02x, 0x%%02x, 0x%%02x)\n" */
/* num*2 �������� 0x%%02x �������B */
static char*
id_diff_LogOutput_make_str(u32 num)
{
	static char format[CHAR_NUM_MAX_AT_1LINE];
	char *format_sub;

	format_sub = id_diff_LogOutput_make_str_sub(num);

	sprintf(format,
			"diff: (x, y) = (%%4ld, %%4ld), ref(%s), tgt(%s)\n",
			format_sub, format_sub);

	return format;
}

/* �ȉ��̂悤�ȕ������Ԃ��B */
/*  "diff: ref(x=%4ld, y=%4ld, 0x%%02x, 0x%%02x, 0x%%02x), tgt(x=%4ld, y=%4ld, 0x%%02x, 0x%%02x, 0x%%02x)\n" */
/* num*2 �������� 0x%%02x �������B */
static char*
id_diff_LogOutput_make_str_for_offset(u32 num)
{
	static char format_for_offset[CHAR_NUM_MAX_AT_1LINE];
	char *format_sub;

	format_sub = id_diff_LogOutput_make_str_sub(num);

	sprintf(format_for_offset,
			"diff: ref(x=%%4ld, y=%%4ld, %s), tgt(x=%%4ld, y=%%4ld, %s)\n",
			format_sub, format_sub);

	return format_for_offset;
}

/* "0x%%02x, 0x%%02x, 0x%%02x" �̂悤�ȕ������Ԃ��B */
/* num �������� 0x%%02x �������B */
static char*
id_diff_LogOutput_make_str_sub(u32 num)
{
	static char format_sub[CHAR_NUM_MAX_AT_1LINE];
	u32 point = 0;
	const char *cat_str = "0x%%02x, ";
	const u32 cat_str_size8 = 9;
	const char *last_cat_str = "0x%%02x\0";
	const u32 last_cat_str_size8 = 8;

	while(num--) {
		if(num == 0) {
			/* �Ō�̏���  */
			strncpy(&format_sub[point], last_cat_str, last_cat_str_size8);
			point += last_cat_str_size8;
		}
		else {
			strncpy(&format_sub[point], cat_str, cat_str_size8);
			point += cat_str_size8;
		}
	}

	return format_sub;
}


/* ====================================================================
| Local Variables:
| tab-width: 4
| End:
| =====================================================================
*/
