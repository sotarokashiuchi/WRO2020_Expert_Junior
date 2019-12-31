/* WRO2019 */
#include "ev3api.h"
#include "app.h"

#if defined(BUILD_MODULE)
#include "module_cfg.h"
#else
#include "kernel_cfg.h"
#endif

/* 定義設定 */
#define R_MOTOR         EV3_PORT_C      //右+
#define L_MOTOR         EV3_PORT_B      //左-
#define COLO_SENSOR     EV3_PORT_3
#define BLACK           10              //黒　反射光
#define WHITE           90              //白　反射光


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


	tslp_tsk(500);
	while(false==ev3_button_is_pressed(ENTER_BUTTON));
	ev3_led_set_color(LED_OFF);
	/* タスクの開始 */
	WRO();

	return 0;
}


int WRO(void) {
	/* 変数定義 */
	int conditions;
	
	/* 本プログラム開始 */
	conditions = 1;
	while(conditions){
		linetrace(100, 1, 1);
	}

	return 0;
}

	
int linetrace(int power, int p_gain, int d_gain){
	/* ライントレース */
    int threshold = (BLACK+WHITE)/2;        //閾値
    uint8_t reflect;            			//反射光

    reflect = ev3_color_sensor_get_reflect(COLO_SENSOR);
    if(reflect<=threshold){
        ev3_motor_set_power(R_MOTOR, power-(threshold-reflect)*p_gain);
    }else{
        ev3_motor_set_power(L_MOTOR, -power+(reflect-threshold)*p_gain);
        ev3_motor_set_power(R_MOTOR, power);
    }

	return 0;
}

