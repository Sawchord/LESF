/* Copyright 2016 Leon Tan
 * Refere to LICENSE file in main directory 
 * or http://www.gnu.org/licenses/old-licenses/gpl-2.0.txt
 * for full license conditions */

#ifndef PTSTREAM_H
#define PTSTREAM_H


/* the stream structure */
typedef struct {
    uint8_t* address;
    uint8_t length;
    uint8_t read_p;
    uint8_t write_p;
} ptstream_t;

/* initialize a stream
 * statically assign buffer
 * arguments: address, length, pointer to buffer */
int8_t pttstream_init(ptstream_t*, uint16_t, uint8_t*);

/* initialize a stream
 * arguments: address, length */
int8_t ptstream_init_alloc (ptstream_t*, uint16_t);

/* frees a stream */
void ptstream_free (ptstream_t*);

/* write n bytes to stream 
 * returns 0 on success
 * return -1 on underflow and does not update the read pointer
 * however, data is written to data pointerand it is up to the user to discard it */
int8_t ptstream_write (ptstream_t*, uint8_t*, uint16_t);

/* read n bytes from stream
 * returns 0 on success */
int8_t ptstream_read (ptstream_t*, uint8_t*, uint16_t);

/* 
 * TODO: printf like write and read
 */



#endif