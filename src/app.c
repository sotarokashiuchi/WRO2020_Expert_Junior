/* WRO2020 */
#include "ev3api.h"
#include "app.h"
#include "kashiuchi.h"
#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>

#if defined(BUILD_MODULE)
#include "module_cfg.h"
#else
#include "kernel_cfg.h"
#endif

/* プログラムロジック */
//file:///D:\EV3\sdk\WRO2020\ロジック\WRO2020strategy_no1.xlsx

/* 定義設定 */
/*
#define A_ARM 0          EV3_PORT_A      //雪アーム
#define B_MOTOR         EV3_PORT_B      //右-
#define C_MOTOR         EV3_PORT_C		//左+
#define D_MOTOR			EV3_PORT_D      //雪,研磨剤出口＆車回収
#define COLOR_1	     	EV3_PORT_1      //中央右
#define COLOR_2	     	EV3_PORT_2      //中央左
#define HT_COLOR_3	    EV3_PORT_3      //右
#define GYRO_4	     	EV3_PORT_4      //ジャイロセンサー
false					0
true					1
:/

/* 関数のプロトタイプ宣言 */
int WRO(void);
int broken_line(int tire_angul,int tire_brake);
int collection_red_3_to_1(void);
int collection_yellow_3_to_1(void);
int collection_blue_3_to_1(void);
int collection_green_3_to_1(void);
int collection_red_1_to_3(void);
int put_red_1_to_3(int);
int put_red_3_to_1(int);
int put_yellow_1_to_3(int);
int put_yellow_3_to_1(int);
int put_blue_1_to_3(int);
int put_blue_3_to_1(int);
int put_green_1_to_3(int);
int put_green_3_to_1(int);
int snow_car_put(int color_direction);
//float *array_command(float *str_p);	//配列操作


/* グローバル変数宣言 */
FILE *fp = NULL;				//ファイルポインタ
FILE *fp2 = NULL;
ER r1;
ER r = E_OK;
int gyro_angle_standard = 0;

int main_task(void) {

	/* タスクの開始 */
	fp = ev3_serial_open_file(EV3_SERIAL_BT);
	fp2 = fopen("/ev3rt/document/angul0405.xlsk","a+");
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

	ev3_speaker_set_volume(100);

	/* 変数 */
	rgb_raw_t val_1, val_2;
	int i=0, j=0, x=0;
	int binary_code[4][2] = {{0}};
	int sta_point_no[2] = {0};
	int road_priority = 0;
	int abrasive_priority = 0;
	int sta_point = 0;
	int max_1 = 0, max_2 = 0;
	char str[64] = {};
	
	//青0　緑1　黄2　赤3

	/* 待機、準備 */
	tone_line();
	fprintf(fp,"%d\n\r",ev3_battery_current_mA());
	fprintf(fp,"%d\n\r",ev3_battery_voltage_mV());
	tslp_tsk(5000);

	ev3_motor_reset_counts(B_MOTOR);
	ev3_motor_reset_counts(C_MOTOR);
	ev3_motor_reset_counts(A_ARM);
	ev3_motor_reset_counts(D_MOTOR);

	/* 
	 *	実験スペース
	 */
	/* purpguramu */
	
	// gyrotrace_task_4_power_p_i_d_angle(100, 2, 0, 0, gyro_angle_standard);
	// ev3_sta_cyc(GYROTRACE_TASK_4);
	// tslp_tsk(240000);
	// ev3_stp_cyc(GYROTRACE_TASK_4);
	// perfect_BRAKE();
	// while(1);
	// ev3_motor_set_power(C_MOTOR,-30);
	// while(-10 <= ev3_motor_get_counts(C_MOTOR))

	// ev3_motor_set_power(B_MOTOR,-30);
    // ev3_motor_set_power(C_MOTOR, 30);
	// tslp_tsk(10000);
	// perfect_BRAKE();

	// ev3_motor_set_power(B_MOTOR,-10);
    // ev3_motor_set_power(C_MOTOR,-10);
    // while(-360<=ev3_gyro_sensor_get_angle(GYRO_4));
	// perfect_BRAKE();

	// rotation(-360, gyro_angle_standard);
	
	// linetrace_task_4_power_p_i_d(15, 0.35, 0.56, 0.06);

	// deceleration(,0);

	/********************************************************************************************************************************************
	 *	バイナリコード色読み
	 ********************************************************************************************************************************************/
	strcpy(str,"strt");
	ev3_lcd_draw_string(str,0,0);
	gyro_angle_standard = 0;
	d_motor_car_close();
	ev3_motor_reset_counts(D_MOTOR);		//Dモータリセット
	d_motor_car_open(1);

	gyro_angle_standard = wall_fix(500);

	/* 直進 */
	gyrotrace_task_4_power_p_i_d_angle(20, 2, 0, 0.5, gyro_angle_standard);
	ev3_sta_cyc(GYROTRACE_TASK_4);
	while(400>=ev3_motor_get_counts(C_MOTOR));

	/* ライン検知 */
	do{
		ev3_color_sensor_get_rgb_raw(COLOR_1, &val_1);
	}while(50<=(val_1.b));
	tone_line();
	ev3_motor_reset_counts(C_MOTOR);
	sta_point_no[0] = val_1.r + val_1.g + val_1.b;
	sta_point_no[1] = ev3_motor_get_counts(C_MOTOR);

	/* ライン色読み＆中心を測定 */
	do{
		ev3_color_sensor_get_rgb_raw(COLOR_1, &val_1);
		if(sta_point_no[0] > val_1.r + val_1.g + val_1.b){
			sta_point_no[0] = val_1.r + val_1.g + val_1.b;
			sta_point_no[1] = ev3_motor_get_counts(C_MOTOR);
		}
	}while(50>(val_1.b) || 15>=ev3_motor_get_counts(C_MOTOR));
	tone_object();

	/* スタート位置代入&表示 */
	if((YELLOW_RGB + RED_RGB)/2 < sta_point_no[0]){
		sta_point = 1;	//黄
		//fprintf(fp,"黄色\r\n");
		strcpy(str,"strt-color is Yellow.");
		ev3_lcd_draw_string(str,0,10);
	}else{
		sta_point = 2;	//赤
		fprintf(fp,"赤色\r\n");
		strcpy(str,"strt-color is Red.");
		ev3_lcd_draw_string(str,0,10);
	}
	tone_line();

	/* 直進 */
	ev3_motor_reset_counts(C_MOTOR);
	while(20-(sta_point_no[1]/2) >= ev3_motor_get_counts(C_MOTOR));

	/* バイナリコード色読み　配列に代入 */
	ev3_motor_reset_counts(C_MOTOR);
	while(560>=ev3_motor_get_counts(C_MOTOR)){
		ht_nxt_color_sensor_measure_rgb(HT_COLOR_3, &val_2);
		if((HT_WHITE_RGB + HT_BRAKE_RGB)/2 <= (val_2.r + val_2.g + val_2.b)){
			switch(ev3_motor_get_counts(C_MOTOR)/70){
				case 0:
					binary_code[0][0]++;
					break;
				case 1:
					binary_code[0][1]++;
					break;
				case 2:
					binary_code[1][0]++;
					break;
				case 3:
					binary_code[1][1]++;
					break;
				case 4:
					binary_code[2][0]++;
					break;
				case 5:
					binary_code[2][1]++;
					break;
				case 6:
					binary_code[3][0]++;
					break;
				case 7:
					binary_code[3][1]++;
					break;
			}
		tone_object();
		}
		tslp_tsk(7);
	}
	perfect_BRAKE();

	/* 配列の最大値を求める */
	for(i=0, max_1=0, max_2=0; i<=3; i++){
		if(max_1 < binary_code[i][0]){
			max_1 = binary_code[i][0];
		}

		if(max_2 < binary_code[i][1]){
			max_2 = binary_code[i][1];
		}
	}

	/* それぞれの最大値に1を代入、表示、その他は0を代入。 */
	for(i = 0; i<=3; i++){
		if(max_1 == binary_code[i][0]){
			binary_code[i][0] = 1;
			//fprintf(fp, "binary_code[%d][%d] i=%d\r\n",i,0,max_1);
			sprintf(str, "binary_code[%d][0]",i);
			ev3_lcd_draw_string(str,0,20);
		}else{
			binary_code[i][0] = 0;
		}

		if(max_2 == binary_code[i][1]){
			binary_code[i][1] = 1;
			//fprintf(fp, "binary_code[%d][%d] i=%d\r\n",i,1, max_2);
			sprintf(str, "binary_code[%d][1]",i);
			ev3_lcd_draw_string(str,0,30);
		}else{
			binary_code[i][1] = 0;
		}
	}

	perfect_BRAKE();
	tslp_tsk(500);

	/********************************************************************************************************************************************
	 *	車と雪回収➡捨てる(start➡)						*****************************************************************************************
	 *	start1 = 黄色のライン							 *****************************************************************************************
	 *	start2 = 赤色のライン							 *****************************************************************************************
	 *	1 = start1赤									*****************************************************************************************
	 *	2 = start1黄									*****************************************************************************************
	 *	3 = start1青									*****************************************************************************************
	 *	4 = start2黄									*****************************************************************************************
	 *	5 = start2赤									*****************************************************************************************
	 *	6 = start2緑									*****************************************************************************************
	 ********************************************************************************************************************************************/
	/* 回収する道路を計算&優先度で決定 */
	if(sta_point==1){				//スタート黄
		if(0 == (binary_code[3][0]+binary_code[3][1])){
			road_priority = 1;		//赤3-1
		}else if(0 == (binary_code[2][0]+binary_code[2][1])){
			road_priority = 2;		//黄1-3
		}else{
			road_priority = 3;		//青3-1
		}
	}else{							//スタート赤
		if(0 == (binary_code[3][0]+binary_code[3][1])){
			road_priority = 4;		//赤1-3
		}else if(0 == (binary_code[2][0]+binary_code[2][1])){
			road_priority = 5;		//黄3-1
		}else{
			road_priority = 6;		//緑3-1
		}
	}

	/* 計算通りに雪と車回収 */
	switch(road_priority){
		case 1:
			/* スタート黄-赤道路回収3-1 */
			gyro_deceleration(-500, gyro_angle_standard, -1);
			/* ライン読み */
			do{
				ev3_color_sensor_get_rgb_raw(COLOR_1, &val_1);
			}while(50<=(val_1.b));
			ev3_motor_reset_counts(C_MOTOR);
			tone_line();

			gyro_deceleration(-70, gyro_angle_standard, 0);
			perfect_BRAKE();
			rotation(-90,gyro_angle_standard);
			gyro_angle_standard = -90;
			gyro_angle_standard = wall_fix(500);

			/* 車＆雪回収 */
			collection_red_3_to_1();
			break;
		case 2:
			/* スタート黄-黄道路回収1-3 */

			/* バック */
			gyro_deceleration(-500, gyro_angle_standard, -1);

			/* ライン読み */
			do{
				ev3_color_sensor_get_rgb_raw(COLOR_1, &val_1);
			}while(50<=(val_1.b));
			ev3_motor_reset_counts(C_MOTOR);
			tone_line();

			/* ブレーキ */
			ev3_stp_cyc(GYROTRACE_TASK_4);
			BRAKE(B_MOTOR);
			BRAKE(C_MOTOR);

			/* バック */
			ev3_motor_reset_counts(B_MOTOR);
			ev3_motor_set_power(B_MOTOR, 20);
			while(100>=ev3_motor_get_counts(B_MOTOR));
			BRAKE(B_MOTOR);
			ev3_motor_reset_counts(C_MOTOR);
			ev3_motor_set_power(C_MOTOR, -20);
			while(-100<=ev3_motor_get_counts(C_MOTOR));
			BRAKE(C_MOTOR);
			BRAKE(B_MOTOR);


			/* 旋回 */
			rotation(-90,gyro_angle_standard);
			gyro_angle_standard = -90;

			/* 壁合わせ */
			gyro_angle_standard = wall_fix(-0);

			/* ジャイロ直進 */
			gyro_deceleration(1570, gyro_angle_standard, 0);

			/* 旋回 */
			rotation(90,gyro_angle_standard);
			tone_line();
			gyro_angle_standard += 90;

			/* 点線ライントレース */
			ev3_motor_set_power(B_MOTOR, -15);
			ev3_motor_set_power(C_MOTOR,  15);

			for(i=1; i<=7; i++){
				broken_line(0,0);
			}

			/* 直進 */
			deceleration(1000,-1);

			/* 色読み */
			while((WHITE_REFLECTED + BRAKE_REFLECTED)/2 < ev3_color_sensor_get_reflect(COLOR_1));
			tone_line();

			/* 直進 */
			deceleration(250,0);

			/* 旋回 */
			rotation(90,gyro_angle_standard);
			gyro_angle_standard = -90;

			/* 壁合わせ */
			gyro_angle_standard = wall_fix(1000);
			
			collection_yellow_3_to_1();
			break;
		case 3:
			/* スタート黄-青道路回収青3-1 */
			gyro_deceleration(1200, gyro_angle_standard, 0);
			rotation(-90, gyro_angle_standard);
			gyro_angle_standard = wall_fix(500);

			collection_blue_3_to_1();
			break;
		case 4:
			/* スタート赤-赤道路回収1-3 */

			collection_red_1_to_3();
			break;
		case 5:
			/* スタート赤-黄道路回収3-1 */
			gyro_deceleration(-500, gyro_angle_standard, -1);
			/* ライン読み */
			do{
				ev3_color_sensor_get_rgb_raw(COLOR_1, &val_1);
			}while(50<=(val_1.b));
			ev3_motor_reset_counts(C_MOTOR);
			tone_line();

			gyro_deceleration(-500, gyro_angle_standard, 0);
			perfect_BRAKE();
			rotation(-90,gyro_angle_standard);
			gyro_angle_standard = -90;
			gyro_angle_standard = wall_fix(500);

			break;
		case 6:
			/* スタート赤-緑道路回収 */
			gyro_deceleration(740, gyro_angle_standard, 0);
			rotation(-90, gyro_angle_standard);
			gyro_angle_standard = wall_fix(500);

			collection_green_3_to_1();
			break;
	}


	/********************************************************************************************************************************************
	 *	車と雪回収➡捨てる(Dispenser➡)					*****************************************************************************************
	 *	1 = Dispenser赤									*****************************************************************************************
	 *	2 = Dispenser黄									*****************************************************************************************
	 *	3 = Dispenser緑									*****************************************************************************************
	 *	4 = Dispenser青									*****************************************************************************************
	 ********************************************************************************************************************************************/
	/* 回収する道路を計算 */
	if(0 == (binary_code[3][0]+binary_code[3][1])){
		road_priority = 1;
	}else if(0 == (binary_code[2][0]+binary_code[2][1])){
		road_priority = 2;
	}else if(0 == (binary_code[1][0]+binary_code[1][1])){
		road_priority = 3;
	}else{
		road_priority = 4;
	}

	/* 計算通りに雪と車回収 */
	switch(road_priority){
		case 1:
			/* Dispenser赤道路回収 */
			break;
		case 2:
			/* Dispenser黄道路回収 */
			break;
		case 3:
			/* Dispenser緑道路回収 */
			break;
		case 4:
			/* Dispenser青道路回収 */
			break;
	}



	/********************************************************************************************************************************************
	 *	研磨剤回収
	 ********************************************************************************************************************************************/


	/********************************************************************************************************************************************
	 *	研磨剤を撒く									 *****************************************************************************************
	 *	start1 = 黄色のライン							 *****************************************************************************************
	 *	start2 = 赤色のライン							 *****************************************************************************************
	 *	1 = 青緑1										*****************************************************************************************
	 *	2 = 青黄1										*****************************************************************************************
	 *	3 = 青赤1										*****************************************************************************************
	 *	4 = 緑黄1										*****************************************************************************************
	 *	5 = 緑赤1										*****************************************************************************************
	 *	6 = 黄赤1										*****************************************************************************************
	 *	7 = 青緑2										*****************************************************************************************
	 *	8 = 青黄2										*****************************************************************************************
	 *	90 = 青赤2										*****************************************************************************************
	 *	10 = 緑黄2										*****************************************************************************************
	 *	11 = 緑赤2										*****************************************************************************************
	 *	12 = 黄赤2										*****************************************************************************************
	 ********************************************************************************************************************************************/	
	/* 研磨剤を撒く道路を計算する */
	for(abrasive_priority = -1, i=0, x=1; abrasive_priority < 0; i++){
		for(j=i+1; j<=3; j++, x++){
			if(2 == binary_code[i][0]+binary_code[i][1]+binary_code[j][0]+binary_code[j][1]){
				abrasive_priority = x;
				break;
			}
		}
	}

	if(sta_point == 2){
		abrasive_priority += 6;
	}

	switch(abrasive_priority){
		case 1:
			/* 青緑1研磨座撒く */
			break;
		case 2:
			/* 青黄1研磨座撒く */
			break;
		case 3:
			/* 青赤1研磨座撒く */
			break;
		case 4:
			/* 緑黄1研磨座撒く */
			break;
		case 5:
			/* 緑赤1研磨座撒く */
			break;
		case 6:
			/* 黄赤1研磨座撒く */
			break;
		case 7:
			/* 青緑2研磨座撒く */
			break;
		case 8:
			/* 青黄2研磨座撒く */
			break;
		case 9:
			/* 青赤2研磨座撒く */
			break;
		case 10:
			/* 緑黄2研磨座撒く */
			break;
		case 11:
			/* 緑赤2研磨座撒く */
			break;
		case 12:
			/* 黄赤2研磨座撒く */
			break;
	}


	return 0;
}





/********************************************************************************************************************************************
 *	赤道路車雪回収
 ********************************************************************************************************************************************/
int collection_red_3_to_1(void){
	int tire_angul = 0;
	ev3_motor_reset_counts(C_MOTOR);
	gyrotrace_task_4_power_p_i_d_angle(30, 2, 0, 0.5, gyro_angle_standard);
	ev3_sta_cyc(GYROTRACE_TASK_4);
	a_arm_reset(false);
	a_arm_reset(true);
	tire_angul += 1000;
	while(tire_angul >= ev3_motor_get_counts(C_MOTOR));
	a_arm(50);
	a_arm(125);
	tire_angul += 440;
	while(tire_angul >= ev3_motor_get_counts(C_MOTOR));
	ev3_stp_cyc(GYROTRACE_TASK_4);
	ev3_motor_set_power(B_MOTOR, -10);
	ev3_motor_set_power(C_MOTOR,  40);
	while((gyro_angle_standard+75) >= ev3_gyro_sensor_get_angle(GYRO_4));
	gyro_angle_standard += 90;
	gyrotrace_task_4_power_p_i_d_angle(30, 2, 0, 0.5, gyro_angle_standard);
	ev3_sta_cyc(GYROTRACE_TASK_4);
	d_motor_car_open(0);
	tire_angul += 1080;
	while(tire_angul >= ev3_motor_get_counts(C_MOTOR));
	a_arm_reset(true);
	tire_angul += 500;
	while(tire_angul >= ev3_motor_get_counts(C_MOTOR));
	a_arm(50);
	tire_angul += gyro_deceleration(500, gyro_angle_standard, 0);
	d_motor_car_close();
	a_arm_reset(true);
	// rotation(-180, gyro_angle_standard);
	ev3_motor_set_power(B_MOTOR, 10);
    ev3_motor_set_power(C_MOTOR, 10);
    while(140>=ev3_gyro_sensor_get_angle(GYRO_4));
	perfect_BRAKE();
	gyro_angle_standard += 180;
	gyro_deceleration(1000, gyro_angle_standard, -1);
	while((BRAKE_REFLECTED+WHITE_REFLECTED)/2 < ev3_color_sensor_get_reflect(COLOR_1));
	tone_line();
	perfect_BRAKE();
	while(1);

	return 0;
}





/********************************************************************************************************************************************
 *	黄道路車雪回収
 ********************************************************************************************************************************************/
int collection_yellow_3_to_1(void){
	/* 雪回収準備 */
	a_arm(0);

	/* ジャイロ直進 */
	ev3_motor_reset_counts(C_MOTOR);
	gyrotrace_task_4_power_p_i_d_angle(20, 2, 0, 0, gyro_angle_standard);
	ev3_sta_cyc(GYROTRACE_TASK_4);
	while(400>=ev3_motor_get_counts(C_MOTOR));
	perfect_BRAKE();

	/* 雪1回収 */
	tslp_tsk(500);
	a_arm(0);
	a_arm(0);

	/* ジャイロ直進 */
	gyrotrace_task_4_power_p_i_d_angle(20, 2, 0, 0, gyro_angle_standard);
	ev3_sta_cyc(GYROTRACE_TASK_4);
	while(1200>=ev3_motor_get_counts(C_MOTOR));
	perfect_BRAKE();

	/* 雪2回収 */
	tslp_tsk(500);
	a_arm(0);
	a_arm(0);

	/* 右回転 */
	ev3_motor_set_power(B_MOTOR, -12);
	ev3_motor_set_power(C_MOTOR, 30);
	while(gyro_angle_standard+72>=ev3_gyro_sensor_get_angle(GYRO_4));
	perfect_BRAKE();

	/* 雪3回収 */
	tslp_tsk(500);
	a_arm(0);

	/* Dアーム広げる */
	ev3_motor_set_power(D_MOTOR, 85);

	/* アーム少し閉じる */
	a_arm(0);



	/* 点線2 */
	ev3_motor_set_power(B_MOTOR, -20);
	ev3_motor_set_power(C_MOTOR, 20);
	ev3_motor_reset_counts(C_MOTOR);
	while(150>=ev3_motor_get_counts(C_MOTOR));
	broken_line(250,false);

	/* 雪回収準備 */
	a_arm(0);

	/* 点線3 */
	broken_line(200,true);
	
	/* 点線4 */
	broken_line(200,true);

	/* 点線5 */
	broken_line(0,false);

	/* 雪5回収準備 */
	a_arm(0);

	/* 点線5 */
	broken_line(200,true);

	/* 点線6 */
	broken_line(0,false);

	a_arm(0);

	/* 点線6 */
	broken_line(200,true);

	/* 点線7 */
	broken_line(200,false);

	/* 雪5,6回収 */
	a_arm(0);

	while(1);

	/* 車回収 */
	d_motor_car_open(0);

	return 0;
}


/********************************************************************************************************************************************
 *	青道路車雪回収
 ********************************************************************************************************************************************/
int collection_blue_3_to_1(void){
	int tire_angul = 0;
	a_arm_reset(false);
	a_arm(125);
	gyrotrace_task_4_power_p_i_d_angle(30, 2, 0, 0.5, gyro_angle_standard);
	ev3_motor_reset_counts(B_MOTOR);
	tire_angul += gyro_deceleration(350, gyro_angle_standard, 0);
	ev3_motor_set_power(B_MOTOR,-20);
	ev3_motor_set_power(C_MOTOR,-20);
	while((BRAKE_REFLECTED+WHITE_REFLECTED)/2 < ev3_color_sensor_get_reflect(COLOR_1));
	perfect_BRAKE();
	tone_line();
	d_motor_car_open(0);
	linetrace_task_4_power_p_i_d(30, 0.35, 0.56, 0.06);
	ev3_sta_cyc(LINETRACE_TASK_4);
	tire_angul += 380;
	while(tire_angul>=(ev3_motor_get_counts(C_MOTOR)+(ev3_motor_get_counts(B_MOTOR)*-1))/2);
	a_arm(200);
	tire_angul += 600;
	while(tire_angul>=(ev3_motor_get_counts(C_MOTOR)+(ev3_motor_get_counts(B_MOTOR)*-1))/2);
	a_arm_reset(true);
	tire_angul += 600;
	while(tire_angul>=(ev3_motor_get_counts(C_MOTOR)+(ev3_motor_get_counts(B_MOTOR)*-1))/2);
	perfect_BRAKE();
	while(1);
	

	return 0;
}




/********************************************************************************************************************************************
 *	緑道路車雪回収
 ********************************************************************************************************************************************/
int collection_green_3_to_1(void){

	/* 準備 */
	d_motor_car_close();
	a_arm(0);

	//直進
	gyro_deceleration(300, gyro_angle_standard, -1);
	ev3_stp_cyc(GYROTRACE_TASK_4);
	ev3_motor_reset_counts(C_MOTOR);


	//回転
	ev3_motor_set_power(B_MOTOR,-20);
	ev3_motor_set_power(C_MOTOR,-20);
	while((BRAKE_REFLECTED+WHITE_REFLECTED)/2 < ev3_color_sensor_get_reflect(COLOR_1));
	tone_line();

	//ライントレース
	linetrace_task_4_power_p_i_d(15, 0.35, 0, 0.06);
	ev3_sta_cyc(LINETRACE_TASK_4);
	while(180>=ev3_motor_get_counts(C_MOTOR));
	perfect_BRAKE();

	/* 雪1回収 */
	tslp_tsk(500);
	a_arm(0);
	a_arm(0);

	ev3_sta_cyc(LINETRACE_TASK_4);
	while(700>=ev3_motor_get_counts(C_MOTOR));
	perfect_BRAKE();

	a_arm(0);

	ev3_sta_cyc(LINETRACE_TASK_4);
	while(850>=ev3_motor_get_counts(C_MOTOR));
	perfect_BRAKE();

	/* 雪3回収 */
	a_arm(0);
	a_arm(0);

	ev3_sta_cyc(LINETRACE_TASK_4);
	while(1000>=ev3_motor_get_counts(C_MOTOR));
	perfect_BRAKE();

	

	ev3_sta_cyc(LINETRACE_TASK_4);
	while(1000>=ev3_motor_get_counts(C_MOTOR));
	perfect_BRAKE();

	return 0;
}


/********************************************************************************************************************************************
 *	赤道路雪回収1-3
 ********************************************************************************************************************************************/
int collection_red_1_to_3(void){

}




/********************************************************************************************************************************************
 *	赤道路研磨剤撒く1-3
 ********************************************************************************************************************************************/
int put_red_1_to_3(int);


/********************************************************************************************************************************************
 *	赤道路研磨剤撒く3-1
 ********************************************************************************************************************************************/
int put_red_3_to_1(int);


/********************************************************************************************************************************************
 *	黄道路研磨剤撒く1-3
 ********************************************************************************************************************************************/
int put_yellow_1_to_3(int);


/********************************************************************************************************************************************
 *	黄道路研磨剤撒く3-1
 ********************************************************************************************************************************************/
int put_yellow_3_to_1(int);


/********************************************************************************************************************************************
 *	青道路研磨剤撒く1-3
 ********************************************************************************************************************************************/
int put_blue_1_to_3(int);


/********************************************************************************************************************************************
 *	青道路研磨剤撒く3-1
 ********************************************************************************************************************************************/
int put_blue_3_to_1(int);


/********************************************************************************************************************************************
 *	緑道路研磨剤撒く1-3
 ********************************************************************************************************************************************/
int put_green_1_to_3(int);


/********************************************************************************************************************************************
 *	緑道路研磨剤撒く3-1
 ********************************************************************************************************************************************/
int put_green_3_to_1(int);





/********************************************************************************************************************************************
 *	点線ライントレース
 ********************************************************************************************************************************************/
int broken_line(int tire_angul,int tire_brake){
	ev3_motor_set_power(B_MOTOR, -20);
	ev3_motor_set_power(C_MOTOR, 20);
	while((BRAKE_REFLECTED + WHITE_REFLECTED)/2+10<=ev3_color_sensor_get_reflect(COLOR_2) && (BRAKE_REFLECTED + WHITE_REFLECTED)/2+10<=ev3_color_sensor_get_reflect(COLOR_1));
	tone_line();
	ev3_motor_reset_counts(C_MOTOR);
	while(tire_angul>=ev3_motor_get_counts(C_MOTOR));
	if(tire_brake==0){
		perfect_BRAKE();
	}
	
	// int i=0;
	// int gyro_angle = 0;
	
	// if(0==line_gein_cfg){
	// 	linetrace_task_4_power_p_i_d(15, 0.4, 0, 0.06);	
	// }
	
	// while((BRAKE_REFLECTED + WHITE_REFLECTED)/2+10<=ev3_color_sensor_get_reflect(COLOR_2) && (BRAKE_REFLECTED + WHITE_REFLECTED)/2+10<=ev3_color_sensor_get_reflect(COLOR_1));
	// tone_line();
	// ev3_stp_cyc(GYROTRACE_TASK_4);
	// ev3_motor_reset_counts(C_MOTOR);
	// ev3_sta_cyc(LINETRACE_TASK_4);
	// while(130>=ev3_motor_get_counts(C_MOTOR));
	// tone_line();
	
	// ev3_stp_cyc(LINETRACE_TASK_4);
	
	// ev3_motor_set_power(B_MOTOR, -15);
	// ev3_motor_set_power(C_MOTOR,  15);
	// while(200>=ev3_motor_get_counts(C_MOTOR));
	
	return 0;
}


/********************************************************************************************************************************************
 *	雪&車置く
 ********************************************************************************************************************************************/
int  snow_car_put(int color_direction){
	if(color_direction==1){
		//黄道路
		/* 直進 */
		gyro_deceleration(1200, gyro_angle_standard, 0);
		
		tslp_tsk(1000);
		/* 回転 */
		rotation(-90, gyro_angle_standard);
		gyro_angle_standard += -90;
			
	
	}else{
		//赤道路
		/* 直進 */
		gyro_deceleration(480, gyro_angle_standard, 0);

		tslp_tsk(1000);

		/* 回転 */
		rotation(90, gyro_angle_standard);
		gyro_angle_standard += 90;
		
	}
	tslp_tsk(1000);
	/* 直進 */
	gyro_deceleration(100, gyro_angle_standard, -1);
    
	/* 色読み */
	while((WHITE_REFLECTED + BRAKE_REFLECTED)/2 < ev3_color_sensor_get_reflect(COLOR_1));
	tone_line();

	gyro_deceleration(100, gyro_angle_standard, 0);

	tslp_tsk(1000);

	/* バック */
	ev3_motor_set_power(B_MOTOR, 9);
	ev3_motor_set_power(C_MOTOR, -30);

	if(gyro_angle_standard<-180){
        while((-180)-5>=ev3_gyro_sensor_get_angle(GYRO_4)-gyro_angle_standard);
    }else{
        while((-180)+10<=ev3_gyro_sensor_get_angle(GYRO_4)-gyro_angle_standard);
    }
	perfect_BRAKE();

	a_arm(0);

	/* 壁合わせ */
	ev3_motor_set_power(B_MOTOR, 20);
	ev3_motor_set_power(C_MOTOR, -20);
	
	tslp_tsk(5000);
	perfect_BRAKE();
}


