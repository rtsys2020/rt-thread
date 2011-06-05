/*
 * File      : interrupt.c
 * This file is part of RT-Thread RTOS
 * COPYRIGHT (C) 2006, RT-Thread Development Team
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://openlab.rt-thread.com/license/LICENSE
 *
 * Change Logs:
 * Date           Author       Notes
 * 2011-01-13     weety      first version
 */

#include <rtthread.h>
#include "at91sam926x.h"

#define MAX_HANDLERS	32

extern rt_uint32_t rt_interrupt_nest;

/* exception and interrupt handler table */
rt_isr_handler_t isr_table[MAX_HANDLERS];
rt_uint32_t rt_interrupt_from_thread, rt_interrupt_to_thread;
rt_uint32_t rt_thread_switch_interrput_flag;


/* --------------------------------------------------------------------
 *  Interrupt initialization
 * -------------------------------------------------------------------- */

rt_uint32_t at91_extern_irq;

#define is_extern_irq(irq) ((1 << (irq)) & at91_extern_irq)

/*
 * The default interrupt priority levels (0 = lowest, 7 = highest).
 */
static rt_uint32_t at91sam9260_default_irq_priority[MAX_HANDLERS] = {
	7,	/* Advanced Interrupt Controller */
	7,	/* System Peripherals */
	1,	/* Parallel IO Controller A */
	1,	/* Parallel IO Controller B */
	1,	/* Parallel IO Controller C */
	0,	/* Analog-to-Digital Converter */
	5,	/* USART 0 */
	5,	/* USART 1 */
	5,	/* USART 2 */
	0,	/* Multimedia Card Interface */
	2,	/* USB Device Port */
	6,	/* Two-Wire Interface */
	5,	/* Serial Peripheral Interface 0 */
	5,	/* Serial Peripheral Interface 1 */
	5,	/* Serial Synchronous Controller */
	0,
	0,
	0,	/* Timer Counter 0 */
	0,	/* Timer Counter 1 */
	0,	/* Timer Counter 2 */
	2,	/* USB Host port */
	3,	/* Ethernet */
	0,	/* Image Sensor Interface */
	5,	/* USART 3 */
	5,	/* USART 4 */
	5,	/* USART 5 */
	0,	/* Timer Counter 3 */
	0,	/* Timer Counter 4 */
	0,	/* Timer Counter 5 */
	0,	/* Advanced Interrupt Controller */
	0,	/* Advanced Interrupt Controller */
	0,	/* Advanced Interrupt Controller */
};

/**
 * @addtogroup AT91SAM926X
 */
/*@{*/

rt_isr_handler_t rt_hw_interrupt_handle(rt_uint32_t vector)
{
	rt_kprintf("Unhandled interrupt %d occured!!!\n", vector);
	return RT_NULL;
}

/*
 * Initialize the AIC interrupt controller.
 */
void at91_aic_init(rt_uint32_t *priority)
{
	rt_uint32_t i;

	/*
	 * The IVR is used by macro get_irqnr_and_base to read and verify.
	 * The irq number is NR_AIC_IRQS when a spurious interrupt has occurred.
	 */
	for (i = 0; i < MAX_HANDLERS; i++) {
		/* Put irq number in Source Vector Register: */
		at91_sys_write(AT91_AIC_SVR(i), i);
		/* Active Low interrupt, with the specified priority */
		at91_sys_write(AT91_AIC_SMR(i), AT91_AIC_SRCTYPE_LOW | priority[i]);
		//AT91_AIC_SRCTYPE_FALLING

		/* Perform 8 End Of Interrupt Command to make sure AIC will not Lock out nIRQ */
		if (i < 8)
			at91_sys_write(AT91_AIC_EOICR, 0);
	}

	/*
	 * Spurious Interrupt ID in Spurious Vector Register is NR_AIC_IRQS
	 * When there is no current interrupt, the IRQ Vector Register reads the value stored in AIC_SPU
	 */
	at91_sys_write(AT91_AIC_SPU, MAX_HANDLERS);

	/* No debugging in AIC: Debug (Protect) Control Register */
	at91_sys_write(AT91_AIC_DCR, 0);

	/* Disable and clear all interrupts initially */
	at91_sys_write(AT91_AIC_IDCR, 0xFFFFFFFF);
	at91_sys_write(AT91_AIC_ICCR, 0xFFFFFFFF);
}


/**
 * This function will initialize hardware interrupt
 */
void rt_hw_interrupt_init(void)
{
	rt_int32_t i;
	register rt_uint32_t idx;
	rt_uint32_t *priority = at91sam9260_default_irq_priority;
	
	at91_extern_irq = (1 << AT91SAM9260_ID_IRQ0) | (1 << AT91SAM9260_ID_IRQ1)
			| (1 << AT91SAM9260_ID_IRQ2);

	/* Initialize the AIC interrupt controller */
	at91_aic_init(priority);

	/* init exceptions table */
	for(idx=0; idx < MAX_HANDLERS; idx++)
	{
		isr_table[idx] = (rt_isr_handler_t)rt_hw_interrupt_handle;
	}

	/* init interrupt nest, and context in thread sp */
	rt_interrupt_nest = 0;
	rt_interrupt_from_thread = 0;
	rt_interrupt_to_thread = 0;
	rt_thread_switch_interrput_flag = 0;
}

/**
 * This function will mask a interrupt.
 * @param vector the interrupt number
 */
void rt_hw_interrupt_mask(int irq)
{
	/* Disable interrupt on AIC */
	at91_sys_write(AT91_AIC_IDCR, 1 << irq);
}

/**
 * This function will un-mask a interrupt.
 * @param vector the interrupt number
 */
void rt_hw_interrupt_umask(int irq)
{
	/* Enable interrupt on AIC */
	at91_sys_write(AT91_AIC_IECR, 1 << irq);
}

/**
 * This function will install a interrupt service routine to a interrupt.
 * @param vector the interrupt number
 * @param new_handler the interrupt service routine to be installed
 * @param old_handler the old interrupt service routine
 */
void rt_hw_interrupt_install(int vector, rt_isr_handler_t new_handler, rt_isr_handler_t *old_handler)
{
	if(vector < MAX_HANDLERS)
	{
		if (old_handler != RT_NULL) *old_handler = isr_table[vector];
		if (new_handler != RT_NULL) isr_table[vector] = new_handler;
	}
}

/*@}*/


static int at91_aic_set_type(unsigned irq, unsigned type)
{
	unsigned int smr, srctype;

	switch (type) {
	case IRQ_TYPE_LEVEL_HIGH:
		srctype = AT91_AIC_SRCTYPE_HIGH;
		break;
	case IRQ_TYPE_EDGE_RISING:
		srctype = AT91_AIC_SRCTYPE_RISING;
		break;
	case IRQ_TYPE_LEVEL_LOW:
		if ((irq == AT91_ID_FIQ) || is_extern_irq(irq))		/* only supported on external interrupts */
			srctype = AT91_AIC_SRCTYPE_LOW;
		else
			return -1;
		break;
	case IRQ_TYPE_EDGE_FALLING:
		if ((irq == AT91_ID_FIQ) || is_extern_irq(irq))		/* only supported on external interrupts */
			srctype = AT91_AIC_SRCTYPE_FALLING;
		else
			return -1;
		break;
	default:
		return -1;
	}

	smr = at91_sys_read(AT91_AIC_SMR(irq)) & ~AT91_AIC_SRCTYPE;
	at91_sys_write(AT91_AIC_SMR(irq), smr | srctype);
	return 0;
}
