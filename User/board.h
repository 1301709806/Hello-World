#ifndef _BOARD_H_
#define _BOARD_H_

/****使用了stm32固件库******/
#include "stm32f10x.h"

//#include "bsp_led.h"


void rt_hw_board_init(void);

void SysTick_Handler(void);
#endif


