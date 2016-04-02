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
    uint8_t init_state
){
    
    thread->body = function;
    
    
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

int8_t ptthread_read_block(ptthread_t* thread, ptstream_t* stream) {
    
    thread->state = BLOCKED;
    
    // blocked read has substate 0
    thread->substate = 0;
    thread->blocker = stream;
    
    return 0;
}

void _update_times (ptthread_t* thread_list,
    uint8_t threadlist_length, 
    uint32_t old_time
){
    
    
    uint8_t i = 0;
    uint32_t now = hal_get_time();
    
    uint16_t timediff;
    if ((int32_t) now - old_time < 0) {
        // if the timer overflowed
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

void _update_block (ptthread_t* thread_list, uint8_t threadlist_length ){
    
    uint8_t i = 0;
    
    for (i = 0; i < threadlist_length; i++) {
        
        // substate 0 means blocking read
        // blocking write may or may not be introduced in the future
        if (thread_list[i].state == BLOCKED && thread_list[i].substate == 0) {
            
            if (thread_list[i].blocker->read_p != thread_list[i].blocker->write_p) {
                
                thread_list[i].state = RUNNING;
                thread_list[i].substate = 0;
                
            }
        }
    }
}

int8_t ptthread_main(ptthread_t* thread_list, uint8_t threadlist_length){
    
    uint8_t current_exec = 0;
    uint8_t last_exec = 0;
    
    uint32_t time0 = 0;
    
    while (1) {
        
        // FIXME: update every loop necessary?
        _update_block(thread_list, threadlist_length);
        
        ptthread_t* thread = &thread_list[current_exec];
        
        if(thread->state == RUNNING) {
            
            last_exec = current_exec;
            
            thread->body(thread);

            _update_times(thread_list, threadlist_length, time0);
            time0 = hal_get_time();
            
            current_exec = (current_exec + 1) % threadlist_length;
                
        }
        else if (thread->state == SLEEP || thread->state == BLOCKED) {
        
            // current exec has run the whole list
            // without finding an executable thread
            if (current_exec == 
                ((last_exec + (threadlist_length - 1 )) % threadlist_length)){
                
                uint8_t next_process = current_exec;
                uint16_t next_tic = thread->substate;
                uint8_t i = 0;
                
                for (i = 0; i < threadlist_length; i++) {
                    
                    if (thread_list[i].state == SLEEP) {
                        if (thread_list[i].substate < next_tic) {
                            
                            next_tic = thread_list[i].substate;
                            next_process = i;
                            
                        }
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
                
            }
            else {
                current_exec = (current_exec + 1) % threadlist_length;
            }
                
        }
        else {
            // should never be reached
        }
    }
}