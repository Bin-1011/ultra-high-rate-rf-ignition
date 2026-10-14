#include "global.h"

void sensor(int _switch,int *led_count,u16 freq,int *count){
	if(_switch == 0){
		P20=0;
		if(P21 == 0){
			OLED_Clear();
			OLED_Show(*count);
			while(cancel == 1){
				if(P32 == 0){
					if(*count > 0){
						OLED_Show(--(*count));
						led_single(led_count,freq);
					}else OLED_Show(*count);
				}
			}
		}
	}else if(_switch == 1)
		P20=1;
}
