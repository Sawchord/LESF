/* Copyright 2016 Leon Tan
 * Refere to LICENSE file in main directory 
 * or http://www.gnu.org/licenses/old-licenses/gpl-2.0.txt
 * for full license conditions */

#ifndef MUCI_H
#define MUCI_H

#include "ptstream.h"



#define INIT 0
#define RTS 1
#define SEND 2
#define CTR 3
#define RECV 4
#define DRAWOUT 5
#define IGNORE 6
#define STARTUP 7

/*typedef struct ipv6_address_t {
    uint8_t add[16];
} ipv6_address_t;*/
typedef uint8_t ipv6_address_t[16];

typedef struct muci_frame_t {
    uint16_t dest;
    uint16_t src;
    uint16_t length;
    uint8_t* buffer;
    uint16_t CRC16;
} muci_frame_t;


typedef struct muci_iface_t {
    volatile uint8_t* data_ddr;
    volatile uint8_t* data_port;
    volatile uint8_t* data_pin;
    uint8_t data_selector;
    volatile uint8_t* clk_ddr;
    volatile uint8_t* clk_port;
    volatile uint8_t* clk_pin;
    uint8_t clk_selector;
    
    
    uint8_t state;
    uint16_t substate;
    
    uint16_t baudrate;
    
    ptstream_t* send_buffer;
    ptstream_t* recv_buffer;
    
    /* Since this is layer 2 stuff, it should have its own abstraction
    // TODO: Make these hardcodeable in program space
    ipv6_address_t address;
    //uint8_t network[14];
    //uint16_t address;*/
    
} muci_iface_t;


int8_t muci_update(muci_iface_t*);
// sends the frame by copying to send buffer
//int8_t muci_send_frame(muci_iface_t*, muci_frame_t*);

// receives frame by reading from readbuffer
//int8_t muci_recv_frame(muci_iface_t*, muci_frame_t*);

// sends length data in buffer to address
int8_t muci_send(muci_iface_t*, uint8_t*, uint16_t*);

// receives message, returns pointer to data, length of message and sender
int8_t muci_recv(muci_iface_t*, ipv6_address_t*, uint8_t**, uint16_t*);

/*
 * Initializes the muci interface
 */
int8_t muci_init(muci_iface_t*, 
                 volatile uint8_t*, // data_ddr
                 volatile uint8_t*, // data_port
                 volatile uint8_t*, // data_pin
                 uint8_t, // data_selector
                 volatile uint8_t*, // clk_ddr
                 volatile uint8_t*, // clk_port
                 volatile uint8_t*, // clk_pin
                 uint8_t, // clk_selector
                 ptstream_t*, // send_buffer
                 ptstream_t*, // recv_buffer
                 uint16_t // baudrate
                );

#endif
