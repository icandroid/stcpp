#include    <sys/time.h>
#include    <sys/resource.h>
#include    <gtest/gtest.h>
#include    <cstdlib>
#include    <iostream>
#include    "stcpp.hpp"

#define THREAD_COUNT    100000
int             thread_counter  = 0;
TEST(TestManyThread,    createThread){
    stcpp::JoinableThread   main(
        [](){
            stcpp::CondVar* cv  = new stcpp::CondVar();
            //
            stcpp::JoinableThread**  threads = new stcpp::JoinableThread*[ THREAD_COUNT ];
            for(int i = 0; i < THREAD_COUNT; i++){
                threads[i]  = new stcpp::JoinableThread(
                    [](stcpp::CondVar* cv){
                        ASSERT_TRUE( cv->wait() );
                        stcpp::Thread::usleep( random() % 10000 );
                        thread_counter++;
                    },
                    cv );
            };
            //
            stcpp::Thread::usleep(1000 * 1000 * 10);
            cv->broadcast();
            for(int i = 0; i < THREAD_COUNT; i++){
                threads[i]->join();
            }
            for(int i = 0; i < THREAD_COUNT; i++){
                delete  threads[i];
            }
            delete  cv;
            delete[] threads;
            ASSERT_TRUE( thread_counter == THREAD_COUNT );
        });
    main.join();
    //
    struct  rusage  usage;
    memset(&usage, 0, sizeof(usage));
    int     ret = getrusage(RUSAGE_SELF, &usage);
    if (ret == 0){
        std::cout << "rss.max = " << usage.ru_maxrss << std::endl;
        std::cout << "rss.ix= " << usage.ru_ixrss << std::endl;
        std::cout << "rss.id= " << usage.ru_idrss << std::endl;
    }
    else{
        std::cerr << "fail to call getrusage" << std::endl;
        FAIL();
    }
}

