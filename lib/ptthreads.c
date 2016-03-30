#include <inttypes.h>
#include <stdlib.h>

#include "hal.h"
#include "ptthreads.h"


int8_t ptthread_init(
    ptthread_t* thread,
    ptthread_body_t function,
    uint8_t init_state,
    uint8_t* data,
    uint16_t data_length
){
    
    thread->body = function;
    
    thread->data = data;
    thread->data_length = data_length;
    
    thread->state = init_state;
    thread->substate = 0;
    
    return 0;
}

int8_t ptthread_setstate(ptthread_t* thread, int8_t state) {
    
    thread->state = state;
    
    return 0;
    
}


int8_t ptthread_delay(ptthread_t* thread, uint16_t ms) {
    
    thread->state = SLEEP;
    thread->substate = ms;
    
    return 0;
    
}

int8_t ptthread_main(ptthread_t* thread_list, uint8_t threadlist_length){
    
    static uint8_t current_exec = 0;
    static uint8_t last_exec = 0;
    
    /* holds the time, the next sleeping thread wants to execute */
    static uint8_t next_tic = 0;
    
    while (1) {
        
        ptthread_t* thread = &thread_list[current_exec];
        
        /* TODO: update waiting times*/
        
        switch(thread->state) {
            
            case ERROR:
                
                return -1;
                
                break;
            
            case INIT:
                
                break;
            
            case RUNNING:
                
                last_exec = current_exec;
                thread->body(thread, thread->data, thread->data_length);
                current_exec = (current_exec + 1) % threadlist_length;
                
                break;
            
            case BLOCKED:
                
                current_exec = (current_exec + 1) % threadlist_length;
                
                break;
            
            case SLEEP:
                
                current_exec = (current_exec + 1) % threadlist_length;
                
                break;
            
            
            default:
                /* should never be reached */
                break;
        }
    }
}