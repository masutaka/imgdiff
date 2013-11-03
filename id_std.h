/**
 *  @file
 *  @brief multi IMaGe DIFF tool 共通ヘッダファイル
 */

#ifndef _ID_STD_H_
#define _ID_STD_H_ 1

/* 数値型 */
typedef long           i32;	/*   signed, 32-bit */
typedef short          i16;	/*   signed, 16-bit */
typedef char           i8;	/*   signed,  8-bit */
typedef unsigned long  u32;	/* unsigned, 32-bit */
typedef unsigned short u16;	/* unsigned, 16-bit */
typedef unsigned char  u8;	/* unsigned,  8-bit */

/* 上記の数値型の有効範囲 */
#define I8MAX	((i32)0x7f)
#define I8MIN	((i32)((i8)(0x80)))
#define U8MIN	((u32)0)
#define U8MAX	((u32)0xff)
#define I16MAX	((i32)0x7fff)
#define I16MIN	((i32)((i16)(0x8000)))
#define U16MIN	((u32)0)
#define U16MAX	((u32)0xffff)
#define I32MAX	((i32)0x7fffffff)
#define I32MIN	((i32)(0x80000000))
#define U32MIN	((u32)0)
#define U32MAX	((u32)0xffffffff)

#ifndef TRUE
#define TRUE	1
#endif
#ifndef true
#define true	1
#endif
#ifndef FALSE
#define FALSE	0
#endif
#ifndef false
#define false	0
#endif
#ifndef NULL
#define NULL	0
#endif
#ifndef NIL
#define NIL	0
#endif

#ifndef __cplusplus
/* 論理型; TRUE か FALSE しか代入してはいけない */
typedef long bool;
#endif /* __cplusplus */

#define CHAR_NUM_MAX_AT_1LINE 256
#define CHAR_NUM_COMMAND_MAX 256

typedef enum
{
	PM_RGB,
	PM_RGBA,
	PM_CMYK,
	PM_CMYKA,
	PM_GRAY,
	PM_UNKNOWN				/* サポート外 */
} photometric_e;

typedef enum
{
	IT_TIFF,
	IT_PNM,
	IT_JPEG,
	IT_UNKNOWN				/* サポート外 */
} image_type_e;

typedef enum
{
	ll_NORMAL,
	ll_QUIET,
	ll_VERBOSE
} loglevel_e;

typedef struct id_image_t id_image_t;
typedef struct id_info_t id_info_t;

typedef bool (*read_image_f)(id_image_t *imagep);
typedef bool (*diff_image_f)(id_info_t *infop);

struct id_image_t
{
    u32 width;
    u32 height;
    u32 bpp;
    u32 channel;
	photometric_e pmetric;
    char *filename;
	void *rawp;
	image_type_e itype;
	read_image_f read_image;	/* イメージ読み込み関数 */
};

typedef struct
{
	i32 x;
	i32 y;
} offset_t;

struct id_info_t
{
	id_image_t ref;				/* リファレンス画像 */
	id_image_t tgt;				/* ターゲット画像 */
    char *diff_filename;		/* diff 画像ファイル名 */
	offset_t offset;			/* ターゲット画像へのオフセット */
	bool inverse;				/* diff 画像を反転 */
    loglevel_e loglevel;		/* ログレベル */
    u32 diff_num;				/* diff ピクセル数 */
    u32 compare_num;			/* 最終的に diff 比較したピクセル数 */
	diff_image_f diff_image;	/* diff 関数 */
};

#define CALC_IMAGE_WIDTH8(wd, bpp, channel)		(((bpp) == 8) ? ((wd) * (channel)) : (((((wd) + 7) / 8 * 8) * (channel) * (bpp)) / 8))

#define CALC_IMAGE_SIZE8(wd, ht, bpp, channel)	(CALC_IMAGE_WIDTH8((wd), (bpp), (channel)) * (ht))
#define CALC_IMAGE_SIZE8v2(wd8, ht)				((wd8) * (ht))

/* １バイト入れ替えマクロ */
/*          MSb                                                         LSb
 *          31<-----------------------------------------------------------0
 *   ===>   7<------------0 15<-----------8 23<----------16 31<----------24 */
#ifdef BIGENDIAN
#define memConvByte(data) (data)
#else   /* LITTLEENDIAN */
#define memConvByte(data)                 \
    ((((u32)(data)             ) >> 24) | \
     (((u32)(data) & 0x00ff0000) >>  8) | \
     (((u32)(data) & 0x0000ff00) <<  8) | \
     (((u32)(data) & 0x000000ff) << 24))
#endif	/* BIGENDIAN */

/* 未実装箇所をこれで括る。 */
#define MIZISSOU 1

#define NOWARNING(arg)						(void)arg;
#define NOWARNING2(arg,arg2)				(void)arg; (void)arg2;
#define NOWARNING3(arg,arg2,arg3)			(void)arg; (void)arg2; (void)arg3;
#define NOWARNING4(arg,arg2,arg3,arg4)		(void)arg; (void)arg2; (void)arg3; (void)arg4;
#define NOWARNING5(arg,arg2,arg3,arg4,arg5)	(void)arg; (void)arg2; (void)arg3; (void)arg4; (void)arg5;

#endif /* _ID_STD_H_ */

/* ====================================================================
| Local Variables:
| tab-width: 4
| End:
| =====================================================================
*/
