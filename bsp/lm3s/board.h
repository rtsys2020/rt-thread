/*
 * File      : board.h
 * This file is part of RT-Thread RTOS
 * COPYRIGHT (C) 2006, RT-Thread Develop Team
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://openlab.rt-thread.com/license/LICENSE
 *
 * Change Logs:
 * Date           Author       Notes
 * 2006-10-08     Bernard      add board.h to this bsp
 */

#ifndef __BOARD_H__
#define __BOARD_H__

// <o> Internal SRAM memory size[Kbytes] <8-64>
//	<i>Default: 64
#define LM3S_SRAM_SIZE         64
#define LM3S_SRAM_END          (0x20000000 + LM3S_SRAM_SIZE * 1024)

void rt_hw_board_led_on(int n);
void rt_hw_board_led_off(int n);
void rt_hw_board_init(void);

void rt_hw_usart_init(void);

#endif
