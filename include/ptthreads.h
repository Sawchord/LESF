#ifndef PTTHREADS_N
#define PTTHREADS_N

#include <inttypes.h>

/* state definitions */
#define INIT 0x0
#define RUNNING 0x1
#define BLOCKED 0x2
#define WAITING 0x3
#define SHUTDOWN 0x4


#include "ptthreads.h"
/* threading features */

/* thread control block entries */
typedef struct ptthread_t ptthread_t;

struct ptthread_t {
    uint16_t (*body) (ptthread_t*, uint8_t*, uint8_t**);
    uint8_t state;
    uint16_t substate;
    
};


int8_t pthread_init_thread_list(ptthread_t*, uint8_t);

int8_t ptthread_init(ptthread_t*);

int8_t ptthread_block(ptthread_t*);

#endif