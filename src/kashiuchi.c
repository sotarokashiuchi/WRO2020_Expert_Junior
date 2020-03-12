/* kashiuchi.c */
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

/*****************************************************************************************************************************************
*****************************************************************************************************************************************/

/* bluetoothグローバル変数 */
    FILE *kashiuchi_fp = NULL;


/* linetrace_task動的な宣言 */
    #define LINETRACE_DELTA_T 0.004       //処理周期
    FILE *fp_2 = NULL;

	float line_p=0, line_i=0, line_d=0;
	float line_old = 0, line_new = 0;
	float line_integral = 0;
    //i=p/0.2
    //d=p*0.075
    


/*
 *   ジャイトレース
 */
    #define GYROTRACE_DELTA_T 0.004       //処理周期
    #define GYROTRACE_POWER 15  //パワー
	#define GYROTRACE_KP 1             //Pゲイン
	#define GYROTRACE_KI 0             //Iゲイン
    #define GYROTRACE_KD 0             //Dゲイン

	float gyro_p=0, gyro_i=0, gyro_d;
	int gyro_old = 0, gyro_new = 0;
	float gyro_integral = 0;

/*****************************************************************************************************************************************
*****************************************************************************************************************************************


/*
 *	bluetooth接続関数
 */
int bluetooth_kashiuchi_fp(void){
    kashiuchi_fp = ev3_serial_open_file(EV3_SERIAL_BT);
    // fp_2 = fopen("/ev3rt/document/LINETRACE.xlsk","a+");
    return 0;
}



/*
 *   ライントレース
 */
void linetrace_task_4(void){
    line_old = line_new;
    line_new = ev3_color_sensor_get_reflect(COLOR_1)-ev3_color_sensor_get_reflect(COLOR_2);
    line_integral += (line_new + line_old) / 2.0 *LINETRACE_DELTA_T; 

    line_p = line_p_gein * line_new;
    line_i = LINETRACE_KI * line_integral;
    line_d = line_d_gein * (line_new - line_old) / LINETRACE_DELTA_T;

    if(line_new>=0){
        ev3_motor_set_power(C_MOTOR, (line_power
)-(line_p + line_i + line_d));
        ev3_motor_set_power(B_MOTOR, -line_power
);
    }else{
        ev3_motor_set_power(B_MOTOR, (-line_power
)-(line_p + line_i + line_d));
        ev3_motor_set_power(C_MOTOR, line_power
);
    }
    // fprintf(fp_2,"%lf\t%lf\t%lf\n",line_p, line_i, line_d);
}



/*
 *   ジャイトレース
 */
void gyrotrace_task_4(void){
    gyro_old = gyro_new;
    gyro_new = ev3_gyro_sensor_get_angle(GYRO_4);
    gyro_integral += (gyro_new + gyro_old) / 2.0 *GYROTRACE_DELTA_T; 
    fprintf(kashiuchi_fp, "%d\r",gyro_new);

    gyro_p = GYROTRACE_KP * gyro_new;
    gyro_i = GYROTRACE_KI * gyro_integral;
    gyro_d = GYROTRACE_KD * (gyro_new - gyro_old) / GYROTRACE_DELTA_T;

    if(gyro_new>=0){
        ev3_motor_set_power(C_MOTOR, (GYROTRACE_POWER)-(gyro_p + gyro_i + gyro_d));
        ev3_motor_set_power(B_MOTOR, -GYROTRACE_POWER);
    }else{
        ev3_motor_set_power(B_MOTOR, (-GYROTRACE_POWER)-(gyro_p + gyro_i + gyro_d));
        ev3_motor_set_power(C_MOTOR, GYROTRACE_POWER);
    }
}



/*
 *	アーム上げる関数
 */
void a_arm_up(void){
    ev3_motor_set_power(A_ARM, 30);
	tslp_tsk(600);
	BRAKE(A_ARM);
}





/*
 *	アーム下げる関数
 */
void a_arm_down(void){
    ev3_motor_set_power(A_ARM, -30);
	tslp_tsk(600);
	BRAKE(A_ARM);
}



/*
 *	アーム下げる関数
 */
void d_motor_car_up(void){

}




/*
 *	音関数
 */
void tone(void){
    ev3_speaker_play_tone(554.37, 100);
}



/*
 *	整数値入力関数
 */
int input_int(int *numbers){
    tslp_tsk(200);
	char char_numbers[10];

    ev3_lcd_set_font(EV3_FONT_MEDIUM);

    while(false==ev3_button_is_pressed(ENTER_BUTTON)){
        sprintf(char_numbers, "%10d", *numbers);
        ev3_lcd_draw_string(char_numbers, 0, 60);

        if(false!=ev3_button_is_pressed(UP_BUTTON)){
            (*numbers)++;
            tslp_tsk(200);
        }
        if(false!=ev3_button_is_pressed(DOWN_BUTTON)){
            (*numbers)--;
            tslp_tsk(200);
        }
        if(false!=ev3_button_is_pressed(RIGHT_BUTTON)){
            (*numbers) *=10;
            tslp_tsk(200);
        }
        if(false!=ev3_button_is_pressed(LEFT_BUTTON)){
            (*numbers) /=10;
            tslp_tsk(200);
        }
    }
    return 0;
}



/*
 *	小数値入力関数
 */
int input_float(float *numbers){
    tslp_tsk(200);
	char char_numbers[10];

    ev3_lcd_set_font(EV3_FONT_MEDIUM);

    while(false==ev3_button_is_pressed(ENTER_BUTTON)){
        sprintf(char_numbers, "%5f", *numbers);
        ev3_lcd_draw_string(char_numbers, 0, 75);

        if(false!=ev3_button_is_pressed(UP_BUTTON)){
            (*numbers)++;
            tslp_tsk(200);
        }
        if(false!=ev3_button_is_pressed(DOWN_BUTTON)){
            (*numbers)--;
            tslp_tsk(200);
        }
        if(false!=ev3_button_is_pressed(RIGHT_BUTTON)){
            (*numbers) *=10;
            tslp_tsk(200);
        }
        if(false!=ev3_button_is_pressed(LEFT_BUTTON)){
            (*numbers) /=10;
            tslp_tsk(200);
        }
    }
    return 0;
}



/*
 *	エラー表示関数
 */
int return_value(double no, ER return_function, char display[20]){
 	if(return_function==E_OK){
 		fprintf(kashiuchi_fp, "%05.1f 正常終了_%s\r\n", no, display);
 		return 1;
 	}else{
 		fprintf(kashiuchi_fp, "\n\n%05.1f error_%s\r\n", no, display);
 		if(return_function==E_PAR){
 			fprintf(kashiuchi_fp, "  E_PAR...");
 			fprintf(kashiuchi_fp, "パス名が無効です。\r\n");
 			fprintf(kashiuchi_fp, "	不正のモーター、センサータイプ\r\n");
 			fprintf(kashiuchi_fp, "   不正の設定値\r\n");
 			fprintf(kashiuchi_fp, "	メモリファイルは無効\r\n");
 			fprintf(kashiuchi_fp, "	画像のオブジェクトは無効\r\n");
 			fprintf(kashiuchi_fp, "	p_image　はNULL\r\n");

 		}else if(return_function==E_ID){
 			fprintf(kashiuchi_fp, "  E_ID...");
 			fprintf(kashiuchi_fp, "不正のボタン番号\r\n");
 			fprintf(kashiuchi_fp, "	不正ID番号\r\n");
 			fprintf(kashiuchi_fp, "	不正のモーター、センサポート番号\r\n");

 		}else if(return_function==E_SYS){
 			fprintf(kashiuchi_fp, "  E_SYS...");
 			fprintf(kashiuchi_fp, "I/Oエラーが発生した(SDカード不良の可能性が高い\r\n");

 		}else if(return_function==E_NOMEM){
 			fprintf(kashiuchi_fp, "  E_NOMEM...");
 			fprintf(kashiuchi_fp, "メモリ不足\r\n");

 		}else if(return_function==E_CTX){
 			fprintf(kashiuchi_fp, "  E_CTX...");
 			fprintf(kashiuchi_fp, "非タスクコンテストから呼び出し\r\n");

 		}else if(return_function==E_MACV){
 			fprintf(kashiuchi_fp, "  E_MACV...");
 			fprintf(kashiuchi_fp, "メモリファイルアクセス違反(p_memfile)(path)\r\n");

 		}else if(return_function==E_OBJ){
 			fprintf(kashiuchi_fp, "  E_OBJ...");
 			fprintf(kashiuchi_fp, "	モータ未接続\r\n");
 			fprintf(kashiuchi_fp, "	画像ファイルは破損\r\n");
 			fprintf(kashiuchi_fp, "	(p_memfile)で指定したメモリファイルは無効\r\n");

 		}else if(return_function==E_NOSPT){
 			fprintf(kashiuchi_fp, "  E_NOSPT...");
 			fprintf(kashiuchi_fp, "これ以上読み込めるファイルの情報がない\r\n");

 		}else if(return_function==E_NORES){
 			fprintf(kashiuchi_fp, "  E_NORES...");
 			fprintf(kashiuchi_fp, "サウンドデバイスが占有せれている\r\n");

 		}else{
 			fprintf(kashiuchi_fp, "エラーの詳細が分かりません\r\n");
 		}
 	return 0;
 	}
 }
