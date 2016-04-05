/* Copyright 2016 Leon Tan
 * Refere to LICENSE file in main directory 
 * or http://www.gnu.org/licenses/old-licenses/gpl-2.0.txt
 * for full license conditions */

/* the header files for the hardware abstraction layer
 * TODO: sound definition of hal functions
 * TODO: folder HAL to include several architectures */


#ifndef HAL_H
#define HAL_H

static volatile uint32_t hal_system_time;

/* Delay execution by n miliseconds */
void hal_delay(uint32_t);
void hal_delay_us(uint32_t);

uint32_t hal_get_time(void);
void hal_update_time(uint32_t);

/* activate interups */
void hal_interrupt_enable(void);

/* deactivate interups*/
void hal_interrupt_disable(void);


/* should be called by main to init hardware properly */
void hal_hardwareinit(void);


#endif