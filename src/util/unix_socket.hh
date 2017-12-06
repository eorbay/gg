/* -*-mode:c++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */

#ifndef UNIX_SOCKET_HH
#define UNIX_SOCKET_HH

#include <utility>
#include <sys/socket.h>
#include <sys/un.h>
#include "file_descriptor.hh"

class UnixDomainSocket 
{
private:
    unsigned int sock_fd;
    struct sockaddr_un remote;
    
public:
    UnixDomainSocket( void );
    ~UnixDomainSocket();
    std::string recv( void );
    void send( const std::string & payload );
};

#endif /* UNIX_SOCKET_HH */
