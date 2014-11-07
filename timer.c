#include <at89x52.h>

__sfr __at 0x80 LED;
__sfr __at 0x90 NUM;
__sfr __at 0xa0 BLINK;

__sbit __at 0x80 LED1;

__bit change_bit = 1;

#define OFF(PIN) do{\
    PIN = 0xff;\
} while(0)

#define ON(PIN) do{\
    PIN = 0x00;\
} while(0)

typedef unsigned char mcu_t;

unsigned int t1 = 0, t2 = 0, cl = 1;
int timer_count = 0;

enum timer_val {
	TIMEOUT_1S = 4000,	// 1s
	TIMEOUT_1S_DEFAULT = 4000,	// 1s
	ONE_MIN = 60,
	TOMATO_VAL = 25,
	SEC_30 = 30,
	BLINK_TIMES = 30,
//
//    BIG_TOMATO = 25,
//    SMALL_TOMATO = 3,
//    REST_TOMATO = 10,
//
	BIG_TOMATO = 25,
	SMALL_TOMATO = 3,
	REST_TOMATO = 10,
};

unsigned char __code backet[] = {
	BIG_TOMATO, SMALL_TOMATO, BIG_TOMATO, SMALL_TOMATO,
	BIG_TOMATO, SMALL_TOMATO, BIG_TOMATO, SMALL_TOMATO,
	REST_TOMATO,
};

unsigned char tomato_number = 9;
unsigned char current_tomato = 0;

unsigned char __code num_left[] = {
	0x0e, 0x1e, 0x2e, 0x3e,
	0x4e, 0x5e, 0x6e, 0x7e,
	0x8e, 0x9e,
};

unsigned char __code num_right[] = {
	0x0d, 0x1d, 0x2d, 0x3d,
	0x4d, 0x5d, 0x6d, 0x7d,
	0x8d, 0x9d,
};

static int tomato_time = 25;
//static int tomato_time = 25;
unsigned char buffer;
unsigned char blink_count = 0;
char blink_enable = 0;
unsigned char min_count = 0;
unsigned char second_30s = 0;
unsigned char ten;
unsigned char single;
unsigned char left_right;
unsigned char sun_show_it;

void timer_init(void);
void blink_led(void);
void display_sun(unsigned int show_num);
void pass_30_s(void);
void my_pwm(void);

void main(void)
{
//   LED = 0xff;
	TL0 = 6;
	TH0 = 6;
	timer_init();
	for (;;) {

		display_sun(tomato_time);
		pass_30_s();
	}

}

void timer_interrupt(void) __interrupt(1)
{
	timer_count++;

	my_pwm();

	if (timer_count >= TIMEOUT_1S) {	// 1s
		timer_count = 0;

		if (second_30s++ >= SEC_30) {
			second_30s = 0;
			buffer++;	// from the TW website!!!

		}		// 30 s for loop

		if (min_count++ >= ONE_MIN) {
			min_count = 0;

			if (tomato_time-- <= 0) {

				if (current_tomato >= tomato_number) {
					current_tomato = 0;
				}
				current_tomato += 1;
				tomato_time = backet[current_tomato];

				blink_enable = 1;
				//blink_led();

			}
		}		// one minute' for loop

	}			// 1second for loop

}

void timer_init(void)
{
	TMOD = 0x02;
	TR0 = 1;
	ET0 = 1;
	EA = 1;

}

void display_sun(unsigned int show_num)
{

	ten = (show_num % 100) / 10;
	single = (show_num % 100) % 10;

	if (left_right % 2) {
		sun_show_it = num_left[ten];
	} else {
		sun_show_it = num_right[single];
	}
	NUM = sun_show_it;
	++left_right;
}

void pass_30_s(void)
{
	LED = ~buffer;
	OFF(BLINK);
}

void blink_led(void)
{
	if (blink_enable == 1) {

		if (blink_count >= BLINK_TIMES) {
			blink_enable = 0;
			//     LED = 0xff;
		}
		blink_count++;

		BLINK = ~BLINK;
	} else {
		OFF(BLINK);
	}

}

void my_pwm(void)
{

	t1++;
	t2++;

	if (t1 <= cl) {
		P2_0 = 0;	//这三行通过t1与cl比较，控制led亮灭，也就是PWM调光
		P2_1 = 0;
		P2_2 = 0;
		P2_3 = 0;
	}
	if (t1 > cl) {
		P2_0 = 1;	//
		P2_1 = 1;
		P2_2 = 1;
		P2_3 = 1;
	}
	if (t1 == 40)
		t1 = 0;		//

	if (t2 >= 510) {	//250次中断以后，进入以下语句，更改cl值，改变亮度。改变比较的值(250)
		t2 = 0;		//可以调整闪烁速度。
		if (change_bit) {
			cl++;
			if (cl == 35)
				change_bit = 0;	//由亮变暗标识
		}

		if (!change_bit) {
			cl--;
			if (cl == 2)
				change_bit = 1;	//由暗变亮标识
		}
	}

}
