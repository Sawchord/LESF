/* Copyright 2016 Leon Tan
 * Refere to LICENSE file in main directory 
 * or http://www.gnu.org/licenses/old-licenses/gpl-2.0.txt
 * for full license conditions */

//#define F_CPU 8000000UL
#define UART_BAUD_RATE 57600


#include <ptthreads.h>
#include <ptstream.h>
#include <hal.h>

#include <avr/io.h>
#include <util/delay.h>
#include <stdio.h>
#include <stdint.h>

/*
int uart_getchar (FILE *stream)
{
    char c;
    
    // echo 
    
    
    loop_until_bit_is_set( UCSR0A, RXC0 );
    c = UDR0;
    printf("%c", c);
    
    return c;
}

*/
uint8_t ledswitch = 0x0;
uint8_t counter = 0;
uint8_t count_read = 0;
int8_t ret = 0;

int8_t tfunc0 (ptthread_t* self, uint8_t* data, uint16_t dlength) {
    
    if (ledswitch == 1) {
        PORTB ^= (1 << PB5);
    }
    
    ptthread_delay(self, 10000);
    
    return 0;
}

int8_t tfunc1 (ptthread_t* self, uint8_t* data, uint16_t dlength) {
    
    PORTB ^= (1 << PB3);
    ptthread_delay(self, 5000);
    
    return 0;
}

int8_t tfunc2 (ptthread_t* self, uint8_t* data, uint16_t dlength) {
    
    if (PINB & (1 << PB4))  {
        ledswitch = 1;
        //printf("Setting switch to %d\n", ledswitch);
    }
    else {
        ledswitch = 0;
    }
    
    
    //printf("Systime:%d\n", hal_get_time());
    
    ptthread_delay(self, 100);
    // hal_delay(1000);
    return 0;
}

ptthread_t thread[3];



int main (void) {
    
    hal_hardwareinit();
    
    DDRB  = 0xFF;
    DDRB &= ~( (1 << PB4) );
    
    //ptstream_t teststream;
    //ptstream_init(&teststream, 100);
    
    /* initializing the threads */
    ptthread_init(&thread[0], tfunc0, RUNNING, NULL, 0);
    ptthread_init(&thread[1], tfunc1, RUNNING, NULL, 0);
    ptthread_init(&thread[2], tfunc2, RUNNING, NULL, 0);
    
    ptthread_main(thread, 3);
    
    /*while(1) {
        
        //tfunc0 (NULL, NULL, 0);
        
        hal_delay(1000);
        
        PORTB ^= ((1 << PB5) & ledswitch);
        printf("PORTB: %d\n", PORTB);
        
        if (PINB & (1 << PB4))  {
            ledswitch = ~ledswitch;
            printf("Setting switch to %d\n", ledswitch);
            scanf("%d", &ret);
        }
        
        if ((1 << PB5) & ledswitch) {
            
            ret= ptstream_write(&teststream, &counter, 1);
            
            printf ("Writing %d with retvalue %d\n", counter, ret);
            
            counter++;
        }
        else {
            ret = ptstream_read(&teststream, &count_read, 1);
            printf("Read %d with retvalue %d\n", count_read, ret);
        }
        
        printf("Read Pointer: %d\n", teststream.read_p);
        printf("Write Pointer: %d\n", teststream.write_p);
        
    }*/
    
    return 0;
}