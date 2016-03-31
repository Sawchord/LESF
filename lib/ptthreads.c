/* Copyright 2016 Leon Tan
 * Refere to LICENSE file in main directory 
 * or http://www.gnu.org/licenses/old-licenses/gpl-2.0.txt
 * for full license conditions */

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


void _update_times (ptthread_t* thread_list, uint8_t threadlist_length, uint32_t old_time){
    int i = 0;
    uint32_t now = hal_get_time();
    
    // not very intelligent overflow prevention
    // FIXME: This can be done better
    if (now - old_time > 65000) {
        old_time = 0;
    }
    
    for (i = 0; i < threadlist_length; i++) {
        
        if (thread_list[i].state == SLEEP) {
            
            if ( (int32_t) thread_list[i].substate - (int16_t) (now - old_time)  < 0) {
                
                thread_list[i].substate = 0;
            }
            else {
                thread_list[i].substate = thread_list[i].substate - (now - old_time);
            }
            
        }
    }
    
    
}


int8_t ptthread_main(ptthread_t* thread_list, uint8_t threadlist_length){
    
    uint8_t current_exec = 0;
    uint8_t last_exec = 0;
    
    uint32_t time0 = 0;
    uint32_t time1 = 0;
    
    
    // if all processes are blocked or waiting, this is the next process, 
    // that will wake up
    uint8_t next_process = 0;
    uint16_t next_tic = 0;
    
    while (1) {
        
        /* experimental */
        //_update_times(thread_list, threadlist_length, time1);
        //time1 = hal_get_time();
        
        
        ptthread_t* thread = &thread_list[current_exec];
        
        switch(thread->state) {
            
            case RUNNING:
                
                last_exec = current_exec;
                
                next_tic = 0;
                
                time0 = hal_get_time();
                thread->body(thread, thread->data, thread->data_length);
                current_exec = (current_exec + 1) % threadlist_length;
                
                // update time of sleeping processes
                _update_times(thread_list, threadlist_length, time0);
                
                break;
            
            case BLOCKED:
                
                /* TODO: blocked not implemented yet */
                
                current_exec = (current_exec + 1) % threadlist_length;
                
                break;
            
            case SLEEP:
                
                // current exec has run the whole list
                // without finding an executable thread
                if (current_exec == 
                    (last_exec + (threadlist_length -1) % threadlist_length)){
                    
                    current_exec = next_process;
                    time0 = hal_get_time();
                    hal_delay(next_tic);
                    _update_times(thread_list, threadlist_length, time0);
                    
                }
                else {
                    
                    if (thread->substate < next_tic) {
                        
                        next_tic = thread->substate;
                        next_process = current_exec;
                        
                    }
                    
                    current_exec = (current_exec + 1) % threadlist_length;
                }
                
                break;
            
            
            default:
                // should never be reached
                break;
        }
    }
}