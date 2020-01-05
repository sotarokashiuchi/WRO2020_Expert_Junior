/* WRO2019 */
#include "ev3api.h"
#include "app.h"
#include <stdio.h>
#include <stdlib.h>

#if defined(BUILD_MODULE)
#include "module_cfg.h"
#else
#include "kernel_cfg.h"
#endif

/* 定義設定 */
#define C_MOTOR         EV3_PORT_C      //右+
#define B_MOTOR         EV3_PORT_B      //左-
#define A_ARM			EV3_PORT_A		//-上
#define D_ARM			EV3_PORT_D
#define COLOR_1	     	EV3_PORT_1
#define COLOR_2	     	EV3_PORT_2
#define COLOR_3	     	EV3_PORT_3
#define HIT_C_4	     	EV3_PORT_4
#define THRESHOLD		50				//閾値
#define BRAKE(p)		ev3_motor_stop(p,true)

/* 関数のプロトタイプ宣言 */
int WRO(void);
int linetrace(int,int,int);			//ライントレース
int return_value(double, ER, char[20]);	//エラー表示
void A_arm_up(double,int);				//Aアーム上げる
void A_arm_down(double);				//Aアーム下げる

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
	ev3_sensor_config(HIT_C_4, HT_NXT_ACCEL_SENSOR);

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
	

	fp = ev3_serial_open_file(EV3_SERIAL_BT);

	fprintf(fp, "\n\n\n\n\n\n\n\n\n\n\n000.0 待機\r\n");
	/* 実験スペース */
	// r1 = ev3_motor_rotate(A_ARM, -180, 55, false);
	// return_value(002, r1, "Aアーム準備");
	// ev3_motor_reset_counts(A_ARM);
	// return_value(005, r1, "Aアームリセット");
	// tslp_tsk(1000);
	// A_arm_up(004,0);
	// tslp_tsk(1000);
	// A_arm_down(006);

	/* 待機 */
	tslp_tsk(500);
	while(false==ev3_button_is_pressed(ENTER_BUTTON));
	
	

	/* 本プログラム開始 */
	r1 = ev3_motor_reset_counts(B_MOTOR);
	ev3_motor_reset_counts(C_MOTOR);
	ev3_motor_reset_counts(A_ARM);
	ev3_motor_reset_counts(D_ARM);
	return_value(001, r1, "角度リセット");

	r1 = ev3_motor_rotate(A_ARM, -180, 55, false);
	return_value(002, r1, "Aアーム準備");

	ev3_motor_set_power(B_MOTOR, -85);
	ev3_motor_set_power(C_MOTOR, 85);
	return_value(003, r, "直進");
	while(-360 <= ev3_motor_get_counts(B_MOTOR));
	BRAKE(B_MOTOR);
	while(600 >= ev3_motor_get_counts(C_MOTOR));
	BRAKE(C_MOTOR);
	return_value(004, r, "90度回転");
	
	ev3_motor_reset_counts(A_ARM);
	return_value(005, r1, "Aアームリセット");

	fprintf(fp, "ライントレース開始\r\n");
	conditions = 1;
	while(conditions<=100000){
		linetrace(100, 1, 1);
		conditions++;
	}
	BRAKE(B_MOTOR);
	BRAKE(C_MOTOR);
	fprintf(fp, "ライントレース終了\r\n");
	return 0;
}

	
int linetrace(int power, int p_gain, int d_gain){
	/* ライントレース */
    static int reflect;            			//反射光//静的なローカル変数
	static int gap;

    	reflect = (ev3_color_sensor_get_reflect(COLOR_1))-(ev3_color_sensor_get_reflect(COLOR_2));
        // sprintf(char_color, "%3d", reflect);
        // ev3_lcd_draw_string(char_color, 0, 80);
        if(reflect<=0){
            ev3_motor_set_power(C_MOTOR, power+((reflect)*p_gain)+(reflect-(gap)));
            ev3_motor_set_power(B_MOTOR, (-power));
        }else{
            ev3_motor_set_power(B_MOTOR, -power+((reflect)*p_gain)+(-(reflect-gap)));
            ev3_motor_set_power(C_MOTOR, power);
        }
        gap = reflect;

	return 0;
}

/* Aアーム上げる関数 */
void A_arm_up(double no_x,int branch){
	branch = 0;
	r1 = ev3_motor_rotate(A_ARM, -850, 85, false);
	return_value(no_x, r1, "A_armアーム上げる関数");
}

/* Aアーム下げる関数 */
void A_arm_down(double no_x){
	int degrees=abs(ev3_motor_get_counts(A_ARM));
	r1 = ev3_motor_rotate(A_ARM, degrees, 35, false);
	return_value(no_x, r1, "A_armアーム下げる関数");
}
















/* エラー表示関数 */
int return_value(double no,ER return_function, char display[20]){
	if(return_function==E_OK){
		fprintf(fp, "%05.1f 正常終了_%s\r\n", no, display);
		return 1;
	}else{
		fprintf(fp, "\n\n%05.1f error_%s\r\n", no, display);
		if(return_function==E_PAR){
			fprintf(fp, "  E_PAR...");
			fprintf(fp, "パス名が無効です。\r\n");
			fprintf(fp, "	不正のモーター、センサータイプ\r\n");
			fprintf(fp, "   不正の設定値\r\n");
			fprintf(fp, "	メモリファイルは無効\r\n");
			fprintf(fp, "	画像のオブジェクトは無効\r\n");
			fprintf(fp, "	p_image　はNULL\r\n");

		}else if(return_function==E_ID){
			fprintf(fp, "  E_ID...");
			fprintf(fp, "不正のボタン番号\r\n");
			fprintf(fp, "	不正ID番号\r\n");
			fprintf(fp, "	不正のモーター、センサポート番号\r\n");

		}else if(return_function==E_SYS){
			fprintf(fp, "  E_SYS...");
			fprintf(fp, "I/Oエラーが発生した(SDカード不良の可能性が高い\r\n");

		}else if(return_function==E_NOMEM){
			fprintf(fp, "  E_NOMEM...");
			fprintf(fp, "メモリ不足\r\n");

		}else if(return_function==E_CTX){
			fprintf(fp, "  E_CTX...");
			fprintf(fp, "非タスクコンテストから呼び出し\r\n");

		}else if(return_function==E_MACV){
			fprintf(fp, "  E_MACV...");
			fprintf(fp, "メモリファイルアクセス違反(p_memfile)(path)\r\n");

		}else if(return_function==E_OBJ){
			fprintf(fp, "  E_OBJ...");
			fprintf(fp, "	モータ未接続\r\n");
			fprintf(fp, "	画像ファイルは破損\r\n");
			fprintf(fp, "	(p_memfile)で指定したメモリファイルは無効\r\n");

		}else if(return_function==E_NOSPT){
			fprintf(fp, "  E_NOSPT...");
			fprintf(fp, "これ以上読み込めるファイルの情報がない\r\n");

		}else if(return_function==E_NORES){
			fprintf(fp, "  E_NORES...");
			fprintf(fp, "サウンドデバイスが占有せれている\r\n");
			
		}else{
			fprintf(fp, "エラーの詳細が分かりません\r\n");
		}
	return 0;
	}
}


