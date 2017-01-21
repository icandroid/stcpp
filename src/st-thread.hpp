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
#pragma once

#include <cassert>
#include <cinttypes>
#include <functional>
#include <iostream>


namespace stcpp{

/**
 *  the base class of normal thread & joinable thread.
 *  plz REMEMBER that the thread is not same to OS thread.
 *
 *  the routines should be run in the context of the thread.
 *
 * */
class   ThreadBase {
public:
    typedef void*   thread_t;
protected:
    thread_t        st_thread;
    bool            is_joinable;
protected:
    static  bool    initialized;
    //
    //  routine to run by `st_thread_create'.
    //
    static  void*   run_thread( void* func );
public:
    //
    //  initialize the library.
    //
    static  void    init();
    //
    //  get the limit of file descriptors. it is a fixed value.
    //  thus, if we wanna to get a different value, we'd set the
    //  proposed value before call any stcpp::Thread routines.
    //
    static  int     getFdLimit();
    //
    //  make the current running thread sleep for some seconds.
    //  since the next scheduling.
    //
    static  int     sleep(int secs );
    //
    //  make the current running thread sleep for some microseconds.
    //
    static  int     usleep(uint64_t ms);
    //
    //  let the current running thread exit.
    //
    static  void    exit(void* retVal = NULL);
    //
    //  whether randomize the thread stack when the next thread is created.
    //
    static  void    randomize_stacks( bool onOff = true);
    //
    //  get thread specific data.
    //
    static  void*   get_data();
    //
    //  set thread specific data.
    //
    static  void    set_data(void* data);
protected:
    ThreadBase() : is_joinable(false){ init(); };
    virtual ~ThreadBase(){};
    //
    //  create the state thread.
    //
    void        create_thread(std::function<void()> func);
public:
    //
    //  check whether the thread is joinable.
    //
    bool        joinable(){ return is_joinable; }
    //
    //  get the native state thread instance.
    //
    thread_t    self(){ return st_thread; };
    //
    //  interrupt the thread (i.e., break the waiting status).
    //
    void        interrupt();
};

//
//  Unjoinable Thread
//
class   Thread : public ThreadBase{
public:
    template<typename _Callable, typename ... _Args>
    explicit
    Thread(_Callable&& _f, _Args&&..._args){
        auto    runobj = std::bind( _f, _args... );
        create_thread(runobj);
    };

    virtual ~Thread(){};
private:
    Thread()                = delete;
    Thread(const Thread&)   = delete;
    Thread( Thread&& )      = delete;
    Thread& operator=(const Thread&)    = delete;
};

//
//  Joinable thread.
//
class   JoinableThread : public ThreadBase{
public:
    template<typename _Callable, typename ... _Args>
    explicit
    JoinableThread(_Callable&& _f, _Args&&..._args) {
        is_joinable = true;
        auto    runobj = std::bind( _f, _args... );
        create_thread(runobj);
    };
    virtual ~JoinableThread(){};
    //
    //  join the thread.
    //
    bool    join(void** retVal = NULL);
private:
    JoinableThread()                        = delete;
    JoinableThread(const JoinableThread&)   = delete;
    JoinableThread( JoinableThread&& )      = delete;
    JoinableThread& operator=(const JoinableThread&)    = delete;
};

/**
 *  Conditional Variable.
 *
 * */
class   CondVar{
public:
    CondVar();
    virtual ~CondVar();
    bool    wait();
    bool    timed_wait(uint64_t ms);
    void    signal();
    void    broadcast();
protected:
    void*   cv;
};

/**
 *  Mutex Lock
 *
 * */
class   Mutex{
public:
    Mutex();
    virtual ~Mutex();
    bool    lock();
    bool    try_lock();
    bool    unlock();
protected:
    void*   mtx;
};

/**
 *  Scoped Lock
 *
 * */
class   ScopedLock{
public:
    ScopedLock(Mutex&);
    virtual ~ScopedLock();
protected:
    Mutex&  mutex_lock;
};

};// namespace stcpp

