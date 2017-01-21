#include <iostream>
#include <cassert>
#include <functional>
#include "st/st.h"
#include "gtest/gtest.h"

//////////////////////////////////////////

class   ExeBase{
public:
    virtual void    run(){};
};

template<typename _Callable>
class   Exe : public ExeBase{
    _Callable   f;
public:
    Exe(_Callable&& _f) : f(std::forward<_Callable>(_f)){};
    virtual void    run(){
        f();
    }
};

class   MyLambda {
public:
    static  void*   thread_func(void* arg){
        std::cout << __FUNCTION__ << std::endl;
        std::cout << st_thread_self() << std::endl;
        ExeBase*    exe = (ExeBase*)arg;
        exe->run();
        delete  exe;
        return NULL;
    }
};

TEST(testST, lambda){
    assert( 0 == st_set_eventsys(ST_EVENTSYS_ALT));
    assert( 0 == st_init() );
    for(int i = 0; i < 20; i++){
        int a = 0;
        auto x = [](){ std::cout << 100 << std::endl; };
        ExeBase*    exe = new Exe<decltype(x)>(std::move(x));
        st_thread_t th  = st_thread_create(MyLambda::thread_func, exe, 1, 0);
        //st_thread_t th  = st_thread_create(&MyLambda::thread_func, &a, 1, 0);
        st_thread_join(th, NULL);
    };
}
