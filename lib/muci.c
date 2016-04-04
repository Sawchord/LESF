/* Copyright 2016 Leon Tan
 * Refere to LICENSE file in main directory 
 * or http://www.gnu.org/licenses/old-licenses/gpl-2.0.txt
 * for full license conditions */

#include <inttypes.h>
#include <stdlib.h>
#include "muci.h"
#include "ptstream.h"

int8_t muci_init(muci_iface_t* interface,
                 uint8_t data_ddr,
                 uint8_t data_port,
                 uint8_t data_pin,
                 uint8_t data_selector,
                 uint8_t clk_ddr,
                 uint8_t clk_port,
                 uint8_t clk_pin,
                 uint8_t clk_selector,
                 ptstream_t* send_buffer,
                 ptstream_t* recv_buffer
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
    
    
    // setting up the interface
    
    // setting RTS/Data to Input
    interface->data_ddr &= ~(1 << interface->data_selector);
    
    //  setting CTR/Clk to Input
    interface->clk_ddr &= ~(1 << interface->clk_selector);
    
    return 0;
}



// TODO: CRC16 test




int8_t muci_update(muci_iface_t* interface) {
    
    switch (interface->state) {
        
        case IGNORE:
            // do nothing
            break;
        
        case STARTUP:
            // TODO: check, wether it is safe to go to init
            
            return STARTUP;
            break;
            
        case INIT:
            
            // set pins correctly to INIT setting
            // setting RTS/Data to Input, no pullup
            interface->data_ddr &= ~(1 << interface->data_selector);
            interface->data_port &= ~(1 << interface->data_selector);
            
            // setting CTR/Clk to Ouput high
            interface->clk_port |= (1 << interface->clk_selector);
            interface->clk_ddr |= (1 << interface->clk_selector);
            
            // check the RTS/Data for HIGH
            if (interface->data_pin & (1 << interface->data_selector)) {
                
                // go to clear to receive state
                interface->state = CTR;
                interface->substate = 0;
                
                // Set CTR/Clk to Input
                interface->clk_ddr &= ~(1 << interface->clk_selector);
                interface->clk_port &= ~(1 << interface->clk_selector);
                
                return CTR;
            }
            
            // check, if send buffer has data
            if (interface->send_buffer->read_p != interface->send_buffer->write_p) {
                
                // go to ready to send state
                interface->state = RTS;
                interface->substate = 0;
                
                // set RTS/Data to Output high
                interface->data_port |= (1 << interface->data_selector);
                interface->data_ddr |= (1 << interface->data_selector);
                
                // set CTR/Clk to in
                interface->clk_ddr &= (1 << interface->data_selector);
                interface->clk_port &= (1 << interface->data_selector);
                
                return RTS;
            }
            
            return INIT;
            break;
        
        case RTS:
            
            // check if CTR/Clk is LOW
            if (! (interface->clk_pin & (1 << interface->clk_selector))) {
                // begin send process
                
                
            }
            else {
                // if network is not clear to receive, wait longer
                interface->substate++;
                
                // TODO: implement transition to DRAWOUT
                return RTS;
            }
            
            
            break;
            
        case CTR:
            
            
            break;
        
        case DRAWOUT:
            
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