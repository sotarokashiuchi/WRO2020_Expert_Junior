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

1 = 赤
2 = 黄
3 = 緑
4 = 青
*/

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
int put_green_1_to_3(int);
int put_green_blue_3_to_1(void);
int car_put(void);
void dispenser_recovery(int);

//float *array_command(float *str_p);	//配列操作


/* グローバル変数宣言 */
FILE *fp = NULL;				//ファイルポインタ
FILE *fp2 = NULL;
ER r1;
ER r = E_OK;
int gyro_angle_standard = 0;
int car_p = 0;
rgb_raw_t val_1, val_2;
char str[64] = {};
int roda_f = 0;
int car_no = 1;
ulong_t pastTime,nowTime = 0;

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
	int i=0, j=0, x=0;
	int binary_code[4][2] = {{0}};
	int sta_point_no[2] = {0};
	int road_priority = 0;
	int abrasive_priority = 0;
	int sta_point = 0;
	int max_1 = 0, max_2 = 0;
	
	
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
	strcpy(str,"strt");
	ev3_lcd_draw_string(str,0,0);
	gyro_angle_standard = 0;
	d_motor_car_close(0);
	ev3_motor_reset_counts(D_MOTOR);		//Dモータリセット

	// ev3_lcd_set_font(1);

	/* 実験スペース */
	// ev3_sta_cyc(GYRO_LOG_TASK_10);

	// ev3_lcd_set_font(1);
	
	// gyro_angle_standard = wall_fix(500);
	// gyro_deceleration(360, gyro_angle_standard, 0, 0);
	// gyro_angle_standard += 30;
	// gyrotrace_task_4_power_p_i_d_angle(85, 6, 1, 2, gyro_angle_standard);
    // ev3_sta_cyc(GYROTRACE_TASK_4);
	// while(1){
	// 	sprintf(str, "GYRO=%3d",(ev3_gyro_sensor_get_angle(GYRO_4)-gyro_angle_standard));
	// 	ev3_lcd_draw_string(str,0,0);
	// }

	// while(1){
	// 	sprintf(str, "GYRO=%3d",(ev3_color_sensor_get_reflect(COLOR_1)));
	// 	ev3_lcd_draw_string(str,0,0);
	// }

	// gyro_angle_standard = wall_fix(1000);
	// put_green_blue_3_to_1();
	// while(10);
	// gyro_angle_standard = wall_fix(500);
	// gyrotrace_task_4_power_p_i_d_angle(85, 15, 16, 1, gyro_angle_standard);
	// // gyrotrace_task_4_power_p_i_d_angle(85, 6, 1, 2, gyro_angle_standard);
	// ev3_sta_cyc(GYROTRACE_TASK_4);
	// while(1){
	// 	sprintf(str, "GYRO=%3d",(ev3_gyro_sensor_get_angle(GYRO_4)-gyro_angle_standard));
	// 	ev3_lcd_draw_string(str,0,0);
	// }

	// a_arm_reset(false);
	// a_arm_reset(true);
	// a_arm(165);
	// while(1);
	
	/********************************************************************************************************************************************
	 *	バイナリコード色読み
	 ********************************************************************************************************************************************/
	gyro_angle_standard = wall_fix(500);

	/* 直進 */
	gyro_deceleration_power(85, gyro_angle_standard, -1);
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
	while(48-(sta_point_no[1]/2) >= ev3_motor_get_counts(C_MOTOR));

	/* バイナリコード色読み　配列に代入 */
	ev3_motor_reset_counts(C_MOTOR);
	i = 0;
	while(568>=ev3_motor_get_counts(C_MOTOR)){//577.85487030288152679163951009098 - 9
		if(i == 0){
			if(418<=ev3_motor_get_counts(C_MOTOR)){
				ev3_stp_cyc(GYROTRACE_TASK_4);
				gyro_deceleration_power(30, gyro_angle_standard, -1);
				i += 1;
			}
		}
		if(i == 1){
			if(528<=ev3_motor_get_counts(C_MOTOR)){
			ev3_stp_cyc(GYROTRACE_TASK_4);
			gyro_deceleration_power(10, gyro_angle_standard, -1);
			}
		}
		
		ht_nxt_color_sensor_measure_rgb(HT_COLOR_3, &val_2);
		if((HT_WHITE_RGB + HT_BRAKE_RGB)/2 <= (val_2.r + val_2.g + val_2.b)){
			switch(ev3_motor_get_counts(C_MOTOR)/72){
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
		}
		tslp_tsk(7);
	}
	perfect_BRAKE();

	// for(i=0; i<=3; i++){
	// 	sprintf(str, "[%d][0]=%d  [%d][1]=%d",i, binary_code[i][0], i, binary_code[i][1]);
	// 	ev3_lcd_draw_string(str,0,(i*10+40));
	// }

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

	for(i=0; i<=3; i++){
		binary_code[i][0] = 0;
		binary_code[i][1] = 0;
	}
	binary_code[0][0] = 1;
	binary_code[1][1] = 1;
	


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
			gyro_deceleration(-500, gyro_angle_standard, -1, 0);
			/* ライン読み */
			do{
				ev3_color_sensor_get_rgb_raw(COLOR_1, &val_1);
			}while(50<=(val_1.b));
			ev3_motor_reset_counts(C_MOTOR);
			tone_line();

			gyro_deceleration(-70, gyro_angle_standard, 0, -1);
			perfect_BRAKE();
			rotation(-90,gyro_angle_standard);
			gyro_angle_standard = -90;
			gyro_angle_standard = wall_fix(500);

			/* 車＆雪回収 */
			collection_red_3_to_1();
			break;
		case 2:
			/* スタート黄-黄道路回収1-3 */
			
			collection_yellow_3_to_1();
			break;
		case 3:
			/* スタート黄-青道路回収青3-1 */
			gyro_deceleration(1200, gyro_angle_standard, 0, -1);
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
			gyro_deceleration(-500, gyro_angle_standard, -1, -1);
			/* ライン読み */
			do{
				ev3_color_sensor_get_rgb_raw(COLOR_1, &val_1);
			}while(50<=(val_1.b));
			ev3_motor_reset_counts(C_MOTOR);
			tone_line();

			gyro_deceleration(-480, gyro_angle_standard, 0, -1);
			perfect_BRAKE();
			rotation(-90,gyro_angle_standard);
			gyro_angle_standard = -90;
			gyro_angle_standard = wall_fix(500);

			collection_yellow_3_to_1();
			break;
		case 6:
			/* スタート赤-緑道路回収 */
			gyro_deceleration(740, gyro_angle_standard, 0, -1);
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
		if(roda_f=1){
			road_priority = 1;
		}
	}
	if(0 == (binary_code[2][0]+binary_code[2][1])){
		if(roda_f=1){
			road_priority = 2;
		}
	}
	if(0 == (binary_code[1][0]+binary_code[1][1])){
		if(roda_f=1){
			road_priority = 3;
		}
	}
	if(0 == (binary_code[0][0]+binary_code[0][1])){
		if(roda_f=1){
			road_priority = 4;
		}		
	}

	/* 計算通りに雪と車回収 */
	switch(road_priority){
		case 1:
			/* Dispenser赤道路回収 */
			break;
		case 2:
			/* Dispenser黄道路回収 */
			a_arm(130);
			gyro_deceleration(150, gyro_angle_standard, 0, -1);
			rotation(-90, gyro_angle_standard);
			gyro_angle_standard -= 90;
			gyro_deceleration_power(85, gyro_angle_standard, 0);
			while(3000 >= ev3_motor_get_counts(C_MOTOR));
			while((BRAKE_REFLECTED+WHITE_REFLECTED)/2 < ev3_color_sensor_get_reflect(COLOR_1));
			gyro_deceleration(300, gyro_angle_standard, 0, 0);
			rotation(90, gyro_angle_standard);
			gyro_angle_standard += 90;
			gyro_angle_standard = wall_fix(500);

			collection_yellow_3_to_1();
			dispenser_recovery(0);
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
			gyro_deceleration(120, gyro_angle_standard, 0, -1);
			rotation(-90, gyro_angle_standard);
			gyro_angle_standard -= 90;
			gyro_deceleration(1610, gyro_angle_standard, 0, 0);
			rotation(90, gyro_angle_standard);
			gyro_angle_standard = wall_fix(500);

			put_green_blue_3_to_1();

			/* 近いほうにゴール */
			rotation(-90, gyro_angle_standard);
			gyro_angle_standard -= 90;
			gyro_angle_standard -= 2;
			gyro_deceleration_power(-85, gyro_angle_standard, 0);
			while(-700 <= ev3_motor_get_counts(C_MOTOR));
			while((BRAKE_REFLECTED+WHITE_REFLECTED)/2 < ev3_color_sensor_get_reflect(COLOR_1));
			ev3_stp_cyc(GYROTRACE_TASK_4);
			gyro_angle_standard = wall_fix(1000);
			while(1);

			
			// rotation(90, gyro_angle_standard);
			// gyro_angle_standard += 90;
			// gyro_deceleration_power(85, gyro_angle_standard, 0);
			// while(150 >= ev3_motor_get_counts(C_MOTOR));
			// while((BRAKE_REFLECTED+WHITE_REFLECTED)/2 < ev3_color_sensor_get_reflect(COLOR_1));
			// gyro_deceleration(250, gyro_angle_standard, 0, 0);
			// rotation(90, gyro_angle_standard);
			// gyro_angle_standard = wall_fix(1000);
			// gyro_deceleration(1700, gyro_angle_standard, 0, 0);
			// rotation(90, gyro_angle_standard);
			// gyro_angle_standard += 90;
			// gyro_deceleration(1500, gyro_angle_standard, 0, 0);
			// rotation(90, gyro_angle_standard);
			// gyro_angle_standard = wall_fix(1000);

			put_green_blue_3_to_1();
			rotation(-90, gyro_angle_standard);
			gyro_angle_standard -= 90;
			gyro_angle_standard -= 4;
			gyro_deceleration_power(-85, gyro_angle_standard, 0);
			while(-700 <= ev3_motor_get_counts(C_MOTOR));
			while((BRAKE_REFLECTED+WHITE_REFLECTED)/2 < ev3_color_sensor_get_reflect(COLOR_1));
			ev3_stp_cyc(GYROTRACE_TASK_4);
			gyro_angle_standard = wall_fix(1000);
			perfect_BRAKE();
			
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
	roda_f = 1;
	int tire_angul = 0;
	gyro_deceleration_power(30, gyro_angle_standard, 0);
	a_arm_reset(false);
	a_arm_reset(true);
	tire_angul += 980;
	while(tire_angul >= ev3_motor_get_counts(C_MOTOR));
	a_arm(50);
	a_arm(165);
	tire_angul += 440;
	while(tire_angul >= ev3_motor_get_counts(C_MOTOR));
	ev3_stp_cyc(GYROTRACE_TASK_4);
	ev3_motor_set_power(B_MOTOR, -10);
	ev3_motor_set_power(C_MOTOR,  40);
	while((gyro_angle_standard+75) >= ev3_gyro_sensor_get_angle(GYRO_4));
	gyro_angle_standard += 90;
	gyro_deceleration_power(30, gyro_angle_standard, -1);
	d_motor_car_open(0);
	tire_angul += 1080;
	while(tire_angul >= ev3_motor_get_counts(C_MOTOR));
	a_arm_reset(true);
	tire_angul += 500;
	while(tire_angul >= ev3_motor_get_counts(C_MOTOR));
	a_arm(50);
	tire_angul += gyro_deceleration(500, gyro_angle_standard, 0, -1);
	d_motor_car_close(1);
	a_arm_reset(true);
	// rotation(180, gyro_angle_standard);
	ev3_motor_set_power(B_MOTOR, 10);
    ev3_motor_set_power(C_MOTOR, 10);
	while(150>=ev3_gyro_sensor_get_angle(GYRO_4));
	perfect_BRAKE();
	gyro_angle_standard += 175;
	gyro_deceleration(1950, gyro_angle_standard, -1, 0);
	while((BRAKE_REFLECTED+WHITE_REFLECTED)/2 < ev3_color_sensor_get_reflect(COLOR_1));
	tone_line();
	a_arm_reset(false);
	gyro_deceleration(-200, gyro_angle_standard, 0, -1);
	rotation(-90,gyro_angle_standard);
	gyro_angle_standard = wall_fix(1000);

	car_put();
	return 0;
}





/********************************************************************************************************************************************
 *	黄道路車雪回収
 ********************************************************************************************************************************************/
int collection_yellow_3_to_1(void){
	roda_f = 2;
	int tire_angul = 0;
	a_arm_reset(false);
		
	gyro_deceleration_power(30, gyro_angle_standard, 0);
	tire_angul += 1200;
	while(tire_angul>=ev3_motor_get_counts(C_MOTOR));
	a_arm(165);
	tire_angul += 180;
	while(tire_angul>=ev3_motor_get_counts(C_MOTOR));
	ev3_stp_cyc(GYROTRACE_TASK_4);
	ev3_motor_set_power(B_MOTOR, -10);
	ev3_motor_set_power(C_MOTOR, 30);
	while(gyro_angle_standard+75>=ev3_gyro_sensor_get_angle(GYRO_4));
	perfect_BRAKE();
	tslp_tsk(500);
	d_motor_car_open(false);
	gyro_angle_standard += 90;
	gyro_deceleration_power(30, gyro_angle_standard, -1);
	ev3_sta_cyc(GYROTRACE_TASK_4);
	a_arm_reset(false);
	tire_angul += 600;
	while(tire_angul>=ev3_motor_get_counts(C_MOTOR));
	a_arm(165);
	tire_angul += 700;
	while(tire_angul>=ev3_motor_get_counts(C_MOTOR));
	a_arm_reset(true);
	tire_angul += 480;
	while(tire_angul>=ev3_motor_get_counts(C_MOTOR));
	a_arm_reset(false);
	gyro_deceleration(750, gyro_angle_standard, 0, -1);
	a_arm(165);
	d_motor_car_close(1);

	ev3_motor_reset_counts(C_MOTOR);
	ev3_motor_set_power(C_MOTOR, 85);
	while(200>=ev3_motor_get_counts(C_MOTOR));
	BRAKE(C_MOTOR);
	ev3_motor_reset_counts(B_MOTOR);
	ev3_motor_set_power(B_MOTOR, -85);
	while(-200<=ev3_motor_get_counts(B_MOTOR));
	BRAKE(B_MOTOR);

	gyro_deceleration(900, gyro_angle_standard, -1, 0);
	while((BRAKE_REFLECTED+WHITE_REFLECTED)/2 < ev3_color_sensor_get_reflect(COLOR_1));
	perfect_BRAKE();
	tone_line();
	
	a_arm_reset(true);
	rotation(95, gyro_angle_standard);
	gyro_angle_standard += 90;
	gyro_angle_standard = wall_fix(1000);

	gyro_deceleration(100, gyro_angle_standard, 0, 0);
	ev3_motor_reset_counts(C_MOTOR);
	ev3_motor_set_power(C_MOTOR, 85);
	while(200>=ev3_motor_get_counts(C_MOTOR));
	BRAKE(C_MOTOR);
	ev3_motor_reset_counts(B_MOTOR);
	ev3_motor_set_power(B_MOTOR, -85);
	while(-200<=ev3_motor_get_counts(B_MOTOR));
	BRAKE(B_MOTOR);


	gyro_deceleration(1300, gyro_angle_standard, 0, 0);
	a_arm_reset(false);
	
	rotation(-90, gyro_angle_standard);
	gyro_angle_standard -= 90;
	gyro_deceleration(100, gyro_angle_standard, -1, -1);
	while((BRAKE_REFLECTED+WHITE_REFLECTED)/2 < ev3_color_sensor_get_reflect(COLOR_1));
	perfect_BRAKE();
	tone_line();
	gyro_deceleration(-200, gyro_angle_standard, 0, -1);
	rotation(-90, gyro_angle_standard);
	gyro_angle_standard -= 90;
	gyro_angle_standard = wall_fix(500);

	car_put();
	return 0;
}


/********************************************************************************************************************************************
 *	青道路車雪回収
 ********************************************************************************************************************************************/
int collection_blue_3_to_1(void){
	roda_f = 4;
	int tire_angul = 0;
	a_arm_reset(false);
	a_arm(165);
	gyro_deceleration_power(30, gyro_angle_standard, 0);
	ev3_motor_reset_counts(B_MOTOR);
	tire_angul += gyro_deceleration(350, gyro_angle_standard, 0, -1);
	ev3_motor_set_power(B_MOTOR,-20);
	ev3_motor_set_power(C_MOTOR,-20);
	while((BRAKE_REFLECTED+WHITE_REFLECTED)/2 < ev3_color_sensor_get_reflect(COLOR_1));
	perfect_BRAKE();
	tone_line();
	d_motor_car_open(0);
	linetrace_task_4_power_p_i_d(20, 0.35, 0, 0.1);
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
	roda_f = 3;

	/* 準備 */
	d_motor_car_close(0);
	a_arm(0);

	//直進
	gyro_deceleration(300, gyro_angle_standard, -1, -1);
	ev3_stp_cyc(GYROTRACE_TASK_4);
	//ライントレース
	linetrace_task_4_power_p_i_d(15, 0.35, 0, 0.06);
	ev3_sta_cyc(LINETRACE_TASK_4);
	while(180>=ev3_motor_get_counts(C_MOTOR));
	perfect_BRAKE();

	ev3_sta_cyc(LINETRACE_TASK_4);
	while(700>=ev3_motor_get_counts(C_MOTOR));
	perfect_BRAKE();

	ev3_sta_cyc(LINETRACE_TASK_4);
	while(850>=ev3_motor_get_counts(C_MOTOR));
	perfect_BRAKE();

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
	roda_f = 1;

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
 *	緑道路研磨剤撒く1-3
 ********************************************************************************************************************************************/
int put_green_1_to_3(int);


/********************************************************************************************************************************************
 *	緑to青道路研磨剤撒く3-1
 ********************************************************************************************************************************************/
int put_green_blue_3_to_1(void){
	int tire_angul = 0;
	// a_arm_reset(true);
	a_arm(165);
	gyro_deceleration(100, gyro_angle_standard, 0, 0);
	rotation(-45, gyro_angle_standard);
	gyro_angle_standard -= 45;
	gyro_deceleration_power(85, gyro_angle_standard, 0);
	tire_angul += 500;
	while(tire_angul>=ev3_motor_get_counts(C_MOTOR));
	kennmazai_put(0);
	tire_angul += 650;
	while(tire_angul>=ev3_motor_get_counts(C_MOTOR));
	gyro_deceleration(150, gyro_angle_standard, 0, 0);
	rotation(-45, gyro_angle_standard);
	gyro_angle_standard -= 45;
	gyro_deceleration(350, gyro_angle_standard, -1, 0);
	while((BRAKE_REFLECTED+WHITE_REFLECTED)/2 < ev3_color_sensor_get_reflect(COLOR_1));
	perfect_BRAKE();
	tone_line();
	kennmazai_put(0);
	ev3_motor_set_power(B_MOTOR,-30);
	while(-70<=(ev3_gyro_sensor_get_angle(GYRO_4)-gyro_angle_standard));
	perfect_BRAKE();
	gyro_angle_standard -= 90;
	gyro_deceleration(700, gyro_angle_standard, 0, 0);
	kennmazai_put(0);
	kennmazai_put(0);
	return 0;
}


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
	return 0;
}


/********************************************************************************************************************************************
 *	車置く
 ********************************************************************************************************************************************/
int  car_put(void){
	int i_c;
	int max_c = 0;

	gyro_deceleration(600, gyro_angle_standard, 0, 0);
	a_arm_reset(true);
	d_motor_car_open(0);
	gyro_deceleration(-105, gyro_angle_standard, 0, -1);
	
	for(i_c=0; i_c<=12; i_c++){
		ht_nxt_color_sensor_measure_rgb(HT_COLOR_3, &val_2);
		tslp_tsk(7);
	}
	tslp_tsk(15);
	if(val_2.b>=80){
		car_p = true;
		tone_object();
	}else{
		car_p = false;
	}

	switch(car_p){
		case true:
			gyro_deceleration(-300, gyro_angle_standard, 0, 0);
			a_arm_reset(false);
			d_motor_car_close(0);
			ev3_motor_reset_counts(C_MOTOR);
			ev3_motor_set_power(C_MOTOR, 85);
			while(80>=ev3_motor_get_counts(C_MOTOR));
			BRAKE(C_MOTOR);
			ev3_motor_reset_counts(B_MOTOR);
			ev3_motor_set_power(B_MOTOR, -85);
			while(-80<=ev3_motor_get_counts(B_MOTOR));
			BRAKE(B_MOTOR);
			gyro_deceleration(200, gyro_angle_standard, 0, 0);
			a_arm(165);
		break;

		case false:
			a_arm_reset(false);
			gyro_deceleration(-360, gyro_angle_standard, 0, 0);
			a_arm_reset(true);
			d_motor_car_close(0);
			ev3_motor_reset_counts(B_MOTOR);
			ev3_motor_set_power(B_MOTOR, -85);
			while(-120<=ev3_motor_get_counts(B_MOTOR));
			BRAKE(B_MOTOR);
			ev3_motor_reset_counts(C_MOTOR);
			ev3_motor_set_power(C_MOTOR, 85);
			while(120>=ev3_motor_get_counts(C_MOTOR));
			BRAKE(C_MOTOR);
			gyro_deceleration(260, gyro_angle_standard, 0, 0);
			a_arm(165);
		break;
	}
	rotation(90, gyro_angle_standard);
	gyro_angle_standard += 90;
	gyro_deceleration(0, gyro_angle_standard, -1, -1);
	while((BRAKE_REFLECTED+WHITE_REFLECTED)/2 < ev3_color_sensor_get_reflect(COLOR_1));
	if(car_no == 1){
		/* 一回目 */
		car_no += 1;
		gyro_deceleration(150, gyro_angle_standard, 0, 0);
		switch(car_p){
			case true:
				a_arm_reset(false);
			break;
			case false:
				a_arm_reset(true);
			break;
		}
		gyro_deceleration(-130, gyro_angle_standard, -1, 0);
	}else{
		/* 二回目 */
		gyro_deceleration(30, gyro_angle_standard, -1, 0);
		tslp_tsk(300);
		perfect_BRAKE();

		switch(car_p){
			case true:
				a_arm_reset(false);
			break;
			case false:
				a_arm_reset(true);
			break;
		}
		gyro_deceleration(-10, gyro_angle_standard, -1, 0);
	}
	while((BRAKE_REFLECTED+WHITE_REFLECTED-20) < ev3_color_sensor_get_reflect(COLOR_1)+ev3_color_sensor_get_reflect(COLOR_2));
	ev3_motor_reset_counts(C_MOTOR);
	tone_line();
	while(-200<=ev3_motor_get_counts(C_MOTOR));
	perfect_BRAKE();
	rotation(-90, gyro_angle_standard);
	gyro_angle_standard -= 90;
	gyro_deceleration(-400, gyro_angle_standard, 0, 0);
	d_motor_car_close(0);
	gyro_angle_standard = wall_fix(1000);
	return 0;
}


void dispenser_recovery(int b_b){
	/* 一つ目 */
	a_arm(165);

	gyro_deceleration(900, gyro_angle_standard, 0, -1);
	rotation(90, gyro_angle_standard);
	gyro_angle_standard += 90;
	gyro_deceleration(1, gyro_angle_standard, -1, -1);
	// ev3_motor_reset_counts(C_MOTOR);
	// gyrotrace_task_4_power_p_i_d_angle(15, 2, 0, 0.5, gyro_angle_standard);
	// ev3_sta_cyc(GYROTRACE_TASK_4);
	while((BRAKE_REFLECTED+WHITE_REFLECTED)/2 < ev3_color_sensor_get_reflect(COLOR_1));
	ev3_motor_reset_counts(C_MOTOR);
	tone_line();
	gyrotrace_task_4_power_p_i_d_angle(15, 2, 0, 0.5, gyro_angle_standard);
	ev3_sta_cyc(GYROTRACE_TASK_4);
	while(200>=ev3_motor_get_counts(C_MOTOR));
	perfect_BRAKE();
	// ev3_stp_cyc(LINETRACE_TASK_4);
	// ev3_motor_reset_counts(C_MOTOR);
	// ev3_motor_reset_counts(B_MOTOR);
	// linetrace_task_4_power_p_i_d(15, 0.35, 0, 0.06);
	// ev3_sta_cyc(LINETRACE_TASK_4);
	// while(200>=(ev3_motor_get_counts(C_MOTOR)+(ev3_motor_get_counts(B_MOTOR)*-1))/2);
	// perfect_BRAKE();
	
	ev3_motor_reset_counts(C_MOTOR);
	ev3_motor_reset_counts(B_MOTOR);
	ev3_motor_set_power(B_MOTOR, 5);
	ev3_motor_set_power(C_MOTOR,-5);
	while(-80<=ev3_motor_get_counts(C_MOTOR));
	perfect_BRAKE();
	tslp_tsk(1000);

	/* 二つ目 */
	gyro_deceleration(-200, gyro_angle_standard, 0, -1);
	rotation(-90, gyro_angle_standard);
	gyro_angle_standard -= 90;
	gyro_deceleration(340, gyro_angle_standard, 0, -1);
	rotation(90, gyro_angle_standard);
	gyro_angle_standard += 90;
	gyro_deceleration(1, gyro_angle_standard, -1, -1);
	while((BRAKE_REFLECTED+WHITE_REFLECTED)/2 < ev3_color_sensor_get_reflect(COLOR_1));
	ev3_motor_reset_counts(C_MOTOR);
	tone_line();
	gyrotrace_task_4_power_p_i_d_angle(15, 2, 0, 0.5, gyro_angle_standard);
	ev3_sta_cyc(GYROTRACE_TASK_4);
	while(200>=ev3_motor_get_counts(C_MOTOR));
	perfect_BRAKE();
	
	ev3_motor_reset_counts(C_MOTOR);
	ev3_motor_reset_counts(B_MOTOR);
	ev3_motor_set_power(B_MOTOR, 5);
	ev3_motor_set_power(C_MOTOR,-5);
	while(-80<=ev3_motor_get_counts(C_MOTOR));
	perfect_BRAKE();
	tslp_tsk(1000);
	gyro_deceleration(-100, gyro_angle_standard, -1, -1);
	while((BRAKE_REFLECTED+WHITE_REFLECTED-20) < ev3_color_sensor_get_reflect(COLOR_1)+ev3_color_sensor_get_reflect(COLOR_2));
	gyro_deceleration(-200, gyro_angle_standard, 0, -1);
	perfect_BRAKE();
	rotation(-90, gyro_angle_standard);
	gyro_angle_standard -= 90;
	gyro_deceleration(-1200, gyro_angle_standard, -1, 0);
	ev3_stp_cyc(GYROTRACE_TASK_4);
	gyro_angle_standard = wall_fix(1000);
}


void gyro_log(void){
	sprintf(str, "GYRO=%5d",(ev3_gyro_sensor_get_angle(GYRO_4)-gyro_angle_standard));
	ev3_lcd_draw_string(str,0,0);
}

