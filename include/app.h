/*
 *		サンプルプログラム(00_sample_tone)のヘッダファイル
 */

/* 定義設定 */
#define C_MOTOR         EV3_PORT_C      //右+
#define B_MOTOR         EV3_PORT_B      //左-
#define A_ARM			EV3_PORT_A		//-上
#define D_ARM			EV3_PORT_D
#define COLOR_1	     	EV3_PORT_1
#define COLOR_2	     	EV3_PORT_2
#define COLOR_3	     	EV3_PORT_3
#define COLOR_4	     	EV3_PORT_4
#define BRAKKU			20			//反射光黒
#define TAIYA_W_2       340  			//二つのタイヤの幅直径(mm)

#define TAIYA			56			//タイヤ直径(mm)
#define TAIYA_EN 175.9291884				//タイヤの円周

#define EN_RITU			3.14159265	//円周率
#define BRAKE(p)		ev3_motor_stop(p,true)

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
/* app.c */
extern int	main_task(void);
/* kashiuchi.c */
void a(void);
#endif /* TOPPERS_MACRO_ONLY */
