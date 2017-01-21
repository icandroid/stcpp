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
#include    "st-io.hpp"
#include    "st/st.h"

namespace   stcpp{

#define TO_ST(p)    ((st_netfd_t)p)

//
// construct from socket, the socket will be closed on destructor.
//
NetFD::NetFD(int           osfd){
    lastError   = 0;
    stfd    = st_netfd_open_socket( osfd );
    if ( stfd == NULL){
        lastError   = errno;
    }
}

NetFD::NetFD( NetFD&& that ) // move constructor.
    :  stfd(that.stfd)
    , lastError( that.lastError){
    that.stfd   = NULL;
};

NetFD::~NetFD(){
    close();
}

bool
NetFD::open(const char* path, int oflag, int mode){
    close();
    stfd    = st_open( path, oflag, mode );
    if ( stfd == NULL){
        lastError   = errno;
    }
    return ( stfd != NULL);
}

void
NetFD::close(){
    if ( stfd ){
        if ( st_netfd_close(  TO_ST(stfd) ) != 0 ){
            lastError   = errno;
        }
        stfd    = NULL;
    };
};

int
NetFD::fileno(){   //  get the underlying OS file descriptor.
    int     ret = st_netfd_fileno( TO_ST(stfd));
    return  ret;
}

bool
NetFD::serialize_accept(){
    int     ret = st_netfd_serialize_accept( TO_ST(stfd));
    if (ret < 0){
        lastError   = errno;
    }
    return  (ret == 0);
};

bool
NetFD::poll(int how, uint64_t timeout ){
    int     ret = st_netfd_poll(  TO_ST(stfd), how, timeout );
    if ( ret < 0 ){
        lastError   = errno;
    }
    return  (ret == 0);
}

std::shared_ptr<NetFD>
NetFD::accept(struct sockaddr* addr, int * addrlen, uint64_t timeout){
    struct sockaddr_in  local_addr;
    struct sockaddr*    paddr  = (struct sockaddr*)&local_addr;
    int                 local_len,  *plen   = &local_len;
    if (addr)       paddr   = addr;
    if (addrlen)    plen    = addrlen;
    //
    st_netfd_t  newfd = st_accept(  TO_ST(stfd), paddr, plen, timeout);
    if (newfd){
        std::shared_ptr<NetFD>  pfd( new NetFD(newfd) );
        return pfd;
    }
    else{
        lastError   = errno;
        return  nullptr;
    }
}

bool
NetFD::connect(const char* host, uint16_t port, uint64_t timeout ){
    if ( stfd == NULL){
        int sock_fd = socket(AF_INET, SOCK_STREAM, 0);
         stfd   = st_netfd_open_socket(sock_fd);
    }
    struct sockaddr_in  addr;
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port   = htons(port);
    addr.sin_addr.s_addr    = inet_addr( host );
    //
    int ret = st_connect(  TO_ST(stfd), (struct sockaddr*)&addr, sizeof(addr), timeout );
    if (ret < 0 ){
        lastError   = errno;
    };
    return (ret == 0);
}

int
NetFD::read(void* buf, size_t nbyte, uint64_t timeout){
    int ret = st_read( TO_ST(stfd), buf, nbyte, timeout);
    if (ret < 0 ){
        lastError   = errno;
    }
    return  ret;
};

int
NetFD::write(void* buf, size_t nbyte, uint64_t timeout){
    int ret = st_write( TO_ST(stfd), buf, nbyte, timeout);
    if (ret < 0){
        lastError   = errno;
    }
    return  ret;
}

bool
NetFD::listen_on(const char* host, uint16_t port){
    if ( stfd ){
        close();
    };
    int     fd = socket(PF_INET, SOCK_STREAM, 0);
    if (fd < 0 ){
        lastError   = errno;
        return  false;
    }
    int     n = 1;
    if (setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, (char*)&n, sizeof(n)) < 0){
        lastError   = errno;
        ::close(fd);
        return  false;
    }
    struct sockaddr_in  addr;
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port   = htons(port);
    addr.sin_addr.s_addr    = inet_addr(host);
    if (::bind(fd, (struct sockaddr*)&addr, sizeof(addr)) < 0){
        lastError   = errno;
        ::close(fd);
        return  false;
    }
    if (::listen(fd, 256) < 0){
        lastError   = errno;
        ::close(fd);
        return  false;
    }
    stfd    = st_netfd_open_socket(fd);
    if ( stfd == NULL){
        lastError   = errno;
        ::close(fd);
        return  false;
    }
    return  true;
}

};  // namespace stcpp.

