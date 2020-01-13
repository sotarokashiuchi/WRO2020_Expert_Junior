
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
int linetrace(int power, float p_gain, float d_gain);			//ライントレース
int return_value(double, ER, char[20]);	//エラー表示
void A_arm_up(double, int);				//Aアーム上げる
void A_arm_down(double);				//Aアーム下げる
void L_rotation(double, int);			//左回転関数
void R_rotation(double, int);			//右回転関数
void straight(double, int);				//直進ゆっくり止まる
void straight_b(double, int);				//直進ゆっくり止まる


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
	ev3_sensor_config(HIT_C_4, HT_NXT_COLOR_SENSOR);

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
	int cokor;

	int reflected_light;    //反射光元値
	int object_1;
	int object_2;
	int object_3;
	int object_4;
	int object_5;
	int object_6;
	int object_7;
	int object_8;
	char char_xx[50];
	int bll;
	bool_t x;
	rgb_raw_t rgb_val;



	fp = ev3_serial_open_file(EV3_SERIAL_BT);

	fprintf(fp, "\n\n\n\n\n\n\n\n\n\n\n000.0 待機\r\n");
	/* 実験スペース */
	// tslp_tsk(500);
	// while(false==ev3_button_is_pressed(ENTER_BUTTON));


	// L_rotation(8, 220);


	// while(1){
	// 	linetrace(45, 0.4, 0.2);
	// }

	// fprintf(fp, "ライントレース開始\r\n");
	// while(1){
	// 	linetrace(85, 0.6, 0.3);
	// }

	// BRAKE(B_MOTOR);
	// BRAKE(C_MOTOR);
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
	reflected_light = ev3_color_sensor_get_reflect(COLOR_3);
	r1 = ev3_motor_reset_counts(B_MOTOR);
	ev3_motor_reset_counts(C_MOTOR);
	ev3_motor_reset_counts(A_ARM);
	ev3_motor_reset_counts(D_ARM);
	return_value(1, r1, "角度リセット");

	r1 = ev3_motor_rotate(A_ARM, -180, 55, false);
	return_value(2, r1, "Aアーム準備");

	ev3_motor_set_power(B_MOTOR, -85);
	ev3_motor_set_power(C_MOTOR, 85);
	return_value(3, r, "直進");
	while(-380 <= ev3_motor_get_counts(B_MOTOR));
	BRAKE(B_MOTOR);
	while(1000 >= ev3_motor_get_counts(C_MOTOR));
	BRAKE(C_MOTOR);
	return_value(4, r, "90度回転");

	ev3_motor_reset_counts(A_ARM);
	return_value(5, r1, "Aアームリセット");

	ev3_motor_rotate(B_MOTOR, -780, 55 ,false);
	r1 = ev3_motor_rotate(C_MOTOR, 780,  55, true);
	return_value(6, r1, "直進");

	r1 = ev3_motor_steer(B_MOTOR, C_MOTOR, 85, -100);
	while(1!=ev3_color_sensor_get_color(COLOR_1));
	return_value(7, r1, "カラー黒");
	straight(8, 220);

	L_rotation(9, 220);

	return_value(8, r, "ライントレース開始");
	ev3_motor_reset_counts(C_MOTOR);
	while(180>=ev3_motor_get_counts(C_MOTOR)){
		linetrace(45, 0.4, 0.2);
	}
	do{
		linetrace(45, 0.4, 0.2);
		tslp_tsk(7);
		x = ht_nxt_color_sensor_measure_rgb(HIT_C_4, &rgb_val);
		if(x!=true){
			fprintf(fp, " エラー\r\n");
		}else if(x!=false){
			fprintf(fp, " 正常\r\n");
		}else{
			fprintf(fp, " ?\r\n");
		}
	}while(50<=rgb_val.b);
	BRAKE(B_MOTOR);
	BRAKE(C_MOTOR);
	return_value(9, r, "ライントレース終了");

	/* オブジェクト有無確認 */
	if((reflected_light+10)<=ev3_color_sensor_get_reflect(COLOR_3)){
		object_1 = 1;			//真
		ev3_speaker_play_tone(1046.50, 100);
	}else{
		object_1 = 0;			//偽
	}
	return_value(10, r, "色読み取り");
	A_arm_up(10,0);

	// ev3_motor_reset_counts(C_MOTOR);
	// r1 = ev3_motor_steer(B_MOTOR, C_MOTOR, 85, 100);
	// while(-210<=ev3_motor_get_counts(C_MOTOR));
	// BRAKE(B_MOTOR);
	// BRAKE(C_MOTOR);

	/* 下がる */
	straight_b(11, 210);

	ev3_motor_reset_counts(C_MOTOR);
	ev3_motor_steer(B_MOTOR, C_MOTOR, 45, 0);
	while(260>=ev3_motor_get_counts(C_MOTOR));
	BRAKE(B_MOTOR);
	BRAKE(C_MOTOR);

	r1 = ev3_motor_steer(B_MOTOR, C_MOTOR, 85, -100);
	while(1!=ev3_color_sensor_get_color(COLOR_1));
	return_value(7, r1, "カラー黒");
	straight(8, 220);

	R_rotation(0, 220);

	return 0;
}
















/* ライントレース */
int linetrace(int power, float p_gain, float d_gain){
    static int reflect;            			//反射光//静的なローカル変数
	static int gap;

    reflect = (ev3_color_sensor_get_reflect(COLOR_1))-(ev3_color_sensor_get_reflect(COLOR_2));
    if(reflect<=0){
        ev3_motor_set_power(C_MOTOR, power+((reflect)*p_gain)+((reflect-(gap))*d_gain));
        ev3_motor_set_power(B_MOTOR, (-power));
    }else{
        ev3_motor_set_power(B_MOTOR, -power+((reflect)*p_gain)+((-(reflect-gap))*d_gain));
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

/* 左回転関数 */
void L_rotation(double no_x, int angle){
	int total = 0;

	ev3_motor_reset_counts(C_MOTOR);
	ev3_motor_steer(B_MOTOR, C_MOTOR, 55, 0);
	while(angle>=ev3_motor_get_counts(C_MOTOR));
	ev3_motor_steer(B_MOTOR, C_MOTOR, 40, 0);
	while(20<=ev3_color_sensor_get_reflect(COLOR_2));
	BRAKE(B_MOTOR);
	BRAKE(C_MOTOR);
	do{
		total = (ev3_color_sensor_get_reflect(COLOR_1))-(ev3_color_sensor_get_reflect(COLOR_2));
		if(5<=total){
			ev3_motor_steer(B_MOTOR, C_MOTOR, 3, 0);
		}
		if(-5>=total){
			ev3_motor_steer(B_MOTOR, C_MOTOR, -3, 0);
		}
	}while(-5>total || 5<total);
	BRAKE(B_MOTOR);
	BRAKE(C_MOTOR);
	return_value(no_x, r, "左回転関数");
}

/* 右回転関数 */
void R_rotation(double no_x, int angle){
	int total = 0;

	ev3_motor_reset_counts(C_MOTOR);
	ev3_motor_steer(B_MOTOR, C_MOTOR, -55, 0);
	while((-angle)<=ev3_motor_get_counts(C_MOTOR));
	ev3_motor_steer(B_MOTOR, C_MOTOR, -40, 0);
	while(20<=ev3_color_sensor_get_reflect(COLOR_1));
	BRAKE(B_MOTOR);
	BRAKE(C_MOTOR);
	do{
		total = (ev3_color_sensor_get_reflect(COLOR_1))-(ev3_color_sensor_get_reflect(COLOR_2));
		if(5<=total){
			ev3_motor_steer(B_MOTOR, C_MOTOR, -3, 0);
		}
		if(-5>=total){
			ev3_motor_steer(B_MOTOR, C_MOTOR, 3, 0);
		}
	}while(-5>total || 5<total);
	BRAKE(B_MOTOR);
	BRAKE(C_MOTOR);
	return_value(no_x, r, "右回転関数");
}

/* ほぼ台形制御 */
void straight(double no_x, int angle){
	angle -=120;
	ev3_motor_reset_counts(B_MOTOR);

	ev3_motor_steer(B_MOTOR, C_MOTOR, 85, -100);
	while((-angle) <= ev3_motor_get_counts(B_MOTOR));


	ev3_motor_steer(B_MOTOR, C_MOTOR, 60, -100);
	while((-(30+angle)) <= ev3_motor_get_counts(B_MOTOR));


	ev3_motor_steer(B_MOTOR, C_MOTOR, 30, -100);
	while((-(100+angle)) <= ev3_motor_get_counts(B_MOTOR));

	ev3_motor_steer(B_MOTOR, C_MOTOR, 20, -100);
	while((-(120+angle)) <= ev3_motor_get_counts(B_MOTOR));


	BRAKE(B_MOTOR);
	BRAKE(C_MOTOR);
	return_value(no_x, r, "直進ゆっくり止まる関数");
}


/* ほぼ台形制御下がる */
void straight_b(double no_x, int angle){
	angle -=120;
	ev3_motor_reset_counts(B_MOTOR);

	ev3_motor_steer(B_MOTOR, C_MOTOR, 85, 100);
	while((angle) >= ev3_motor_get_counts(B_MOTOR));


	ev3_motor_steer(B_MOTOR, C_MOTOR, 60, 100);
	while(((30+angle)) >= ev3_motor_get_counts(B_MOTOR));


	ev3_motor_steer(B_MOTOR, C_MOTOR, 30, 100);
	while(((100+angle)) >= ev3_motor_get_counts(B_MOTOR));

	ev3_motor_steer(B_MOTOR, C_MOTOR, 20, 100);
	while(((120+angle)) >= ev3_motor_get_counts(B_MOTOR));


	BRAKE(B_MOTOR);
	BRAKE(C_MOTOR);
	return_value(no_x, r, "バックゆっくり止まる関数");
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
