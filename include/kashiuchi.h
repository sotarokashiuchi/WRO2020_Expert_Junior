//kashiuchi.h



/*
 *	bluetooth接続関数
 */
int bluetooth_kashiuchi_fp(void);


/*
 *   ライントレース
 */
extern void linetrace_task_4(void);

extern void linetrace_task_4_power_p_i_d(int power, float p, float i, float d);

/*
 *   ジャイトレース
 */
extern void gyrotrace_task_4(void);

extern void gyrotrace_task_4_power_p_i_d_angle(int power, float p, float i, float d, int angle);

extern void gyrotrace_b_task_4(void);




/*
 *   回転
 */
int rotation(int angul, int angul_cfg);



/*
 *   減速
 */
int deceleration(int angul, int stp);



/*
 *   ジャイロ減速
 */
int gyro_deceleration(int angul, int gyro_angle_standard, int stp, int power);



int gyro_deceleration_power(int power, int gyro_angle_standard, int reset);


/*
 *   線合わせ
 */
// void line_fix(int color_reflect);



/*
 *   壁合わせ
 */
int wall_fix(int angle);



/*
 *	アームリセット関数
 */
void a_arm_reset(int);


/*
 *	アーム左右角度関数
 */
void a_arm(int);



/*
 *	Dアーム開ける関数
 */
void d_motor_car_open(int);



/*
 *	Dアーム閉じる関数
 */
void d_motor_car_close(int);



/*
 *	研磨材置く関数
 */
void kennmazai_put(int);



/*
 *	音関数
 */
void tone_object(void);

void tone_line(void);



/*
 *	完全停止
 */
void perfect_BRAKE(void);


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
