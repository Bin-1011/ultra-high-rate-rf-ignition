#include "delay.h"
#define LED P0	//定义LED地址

void led_single(int *led_count,u16 freq);
void led_func(int selected,int *led_count,int *count,u16 freq);
void led_single(int *led_count,u16 freq);
