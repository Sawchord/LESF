/* Copyright 2016 Leon Tan
 * Refere to LICENSE file in main directory 
 * or http://www.gnu.org/licenses/old-licenses/gpl-2.0.txt
 * for full license conditions */
#include <inttypes.h>
#include <stdlib.h>

#include "ptstream.h"


int8_t ptstream_init(ptstream_t* address, uint8_t length, uint8_t* buffer) {
    
    address->address = buffer;
    address->length = length;
    address->read_p = 0;
    address->write_p = 0;
    
    return 0;
    
}

int8_t ptstream_init_alloc(ptstream_t* address, uint8_t length) {
    
    /* allocate buffer */
    uint8_t* buffer;
    if (  (buffer = (uint8_t*) malloc(length * sizeof (uint8_t)) )   == NULL) {
        return -1;
    }
    
    ptstream_init(address, length, buffer);
    
    return 0;
}


void ptstream_free(ptstream_t* address) {
    free(address->address);
    free (address);
}

int8_t ptstream_read(ptstream_t* address, uint8_t* data, uint8_t length) {
    
    /* read pointer scopes from 0 to 255
     * NOTE: in a real parallel system, here is a race condition
     * This is going to be problematic, when using ptstreams between interupscontroller and ptthreads
     */
    int reader = 0;
    
    while (reader < length) {
        
        if ( ((reader + address->read_p) % 255) == address->write_p){
            return -1;
        }
        
        data[reader] = address->address[((reader + address->read_p) % 255)];
        
        reader++;
    }
    
    // update read_p
    address->read_p = (address->read_p + reader) % 255;
    
    
    return 0;
}


int8_t ptstream_write(ptstream_t* address, uint8_t* data, uint8_t length) {
    
    int writer = 0;
    
    while (writer < length) {
        writer++;
        
        if ( ((writer + address->write_p) % 255) == address->read_p ){
            return -1;
        }
        
        address->address[((writer + address->write_p) % 255 )] = data[writer-1];
        
    }
    
    // update write_p
    address->write_p = (address->write_p + writer) % 255;
    
    return 0;
}