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
/* threading features */


/* thread control block entries */
typedef struct ptthread_t ptthread_t;
typedef uint8_t (*ptthread_body_t)(ptthread_t* self, uint8_t* data, uint16_t length);

struct ptthread_t {
    //uint8_t (*body) (ptthread_t*, uint8_t*, uint8_t**);
    ptthread_body_t body;
    
    uint8_t* data;
    uint16_t data_length;
    
    int8_t state;
    uint16_t substate;
    
};



/* initializes a single ptthread_t in initial state */
int8_t ptthread_init(ptthread_t*, ptthread_body_t, uint8_t, uint8_t*, uint16_t);

/* change the state of a specific ptthread  */
int8_t ptthread_setstate(ptthread_t*, int8_t);

/* delay execution by at least n ms */
int8_t ptthread_delay(ptthread_t*, uint16_t);

/* the ptthreads core loop */
int8_t ptthread_main(ptthread_t*, uint8_t);

#endif