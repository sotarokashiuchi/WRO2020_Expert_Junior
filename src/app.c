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
*/

/* 関数のプロトタイプ宣言 */
int WRO(void);
int broken_line(int tire_angul,int tire_brake);
int collection_red_3_to_1(void);
int collection_yellow_3_to_1(void);
int collection_blue_3_to_1(void);
int collection_green_3_to_1(void);
int collection_red_1_to_3(void);
int put_red_yellow_1_to_3(int);
int put_red_yellow_3_to_1(int, int);
int put_yellow_1_to_3(int);
int put_yellow_3_to_1(int);
int put_blue_1_to_3(int);
int put_green_1_to_3(int);
int put_green_blue_3_to_1(void);
int car_put(void);
void dispenser_recovery(int);
void dispenser(void);

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

	gyro_angle_standard = wall_fix(1000);
	a_arm_reset(false);
	a_arm(165);

	// ev3_lcd_set_font(1);

	/* 実験スペース */
	// ev3_sta_cyc(GYRO_LOG_TASK_10);

	// ev3_lcd_set_font(1);

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

	/********************************************************************************************************************************************
	 *	研磨剤回収
	 ********************************************************************************************************************************************/
	if(sta_point == 1){
		//黄
		gyro_deceleration(-700, gyro_angle_standard, 0, 0);
		rotation(-90, gyro_angle_standard);
		gyro_angle_standard = wall_fix(1000);
	}else{
		//赤
		gyro_deceleration(-1050, gyro_angle_standard, 0, 0);
		rotation(-90, gyro_angle_standard);
		gyro_angle_standard = wall_fix(1000);
		int tire_angul = 0;
		gyro_deceleration(1700, gyro_angle_standard, 0, 0);
		rotation(90, gyro_angle_standard);
		gyro_angle_standard += 90;
		gyro_deceleration(3100, gyro_angle_standard, -1, 0);
		while((BRAKE_REFLECTED+WHITE_REFLECTED)/2 < ev3_color_sensor_get_reflect(COLOR_1));
		perfect_BRAKE();
		tone_line();
		rotation(90, gyro_angle_standard);
		gyro_angle_standard = wall_fix(1000);
	}

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
			rotation(90, gyro_angle_standard);
			gyro_angle_standard += 90;
			gyro_deceleration(1610, gyro_angle_standard, 0, 0);
			rotation(90, gyro_angle_standard);
			gyro_angle_standard = wall_fix(1000);

			put_green_blue_3_to_1();

			rotation(90, gyro_angle_standard);
			gyro_angle_standard += 90;
			gyro_deceleration_power(85, gyro_angle_standard, 0);
			while(150 >= ev3_motor_get_counts(C_MOTOR));
			while((BRAKE_REFLECTED+WHITE_REFLECTED)/2 < ev3_color_sensor_get_reflect(COLOR_1));
			gyro_deceleration(250, gyro_angle_standard, 0, 0);
			rotation(90, gyro_angle_standard);
			gyro_angle_standard = wall_fix(1000);
			gyro_deceleration(1700, gyro_angle_standard, 0, 0);
			rotation(90, gyro_angle_standard);
			gyro_angle_standard += 90;
			gyro_deceleration(1500, gyro_angle_standard, 0, 0);
			rotation(90, gyro_angle_standard);
			gyro_angle_standard = wall_fix(1000);
			put_green_blue_3_to_1();
			rotation(90, gyro_angle_standard);
			gyro_angle_standard += 90;
			gyro_deceleration(500, gyro_angle_standard, 0, 0);
			break;
		case 2:
			/* 青黄1研磨座撒く */
			gyro_deceleration(120, gyro_angle_standard, 0, 0);
			rotation(90, gyro_angle_standard);
			gyro_angle_standard += 90;
			gyro_deceleration(3400, gyro_angle_standard, 0, 0);
			rotation(-90, gyro_angle_standard);
			gyro_angle_standard += -90;
			gyro_deceleration(1250, gyro_angle_standard, -1, 0);
			while((BRAKE_REFLECTED+WHITE_REFLECTED)/2 < ev3_color_sensor_get_reflect(COLOR_1));
			perfect_BRAKE();
			tone_line();
			rotation(180, gyro_angle_standard);
			gyro_angle_standard = wall_fix(1000);

			put_red_yellow_3_to_1(-1, 3);

			put_green_blue_3_to_1();
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
			put_red_yellow_3_to_1(-1, 1);

			put_red_yellow_3_to_1(-1, 1);
			break;

		case 7:
			/* 青緑2研磨座撒く */
			gyro_deceleration(120, gyro_angle_standard, 0, 0);
			rotation(90, gyro_angle_standard);
			gyro_angle_standard += 90;
			gyro_deceleration(2000, gyro_angle_standard, 0, 0);
			rotation(-90, gyro_angle_standard);
			gyro_angle_standard += -90;
			gyro_angle_standard = wall_fix(1000);
			put_green_blue_3_to_1();

			rotation(90, gyro_angle_standard);
			gyro_angle_standard += 90;
			gyro_deceleration(500, gyro_angle_standard, -1, 0);
			while((BRAKE_REFLECTED+WHITE_REFLECTED)/2 < ev3_color_sensor_get_reflect(COLOR_1));
			perfect_BRAKE();
			tone_line();
			rotation(90, gyro_angle_standard);
			gyro_angle_standard = wall_fix(1000);

			put_red_yellow_3_to_1(-1, 1);
			gyro_deceleration(250, gyro_angle_standard, 0, 0);
			rotation(-90, gyro_angle_standard);
			gyro_angle_standard = wall_fix(1000);
			break;
		case 8:
			/* 青黄2研磨座撒く */
			gyro_deceleration(120, gyro_angle_standard, 0, 0);
			rotation(90, gyro_angle_standard);
			gyro_angle_standard += 90;
			gyro_deceleration(2000, gyro_angle_standard, 0, 0);

			rotation(-90, gyro_angle_standard);
			gyro_angle_standard = wall_fix(1000);
			
			put_green_blue_3_to_1();
			rotation(90, gyro_angle_standard);
			gyro_angle_standard += 90;
			gyro_deceleration(500, gyro_angle_standard, -1, 0);
			while((BRAKE_REFLECTED+WHITE_REFLECTED)/2 < ev3_color_sensor_get_reflect(COLOR_1));
			perfect_BRAKE();
			tone_line();
			rotation(90, gyro_angle_standard);
			gyro_angle_standard = wall_fix(1000);

			gyro_deceleration(120, gyro_angle_standard, 0, 0);
			rotation(90, gyro_angle_standard);
			gyro_angle_standard += 90;
			gyro_deceleration(300, gyro_angle_standard, 0, 0);
			put_red_yellow_1_to_3(0);
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
			put_red_yellow_3_to_1(-1, 1);

			gyro_deceleration(1700, gyro_angle_standard, 0, 0);
			rotation(90, gyro_angle_standard);
			gyro_angle_standard += 90;
			gyro_deceleration(3000, gyro_angle_standard, -1, 0);
			while((BRAKE_REFLECTED+WHITE_REFLECTED)/2 < ev3_color_sensor_get_reflect(COLOR_1));
			perfect_BRAKE();
			tone_line();
			rotation(90, gyro_angle_standard);
			gyro_angle_standard = wall_fix(1000);

			gyro_deceleration(120, gyro_angle_standard,0, 0);
			rotation(90, gyro_angle_standard);
			gyro_angle_standard += 90;
			
			gyro_deceleration(350, gyro_angle_standard, 0, 0);
			put_red_yellow_1_to_3(0);
			break;
	}


	return 0;
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
	rotation(-90, gyro_angle_standard);
	gyro_angle_standard += -90;
	gyro_deceleration(1100, gyro_angle_standard, -1, 0);
	while((BRAKE_REFLECTED+WHITE_REFLECTED)/2 < ev3_color_sensor_get_reflect(COLOR_1));
	perfect_BRAKE();
	tone_line();
	kennmazai_put(0);
	kennmazai_put(0);
}



/********************************************************************************************************************************************
 *	側面道路研磨剤撒く3-1
 ********************************************************************************************************************************************/
int put_red_yellow_3_to_1(int point, int level){
	int tire_angul = 0;
	gyro_deceleration(1700, gyro_angle_standard, 0, 0);
	kennmazai_put(0);
	rotation(90, gyro_angle_standard);
	gyro_angle_standard += 90;
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
			while((BRAKE_REFLECTED+WHITE_REFLECTED)/2 < ev3_color_sensor_get_reflect(COLOR_1));
			perfect_BRAKE();
			tone_line();
		}else{
			/* 赤色　短い */
			tire_angul += 1600;
			while(tire_angul>=ev3_motor_get_counts(C_MOTOR));
			while((BRAKE_REFLECTED+WHITE_REFLECTED)/2 < ev3_color_sensor_get_reflect(COLOR_1));
			tone_line();
			gyro_deceleration(300, gyro_angle_standard, 0, 0);
		}
		if(level == 1){
			rotation(90, gyro_angle_standard);
			gyro_angle_standard = wall_fix(1000);
		}
	}else if(level == 2 || level == 3){
		gyro_deceleration(500, gyro_angle_standard, 0, 0);
		kennmazai_put(0);
		kennmazai_put(0);
		if(level == 3){
			rotation(90, gyro_angle_standard);
			gyro_angle_standard = wall_fix(1000);
		}
		
	}
}


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


void dispenser_recovery(int b_b){
	/* 一つ目 */
	if(b_b == 0){
		//黒先回収
		gyro_deceleration(520, gyro_angle_standard, 0, -1);
	}else{
		//青先回収
		gyro_deceleration(850, gyro_angle_standard, 0, -1);		
	}
	perfect_BRAKE();
	tslp_tsk(500);

	rotation(-90, gyro_angle_standard);
	gyro_angle_standard += -90;
	dispenser();
	gyro_deceleration(-200, gyro_angle_standard, 0, -1);

	perfect_BRAKE();
	tslp_tsk(500);

	/* 二つ目 */
	if(b_b == 0){
		//黒先回収
		rotation(90, gyro_angle_standard);
		gyro_angle_standard += 90;
		gyro_deceleration(340, gyro_angle_standard, 0, -1);
		rotation(-90, gyro_angle_standard);
		gyro_angle_standard += -90;
	}else{
		//青先回収
		rotation(-90, gyro_angle_standard);
		gyro_angle_standard += -90;
		gyro_deceleration(340, gyro_angle_standard, 0, -1);
		rotation(90, gyro_angle_standard);
		gyro_angle_standard += 90;
	}
	gyro_deceleration(1, gyro_angle_standard, -1, -1);
	dispenser();
	gyro_deceleration(-200, gyro_angle_standard, 0, -1);
	perfect_BRAKE();
	rotation(90, gyro_angle_standard);
	gyro_angle_standard += 90;
	gyro_deceleration(-500, gyro_angle_standard, -1, 0);
	perfect_BRAKE();
	gyro_angle_standard = wall_fix(1000);
}


void dispenser(void){
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
}


void gyro_log(void){
	sprintf(str, "GYRO=%5d",(ev3_gyro_sensor_get_angle(GYRO_4)-gyro_angle_standard));
	ev3_lcd_draw_string(str,0,0);
}

