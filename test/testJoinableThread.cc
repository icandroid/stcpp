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
    recursive( 1001);
};

TEST( threadTest, createJoinableThread){
    count   = 0;
    stcpp::JoinableThread*  threads[COUNT_LOOP];
    for(int i = 0; i < COUNT_LOOP; i++){
        threads[i] = new stcpp::JoinableThread(
            [](int a ) {
                stcpp::Thread::usleep(1);
                somework();
                count++;
                stcpp::Thread::exit();
            }, 100 );
    }
    for(int i = 0; i < COUNT_LOOP; i++){
        threads[i]->join();
    }
    stcpp::Thread::usleep(1);
    for(int i = 0; i < COUNT_LOOP; i++){
        delete  threads[i];
    }
    ASSERT_EQ( count, COUNT_LOOP );
    std::cout << __FUNCTION__ << " done " << std::endl;
}

