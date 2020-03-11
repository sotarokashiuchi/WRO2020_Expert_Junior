//kashiuchi.h



/*
 *	bluetooth接続関数
 */
int bluetooth_kashiuchi_fp(void);


/*
 *   ライントレース
 */
extern void linetrace_task(void);


/*
 *   ジャイトレース
 */
extern void gyrotrace_task(void);


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
