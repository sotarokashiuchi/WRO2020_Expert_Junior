/* WRO2020 */
#include "ev3api.h"
#include "app.h"
#include "kashiuchi.h"
#include <stdio.h>
#include <math.h>
#include <stdlib.h>

#if defined(BUILD_MODULE)
#include "module_cfg.h"
#else
#include "kernel_cfg.h"
#endif

/* 定義設定 */
/*
#define A_ARM           EV3_PORT_A      //雪アーム
#define B_MOTOR         EV3_PORT_B      //右-
#define C_MOTOR         EV3_PORT_C		//左+
#define D_MOTOR			EV3_PORT_D      //雪,研磨剤出口＆車回収
#define COLOR_1	     	EV3_PORT_1      //中央右
#define COLOR_2	     	EV3_PORT_2      //中央左
#define HT_COLOR_3	    EV3_PORT_3      //右
#define GYRO_4	     	EV3_PORT_4      //ジャイロセンサー
:/

/* 関数のプロトタイプ宣言 */
int WRO(void);

/* グローバル変数宣言 */
FILE *fp = NULL;				//ファイルポインタ
ER r1;
ER r = E_OK;


int main_task(void) {

	/* タスクの開始 */
	fp = ev3_serial_open_file(EV3_SERIAL_BT);
	WRO();

	return 0;
}


int WRO(void) {
	bluetooth_kashiuchi_fp();
	

	/* コンフィング宣言 */
	ev3_sensor_config(COLOR_1, COLOR_SENSOR);
	ev3_sensor_config(COLOR_2, COLOR_SENSOR);
	ev3_sensor_config(HT_COLOR_3, HT_NXT_COLOR_SENSOR);
	ev3_sensor_config(GYRO_4, GYRO_SENSOR);

	ev3_motor_config(A_ARM, MEDIUM_MOTOR);
	ev3_motor_config(B_MOTOR, MEDIUM_MOTOR);
	ev3_motor_config(C_MOTOR, MEDIUM_MOTOR);
	ev3_motor_config(D_MOTOR, MEDIUM_MOTOR);

	/* 変数 */
	rgb_raw_t val_1;
	int i=0;

	/* 待機、準備 */
	tslp_tsk(2500);
	// while(false==ev3_button_is_pressed(ENTER_BUTTON));
	return_value(8, r, "OK");
	// while('0' != fgetc(fp));
	// while(1){
	// 	fprintf(fp, "%d\r",ev3_color_sensor_get_reflect(COLOR_1));
	// }
	
	/* purpguramu */
	ev3_motor_reset_counts(B_MOTOR);
	ev3_motor_reset_counts(C_MOTOR);
	ev3_motor_reset_counts(A_ARM);
	ev3_motor_reset_counts(D_MOTOR);

	ev3_motor_set_power(B_MOTOR, -30);
	ev3_motor_set_power(C_MOTOR, 30);
	while(200>=ev3_motor_get_counts(C_MOTOR));
	while(40<=ev3_color_sensor_get_reflect(COLOR_1));
	ev3_motor_reset_counts(C_MOTOR);
	while(35>=ev3_motor_get_counts(C_MOTOR));
	ht_nxt_color_sensor_measure_rgb(HT_COLOR_3, &val_1);
	if(30<=(val_1.r + val_1.g + val_1.b)){
		tone();
	}
	ev3_motor_reset_counts(C_MOTOR);
	for(i=0; i<=7; i++){
		while(66>=ev3_motor_get_counts(C_MOTOR));
		ht_nxt_color_sensor_measure_rgb(HT_COLOR_3, &val_1);
		if(30<=(val_1.r + val_1.g + val_1.b)){
			tone();
		}
		ev3_motor_reset_counts(C_MOTOR);
	}
	

	
	BRAKE(B_MOTOR);
	BRAKE(C_MOTOR);
	

//35 60
	// ev3_sta_cyc(LINETRACE_TASK);

	// ev3_motor_set_power(D_MOTOR, 85);
	// tslp_tsk(2000);
	// BRAKE(D_MOTOR);

	// ev3_motor_set_power(A_ARM, -10);
	// tslp_tsk(800);
	// BRAKE(A_ARM);

	// while(360>=ev3_gyro_sensor_get_angle(GYRO_4)){
	// 	ev3_motor_set_power(B_MOTOR, 55);
	// 	ev3_motor_set_power(C_MOTOR, 55);
	// }
	// while(380>=ev3_gyro_sensor_get_angle(GYRO_4)){
	// 	ev3_motor_set_power(B_MOTOR, 20);
	// 	ev3_motor_set_power(C_MOTOR, 20);
	// }
	// BRAKE(B_MOTOR);
	// BRAKE(C_MOTOR);

	// while(0<=ev3_gyro_sensor_get_angle(GYRO_4)){
	// 	ev3_motor_set_power(B_MOTOR, -55);
	// 	ev3_motor_set_power(C_MOTOR, -55);
	// }
	// while(0<=ev3_gyro_sensor_get_angle(GYRO_4)){
	// 	ev3_motor_set_power(B_MOTOR, -20);
	// 	ev3_motor_set_power(C_MOTOR, -20);
	// }
	// BRAKE(B_MOTOR);
	// BRAKE(C_MOTOR);

	// ev3_motor_set_power(D_MOTOR, -30);
	// tslp_tsk(2000);
	// BRAKE(D_MOTOR);

	// ev3_motor_rotate(B_MOTOR, -360, 50, false);
	// ev3_motor_rotate(C_MOTOR,  360, 50, true);

	// ev3_motor_set_power(D_MOTOR, 30);
	// tslp_tsk(2000);
	// BRAKE(D_MOTOR);

	

	// ev3_motor_set_power(D_MOTOR, -30);
	// tslp_tsk(2000);
	// BRAKE(D_MOTOR);

	// ev3_motor_set_power(D_MOTOR, 30);
	// tslp_tsk(2000);
	// BRAKE(D_MOTOR);

	// ev3_motor_reset_counts(B_MOTOR);
	// ev3_motor_reset_counts(C_MOTOR);
	// while(1){
	// 	ev3_motor_rotate(B_MOTOR, -15, 50, false);
	// 	ev3_motor_rotate(C_MOTOR,  15, 50, true);
	// 	ev3_motor_rotate(B_MOTOR,  15, 50, false);
	// 	ev3_motor_rotate(C_MOTOR, -15, 50, true);
	// }



	return 0;

}
