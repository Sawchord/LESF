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
    
    uint16_t timediff;
    
    // not very intelligent overflow prevention
    //if (now - old_time > 120000) {
    //    old_time = 0;
    //}
    
    if (now - old_time < 0) {
        // if the timer voerflowed
        timediff = now + (65335 - old_time);
    }
    else {
        timediff = now - old_time;
    }
    
    
    for (i = 0; i < threadlist_length; i++) {
        
        if (thread_list[i].state == SLEEP) {
            
            if ( thread_list[i].substate < timediff) {
                
                thread_list[i].substate = 0;
            }
            else {
                thread_list[i].substate = thread_list[i].substate - timediff;
            }
            
        }
    }
    
    
}


int8_t ptthread_main(ptthread_t* thread_list, uint8_t threadlist_length){
    
    uint8_t current_exec = 0;
    uint8_t last_exec = 0;
    
    uint32_t time0 = 0;
    
    while (1) {
        
        
        
        ptthread_t* thread = &thread_list[current_exec];
        
        switch(thread->state) {
            
            case RUNNING:
                
                last_exec = current_exec;
                
                thread->body(thread, thread->data, thread->data_length);

                _update_times(thread_list, threadlist_length, time0);
                time0 = hal_get_time();
                
                current_exec = (current_exec + 1) % threadlist_length;
                
                break;
            
            case BLOCKED:
                
                /* TODO: blocked not implemented yet */
                
                current_exec = (current_exec + 1) % threadlist_length;
                
                break;
            
            case SLEEP:
                
                // current exec has run the whole list
                // without finding an executable thread
                if (current_exec == 
                    ((last_exec + (threadlist_length - 1 )) % threadlist_length)){
                    
                    uint8_t next_process = current_exec;
                    uint16_t next_tic = thread->substate;
                    uint8_t i = 0;
                    
                    for (i = 0; i < threadlist_length; i++) {
                        
                        if (thread_list[i].substate < next_tic) {
                            
                            next_tic = thread_list[i].substate;
                            next_process = i;
                            
                        }
                    }
                    
                    
                    current_exec = next_process;
                    time0 = hal_get_time();
                    hal_delay(next_tic);
                    _update_times(thread_list, threadlist_length, time0);
                    
                    // set next process running
                    thread_list[current_exec].state = RUNNING;
                    thread_list[current_exec].substate = 0;
                    
                    time0 = hal_get_time();

                    break;
                }
                
                current_exec = (current_exec + 1) % threadlist_length;
                
                break;
            
            
            default:
                // should never be reached
                break;
        }
    }
}