/*
Copyright (c) 2017, Peixu Zhu (朱培旭)
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

* Redistributions of source code must retain the above copyright notice, this
  list of conditions and the following disclaimer.

* Redistributions in binary form must reproduce the above copyright notice,
  this list of conditions and the following disclaimer in the documentation
  and/or other materials provided with the distribution.

* Neither the name of the copyright holder nor the names of its
  contributors may be used to endorse or promote products derived from
  this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/
#include "st-thread.hpp"
#include "st/st.h"


namespace   stcpp{

#define TO_ST(p)    ((st_thread_t)p)
//
//  thread executor wrapper.
//
class   _ExeWrapper{
    std::function<void()>   func;
public:
    _ExeWrapper(std::function<void()>_f) : func(_f){};
    void    run(){
        func();
    }
};

bool    ThreadBase::initialized = false;

void
ThreadBase::init(){
    if (!initialized){
        initialized = true;
        assert(0 == st_set_eventsys( ST_EVENTSYS_ALT ) );   // before st_init
        assert(0 == st_init() );
    }
}

int
ThreadBase::getFdLimit(){
    return st_getfdlimit();
}

int
ThreadBase::sleep(int secs ){
    return st_sleep(secs);
}

int
ThreadBase::usleep(uint64_t ms){
    return st_usleep(ms);
}

void
ThreadBase::exit(void* retVal ){
    st_thread_exit(retVal);
}

void
ThreadBase::randomize_stacks( bool onOff ){
    st_randomize_stacks((int)onOff);
}

void*
ThreadBase::get_data(){
    return st_thread_getspecific(0);
};

void
ThreadBase::set_data(void* data){
    if (st_thread_setspecific(0, data) != 0 ){
        int keyn;
        st_key_create(&keyn, NULL);
        st_thread_setspecific(0, data);
    };
}

void
ThreadBase::interrupt(){
    st_thread_interrupt( TO_ST(st_thread) );
}

void*
ThreadBase::run_thread( void* func ){
    _ExeWrapper*    exe = (_ExeWrapper*)func;
    exe->run();
    delete exe;
    return 0;
}

void
ThreadBase::create_thread(std::function<void()> func){
    _ExeWrapper*   exe = new _ExeWrapper(func);
    st_thread   = st_thread_create( ThreadBase::run_thread, exe, is_joinable ? 1 : 0, 0);
}

bool
JoinableThread::join(void** retVal ){
    if (is_joinable){
        is_joinable = false;
        if ( 0 == st_thread_join(TO_ST(st_thread), retVal) )
            return true;
    }
    return false;
}

/////////////////////////////////////////////////////////////////
#define TO_CV(p)    ((st_cond_t)p)
CondVar::CondVar(){
    cv  = st_cond_new();
}

CondVar::~CondVar(){
    if (cv){
        st_cond_destroy( TO_CV(cv) );
        cv  = NULL;
    }
}

bool
CondVar::wait(){
    return  ( 0 == st_cond_wait( TO_CV(cv) ) );
};

bool
CondVar::timed_wait(uint64_t ms){
    return  ( 0 == st_cond_timedwait( TO_CV(cv), ms) );
};

void
CondVar::signal(){
    st_cond_signal( TO_CV(cv) );
}

void
CondVar::broadcast(){
    st_cond_broadcast( TO_CV(cv) );
}

///////////////////////////////////////////////////
#define TO_MUTEX(p) ((st_mutex_t)p)
Mutex::Mutex(){
    mtx = st_mutex_new();
}

Mutex::~Mutex(){
    if (mtx){
        st_mutex_destroy(TO_MUTEX(mtx));
        mtx = NULL;
    }
}

bool
Mutex::lock(){
    return (0 == st_mutex_lock(TO_MUTEX(mtx)) );
}

bool
Mutex::try_lock(){
    return (0 == st_mutex_trylock( TO_MUTEX(mtx) ));
}

bool
Mutex::unlock(){
    return (0 == st_mutex_unlock( TO_MUTEX(mtx)) );
}

ScopedLock::ScopedLock(Mutex& m) : mutex_lock(m){
    mutex_lock.lock();
}

ScopedLock::~ScopedLock(){
    mutex_lock.unlock();
}

};  // namespace stcpp

