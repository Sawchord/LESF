/* Copyright 2016 Leon Tan
 * Refere to LICENSE file in main directory 
 * or http://www.gnu.org/licenses/old-licenses/gpl-2.0.txt
 * for full license conditions */

#include <inttypes.h>
#include <stdlib.h>
#include "hal.h"
#include "muci.h"
#include "ptstream.h"

// need to be removed later
#include <avr/io.h>


#define US 1000000L
#define WAIT_ITS 8

int8_t muci_init(muci_iface_t* interface,
                 volatile uint8_t* data_ddr,
                 volatile uint8_t* data_port,
                 volatile uint8_t* data_pin,
                 uint8_t data_selector,
                 volatile uint8_t* clk_ddr,
                 volatile uint8_t* clk_port,
                 volatile uint8_t* clk_pin,
                 uint8_t clk_selector,
                 ptstream_t* send_buffer,
                 ptstream_t* recv_buffer,
                 uint16_t baudrate
                 ){
    
    interface->data_ddr = data_ddr;
    interface->data_port = data_port;
    interface->data_pin = data_pin;
    interface->data_selector = data_selector;
    interface->clk_ddr = clk_ddr;
    interface->clk_port = clk_port;
    interface->clk_pin = clk_pin;
    interface->clk_selector = clk_selector;
    
    interface->state = INIT;
    interface->substate = 0;
    
    interface->send_buffer = send_buffer;
    interface->recv_buffer = recv_buffer;
    
    interface->baudrate = baudrate;
    
    // setting up the interface
    
    // setting RTS/Data to Input
    *(interface->data_ddr) &= ~(1 << interface->data_selector);
    
    //  setting CTR/Clk to Input
    *(interface->clk_ddr) &= ~(1 << interface->clk_selector);
    
    return 0;
}



// TODO: CRC16 test


int8_t muci_update(muci_iface_t* interface) {
    uint32_t delta_t;
    
    switch (interface->state) {
        
        case IGNORE:
            // do nothing
            
            return IGNORE;
            break;
        
        case STARTUP:
            // TODO: check, wether it is safe to go to init
            
            return STARTUP;
            break;
            
        case INIT:
            
            // set pins correctly to INIT setting
            // setting RTS/Data to Input, no pullup
            *(interface->data_ddr) &= ~(1 << interface->data_selector);
            *(interface->data_port) &= ~(1 << interface->data_selector);
            
            // setting CTR/Clk to Ouput high
            *(interface->clk_port) |= (1 << interface->clk_selector);
            *(interface->clk_ddr) |= (1 << interface->clk_selector);
            
            // check the RTS/Data for HIGH
            if ( *(interface->data_pin) & (1 << interface->data_selector)) {
                
                // go to clear to receive state
                interface->state = CTR;
                interface->substate = 0;
                
                
                return CTR;
            }
            
            // check, if send buffer has data
            if (interface->send_buffer->read_p != interface->send_buffer->write_p) {
                
                // go to ready to send state
                interface->state = RTS;
                interface->substate = 0;
                
                // set RTS/Data to Output high
                *(interface->data_port) |= (1 << interface->data_selector);
                *(interface->data_ddr) |= (1 << interface->data_selector);
                
                // set CTR/Clk to in
                *(interface->clk_ddr) &= (1 << interface->data_selector);
                *(interface->clk_port) &= (1 << interface->data_selector);
                
                // wait for CTR from other nodes
                return RTS;
            }
            
            return INIT;
            break;
        
        case RTS:
            
            // need to deactivate interrupts, this is a timed event
            hal_interrupt_disable();
            // count time in microseconds
            delta_t = 0;
            
            // polling wait until CTR/Clk gets low
            while ( *(interface->clk_pin) & (1 << interface->clk_selector)) {
                
                delta_t += US / interface->baudrate;
                hal_delay_us(US / interface->baudrate);
                
                //printf("Sender waits until CTR gets low.\n");
                
                // TODO: DRAWOUT if time takes to long
            }
            
            // init the send process
            
            // set RTS/Data to output LOW
            *(interface->data_port) &= ~(1 << interface->data_selector);
            *(interface->data_ddr) |= (1 << interface->data_selector);
            
            // set CTR/Clk to output LOW
            *(interface->clk_port) &= ~(1 << interface->clk_selector);
            *(interface->clk_ddr) |= (1 << interface->clk_selector);
            
            // wait at least two phases to prevent RTS to be read as data
            hal_delay_us( (2*US)/ interface->baudrate);
            
            // begin the send process
            // send all data on the send_buffer
            while (interface->send_buffer->read_p != interface->send_buffer->write_p) {
                uint8_t bitc = 0;
                
                // update delta_t
                delta_t += 8 * (US / interface->baudrate);
                
                uint8_t byte;
                ptstream_read(interface->send_buffer, &byte, 1);
                
                for (bitc = 0; bitc < 8; bitc++) {
                    
                    // put the data to the Data Pin
                    *(interface->data_port) ^= ( -((byte >> bitc) & 1) ^ *(interface->data_port))
                    & (1 << interface->data_selector);
                        
                    //printf(".%d.", (byte >> bitc) & 1);
                    
                    // raise Clk 
                    *(interface->clk_port) |= (1 << interface->clk_selector);
                    
                    
                    // wait
                    hal_delay_us(US / (2* interface->baudrate) );
                    
                    // falling Clk 
                    *(interface->clk_port) &= ~(1 << interface->clk_selector);
                    
                    //printf("Sending %02x bit nr %d\n", byte, bitc);
                    
                    // wait
                    hal_delay_us(US / (2* interface->baudrate) );
                }
                
            }
            
            // update time and go back to init
            interface->state = INIT;
            interface->substate = 0;
            
            hal_update_time(delta_t / 1000);
            hal_interrupt_enable();
            
            // To signal hangup, wait for WAIT_ITS us
            hal_delay_us(WAIT_ITS);
            
            return INIT;
            
            break;
            
        case CTR:
            // start timed event
            hal_interrupt_disable();
            
            // Set CTR/Clk to Input to signal ready
            *(interface->clk_ddr) &= ~(1 << interface->clk_selector);
            *(interface->clk_port) &= ~(1 << interface->clk_selector);
            
            // wait at least two phases to prevent RTS to be read as data
            hal_delay_us( (2*US)/ interface->baudrate);
            
            delta_t = 0;
            
            uint8_t byte = 0;
            uint8_t bitc = 0;
            while (1) {
                
                // polling wait until CTR/Clk raises high
                while ( !(*(interface->clk_pin) & (1 << interface->clk_selector)) ) {
                    
                    delta_t += US / (2* interface->baudrate);
                    hal_delay_us(US / (2* interface->baudrate));
                    
                    //printf("Receiver waits until CTR gets high.\n");
                    //printf("^");
                    
                    interface->substate++;
                    if (interface->substate > 2 * WAIT_ITS) {
                        
                        *(interface->clk_port) |= (1 << interface->clk_selector);
                        *(interface->clk_ddr) |= ( 1 << interface->clk_selector);
                        
                        // got back to init
                        interface->state = INIT;
                        interface->substate = 0;
                        
                        hal_update_time(delta_t / 1000);
                        hal_interrupt_enable();
                        return INIT;
                    }
                    
                }
                
                interface->substate = 0;
                
                uint8_t read_bit;
                read_bit = ( ( *(interface->data_pin) & (1 << interface->data_selector) )
                >> interface->data_selector);
                
                //printf(".%d.", read_bit);
                
                byte ^= (-read_bit ^ byte) & (1 << bitc);
                
                bitc++;
                
                if (bitc == 8) {
                    
                    //printf("B:%x", byte);
                    ptstream_write(interface->recv_buffer, &byte, 1);
                    bitc = 0;
                    byte = 0;
                }
                
                
                // polling wait until CTR/Clk gets low
                while (*(interface->clk_pin) & (1 << interface->clk_selector)) {
                    
                    delta_t += US / (2* interface->baudrate);
                    hal_delay_us(US / (2* interface->baudrate));
                    
                    //printf("Receiver waits until CTR gets low.\n");
                    //printf("-");
                    
                    interface->substate++;
                    if (interface->substate > 2 * WAIT_ITS) {
                        
                        *(interface->clk_port) |= (1 << interface->clk_selector);
                        *(interface->clk_ddr) |= ( 1 << interface->clk_selector);
                        
                        // got back to init
                        interface->state = INIT;
                        interface->substate = 0;
                        
                        hal_update_time(delta_t / 1000);
                        hal_interrupt_enable();
                        return INIT;
                    }
                }
                
                interface->substate = 0;
                
            }
            
            /* probably dead code
            printf("time to go back to init\n");
            // update time go back to init
            interface->state = INIT;
            interface->substate = 0;
            
            hal_update_time(delta_t / 1000);
            hal_interrupt_enable();
            return INIT;
            */
            break;
        
        case DRAWOUT:
            
            //TODO: Collision management
            
            break;
            
        default:
            
            break;
        
    }
    
    return -1;
}



int8_t muci_send(muci_iface_t* interface,
                 uint8_t* buffer,
                 uint16_t* buffer_length
                 ){
    
    return 0;
    
}

int8_t muci_recv(muci_iface_t* interface,
                 ipv6_address_t* src,
                 uint8_t** buffer,
                 uint16_t* buffer_length
                 ){
    
    return 0;
    
}