//kashiuchi.h



/*
 *	bluetooth接続関数
 */
int bluetooth_kashiuchi_fp(void);


/*
 *   ライントレース
 */
extern void linetrace_task_4(void);
extern float *line_power;  //30  //パワー
extern	float *line_p_gein; //0.26             //Pゲイン
extern	float *line_i_gein; //0.56             //Iゲイン
extern	float *line_d_gein; //0.04       //Dゲイン


/*
 *   ジャイトレース
 */
extern void gyrotrace_task_4(void);


/*
 *	アーム下げる関数
 */
void a_arm_down(void);


/*
 *	アーム上げる関数
 */
void a_arm_up(void);



/*
 *	音関数
 */
void tone(void);



/*
 *	整数値入力関数
 */
int input_int(int*);


/*
 *	小数値入力関数
 */
int input_float(float*);


/*
 *	エラー表示関数
 */
int return_value(double, ER, char[20]);
