/* the header files for the hardware abstraction layer
 * TODO: sound definition of hal functions
 * TODO: folder HAL to include several architectures */


#ifndef HAL_H
#define HAL_H

static uint32_t hal_system_time = 0;

/* Delay execution by n miliseconds */
void hal_delay(uint16_t);

/* should be called by main to init hardware properly */
void hal_hardwareinit(void);


#endif