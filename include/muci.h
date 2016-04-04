/* Copyright 2016 Leon Tan
 * Refere to LICENSE file in main directory 
 * or http://www.gnu.org/licenses/old-licenses/gpl-2.0.txt
 * for full license conditions */

#ifndef MUCI_H
#define MUCI_H

typedef struct muci_iface_t {
    uint8_t data_port;
    uint8_t data_pin;
    uint8_t clk_port;
    uint8_t clk_pin;
    
} muci_iface_t;


/*
 * Initializes the muci interface
 */
int8_t muci_init(muci_iface_t*, uint8_t, uint8_t, uint8_t, uint8_t);

/*
 * Sends data oer the muci network
 * Arguments: interface, buffer, bufferlength
 */
int8_t muci_send(muci_iface_t*, void*, uint16_t);

/* 
 * Non-blocking receive routine
 * Must be called in apropiate time to receive data
 * Return 0 on success
 * Return -1 on buffer overflow
 * Return 1 on no data
 * Arguments: Interface, buffer, bufferlength
 */
int8_t muci_recv(muci_iface_t*, void*, uint16_t);

#endif
