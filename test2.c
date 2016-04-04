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

// the muci_interface used on this device
muci_iface_t interface0;

// the streams for the interface
ptstream_t send0;
ptstream_t recv0;

ptthread_t thread[2];

ipv6_address_t address = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x00, 0x01 };

int8_t muci_updater(ptthread_t* self) {
    
    muci_update(&interface0);
    ptthread_delay(self, 10);
    
    return 0;
}

int8_t sender(ptthread_t* self) {
    
    // TODO: generate something to print
    // TODO: send over wire
    
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
    
    ptstream_init_alloc(&send0, 255);
    ptstream_init_alloc(&recv0, 255);
    
    muci_init(&interface0, DDRB, PORTB, PINB, PB2,
            DDRB, PORTB, PINB, PB1, &send0, &recv0);
    
    ptthread_init(&thread[0], debug_out, RUNNING);
    ptthread_init(&thread[1], muci_updater, RUNNING);
    
    
    
    ptthread_main(thread, 2);
}