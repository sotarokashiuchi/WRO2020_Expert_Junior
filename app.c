/* WRO2019 */
#include "ev3api.h"
#include "app.h"
#include <stdio.h>

#if defined(BUILD_MODULE)
#include "module_cfg.h"
#else
#include "kernel_cfg.h"
#endif

/* 定義設定 */
#define R_MOTOR         EV3_PORT_C      //右+
#define L_MOTOR         EV3_PORT_B      //左-
#define COLO_SENSOR     EV3_PORT_3
#define THRESHOLD		50				//閾値
#define BRAKE(p)		ev3_motor_stop(p,true)

/* 関数のプロトタイプ宣言 */
int WRO(void);
int linetrace(int,int,int);			//ライントレース


int main_task(void) {
	/* プログラム準備設定 */
	ev3_motor_config(L_MOTOR, MEDIUM_MOTOR);
    ev3_motor_config(R_MOTOR, MEDIUM_MOTOR);
    ev3_sensor_config(COLO_SENSOR, COLOR_SENSOR);

	ev3_lcd_set_font(EV3_FONT_MEDIUM);
    ev3_lcd_draw_string("technology", 0, 0);
    ev3_lcd_draw_string("", 0, 15);
	ev3_speaker_set_volume(10);


	
	/* タスクの開始 */
	WRO();

	return 0;
}


int WRO(void) {
	/* 変数定義 */
	int conditions;			//条件
	FILE *fp = NULL;				//ファイルポインタ

	fp = ev3_serial_open_file(EV3_SERIAL_BT);

	/* 待機 */
	tslp_tsk(500);
	while(false==ev3_button_is_pressed(ENTER_BUTTON));
	fprintf(fp, "\n\n\n\n\n\n\n\n\n\n\n000スタート\r\n");
	

	/* 本プログラム開始 */
	fprintf(fp, "001ライントレース開始\r\n");
	conditions = 1;
	while(conditions<=100000){
		linetrace(100, 1, 1);
		conditions++;
	}
	BRAKE(R_MOTOR);
	BRAKE(L_MOTOR);
	fprintf(fp, "002ライントレース終了\r\n");
	return 0;
}

	
int linetrace(int power, int p_gain, int d_gain){
	/* ライントレース */
    static uint8_t reflect;            			//反射光//静的なローカル変数

    reflect = ev3_color_sensor_get_reflect(COLO_SENSOR);
    if(reflect<=THRESHOLD){
        ev3_motor_set_power(R_MOTOR, power-(THRESHOLD-reflect)*p_gain);
		ev3_motor_set_power(L_MOTOR, (-power));
    }else{
        ev3_motor_set_power(L_MOTOR, -power+(reflect-THRESHOLD)*p_gain);
        ev3_motor_set_power(R_MOTOR, power);
    }

	return 0;
}

