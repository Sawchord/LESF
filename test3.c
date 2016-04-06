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
      
    
int8_t muci_updater(ptthread_t* self) {
    
    int8_t ret;
    
    ret = muci_update(&interface0);
    if (ret == INIT || ret == RTS || ret == CTR) {
        
        // check again
        ptthread_delay(self, 10);
        
        //return 0;
    }
    
    //printf("IFSTATE: %d\n", ret);
    return 0;
}
    
int8_t sender(ptthread_t* self) {
    
    
    
    ptthread_delay(self, 2000);
    return 0;
}

int8_t recv_out(ptthread_t* self) {
    
    uint16_t val;
    
    //printf("Receied data: \n");
        
    if (ptstream_read(&recv0, &val, 2) == 0) {
        printf("%d ", val);
    }
    
    
    // wait for new data
    ptthread_read_block(self, &recv0);
    
    return 0;
}
    
    
int main (void) {
    
    hal_hardwareinit();
    
    DDRB  = 0xFF;
    
    DDRD  = 0xFF;
    
    
    
    ptstream_init_alloc(&send0, 255);
    ptstream_init_alloc(&recv0, 255);
    
    muci_init(&interface0, &DDRD, &PORTD, &PIND, PD7,
                &DDRD, &PORTD, &PIND, PD6, &send0, &recv0, 3000);
    
    //ptstream_write(&send0, (void*) &teststring, 100);
    //printf("Send text message\n");
    
    ptthread_init(&thread[0], recv_out, RUNNING);
    ptthread_init(&thread[1], muci_updater, RUNNING);
    
    
    
    ptthread_main(thread, 2);
}