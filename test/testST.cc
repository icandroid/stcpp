#include <iostream>
#include <cassert>
#include <functional>
#include "st/st.h"
#include "gtest/gtest.h"

static  void*   thread_func(void* args){
    int*    pa = (int*)args;
    std::cout << (*pa) + 123 << std::endl;
    std::cout << "self=" << st_thread_self() << std::endl;
    return NULL;
}

TEST(testST, UnjoinableThread_func ){
    assert( 0 == st_set_eventsys(ST_EVENTSYS_ALT) );
    assert( 0 == st_init() );
    int     a = 100;
    for(int i = 0; i < 2; i++){
        st_thread_t     th = st_thread_create( thread_func, &a, 0, 0);
        std::cout << "th = " << th << std::endl;
    }
    for(int i = 0; i < 10; i++){
        st_usleep(100);
    }
}

TEST(testST, joinableThread_func ){
    int     a = 100;
    for(int i = 0; i < 2; i++){
        st_thread_t     th = st_thread_create( thread_func, &a, 1, 0);
        std::cout << "th = " << th << std::endl;
        void*   retVal = 0;
        std::cout << "before join" << std::endl;
        st_thread_join(th, &retVal);
        std::cout << "after join" << std::endl;
    }
}

//////////////////////////////////////////////////////

class   StaticMethod {
public:
    StaticMethod(int a) : a(a) {};
    virtual ~StaticMethod(){};
    static  void*   thread_func(void* args){
        StaticMethod*    pa = (StaticMethod*)args;
        std::cout << pa->a + 123 << std::endl;
        std::cout << "self=" << st_thread_self() << std::endl;
        return NULL;
    }
    int a;
    //
    std::function<void*()>   f = []()->void*{
            std::cout <<  123 << std::endl;
            std::cout << "self=" << st_thread_self() << std::endl;
            return NULL;
        };
    static void*    do_func(void* args){
        StaticMethod*    pa = (StaticMethod*)args;
        return pa->f();
    }
    int     func_count = 0;
    int     lambda_count = 0;
};

TEST(testST, joinableThread_StaticMethod){
    int     a = 100;
    for(int i = 0; i < 2; i++){
        StaticMethod*   s = new StaticMethod(100);
        st_thread_t     th = st_thread_create( &StaticMethod::thread_func, s, 1, 0);
        std::cout << "th = " << th << std::endl;
        void*   retVal = 0;
        std::cout << "before join" << std::endl;
        st_thread_join(th, &retVal);
        std::cout << "after join" << std::endl;
        delete s;
    }
}

TEST(testST, joinableThread_stdfunction){
    int     a = 100;
    for(int i = 0; i < 2; i++){
        StaticMethod*   s = new StaticMethod(100);
        st_thread_t     th = st_thread_create( &StaticMethod::do_func, s, 1, 0);
        std::cout << "th = " << th << std::endl;
        void*   retVal = 0;
        std::cout << "before join" << std::endl;
        st_thread_join(th, &retVal);
        std::cout << "after join" << std::endl;
        delete s;
    }
}

//////////////////////////////////////////
