/**
 *  @file
 *  @brief Doxygen �p�h�L�������g
 */

#ifndef _ID_README_H_
#define _ID_README_H_ 1

/**
 * @mainpage imgdiff (multi IMaGe DIFF tool)
 *
 * @section intro imgdiff �Ƃ́H
 *
 * �Q�̉摜���r����c�[���ł��B@n
 * �T�|�[�g����摜�t�H�[�}�b�g�ƊK���A�`���l���͈ȉ��̒ʂ�ł��B@n
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
 *     <td>�T�|�[�g(pbm)</td>
 *     <td>��T�|�[�g(ppm?)</td>
 *   </tr>
 *   <tr>
 *     <td>2bpp</td>
 *     <td>�T�|�[�g(pgm)</td>
 *     <td>��T�|�[�g(ppm)</td>
 *   </tr>
 *   <tr>
 *     <td>4bpp</td>
 *     <td>�T�|�[�g(pgm)</td>
 *     <td>��T�|�[�g(ppm)</td>
 *   </tr>
 *   <tr>
 *     <td>8bpp</td>
 *     <td>�T�|�[�g(pgm)</td>
 *     <td>�T�|�[�g(ppm)</td>
 *   </tr>
 * </table>
 *
 * - tiff@n
 * �K���� 8bpp�A�`���l���� GRAY,RGB,RGBA,CMYK,CMYKA ���T�|�[�g�B
 *
 * - jpeg@n
 * Solaris9 �̂ݑΉ��B�K���� 8bpp�A�`���l���� GRAY,RGB,CMYK ���T�|�[�g�B
 *
 * @section environment �����
 *
 * - gcc �ł̃r���h�ƁASolaris9 �� cygwin �ł̓�����m�F���Ă��܂��B
 * - Visual C++ 2005 �ł̃r���h�� WindowsXP �ł̓�����m�F���Ă��܂��B
 *
 * @section usage �g�p���@
 *
 * - ��{@n
 * @n
 * % imgdiff A.tif B.tif@n
 * @n
 * diff �����������s�N�Z�������o�͂���܂��B�T�C�Y������Ă��Ă���r�ł��܂��B
 *
 * - diff ���W���_���v@n
 * @n
 * % imgdiff -v A.tif B.tif@n
 *
 * - diff �摜���쐬@n
 * @n
 * % imgdiff A.tif B.tif diff.tif@n
 * @n
 * ��R�������w�肷��ƁA������ tiff �摜�Ƃ��ďo�͂��܂��B@n
 * ���� diff �Ȃ��A���� diff ����̃s�N�Z���ł��B@n
 * @n
 * �������Ȃ��ƁAtiff �����܂���B���� diff.tif �����݂��Ă���ƁA@n
 * �����̗L���Ɋւ�炸�폜����܂��B@n
 *
 * - �I�t�Z�b�g���w�肵����r@n
 * @n
 * % imgdiff -x 10 -y -30 A.tif B.tif@n
 * @n
 * �I�t�Z�b�g���w�肵����r���o���܂��B@n
 * �K�͉摜(��L�̏ꍇ A.tif)����Ƃ����l���w�肵�ĉ������B@n
 *
 * - �w���v@n
 * @n
 * % imgdiff -h@n
 *
 * @section return �߂�l
 *
 * imgdiff �͈ȉ��̖߂�l��Ԃ��܂��B
 *
 * - 0: ����I����diff�Ȃ�
 * - 1: ����I����diff����
 * - ����ȊO: �ُ�I��
 *
 * @section build �r���h���@
 *
 * - gcc(Solaris9|cygwin|Linux) �̏ꍇ@n
 * @n
 * make ���邾���ł��B@n
 * @n
 * % make@n
 * @n
 * �f�o�b�O���ɂ� Makefile �̈ȉ��̉ӏ����R�����g�A�E�g���āAmake ���ĉ������B@n
 * @n
 * # For Release@n
 * CFLAGS   = -D$(ENDIAN) -D$(JPEG) -O3 -Wall -Wextra@n
 * LDFLAGS  = -s@n
 *
 * - Visual C++ 2005 �̏ꍇ@n
 * @n
 * imgdiff.sln �������p���������B@n
 * �����[�X���ɂ� Release �r���h�����ĉ������B@n
 *
 * @section library ���C�u����
 *
 * ���̃\�t�g�E�F�A�͈ȉ��̃��C�u�������g�p���Ă��܂��B@n
 *
 * - libtiff@n
 * Copyright (c) 1988-1997 Sam Leffler@n
 * Copyright (c) 1991-1997 Silicon Graphics, Inc.@n
 *
 * - libjpeg@n
 * This software is copyright (C) 1991-1998, Thomas G. Lane.@n
 * All Rights Reserved except as specified below.@n
 *
 * @section warning ���ӎ���
 *
 * Windows �œ��삳���邽�߂ɂ� Windows �ł� libtiff ���K�v�ł��B@n
 * v3.8.2 �œ���m�F���Ă��܂��B@n
 * @n
 * http://gnuwin32.sourceforge.net/packages/tiff.htm
 *
 * @section todo TODO
 *
 * - Windows �ł� Jpeg ��������悤�ɂ���B
 * - Windows �ł� libtiff �� libjpeg ��ÓI�����N������B
 * - id_jpeg.c �́Ajpeg_(open|read|close) �֐�������Đ������������ǂ��ȁB
 * - 1,2,4bpp ppm �摜�ɂ��Ή�����B@n
 * - id_diff.c �̈ȉ��̊֐�����������B@n
 *   id_diff_Diff1bpp1channel()@n
 *   id_diff_Diff1bpp3channel()@n
 *   id_diff_Diff2bpp1channel()@n
 *   id_diff_Diff2bpp3channel()@n
 *   id_diff_Diff4bpp1channel()@n
 *   id_diff_Diff4bpp3channel()@n
 * - diff �摜�� *.tif �ȊO���w��ł���悤�ɂ��Ă��ʔ�����������Ȃ��B
 * - �S�s�N�Z�����������ɓW�J���Ĕ�r���铮��ł��邽�߁A����ȉ摜�̏ꍇ@n
 *   �� memory alloc error �ɂȂ邱�Ƃ�����B������邽�߂ɂ́A�Ⴆ�΃X�L��@n
 *   �����C���P�ʂŔ�r����K�v������B
 *
 * @section go �������� main() �ɔ��ł݂܂��傤�I�I
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
