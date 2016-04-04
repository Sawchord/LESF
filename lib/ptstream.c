/* Copyright 2016 Leon Tan
 * Refere to LICENSE file in main directory 
 * or http://www.gnu.org/licenses/old-licenses/gpl-2.0.txt
 * for full license conditions */

#include <inttypes.h>
#include <stdlib.h>

#include "ptstream.h"


int8_t ptstream_init(ptstream_t* address, uint16_t length, uint8_t* buffer) {
    
    address->address = buffer;
    address->length = length;
    address->read_p = 0;
    address->write_p = 0;
    
    return 0;
    
}

int8_t ptstream_init_alloc(ptstream_t* address, uint16_t length) {
    
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

int8_t ptstream_read(ptstream_t* stream, uint8_t* data, uint16_t length) {
    
    /* NOTE: in a real parallel system, here is a race condition
     * This is going to be problematic, when using ptstreams between interupscontroller and ptthreads
     */
    int reader = 0;
    
    while (reader < length) {
        
        if ( ((reader + stream->read_p) % stream->length) == stream->write_p){
            return -1;
        }
        
        *(data + (reader * sizeof(uint8_t))) = 
            stream->address[((reader + stream->read_p) % stream->length)];
        
        reader++;
    }
    
    // update read_p
    stream->read_p = (stream->read_p + reader) % stream->length;
    
    
    return 0;
}


int8_t ptstream_write(ptstream_t* stream, uint8_t* data, uint16_t length) {
    
    int writer = 0;
    
    while (writer < length) {
        
        if ( ((writer + stream->write_p + stream->length + 1) % stream->length) 
            == stream->read_p ){
            return -1;
        }
        
        stream->address[((writer + stream->write_p) % stream->length )] = 
             *(data + (writer * sizeof(uint8_t)));
        writer++;
        
    }
    
    // update write_p
    stream->write_p = (stream->write_p + writer) % stream->length;
    
    return 0;
}