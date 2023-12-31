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
タイヤの直径62.4㎜
タイヤの円周19.603538158400309808006894711664
1 = 赤
2 = 黄
3 = 緑
4 = 青
GYRO PID 2, 0, 0.5
*/

/* 関数のプロトタイプ宣言 */
int WRO(void);
int broken_line(int tire_angul,int tire_brake);
int collection_red_3_to_1(void);
int collection_yellow_3_to_1(void);
int collection_yellow_1_to_3(void);
int collection_blue_3_to_1(void);
int collection_green_3_to_1(void);
int collection_red_1_to_3(void);
int put_red_yellow_1_to_3(int);
int put_red_yellow_3_to_1(int, int, int);
int line_color(int, int);
int put_yellow_3_to_1(int);
int goal(int);
int move_green_blue_1_to_red_yellow_3(int);
int put_green_blue_3_to_1(void);
int car_put(void);
void dispenser_recovery(int);
void dispenser(void);
int snow_put(int);
void collection_road_yello(int);
void collection_road_red(int);


/* グローバル変数宣言 */
FILE *fp = NULL;				//ファイルポインタ
FILE *fp2 = NULL;
ER r1;
ER r = E_OK;
int gyro_angle_standard = 0;
int car_p = 0;
// int kennmazai_put_f = 0;	//false = 0 = put
rgb_raw_t val_1, val_2;
char str[64] = {};
int str_p = 0;
int roda_f = 0;
int car_no = 1;
ulong_t pastTime,nowTime = 0;
memfile_t memfile;
int binary_code[4][2] = {{0}};


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
	// ev3_memfile_load("ev3rt/res/awf.wav", &memfile);
	tslp_tsk(5000);
	while(false==ev3_button_is_pressed(ENTER_BUTTON));

	ev3_motor_reset_counts(B_MOTOR);
	ev3_motor_reset_counts(C_MOTOR);
	ev3_motor_reset_counts(A_ARM);
	ev3_motor_reset_counts(D_MOTOR);
	strcpy(str,"strt");
	ev3_lcd_draw_string(str,0,str_p*10);
	str_p += 1;
	gyro_angle_standard = 0;
	d_motor_car_close(0);
	ev3_motor_reset_counts(D_MOTOR);		//Dモータリセット

	gyro_angle_standard = wall_fix(500);
	a_arm_reset(false);
	a_arm(A_ARM_C);

	// ev3_lcd_set_font(1);

	/* 実験スペース */
	// ev3_sta_cyc(GYRO_LOG_TASK_10);

	// ev3_lcd_set_font(1);
	// ev3_speaker_play_file(&memfile, SOUND_MANUAL_STOP);
	// tslp_tsk(10000);
	
	// gyro_angle_standard = wall_fix(1000);
	// // collection_yellow_1_to_3();
	// snow_put(0);
	// while(1);



	// gyro_deceleration_power(85, gyro_angle_standard, 0);
	// while(-1000<=ev3_motor_get_counts(B_MOTOR));
	// ev3_stp_cyc(GYROTRACE_TASK_4);
	// // BRAKE(C_MOTOR);
	// // while(-1500>=ev3_motor_get_counts(B_MOTOR));
	// ev3_motor_set_power(C_MOTOR,-85);
	// ev3_motor_set_power(B_MOTOR,-85);
	
	// while(0+53<=ev3_gyro_sensor_get_angle(GYRO_4)-(-90));
	// ev3_motor_set_power(B_MOTOR,-15);
	// ev3_motor_set_power(C_MOTOR,-15);
	// while(0+33<=ev3_gyro_sensor_get_angle(GYRO_4)-(-90));
	// ev3_motor_set_power(B_MOTOR,-9);
	// ev3_motor_set_power(C_MOTOR,-9);
	// while((0+13<=ev3_gyro_sensor_get_angle(GYRO_4)-(-90)));
	// BRAKE(B_MOTOR);
	// BRAKE(C_MOTOR);
	// // while(-90 <= ev3_gyro_sensor_get_angle(GYRO_4));
	// tslp_tsk(100);
	// perfect_BRAKE();
	
	

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
		ev3_lcd_draw_string(str,0,str_p*10);
		str_p += 1;
	}else{
		sta_point = 2;	//赤
		fprintf(fp,"赤色\r\n");
		strcpy(str,"strt-color is Red.");
		ev3_lcd_draw_string(str,0,str_p*10);
		str_p += 1;
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

	for(i=0; i<=3; i++){
		sprintf(str, "[%d][0]=%d  [%d][1]=%d",i, binary_code[i][0], i, binary_code[i][1]);
		ev3_lcd_draw_string(str,0,((str_p*10)));
		str_p += 1;
	}

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
			ev3_lcd_draw_string(str,0,str_p*10);
			str_p += 1;
		}else{
			binary_code[i][0] = 0;
		}

		if(max_2 == binary_code[i][1]){
			binary_code[i][1] = 1;
			//fprintf(fp, "binary_code[%d][%d] i=%d\r\n",i,1, max_2);
			sprintf(str, "binary_code[%d][1]",i);
			ev3_lcd_draw_string(str,0,str_p*10);
			str_p += 1;
		}else{
			binary_code[i][1] = 0;
		}
	}


	perfect_BRAKE();

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

	if(sta_point == 1){
		//黄
		gyro_deceleration(300, gyro_angle_standard, 0, 0);
		gyro_angle_standard += rotation(-90, gyro_angle_standard);
		gyro_deceleration(100, gyro_angle_standard, 0, 0);
		gyro_angle_standard += rotation(-90, gyro_angle_standard);
		gyro_deceleration(1000, gyro_angle_standard, -1, 0);
		line_color(0, 0);
		gyro_angle_standard += rotation(90, gyro_angle_standard);
		gyro_angle_standard = wall_fix(1000);
	}
	


	/********************************************************************************************************************************************
	*	車と雪回収➡捨てる(start➡)						*****************************************************************************************
	*	1 = 赤道路回収									*****************************************************************************************
	*	2 = 黄道路回収									*****************************************************************************************
	*	3 = 青道路回収									*****************************************************************************************
	*	4 = 緑道路回収									*****************************************************************************************
	********************************************************************************************************************************************/
	/* 1回目　回収する道路を計算&優先度で決定 */
	if(sta_point==1){				//スタート黄
		if(0 == (binary_code[3][0]+binary_code[3][1])){
			collection_road_yello(1);		//赤3-1
		}else if(0 == (binary_code[2][0]+binary_code[2][1])){
			collection_road_yello(2);		//黄1-3
		}else{
			collection_road_yello(4);		//青3-1
		}
	}else{							//スタート赤
		if(0 == (binary_code[3][0]+binary_code[3][1])){
			collection_road_red(1);		//赤1-3
		}else if(0 == (binary_code[2][0]+binary_code[2][1])){
			collection_road_red(2);		//黄3-1
		}else{
			collection_road_red(3);		//緑3-1
		}
	}

	/* 2回目　回収する道路を計算&優先度で決定 */
	if(0 == (binary_code[2][0]+binary_code[2][1])){
		//黄
		if(2 != roda_f){
			collection_road_yello(2);
			roda_f = -1;
		}
	}
	if(-1 != roda_f){
		if((0 == (binary_code[1][0]+binary_code[1][1]))){
			//緑
			if(3 != roda_f){
				collection_road_yello(3);
				roda_f = -1;
			}
		}
	}
	if(-1 != roda_f){
		if(0 == ((binary_code[0][0]+binary_code[0][1]))){
			//青
			if(4 != roda_f){
				collection_road_yello(4);
				roda_f = -1;
			}		
		}
	}


	sprintf(str, "Point no1 road_priority=%d",road_priority);
	ev3_lcd_draw_string(str,0,str_p*10);
	str_p += 1;


	/********************************************************************************************************************************************
	 *	研磨剤回収
	 ********************************************************************************************************************************************/
	/* 初めに捨てる道路を求める（４道路） */
	/* 次にその道路はどちらにまけばいいのか求める */
	if(((abrasive_priority == 6) || (abrasive_priority == 12)) || ((abrasive_priority == 3) || (abrasive_priority == 11))){
		//赤道路
		if(binary_code[3][0] == 1){
			//黒い研磨材が優先
			dispenser_recovery(0);
		}else{
			//青い研磨材が優先
			dispenser_recovery(1);
		}
	}else if((abrasive_priority == 10) || (abrasive_priority == 2)){
		//黄道路
		if(binary_code[2][0] == 1){
			//黒い研磨材が優先
			dispenser_recovery(0);
		}else{
			//青い研磨材が優先
			dispenser_recovery(1);
		}
	}else if((abrasive_priority == 9) || ((abrasive_priority == 8) || (abrasive_priority == 7))){
		//青道路
		if(binary_code[0][0] == 1){
			//黒い研磨材が優先
			dispenser_recovery(0);
		}else{
			//青い研磨材が優先
			dispenser_recovery(1);
		}
	}else{
		//緑道路
		if(binary_code[1][0] == 1){
			//黒い研磨材が優先
			dispenser_recovery(0);
		}else{
			//青い研磨材が優先
			dispenser_recovery(1);
		}
	}

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
	
	switch(abrasive_priority){
		case 1:
			/* 青緑1研磨座撒く */
			gyro_deceleration(1700, gyro_angle_standard, 0, 0);
			gyro_angle_standard += rotation(90, gyro_angle_standard);
			gyro_deceleration(1610, gyro_angle_standard, 0, 0);
			gyro_angle_standard += rotation(90, gyro_angle_standard);
			gyro_angle_standard = wall_fix(1000);
			put_green_blue_3_to_1();
			move_green_blue_1_to_red_yellow_3(0);

			gyro_deceleration(1700, gyro_angle_standard, 0, 0);
			gyro_angle_standard += rotation(90, gyro_angle_standard);
			gyro_deceleration(1500, gyro_angle_standard, 0, 0);
			gyro_angle_standard += rotation(90, gyro_angle_standard);
			gyro_angle_standard = wall_fix(1000);
			put_green_blue_3_to_1();

			gyro_deceleration(100, gyro_angle_standard, 0, 0);
			gyro_angle_standard += rotation(90, gyro_angle_standard);
			gyro_deceleration(500, gyro_angle_standard, -1, 0);
			line_color(0, 0);
			goal(0);
			break;
		case 2:
			/* 青黄1研磨座撒く */
			gyro_deceleration(120, gyro_angle_standard, 0, 0);
			gyro_angle_standard += rotation(90, gyro_angle_standard);
			gyro_deceleration(3450, gyro_angle_standard, 0, 0);
			gyro_angle_standard += rotation(-90, gyro_angle_standard);
			gyro_deceleration(1250, gyro_angle_standard, -1, 0);
			line_color(0, 0);
			gyro_angle_standard += rotation(-180, gyro_angle_standard);
			gyro_angle_standard = wall_fix(1000);

			put_red_yellow_3_to_1(-1, 3, -1);

			put_green_blue_3_to_1();
			gyro_deceleration(100, gyro_angle_standard, 0, 0);
			gyro_angle_standard += rotation(90, gyro_angle_standard);
			gyro_deceleration(600, gyro_angle_standard, -1, 0);
			line_color(0, 0);
			goal(0);
			break;
		case 3:
			/* 青赤1研磨座撒く */
			put_red_yellow_3_to_1(1, 1, -1);
			gyro_deceleration(1700, gyro_angle_standard, 0, 0);
			gyro_angle_standard += rotation(90, gyro_angle_standard);
			gyro_deceleration(1550, gyro_angle_standard, 0,0);
			gyro_angle_standard += rotation(90, gyro_angle_standard);
			gyro_angle_standard = wall_fix(1000);

			put_green_blue_3_to_1();
			gyro_deceleration(100, gyro_angle_standard, 0, 0);
			gyro_angle_standard += rotation(90, gyro_angle_standard);
			gyro_deceleration(600, gyro_angle_standard, -1, 0);
			line_color(0, 0);
			goal(0);
			break;
		case 4:
			/* 緑黄1研磨座撒く */
			gyro_deceleration(1700, gyro_angle_standard, 0, 0);
			gyro_angle_standard += rotation(90, gyro_angle_standard);
			gyro_deceleration_power(85, gyro_angle_standard, 0);
			gyro_deceleration(1500, gyro_angle_standard, 0, 0);
			gyro_angle_standard += rotation(90, gyro_angle_standard);
			gyro_angle_standard = wall_fix(1000);
			put_green_blue_3_to_1();
			move_green_blue_1_to_red_yellow_3(0);
			put_red_yellow_3_to_1(0, 0, 0);
			goal(0);
			break;
		case 5:
			/* 緑赤1研磨座撒く */
			gyro_deceleration(1700, gyro_angle_standard, 0, 0);
			gyro_angle_standard += rotation(90, gyro_angle_standard);
			gyro_deceleration_power(85, gyro_angle_standard, 0);
			gyro_deceleration(1500, gyro_angle_standard, 0, 0);
			gyro_angle_standard += rotation(90, gyro_angle_standard);
			gyro_angle_standard = wall_fix(1000);
			put_green_blue_3_to_1();
			move_green_blue_1_to_red_yellow_3(0);

			gyro_deceleration(120, gyro_angle_standard, 0, 0);
			gyro_angle_standard += rotation(90, gyro_angle_standard);
			gyro_deceleration(700, gyro_angle_standard, 0, 0);
			put_red_yellow_1_to_3(1);
			gyro_deceleration(200, gyro_angle_standard, 0, 0);
			gyro_angle_standard += rotation(90, gyro_angle_standard);
			gyro_deceleration(100, gyro_angle_standard, -1, 0);
			line_color(0, 0);
			goal(0);
			break;
		case 6:
			/* 黄赤1研磨座撒く */
			put_red_yellow_3_to_1(-1, 1, -1);
			put_red_yellow_3_to_1(-1, 1, 0);
			goal(0);
			break;
		case 7:
			/* 青緑2研磨座撒く */
			gyro_deceleration(120, gyro_angle_standard, 0, 0);
			gyro_angle_standard += rotation(90, gyro_angle_standard);
			gyro_deceleration(2000, gyro_angle_standard, 0, 0);
			gyro_angle_standard += rotation(-90, gyro_angle_standard);
			gyro_angle_standard = wall_fix(1000);
			put_green_blue_3_to_1();
			move_green_blue_1_to_red_yellow_3(1);
			put_red_yellow_3_to_1(-1, 1, 0);
			goal(0);
			break;
		case 8:
			/* 青黄2研磨座撒く */
			gyro_deceleration(120, gyro_angle_standard, 0, 0);
			gyro_angle_standard += rotation(90, gyro_angle_standard);
			gyro_deceleration(2000, gyro_angle_standard, 0, 0);
			gyro_angle_standard += rotation(-90, gyro_angle_standard);
			gyro_angle_standard = wall_fix(1000);
			put_green_blue_3_to_1();
			move_green_blue_1_to_red_yellow_3(1);

			gyro_deceleration(120, gyro_angle_standard, 0, 0);
			gyro_angle_standard += rotation(90, gyro_angle_standard);
			gyro_deceleration(400, gyro_angle_standard, 0, 0);
			put_red_yellow_1_to_3(0);
			goal(1);
			break;
		case 9:
			/* 青赤2研磨座撒く */
			gyro_deceleration(120, gyro_angle_standard, 0, 0);
			gyro_angle_standard += rotation(90, gyro_angle_standard);
			gyro_deceleration(2000, gyro_angle_standard, 0, 0);
			gyro_angle_standard += rotation(-90, gyro_angle_standard);
			gyro_angle_standard = wall_fix(1000);

			put_green_blue_3_to_1();
			
			move_green_blue_1_to_red_yellow_3(1);

			put_red_yellow_3_to_1(-1, 1, 0);
			goal(0);
			break;
		case 10:
			/* 緑黄2研磨座撒く */
			gyro_deceleration(120, gyro_angle_standard, 0, 0);
			gyro_angle_standard += rotation(90, gyro_angle_standard);
			gyro_deceleration(300, gyro_angle_standard, 0, 0);
			put_red_yellow_1_to_3(0);

			gyro_angle_standard += rotation(180, gyro_angle_standard);
			gyro_angle_standard = wall_fix(1000);
			gyro_deceleration(120, gyro_angle_standard, 0, 0);
			gyro_angle_standard += rotation(90, gyro_angle_standard);
			gyro_deceleration(1900, gyro_angle_standard, 0, 0);
			gyro_angle_standard += rotation(-90, gyro_angle_standard);
			gyro_angle_standard = wall_fix(1000);
			put_green_blue_3_to_1();

			gyro_deceleration(100, gyro_angle_standard, 0, 0);
			gyro_angle_standard += rotation(90, gyro_angle_standard);
			gyro_deceleration(100, gyro_angle_standard, -1, 0);
			line_color(0, 0);
			goal(0);
			break;
		case 11:
			/* 緑赤2研磨座撒く */
			put_red_yellow_3_to_1(1, 3, -1);
			put_green_blue_3_to_1();

			gyro_angle_standard += rotation(90, gyro_angle_standard);
			gyro_deceleration(100, gyro_angle_standard, -1, 0);
			line_color(0, 0);
			goal(0);
			break;
		case 12:
			/* 黄赤2研磨座撒く */
			put_red_yellow_3_to_1(-1, 1, -1);

			gyro_deceleration(1700, gyro_angle_standard, 0, 0);
			gyro_angle_standard += rotation(90, gyro_angle_standard);
			gyro_deceleration(3000, gyro_angle_standard, -1, 0);
			line_color(0, 0);
			gyro_angle_standard += rotation(90, gyro_angle_standard);
			gyro_angle_standard = wall_fix(1000);
			gyro_deceleration(120, gyro_angle_standard ,0, 0);
			gyro_angle_standard += rotation(90, gyro_angle_standard);
			gyro_deceleration(400, gyro_angle_standard, 0, 0);
			put_red_yellow_1_to_3(0);
			goal(1);
			break;
	}
	return 0;
}

/********************************************************************************************************************************************
 *	計算通りに雪と車回収
 ********************************************************************************************************************************************/
void collection_road_red(int collection_road_priority){
	switch(collection_road_priority){
		case 1:
			/* 赤道路 */
			gyro_deceleration(100, gyro_angle_standard, 0, 0);
			ev3_motor_reset_counts(B_MOTOR);
			ev3_motor_set_power(B_MOTOR, -85);
			while(-160<=ev3_motor_get_counts(B_MOTOR));
			BRAKE(B_MOTOR);
			ev3_motor_reset_counts(C_MOTOR);
			ev3_motor_set_power(C_MOTOR, 85);
			while(160>=ev3_motor_get_counts(C_MOTOR));
			BRAKE(C_MOTOR);

			collection_red_1_to_3();
			break;
		case 2:
			/* 黄道路 */
			gyro_deceleration(-750, gyro_angle_standard, 0, 0);
			ev3_motor_reset_counts(B_MOTOR);
			ev3_motor_set_power(B_MOTOR, 85);
			while(180>=ev3_motor_get_counts(B_MOTOR));
			BRAKE(B_MOTOR);
			ev3_motor_reset_counts(C_MOTOR);
			ev3_motor_set_power(C_MOTOR, -85);
			while(-180<=ev3_motor_get_counts(C_MOTOR));
			BRAKE(C_MOTOR);

			gyro_deceleration_power(-85, gyro_angle_standard, 0);
			line_color(0, -1);
			gyro_deceleration(-130, gyro_angle_standard, 0, 0);
			gyro_angle_standard += rotation(-90, gyro_angle_standard);
			gyro_angle_standard = wall_fix(1000);
			collection_yellow_3_to_1();
			break;
		case 3:
			/* 緑道路 */
			gyro_deceleration(100, gyro_angle_standard, 0, 0);
			ev3_motor_reset_counts(B_MOTOR);
			ev3_motor_set_power(B_MOTOR, -85);
			while(-180<=ev3_motor_get_counts(B_MOTOR));
			BRAKE(B_MOTOR);
			ev3_motor_reset_counts(C_MOTOR);
			ev3_motor_set_power(C_MOTOR, 85);
			while(180>=ev3_motor_get_counts(C_MOTOR));
			BRAKE(C_MOTOR);

			gyro_deceleration(500, gyro_angle_standard, 0, 0);
			gyro_angle_standard += rotation(-90, gyro_angle_standard);
			gyro_angle_standard = wall_fix(1000);
			collection_green_3_to_1();
			break;
		default:
			/* 0値 */
			break;
			sprintf(str, "Error");
			ev3_lcd_draw_string(str,0,str_p*10);
			str_p += 1;
			break;
	}
}

/********************************************************************************************************************************************
 *	計算通りに雪と車回収
 ********************************************************************************************************************************************/
void collection_road_yello(int collection_road_priority){
	switch(collection_road_priority){
		case 1:
			/* 赤道路 */
			collection_red_3_to_1();
			break;
		case 2:
			/* 黄道路 */
			gyro_deceleration(120, gyro_angle_standard, 0, 0);
			gyro_angle_standard += rotation(90, gyro_angle_standard);
			gyro_deceleration(430, gyro_angle_standard, 0, 0);
			collection_yellow_1_to_3();
			break;
		case 3:
			/* 緑道路 */
			gyro_deceleration(1700, gyro_angle_standard, 0, 0);
			gyro_angle_standard += rotation(90, gyro_angle_standard);
			gyro_deceleration(1700, gyro_angle_standard, 0, 0);
			gyro_angle_standard += rotation(90, gyro_angle_standard);
			gyro_angle_standard = wall_fix(1000);
			collection_green_3_to_1();
			break;
		case 4:
			/* 青道路 */
			gyro_deceleration(120, gyro_angle_standard, 0, 0);
			gyro_angle_standard += rotation(90, gyro_angle_standard);
			gyro_deceleration(2000, gyro_angle_standard, 0, 0);
			gyro_angle_standard += rotation(-90, gyro_angle_standard);
			gyro_angle_standard = wall_fix(500);
			collection_blue_3_to_1();
			break;
		default:
			/* 0値 */
			break;
			sprintf(str, "Error");
			ev3_lcd_draw_string(str,0,str_p*10);
			str_p += 1;
	}
}



/********************************************************************************************************************************************
 *	緑道路車雪回収 3-1
 ********************************************************************************************************************************************/
int collection_green_3_to_1(void){
	roda_f = 3;
	int tire_angul = 0;
	// a_arm_reset(true);
	a_arm(A_ARM_C);
	tire_angul += 300;
	gyro_deceleration(tire_angul, gyro_angle_standard, 0, 0);
	gyro_angle_standard += rotation(-45, gyro_angle_standard);
	gyro_deceleration_power(85, gyro_angle_standard, 0);
	a_arm_reset(false);
	tire_angul += 0;
	while(tire_angul>=ev3_motor_get_counts(C_MOTOR));
	a_arm_reset(true);
	tire_angul = ev3_motor_get_counts(C_MOTOR);
	tire_angul += 100;
	while(tire_angul>=ev3_motor_get_counts(C_MOTOR));
	gyro_deceleration(150, gyro_angle_standard, 0, 0);
	a_arm_reset(false);
	gyro_deceleration(150, gyro_angle_standard, 0, 0);
	gyro_angle_standard += rotation(-45, gyro_angle_standard);
	a_arm_reset(true);
	gyro_deceleration(350, gyro_angle_standard, -1, 0);
	line_color(0, 0);
	a_arm(230);
	ev3_motor_set_power(B_MOTOR,-30);
	while(-70<=(ev3_gyro_sensor_get_angle(GYRO_4)-gyro_angle_standard));
	perfect_BRAKE();
	gyro_angle_standard -= 90;
	gyro_deceleration(700, gyro_angle_standard, 0, 0);

	snow_put(1);
	return 0;
}


/********************************************************************************************************************************************
 *	青道路車雪回収 3-1
 ********************************************************************************************************************************************/
int collection_blue_3_to_1(void){
	roda_f = 4;
	int tire_angul = 0;
	// a_arm_reset(true);
	a_arm(A_ARM_C);
	tire_angul += 300;
	gyro_deceleration(tire_angul, gyro_angle_standard, 0, 0);
	gyro_angle_standard += rotation(-45, gyro_angle_standard);
	gyro_deceleration_power(85, gyro_angle_standard, 0);
	a_arm_reset(false);
	tire_angul += 0;
	while(tire_angul>=ev3_motor_get_counts(C_MOTOR));
	a_arm_reset(true);
	tire_angul = ev3_motor_get_counts(C_MOTOR);
	tire_angul += 100;
	while(tire_angul>=ev3_motor_get_counts(C_MOTOR));
	gyro_deceleration(150, gyro_angle_standard, 0, 0);
	a_arm_reset(false);
	gyro_deceleration(150, gyro_angle_standard, 0, 0);
	gyro_angle_standard += rotation(-45, gyro_angle_standard);
	a_arm(A_ARM_C);
	gyro_deceleration(350, gyro_angle_standard, -1, 0);
	line_color(0, 0);
	ev3_motor_set_power(B_MOTOR,-30);
	while(-70<=(ev3_gyro_sensor_get_angle(GYRO_4)-gyro_angle_standard));
	perfect_BRAKE();
	gyro_angle_standard -= 90;
	gyro_deceleration_power(85, gyro_angle_standard, 0);
	a_arm_reset(false);
	tire_angul = ev3_motor_get_counts(C_MOTOR);
	gyro_deceleration((700-tire_angul), gyro_angle_standard, 0, 0);

	a_arm_reset(true);
	// gyro_deceleration(50, gyro_angle_standard, 0, 0);
	gyro_angle_standard += rotation(90, gyro_angle_standard);
	gyro_deceleration(500, gyro_angle_standard, -1, 0);
	line_color(0, 0);

	snow_put(3);
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
	a_arm(A_ARM_C);
	tire_angul += 180;
	while(tire_angul>=ev3_motor_get_counts(C_MOTOR));
	ev3_stp_cyc(GYROTRACE_TASK_4);
	ev3_motor_set_power(B_MOTOR, -10);
	ev3_motor_set_power(C_MOTOR, 30);
	while(gyro_angle_standard+75>=ev3_gyro_sensor_get_angle(GYRO_4));
	perfect_BRAKE();
	tslp_tsk(500);
	// d_motor_car_open(false);
	gyro_angle_standard += 90;
	gyro_deceleration_power(30, gyro_angle_standard, -1);
	ev3_sta_cyc(GYROTRACE_TASK_4);
	a_arm_reset(false);
	tire_angul += 600;
	while(tire_angul>=ev3_motor_get_counts(C_MOTOR));
	a_arm(A_ARM_C);
	tire_angul += 700;
	while(tire_angul>=ev3_motor_get_counts(C_MOTOR));
	a_arm_reset(true);
	tire_angul += 480;
	while(tire_angul>=ev3_motor_get_counts(C_MOTOR));
	a_arm_reset(false);
	gyro_deceleration(750, gyro_angle_standard, 0, -1);
	a_arm(A_ARM_C);
	// d_motor_car_close(1);

	ev3_motor_reset_counts(C_MOTOR);
	ev3_motor_set_power(C_MOTOR, 85);
	while(200>=ev3_motor_get_counts(C_MOTOR));
	BRAKE(C_MOTOR);
	ev3_motor_reset_counts(B_MOTOR);
	ev3_motor_set_power(B_MOTOR, -85);
	while(-200<=ev3_motor_get_counts(B_MOTOR));
	BRAKE(B_MOTOR);

	gyro_deceleration(900, gyro_angle_standard, -1, 0);
	line_color(0, 0);

	snow_put(3);
	return 0;
}

/********************************************************************************************************************************************
 *	黄道路車雪回収1-3
 ********************************************************************************************************************************************/
int collection_yellow_1_to_3(void){
	roda_f = 2;
	int tire_angul = 0;
	gyro_deceleration_power(85, gyro_angle_standard, 0);
	a_arm_reset(true);
	tire_angul += 900;
	while(tire_angul>=ev3_motor_get_counts(C_MOTOR));
	a_arm_reset(false);
	tire_angul += 200;
	while(tire_angul>=ev3_motor_get_counts(C_MOTOR));
	a_arm(A_ARM_C);
	tire_angul += 1350;
	while(tire_angul>=ev3_motor_get_counts(C_MOTOR));
	a_arm(50);
	tire_angul += 300;
	while(tire_angul>=ev3_motor_get_counts(C_MOTOR));
	a_arm(A_ARM_C);
	gyro_deceleration(150, gyro_angle_standard, 0, 0);
	gyro_angle_standard += rotation(-90, gyro_angle_standard);
	a_arm_reset(true);
	gyro_deceleration_power(85, gyro_angle_standard, 0);
	tire_angul = 200;
	while(tire_angul>=ev3_motor_get_counts(C_MOTOR));
	a_arm_reset(false);
	tire_angul += 500;
	while(tire_angul>=ev3_motor_get_counts(C_MOTOR));
	a_arm_reset(true);
	gyro_deceleration(150, gyro_angle_standard, 0, 0);
	a_arm_reset(false);
	gyro_deceleration_power(85, gyro_angle_standard, 0);
	line_color(0, -1);
	gyro_deceleration(250, gyro_angle_standard, 0, 0);
	gyro_angle_standard += rotation(-90, gyro_angle_standard);
	gyro_angle_standard = wall_fix(1000);

	snow_put(0);
	return 0;
}



/********************************************************************************************************************************************
 *	赤道路車雪回収
 ********************************************************************************************************************************************/
int collection_red_3_to_1(void){
	roda_f = 1;
	int tire_angul = 0;
	gyro_deceleration(100, gyro_angle_standard, 0, 0);
	ev3_motor_reset_counts(C_MOTOR);
	ev3_motor_set_power(C_MOTOR, 85);
	while(200>=ev3_motor_get_counts(C_MOTOR));
	BRAKE(C_MOTOR);
	ev3_motor_reset_counts(B_MOTOR);
	ev3_motor_set_power(B_MOTOR, -85);
	while(-200<=ev3_motor_get_counts(B_MOTOR));
	BRAKE(B_MOTOR);
	gyro_deceleration_power(30, gyro_angle_standard, -1);
	a_arm_reset(true);
	tire_angul += 980;
	while(tire_angul >= ev3_motor_get_counts(C_MOTOR));
	a_arm(50);
	a_arm(A_ARM_C);
	tire_angul += 360;
	while(tire_angul >= ev3_motor_get_counts(C_MOTOR));
	ev3_stp_cyc(GYROTRACE_TASK_4);
	ev3_motor_set_power(B_MOTOR, -10);
	ev3_motor_set_power(C_MOTOR,  40);
	while((gyro_angle_standard+75) >= ev3_gyro_sensor_get_angle(GYRO_4));
	gyro_angle_standard += 90;
	gyro_deceleration_power(30, gyro_angle_standard, -1);
	tire_angul += 1080;
	while(tire_angul >= ev3_motor_get_counts(C_MOTOR));
	a_arm_reset(true);
	tire_angul += 500;
	while(tire_angul >= ev3_motor_get_counts(C_MOTOR));
	a_arm(50);
	tire_angul += gyro_deceleration(800, gyro_angle_standard, 0, -1);
	a_arm_reset(true);
	rotation(180, gyro_angle_standard);
	gyro_angle_standard += 177;

	gyro_deceleration_power(85, gyro_angle_standard, 0);
	a_arm(A_ARM_C);
	while(500 >= ev3_motor_get_counts(C_MOTOR));
	gyro_deceleration(1650, gyro_angle_standard, -1, 0);
	line_color(0, 0);
	a_arm_reset(false);
	snow_put(-1);
	return 0;
}

/********************************************************************************************************************************************
 *	赤道路車雪回収
 ********************************************************************************************************************************************/
int collection_red_1_to_3(void){
	roda_f = 1;
	int tire_angul = 0;
	gyro_deceleration_power(85, gyro_angle_standard, 0);
	tire_angul += 200;
	while(tire_angul >= ev3_motor_get_counts(C_MOTOR));
	a_arm_reset(false);
	tire_angul += 300;
	while(tire_angul >= ev3_motor_get_counts(C_MOTOR));
	a_arm(A_ARM_C);
	tire_angul += 700;
	while(tire_angul >= ev3_motor_get_counts(C_MOTOR));
	a_arm_reset(false);
	tire_angul += 800;
	while(tire_angul >= ev3_motor_get_counts(C_MOTOR));
	gyro_deceleration(150, gyro_angle_standard, -1, 0);

	snow_put(-1);
}


/********************************************************************************************************************************************
 *	側面道路研磨剤撒く1-3
 ********************************************************************************************************************************************/
int put_red_yellow_1_to_3(int point){
	int tire_angul = 0;
	gyro_deceleration_power(85, gyro_angle_standard, 0);
	tire_angul += 500;
	while(tire_angul>=ev3_motor_get_counts(C_MOTOR));
	kennmazai_put(0);
	if(point == 0){
		/* 黄色　長い */
		tire_angul += 2400;
	}else{
		/* 赤色　短い */
		tire_angul += 2000;
	}
	while(tire_angul>=ev3_motor_get_counts(C_MOTOR));
	gyro_deceleration(200, gyro_angle_standard, 0, 0);
	kennmazai_put(0);
	gyro_angle_standard += rotation(-90, gyro_angle_standard);
	gyro_deceleration(1000, gyro_angle_standard, -1, 0);
	line_color(0, 0);
	kennmazai_put(0);
	kennmazai_put(0);
}



/********************************************************************************************************************************************
 *	側面道路研磨剤撒く3-1
 ********************************************************************************************************************************************/
int put_red_yellow_3_to_1(int point, int level, int stop){
	int tire_angul = 0;
	gyro_deceleration(1700, gyro_angle_standard, 0, 0);
	kennmazai_put(0);
	gyro_angle_standard += rotation(90, gyro_angle_standard);
	gyro_deceleration_power(85, gyro_angle_standard, 0);
	tire_angul += 568;
	while(tire_angul>=ev3_motor_get_counts(C_MOTOR));
	kennmazai_put(0);

	if(level == 0 || level == 1){
		tire_angul += 568;
		while(tire_angul>=ev3_motor_get_counts(C_MOTOR));
		kennmazai_put(0);
		kennmazai_put(0);
		if(point == 0){
			/* 黄色　長い */
			gyro_deceleration(1650, gyro_angle_standard, -1, 0);
			line_color(0, -1);
			if(stop == 0){
				return 0;
			}
		}else{
			/* 赤色　短い */
			tire_angul += 1600;
			while(tire_angul>=ev3_motor_get_counts(C_MOTOR));
			line_color(0, -1);
			if(stop == 0){
				return 0;
			}
			gyro_deceleration(300, gyro_angle_standard, 0, 0);
		}
		if(level == 1){
			gyro_angle_standard += rotation(90, gyro_angle_standard);
			gyro_angle_standard = wall_fix(1000);
		}
	}else if(level == 2 || level == 3){
		gyro_deceleration(500, gyro_angle_standard, 0, 0);
		kennmazai_put(0);
		kennmazai_put(0);
		if(level == 3){
			gyro_angle_standard += rotation(90, gyro_angle_standard);
			gyro_angle_standard = wall_fix(1000);
		}
		
	}
}


/********************************************************************************************************************************************
 *	色読み
 ********************************************************************************************************************************************/
int line_color(int l_color, int stop){
	if(l_color == 0){
		//黒色読み
		while((BRAKE_REFLECTED+WHITE_REFLECTED)/2 < ev3_color_sensor_get_reflect(COLOR_1));
		if(stop == 0){
			perfect_BRAKE();
		}
		tone_line();
	}
}


/********************************************************************************************************************************************
 *	黄道路研磨剤撒く3-1
 ********************************************************************************************************************************************/
int put_yellow_3_to_1(int);


/********************************************************************************************************************************************
 *	線からゴールまで
 ********************************************************************************************************************************************/
int goal(int point){
	if(point == 0){
		//右回転
		gyro_deceleration(200, gyro_angle_standard, 0, 0);
		ev3_motor_set_power(C_MOTOR, 85);
		gyro_angle_standard += 90;
		while((gyro_angle_standard-15) >= ev3_gyro_sensor_get_angle(GYRO_4));
		gyro_angle_standard = wall_fix(1000);
	}else{
		//左回転
		gyro_deceleration(150, gyro_angle_standard, 0, 0);
		ev3_motor_set_power(B_MOTOR, -85);
		gyro_angle_standard += -90;
		while((gyro_angle_standard+15) <= ev3_gyro_sensor_get_angle(GYRO_4));
		gyro_angle_standard = wall_fix(1000);
	}
}





/********************************************************************************************************************************************
 *	緑to青道路研磨剤撒く3-1
 ********************************************************************************************************************************************/
int put_green_blue_3_to_1(void){
	int tire_angul = 0;
	// a_arm_reset(true);
	a_arm(A_ARM_C);
	gyro_deceleration(100, gyro_angle_standard, 0, 0);
	gyro_angle_standard += rotation(-45, gyro_angle_standard);
	gyro_deceleration_power(85, gyro_angle_standard, 0);
	tire_angul += 500;
	while(tire_angul>=ev3_motor_get_counts(C_MOTOR));
	kennmazai_put(0);
	tire_angul += 650;
	while(tire_angul>=ev3_motor_get_counts(C_MOTOR));
	gyro_deceleration(150, gyro_angle_standard, 0, 0);
	gyro_angle_standard += rotation(-45, gyro_angle_standard);
	gyro_deceleration(350, gyro_angle_standard, -1, 0);
	line_color(0, 0);
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
 *	緑道路研磨剤撒く1-3
 ********************************************************************************************************************************************/
int move_green_blue_1_to_red_yellow_3(int point){
	gyro_angle_standard += rotation(90, gyro_angle_standard);
	if(point == 0){
		//黄色
		gyro_deceleration_power(85, gyro_angle_standard, 0);
		while(150 >= ev3_motor_get_counts(C_MOTOR));
		line_color(0, -1);
		gyro_deceleration(250, gyro_angle_standard, 0, 0);
	}else{
		//赤色
		gyro_deceleration(500, gyro_angle_standard, -1, 0);
		line_color(0, 0);
	}
	gyro_angle_standard += rotation(90, gyro_angle_standard);
	gyro_angle_standard = wall_fix(1000);
}

/********************************************************************************************************************************************
 *	雪置く
 ********************************************************************************************************************************************/
int snow_put(int rodo_c){
	int tire_angul = 0;
	if(rodo_c == 0 || rodo_c == 1){
		if(rodo_c == 0){
			//黄道路
			gyro_deceleration_power(85, gyro_angle_standard, 0);
			tire_angul += 500;
			while(tire_angul>=ev3_motor_get_counts(C_MOTOR));
			a_arm(A_ARM_C);
			tire_angul += 2000;
			while(tire_angul>=ev3_motor_get_counts(C_MOTOR));
			gyro_deceleration(1000, gyro_angle_standard, -1, 0);
			line_color(0, 0);
		}else{
			//緑道路
			gyro_deceleration(50, gyro_angle_standard, 0, 0);
			gyro_angle_standard += rotation(90, gyro_angle_standard);
			gyro_deceleration(200, gyro_angle_standard, -1, 0);
			line_color(0, 0);
			gyro_angle_standard += rotation(100, gyro_angle_standard);
			gyro_angle_standard = wall_fix(500);
			gyro_deceleration(120, gyro_angle_standard, 0, 0);
			gyro_angle_standard += rotation(90, gyro_angle_standard);

			gyro_deceleration_power(85, gyro_angle_standard, 0);
			tire_angul += 500;
			while(tire_angul>=ev3_motor_get_counts(C_MOTOR));
			a_arm(A_ARM_C);
			tire_angul += 2000;
			while(tire_angul>=ev3_motor_get_counts(C_MOTOR));
			gyro_deceleration(700, gyro_angle_standard, -1, 0);
			line_color(0, 0);
		}
		a_arm(50);
	}else{
		if(rodo_c == 2){
			//青道路
			a_arm_reset(true);
			// gyro_deceleration(50, gyro_angle_standard, 0, 0);
			gyro_angle_standard += rotation(90, gyro_angle_standard);
			gyro_deceleration(500, gyro_angle_standard, -1, 0);
			line_color(0, 0);
			gyro_angle_standard += rotation(100, gyro_angle_standard);
			gyro_angle_standard = wall_fix(1000);

			gyro_deceleration_power(85, gyro_angle_standard, 0);
			tire_angul += 500;
			while(tire_angul>=ev3_motor_get_counts(C_MOTOR));
			a_arm(A_ARM_C);
			tire_angul += 500;
			while(tire_angul>=ev3_motor_get_counts(C_MOTOR));
			gyro_deceleration(600, gyro_angle_standard, 0, 0);
			gyro_angle_standard += rotation(-90, gyro_angle_standard);
			gyro_deceleration(100, gyro_angle_standard, 0, 0);
		}else{
			line_color(0, 0);
			a_arm(50);
		}
	}
	if(3 == rodo_c){
		a_arm_reset(true);
		rotation(95, gyro_angle_standard);
		gyro_angle_standard += 90;
		gyro_angle_standard = wall_fix(1000);

		gyro_deceleration(200, gyro_angle_standard, 0, 0);
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
		line_color(0, 0);
	}
	gyro_deceleration(-200, gyro_angle_standard, 0, 0);
	gyro_angle_standard += rotation(-90, gyro_angle_standard);
	// ev3_motor_reset_counts(B_MOTOR);
	// ev3_motor_set_power(B_MOTOR, -85);
	// while((gyro_angle_standard-80) <= ev3_gyro_sensor_get_angle(GYRO_4));
	// gyro_angle_standard += -90;
	a_arm(A_ARM_C);
	gyro_deceleration_power(85, gyro_angle_standard, 0);
	a_arm_reset(false);
	a_arm(A_ARM_C);
	while(1000 >= ev3_motor_get_counts(C_MOTOR));
	gyro_deceleration(500, gyro_angle_standard, 0, 0);
	gyro_angle_standard += rotation(90, gyro_angle_standard);
	gyro_deceleration(100, gyro_angle_standard, -1, 0);
	line_color(0, 0);
	gyro_angle_standard += rotation(90, gyro_angle_standard);
	gyro_angle_standard = wall_fix(1000);
}

void dispenser_recovery(int b_b){
	/* 一つ目 */
	if(b_b == 0){
		//黒先回収
		gyro_deceleration(550, gyro_angle_standard, 0, -1);
	}else{
		//青先回収
		gyro_deceleration(880, gyro_angle_standard, 0, -1);		
	}
	perfect_BRAKE();
	tslp_tsk(500);

	gyro_angle_standard += rotation(-90, gyro_angle_standard);
	dispenser();
	gyro_deceleration(-200, gyro_angle_standard, 0, -1);

	perfect_BRAKE();
	tslp_tsk(500);

	/* 二つ目 */
	if(b_b == 0){
		//黒先回収
		gyro_angle_standard += rotation(90, gyro_angle_standard);
		gyro_deceleration(340, gyro_angle_standard, 0, -1);
		gyro_angle_standard += rotation(-90, gyro_angle_standard);
	}else{
		//青先回収
		gyro_angle_standard += rotation(-90, gyro_angle_standard);
		gyro_deceleration(340, gyro_angle_standard, 0, -1);
		gyro_angle_standard += rotation(90, gyro_angle_standard);
	}
	gyro_deceleration(1, gyro_angle_standard, -1, -1);
	dispenser();
	gyro_deceleration(-200, gyro_angle_standard, 0, -1);
	perfect_BRAKE();
	gyro_angle_standard += rotation(90, gyro_angle_standard);
	gyro_deceleration(-500, gyro_angle_standard, -1, 0);
	perfect_BRAKE();
	gyro_angle_standard = wall_fix(1000);
}


void dispenser(void){
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
}


void gyro_log(void){
	sprintf(str, "GYRO=%5d",(ev3_gyro_sensor_get_angle(GYRO_4)-gyro_angle_standard));
	ev3_lcd_draw_string(str,0,0);
}



