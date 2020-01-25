/*
 *		サンプルプログラム(00_sample_tone)のヘッダファイル
 */

/*
 *  ターゲット依存の定義
 */
#include "target_test.h"

/*
 *  ターゲットに依存する可能性のある定数の定義
 */

#ifndef STACK_SIZE
#define	STACK_SIZE		4096		/* タスクのスタックサイズ */
#endif /* STACK_SIZE */


/*
 *  関数のプロトタイプ宣言
 */
#ifndef TOPPERS_MACRO_ONLY
extern int	main_task(void);
#endif /* TOPPERS_MACRO_ONLY */
