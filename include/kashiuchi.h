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




/*
 *   回転
 */
void rotation(int angul, int angul_cfg);



/*
 *   減速
 */
int deceleration(int angul, int stp);



/*
 *   ジャイロ減速
 */
int gyro_deceleration(int angul, int gyro_angle_standard, int stp);


/*
 *   線合わせ
 */
line_fix(int color_reflect);



/*
 *	アーム下げる関数
 */
void a_arm_down(void);


/*
 *	アーム上げる関数
 */
void a_arm_up(void);



/*
 *	Dアーム上げる関数
 */
void d_motor_car_up(void);



/*
 *	Dアーム下げる関数
 */
void d_motor_car_down(void);



/*
 *	音関数
 */
void tone_object(void);

void tone_line(void);



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
