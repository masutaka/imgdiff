/**
 *  @file
 *  @brief Doxygen 用ドキュメント
 */

#ifndef _ID_README_H_
#define _ID_README_H_ 1

/**
 * @mainpage imgdiff (multi IMaGe DIFF tool)
 *
 * @section intro imgdiff とは？
 *
 * ２つの画像を比較するツールです。@n
 * サポートする画像フォーマットと階調、チャネルは以下の通りです。@n
 *
 * - pbm/pgm/ppm
 * <table>
 *   <tr>
 *     <td></td>
 *     <td>1channel</td>
 *     <td>3channel</td>
 *   </tr>
 *   <tr>
 *     <td>1bpp</td>
 *     <td>サポート(pbm)</td>
 *     <td>非サポート(ppm?)</td>
 *   </tr>
 *   <tr>
 *     <td>2bpp</td>
 *     <td>サポート(pgm)</td>
 *     <td>非サポート(ppm)</td>
 *   </tr>
 *   <tr>
 *     <td>4bpp</td>
 *     <td>サポート(pgm)</td>
 *     <td>非サポート(ppm)</td>
 *   </tr>
 *   <tr>
 *     <td>8bpp</td>
 *     <td>サポート(pgm)</td>
 *     <td>サポート(ppm)</td>
 *   </tr>
 * </table>
 *
 * - tiff@n
 * 階調は 8bpp、チャネルは GRAY,RGB,RGBA,CMYK,CMYKA をサポート。
 *
 * - jpeg@n
 * Solaris9 のみ対応。階調は 8bpp、チャネルは GRAY,RGB,CMYK をサポート。
 *
 * @section environment 動作環境
 *
 * - gcc でのビルドと、Solaris9 と cygwin での動作を確認しています。
 * - Visual C++ 2005 でのビルドと WindowsXP での動作を確認しています。
 *
 * @section usage 使用方法
 *
 * - 基本@n
 * @n
 * % imgdiff A.tif B.tif@n
 * @n
 * diff が発生したピクセル数が出力されます。サイズが違っていても比較できます。
 *
 * - diff 座標をダンプ@n
 * @n
 * % imgdiff -v A.tif B.tif@n
 *
 * - diff 画像を作成@n
 * @n
 * % imgdiff A.tif B.tif diff.tif@n
 * @n
 * 第３引数を指定すると、差分を tiff 画像として出力します。@n
 * 黒が diff なし、白が diff ありのピクセルです。@n
 * @n
 * 差分がないと、tiff を作りません。既に diff.tif が存在していると、@n
 * 差分の有無に関わらず削除されます。@n
 *
 * - オフセットを指定した比較@n
 * @n
 * % imgdiff -x 10 -y -30 A.tif B.tif@n
 * @n
 * オフセットを指定した比較が出来ます。@n
 * 規範画像(上記の場合 A.tif)を基準とした値を指定して下さい。@n
 *
 * - ヘルプ@n
 * @n
 * % imgdiff -h@n
 *
 * @section return 戻り値
 *
 * imgdiff は以下の戻り値を返します。
 *
 * - 0: 正常終了＆diffなし
 * - 1: 正常終了＆diffあり
 * - それ以外: 異常終了
 *
 * @section build ビルド方法
 *
 * - gcc(Solaris9|cygwin|Linux) の場合@n
 * @n
 * make するだけです。@n
 * @n
 * % make@n
 * @n
 * デバッグ時には Makefile の以下の箇所をコメントアウトして、make して下さい。@n
 * @n
 * # For Release@n
 * CFLAGS   = -D$(ENDIAN) -D$(JPEG) -O3 -Wall -Wextra@n
 * LDFLAGS  = -s@n
 *
 * - Visual C++ 2005 の場合@n
 * @n
 * imgdiff.sln をご利用ください。@n
 * リリース時には Release ビルドをして下さい。@n
 *
 * @section library ライブラリ
 *
 * このソフトウェアは以下のライブラリを使用しています。@n
 *
 * - libtiff@n
 * Copyright (c) 1988-1997 Sam Leffler@n
 * Copyright (c) 1991-1997 Silicon Graphics, Inc.@n
 *
 * - libjpeg@n
 * This software is copyright (C) 1991-1998, Thomas G. Lane.@n
 * All Rights Reserved except as specified below.@n
 *
 * @section warning 注意事項
 *
 * Windows で動作させるためには Windows 版の libtiff が必要です。@n
 * v3.8.2 で動作確認しています。@n
 * @n
 * http://gnuwin32.sourceforge.net/packages/tiff.htm
 *
 * @section todo TODO
 *
 * - Windows でも Jpeg を扱えるようにする。
 * - Windows では libtiff や libjpeg を静的リンクさせる。
 * - id_jpeg.c は、jpeg_(open|read|close) 関数を作って整理した方が良いな。
 * - 1,2,4bpp ppm 画像にも対応する。@n
 * - id_diff.c の以下の関数を実装する。@n
 *   id_diff_Diff1bpp1channel()@n
 *   id_diff_Diff1bpp3channel()@n
 *   id_diff_Diff2bpp1channel()@n
 *   id_diff_Diff2bpp3channel()@n
 *   id_diff_Diff4bpp1channel()@n
 *   id_diff_Diff4bpp3channel()@n
 * - diff 画像に *.tif 以外を指定できるようにしても面白いかもしれない。
 * - 全ピクセルをメモリに展開して比較する動作であるため、巨大な画像の場合@n
 *   は memory alloc error になることがある。回避するためには、例えばスキャ@n
 *   ンライン単位で比較する必要がある。
 *
 * @section go さっそく main() に飛んでみましょう！！
 *
 * @ref main
 *
 */

#endif /* _ID_README_H_ */

/* ====================================================================
| Local Variables:
| tab-width: 4
| End:
| =====================================================================
*/
