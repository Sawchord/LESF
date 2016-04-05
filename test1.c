/* Copyright 2016 Leon Tan
 * Refere to LICENSE file in main directory 
 * or http://www.gnu.org/licenses/old-licenses/gpl-2.0.txt
 * for full license conditions */

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

uint16_t ADC_Read( uint8_t channel )
{
    // Select channel
    ADMUX = (ADMUX & ~(0x1F)) | (channel & 0x1F);
    ADCSRA |= (1<<ADSC);            // single conversion
    while (ADCSRA & (1<<ADSC) ) {   // wait for conversion to finish
    }
    return ADCW;                    // read ADC
}

uint8_t ledswitch = 1;
uint8_t counter = 0;
uint8_t count_read = 0;
uint16_t analog_value = 0;
uint16_t old_analog_value = 0;
int8_t ret = 0;

static ptthread_t thread[6];
static uint8_t buffer[100];
static ptstream_t teststream;

int8_t yled_blink(ptthread_t* self) {
    
    if (ledswitch == 1) {
        PORTB ^= (1 << PB5);
        //printf("Setting PB5 to %d\n", (PORTB >> PB5) & 1  );
    }
    
    ptthread_delay(self, 435);
    
    return 0;
}

int8_t rled_blink(ptthread_t* self) {
    
    PORTB ^= (1 << PB3);
    //printf("Setting PB3 to %d\n", (PORTB >> PB3) & 1  );
    
    ptthread_delay(self, 1106);
    
    return 0;
}

int8_t switch_check(ptthread_t* self) {
    
    static uint8_t pushed = 0;
    
    pushed = (pushed << 1) | ((PINB & (1 << PB4)) >> PB4) ;
    
    if ((pushed & 3) == 1) {
        ledswitch ^=  1;
        printf("Setting switch to %d\n", ledswitch);
    }
    
    ptthread_delay(self, 50);
    
    return 0;
}

int8_t bled_pulse(ptthread_t* self) {
    
    static uint16_t duty_circle = 0;
    static uint8_t direction = 1;
    
    //PORTB ^= (1 << PB1);
    
    if (direction) {
        duty_circle += 65335/ 50;
    }
    else {
        duty_circle -= 65335/ 50;
    }
    
    if (duty_circle > 65000) {
        direction = 0;
        //OCR1A = 0xFFFF;
        //ptthread_delay(self, 2000);
        //return 0;
    }
    else if (duty_circle < 1000){
        direction = 1;
    }
    OCR1A = duty_circle;
    //OCR1B = duty_circle;
    
    
    
    ptthread_delay(self, 20);
    
    return 0;
    
}

int8_t analog_read(ptthread_t* self) {
    
    int8_t ret;
    uint16_t temp;
    
    analog_value = ADC_Read(0);
    
    //printf ("Delta ACD: %d\n", ((int32_t) analog_value - old_analog_value));
    
    if ( (int32_t) analog_value - (int32_t) old_analog_value > 50 ||
         (int32_t) old_analog_value - (int32_t) analog_value > 50
    ){
        
        ret = ptstream_write (&teststream, &analog_value, 2);
        //printf("Val canged\n");
    }
    
    old_analog_value = analog_value;
       
    ptthread_delay(self, 100);
    
    return 0;
}

int8_t output(ptthread_t* self) {
    uint16_t val;
    static uint16_t times = 0;
    if (ptstream_read( &teststream, &val, 2) == 0) {
        times++;
        printf("Read from stream: %u times: %u\n", val, times);
    }
    
    ptthread_read_block(self, &teststream);
    
    return 0;
}

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
    
    // Initialize the ADC circuits
    // Set external VREF
    ADMUX = (1<<REFS0);    
    //ADMUX = (1<<REFS1) | (1<<REFS0);
    
    // Set ACD to single conversion
    ADCSRA = (1<<ADPS1) | (1<<ADPS0);     // Prescaler here??
    ADCSRA |= (1<<ADEN);                  // activate ADC
    
    
    
    ptstream_init(&teststream, 100, &buffer);
    
    /* initializing the threads */
    ptthread_init(&thread[0], yled_blink, RUNNING);
    ptthread_init(&thread[1], rled_blink, RUNNING);
    ptthread_init(&thread[2], switch_check, RUNNING);
    ptthread_init(&thread[3], bled_pulse, RUNNING);
    ptthread_init(&thread[4], analog_read, RUNNING);
    ptthread_init(&thread[5], output, RUNNING);
    
    ptthread_main(thread, 6);
    
    
    return 0;
}