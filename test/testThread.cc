#include <unistd.h>
#include <cstdio>
#include "stcpp.hpp"
#include "gtest/gtest.h"

int         count       = 0;
const int   COUNT_LOOP  = 1000;
//
static  void    recursive(int depth){
    if ( depth ) recursive( depth - 1 );
}

static  void    somework(){
    char    buf[1024];
    FILE*   fp = fopen("/dev/random", "rb");
    fread(buf, sizeof(buf), 1, fp);
    fclose(fp);
    recursive( 1001 );
};


class   threadTest  : public testing::Test {
protected:
    virtual void    SetUp(){
        std::cout << __FUNCTION__ << std::endl;
    }

    virtual void    TearDown(){
        std::cout << __FUNCTION__ << std::endl;
    }

    static  void    SetUpTestCase(){
        std::cout << __FUNCTION__ << std::endl;
    }

    static  void    TearDownTestCase(){
        std::cout << __FUNCTION__ << std::endl;
    }
};
//
TEST_F( threadTest, createThread){
    count   = 0;
    for(int i = 0; i < COUNT_LOOP; i++){
        stcpp::Thread *th = new stcpp::Thread(
            [](int a, int b ) {
                stcpp::Thread::usleep(1);
                somework();
                count++;
                int     data = 0, *pdata    = &data;
                stcpp::Thread::set_data(pdata);
                ASSERT_EQ( stcpp::Thread::get_data(), pdata);
            }, 100, 100 );
        delete  th;
    }
    for(int i = 0; i< 10; i++){
        stcpp::Thread::usleep(100);
        if (count < COUNT_LOOP) i--;    // waiting for the thread executor .
    }
    std::cout << "fd-limit = " << stcpp::Thread::getFdLimit() << std::endl;
    ASSERT_EQ( count , COUNT_LOOP );
    std::cout << __FUNCTION__ << " done " << std::endl;
    stcpp::Thread::exit();
}
//
