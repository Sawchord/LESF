/* Copyright 2016 Leon Tan
 * Refere to LICENSE file in main directory 
 * or http://www.gnu.org/licenses/old-licenses/gpl-2.0.txt
 * for full license conditions */

#define UART_BAUD_RATE 57600

#include <ptthreads.h>
#include <ptstream.h>
#include <hal.h>
#include <muci.h>

#include <avr/io.h>
#include <util/delay.h>
#include <stdio.h>
#include <stdint.h>

uint16_t analog_value;
uint16_t old_analog_value;

const char teststring[] = "This is a convenient teststring";

// the muci_interface used on this device
muci_iface_t interface0;

// the streams for the interface
ptstream_t send0;
ptstream_t recv0;

ptthread_t thread[3];

const ipv6_address_t address = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x00, 0x01 };

uint16_t ADC_Read( uint8_t channel )
    {
        // Select channel
        ADMUX = (ADMUX & ~(0x1F)) | (channel & 0x1F);
        ADCSRA |= (1<<ADSC);            // single conversion
        while (ADCSRA & (1<<ADSC) ) {   // wait for conversion to finish
        }
        return ADCW;                    // read ADC
    }

// analog read from test1 to generate some output data
int8_t analog_read(ptthread_t* self) {
        
        //int8_t ret;
        //uint16_t temp;
        
        analog_value = ADC_Read(0);
        
        //printf ("Delta ACD: %d\n", ((int32_t) analog_value - old_analog_value));
        
        if ( (int32_t) analog_value - (int32_t) old_analog_value > 50 ||
            (int32_t) old_analog_value - (int32_t) analog_value > 50
        ){
            
            ptstream_write (&send0, &analog_value, 2);
            //printf("Val canged\n");
            PORTB ^= (1 << PB5);
            printf("Read analog value %d \n", analog_value);
        }
        
        old_analog_value = analog_value;
        
        //ret = ptstream_read(&teststream, &temp, 2);
        
        
        ptthread_delay(self, 100);
        
        return 0;
    }    

    
int8_t muci_updater(ptthread_t* self) {
    
    int8_t ret;
    
    ret = muci_update(&interface0);
    if (ret == INIT || ret == RTS || ret == CTR) {
        
        // check again
        ptthread_delay(self, 10);
        
        //return 0;
    }
    
    printf("IFSTATE: %d\n", ret);
    return 0;
}

int8_t sender(ptthread_t* self) {
    
    
    
    ptthread_delay(self, 2000);
    return 0;
}

int8_t debug_out(ptthread_t* self) {
    ipv6_address_t msg_src;
    uint8_t* buf;
    uint16_t buflen;
    
    ptthread_read_block(self, &recv0);
    muci_recv(&interface0, &msg_src, &buf, &buflen);
    
    // TODO: convenient print
    
    return 0;
}


int main (void) {
    
    hal_hardwareinit();
    
    DDRB  = 0xFF;
    
    DDRD  = 0xFF;
    
    // Initialize the ADC circuits
    // Set external VREF
    ADMUX = (1<<REFS0);    
    //ADMUX = (1<<REFS1) | (1<<REFS0);
    
    // Set ACD to single conversion
    ADCSRA = (1<<ADPS1) | (1<<ADPS0);     // Prescaler here??
    ADCSRA |= (1<<ADEN);                  // activate ADC
    
    
    
    ptstream_init_alloc(&send0, 255);
    ptstream_init_alloc(&recv0, 255);
    
    muci_init(&interface0, &DDRD, &PORTD, &PIND, PD7,
            &DDRD, &PORTD, &PIND, PD6, &send0, &recv0, 115200);
    
    ptstream_write(&send0, &teststring, 100);
    printf("Send text message\n");
    
    ptthread_init(&thread[0], analog_read, RUNNING);
    ptthread_init(&thread[1], muci_updater, RUNNING);
    
    
    
    ptthread_main(thread, 2);
}