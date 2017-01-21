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
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <cerrno>
#include <cstring>
#include <cinttypes>
#include <memory>


namespace   stcpp{

/**
 *  Wrapper of IO file descriptor.
 *  @NOTE that the timeout in the class is measured in microseconds.
 *
 * */
class   NetFD {
protected:
    void*           stfd;
    int             lastError;
public:
    NetFD() : stfd(NULL){ lastError = 0; };
    NetFD(int           osfd);  // from socket, the socket will be closed on destructor.
    NetFD( NetFD&&      that ); // move constructor.
    //
    //  the underlying fd will also be closed and freed.
    //
    virtual ~NetFD();
private:
    NetFD(void* _stfd) : stfd(_stfd){ lastError = 0;};
    NetFD( const NetFD& )           = delete;
    NetFD( NetFD& )                 = delete;
    NetFD& operator=(const NetFD&)  = delete;
    //
public:
    //
    //  open the file.
    //  the argument is same to OS `read' function.
    //  return true on success, or false on failure.
    //
    bool    open(const char* path, int oflag, int mode);
    //
    //  close the file descriptor
    //
    void    close();
    //
    //  get the underlying OS fd.
    //
    int     fileno();
    //
    //  for Solaris etc, which require serialize accept.
    //
    bool    serialize_accept();
    //
    //  poll the fd.
    //
    bool    poll(int how, uint64_t timeout );
    //
    //  listen TCP port on host.
    //
    bool    listen_on(const char* host, uint16_t port);
    //
    //  accept a TCP connection, and return a new FD.
    //
    std::shared_ptr<NetFD>
            accept(struct sockaddr* addr, int * addrlen, uint64_t timeout);
    //
    //  connect to peer TCP port.
    //
    bool    connect(const char* host, uint16_t port, uint64_t timeout );
    //
    //  read data from the fd.
    //
    int     read(void* buf, size_t nbyte, uint64_t timeout);
    //
    //  write data to the fd.
    //
    int     write(void* buf, size_t nbyte, uint64_t timeout);

    int     getLastError() { return lastError; }
};

};  // namespace stcpp

