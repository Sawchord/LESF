/* Copyright 2016 Leon Tan
 * Refere to LICENSE file in main directory 
 * or http://www.gnu.org/licenses/old-licenses/gpl-2.0.txt
 * for full license conditions */

#ifndef PTTHREADS_N
#define PTTHREADS_N

#include <inttypes.h>

/* schreduler state definitions */
/* TODO: make em right */

/* thread state definitions */
#define ERROR -1
#define INIT 0
#define RUNNING 1
#define BLOCKED 2
#define SLEEP 3
#define SHUTDOWN 2


#include "ptthreads.h"
#include "ptstream.h"
/* threading features */


/* thread control block entries */
typedef struct ptthread_t ptthread_t;
typedef int8_t (*ptthread_body_t)(ptthread_t* self);

struct ptthread_t {
    //uint8_t (*body) (ptthread_t*, uint8_t*, uint8_t**);
    ptthread_body_t body;
    int8_t state;
    uint16_t substate;
    ptstream_t* blocker;
    
};



/* initializes a single ptthread_t in initial state */
int8_t ptthread_init(ptthread_t*, ptthread_body_t, uint8_t);

/* change the state of a specific ptthread  */
int8_t ptthread_setstate(ptthread_t*, int8_t);

/* delay execution by at least n ms */
int8_t ptthread_delay(ptthread_t*, uint16_t);

/* block until stream has data*/
int8_t ptthread_read_block(ptthread_t*, ptstream_t*);

/* the ptthreads core loop */
int8_t ptthread_main(ptthread_t*, uint8_t);

#endif