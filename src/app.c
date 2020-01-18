/* WRO2019 */
#include "ev3api.h"
#include "app.h"
#include <stdio.h>
#include <math.h>
#include <stdlib.h>

#if defined(BUILD_MODULE)
#include "module_cfg.h"
#else
#include "kernel_cfg.h"
#endif



/* 関数のプロトタイプ宣言 */
int WRO(void);

/* グローバル変数宣言 */
FILE *fp = NULL;				//ファイルポインタ
ER r1;
ER r = E_OK;


int main_task(void) {
	/* プログラム準備設定 */
	ev3_motor_config(B_MOTOR, MEDIUM_MOTOR);
    ev3_motor_config(C_MOTOR, MEDIUM_MOTOR);
	ev3_motor_config(A_ARM	, MEDIUM_MOTOR);
	ev3_motor_config(D_ARM	, MEDIUM_MOTOR);
    ev3_sensor_config(COLOR_1, COLOR_SENSOR);
	ev3_sensor_config(COLOR_2, COLOR_SENSOR);
	ev3_sensor_config(COLOR_3, COLOR_SENSOR);
	ev3_sensor_config(COLOR_4, COLOR_SENSOR);

	ev3_lcd_set_font(EV3_FONT_MEDIUM);
    ev3_lcd_draw_string("technology", 0, 0);
    ev3_lcd_draw_string("", 0, 15);
	ev3_speaker_set_volume(10);


	
	/* タスクの開始 */
	WRO();

	return 0;
}


int WRO(void) {
	tslp_tsk(500);
	while(false==ev3_button_is_pressed(ENTER_BUTTON));
	tslp_tsk(500);
	/* 変数定義 */
	double a,b,x,distance;
	double angur = 0;
	double COLOR_W = 188;			//カラーセンサー幅
	int ifbunn = 0;

	FILE *fp = NULL;
	fp = ev3_serial_open_file(EV3_SERIAL_BT);


	/* 直進 */
	ev3_motor_steer(B_MOTOR, C_MOTOR, 20, -100);

	/* カラーセンサーからズレを計算 */
	do{
		fprintf(fp,"1\r\n");
		if(20>=ev3_color_sensor_get_reflect(COLOR_4)){
			ifbunn = 1;
			fprintf(fp,"2\r\n");
			ev3_motor_reset_counts(C_MOTOR);
			ev3_motor_steer(B_MOTOR, C_MOTOR, 20, -100);
			while(0==angur){
				fprintf(fp,"3\r\n");
				if(20>=ev3_color_sensor_get_reflect(COLOR_3)){
					fprintf(fp,"4\r\n");
					angur = ev3_motor_get_counts(C_MOTOR);
				}
			}
		}

		if(20>=ev3_color_sensor_get_reflect(COLOR_3)){
			if(ifbunn!=1){
				ifbunn = 2;
			}
			
			ev3_motor_reset_counts(C_MOTOR);
			ev3_motor_steer(B_MOTOR, C_MOTOR, 20, -100);
			while(0==angur){
				if(20>=ev3_color_sensor_get_reflect(COLOR_4)){
					angur = ev3_motor_get_counts(C_MOTOR);
				}
			}
		}
	}while(0>=angur);
	BRAKE(B_MOTOR);
	BRAKE(C_MOTOR);

	ev3_lcd_draw_string("OK", 0, 40);

	// /* 進んだ距離を角度からmmに変換 */
	angur = TAIYA_EN*(angur/360);

	/* 角度xを求める(逆三角関数) */
	x = asin(angur/sqrt(COLOR_W*COLOR_W+angur*angur))*57.2957795785523;
    fprintf(fp, "%f\r\n",x);

	// /* 角度xから進む距離を求める (mm)から(角度)*/
	distance = (TAIYA_W_2 *(x/360)*EN_RITU)/TAIYA_EN*360;

	fprintf(fp, "%f\r\n",distance);

	if (ifbunn==1){
		fprintf(fp, "1\r\n");
		ev3_motor_rotate(B_MOTOR, -distance, 30, true);
	}else{
		fprintf(fp, "2\r\n");
		ev3_motor_rotate(C_MOTOR, distance, 30, true);
	}
	return 0;
	
}


