/**
 *  @file
 *  @brief DIFF 画像クラス公開ヘッダファイル
 */

#ifndef _ID_DIFF_IMAGE_H_
#define _ID_DIFF_IMAGE_H_ 1

#include "id_std.h"

extern void* id_diff_DiffImageOpen(id_info_t *infop);
extern void id_diff_DiffImageWrite1Pixel(void *dp, bool match);
extern void id_diff_DiffImageClose(void *dp, u32 diff_num);

#endif /* _ID_DIFF_IMAGE_H_ */

/* ====================================================================
| Local Variables:
| tab-width: 4
| End:
| =====================================================================
*/
