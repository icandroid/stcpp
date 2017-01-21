#include <iostream>
#include <gtest/gtest.h>
#include "stcpp.hpp"


bool    wait_accept = false;
int count_read  = 0,
    count_write = 0;

#define TEST_HOST   "127.0.0.1"
#define TEST_PORT   12345

#define BUF_SIZE    1024

TEST(testNetRW, readWrite){
    wait_accept = false;
    stcpp::JoinableThread   thread(
        [](){
            stcpp::NetFD    fd;
            ASSERT_EQ(true, fd.listen_on(TEST_HOST, TEST_PORT));
            wait_accept = true;
            auto   client = fd.accept( NULL, NULL, 1000 * 1000 * 10);
            if (!client){
                std::cout << "accept error: " << strerror(fd.getLastError() )
                          << std::endl;
            }
            else{
                std::cout << "client fd = " << client->fileno() << std::endl;
                char    buf[BUF_SIZE];
                count_read  = client->read(buf, sizeof(buf), 0);
                ASSERT_EQ(count_read, BUF_SIZE);
                ASSERT_EQ(count_read, client->write(buf, BUF_SIZE, 0) );
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
                char    buf[BUF_SIZE];
                for(int i = 0; i < BUF_SIZE; i++){
                    buf[i]  = (char)(i);
                }
                count_write = fd.write(buf, BUF_SIZE, 0);
                ASSERT_EQ(count_write, BUF_SIZE);
                ASSERT_EQ(count_write, fd.read(buf, sizeof(buf), 1000 * 1000));
            }
            ASSERT_EQ(true, ret);
        });
    thread.join();
    t2.join();
}

