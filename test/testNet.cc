#include <iostream>
#include <memory>
#include <gtest/gtest.h>
#include "stcpp.hpp"

struct  Result{
    int error_listen;
}result;

bool    wait_accept = false;

#define TEST_HOST   "127.0.0.1"
#define TEST_PORT   12345

TEST(testNet, listen){
    wait_accept = false;
    stcpp::JoinableThread   thread(
        [](){
            stcpp::NetFD    fd;
            ASSERT_EQ(true, fd.listen_on(TEST_HOST, TEST_PORT));
            struct sockaddr_in  addr;
            memset(&addr, 0, sizeof(addr));
            int                 addrlen = 0;
            std::cout << "accepting connection..." << std::endl;
            wait_accept = true;
            auto   newfd = fd.accept((struct sockaddr*)&addr,
                                     &addrlen, 1000 * 1000 * 10);
            std::cout << "accepted" << std::endl;
            if (!newfd){
                std::cout << "accept error: " << strerror(fd.getLastError() )
                          << std::endl;
            }
        }
    );

    stcpp::JoinableThread   t2(
        [](){
            stcpp::NetFD fd;
            while( ! wait_accept ){
                stcpp::Thread::usleep(1000 * 10);
            }
            bool    ret = fd.connect( TEST_HOST, TEST_PORT, 1000 * 1000 * 10);
            if (! ret ){
                std::cout << "connect error: " << strerror(fd.getLastError())
                          << std::endl;
            }
            else{
                std::cout << "connected" << std::endl;
            }
            ASSERT_EQ(true, ret);
        });
    thread.join();
    t2.join();
}

