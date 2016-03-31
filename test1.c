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
uint8_t ledswitch = 1;
uint8_t counter = 0;
uint8_t count_read = 0;
int8_t ret = 0;

int8_t tfunc0 (ptthread_t* self, uint8_t* data, uint16_t dlength) {
    
    if (ledswitch == 1) {
        PORTB ^= (1 << PB5);
        //printf("Setting PB5 to %d\n", (PORTB >> PB5) & 1  );
    }
    
    ptthread_delay(self, 435);
    
    return 0;
}

int8_t tfunc1 (ptthread_t* self, uint8_t* data, uint16_t dlength) {
    
    PORTB ^= (1 << PB3);
    //printf("Setting PB3 to %d\n", (PORTB >> PB3) & 1  );
    
    ptthread_delay(self, 1106);
    
    return 0;
}

int8_t tfunc2 (ptthread_t* self, uint8_t* data, uint16_t dlength) {
    
    static uint8_t pushed = 0;
    
    
    if (PINB & (1 << PB4))  {
        pushed = (pushed << 1) | 1;
    }
    else {
        pushed = (pushed << 1) | 0;
    }
    
    if ((pushed & 3) == 1) {
        ledswitch ^=  1;
        printf("Setting switch to %d\n", ledswitch);
    }
    
    ptthread_delay(self, 50);
    
    return 0;
}

int8_t tfunc3 (ptthread_t* self, uint8_t* data, uint16_t dlength) {
    
    static uint16_t duty_circle = 0;
    static uint8_t direction = 1;
    
    //PORTB ^= (1 << PB1);
    
    if (direction) {
        duty_circle += 65335/50;
    }
    else {
        duty_circle -= 65335/50;
    }
    
    if (duty_circle > 65200) {
        direction = 0;
        OCR1A = 0xFFFF;
        ptthread_delay(self, 2000);
        return 0;
    }
    else if (duty_circle < 100){
        direction = 1;
    }
    OCR1A = duty_circle;
    //OCR1B = duty_circle;
    
    
    
    ptthread_delay(self, 20);
    
    return 0;
    
}

ptthread_t thread[4];



int main (void) {
    
    hal_hardwareinit();
    
    DDRB  = 0xFF;
    DDRB &= ~( (1 << PB4) );
    
    // set up PB1 as PWM output 
    // set TOP to 16bit
    ICR1 = 0xFFFF;
    
    // PB1 and PB2 is now an output
    DDRB |= (1 << DDB1)|(1 << DDB2);
    
    
    
    // set none-inverting mode
    TCCR1A |= (1 << COM1A1)|(1 << COM1B1);
    
    // set Fast PWM mode using ICR1 as TOP
    TCCR1A |= (1 << WGM11);
    TCCR1B |= (1 << WGM12)|(1 << WGM13);
    
    // START the timer with no prescaler
    TCCR1B |= (1 << CS10);
    
    
    //ptstream_t teststream;
    //ptstream_init(&teststream, 100);
    
    /* initializing the threads */
    ptthread_init(&thread[0], tfunc0, RUNNING, NULL, 0);
    ptthread_init(&thread[1], tfunc1, RUNNING, NULL, 0);
    ptthread_init(&thread[2], tfunc2, RUNNING, NULL, 0);
    ptthread_init(&thread[3], tfunc3, RUNNING, NULL, 0);
    
    ptthread_main(thread, 4);
    
    
    return 0;
}