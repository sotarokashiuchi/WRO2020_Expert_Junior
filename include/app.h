/*
 *		WRO2020ヘッダーファイル
 */

/* 定義設定 */
#define A_ARM           EV3_PORT_A      //雪アーム
#define B_MOTOR         EV3_PORT_B      //右
#define C_MOTOR         EV3_PORT_C		//左
#define D_MOTOR			EV3_PORT_D      //雪,研磨剤出口＆車回収
#define COLOR_1	     	EV3_PORT_1      //中央右
#define COLOR_2	     	EV3_PORT_2      //中央左
#define HT_COLOR_3	    EV3_PORT_3      //右
#define GYRO_4	     	EV3_PORT_4      //ジャイロセンサー
#define WHITE_REFLECTED 50              //純カラーセンサー白反射光
#define WHITE_RGB       450             //純カラーセンサー白RGB和
#define HT_WHITE_RGB    110             //HTカラーセンサー白RGB和
#define BRAKE_REFLECTED 6               //純カラーセンサー黒反射光
#define BRAKE_RGB       45              //純カラーセンサー黒RGB和
#define HT_BRAKE_RGB    12              //HTカラーセンサー黒RGB和
#define YELLOW_RGB      320             //純カラーセンサー黄RGB和
#define HT_YELLOW_RGB   
#define RED_RGB         180             //純カラーセンサー赤RGB和
#define HT_RED_RGB      35              //HTカラーセンサー赤RGB和
#define A_ARM_C         170              //a_armの中央値


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
extern void gyro_log(void);
#endif /* TOPPERS_MACRO_ONLY */
