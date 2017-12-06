/* -*-mode:c++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */

#include <sys/socket.h>
#include <sys/types.h>
#include <sys/un.h>

#include "unix_socket.hh"
#include "exception.hh"

static const std::string pathname = "/tmp/uds_sock";

UnixDomainSocket::UnixDomainSocket( void ) : sock_fd(0), remote()
{
    sock_fd = CheckSystemCall( "socket", ::socket( AF_UNIX, SOCK_STREAM, 0) );
    remote.sun_family = AF_UNIX;
    strcpy(remote.sun_path, pathname.c_str());
    int len = sizeof(sa_family_t) + strlen(remote.sun_path) + 1;
    CheckSystemCall( "connect", ::connect( sock_fd, (struct sockaddr*)&remote, len ) );

}

UnixDomainSocket::~UnixDomainSocket()
{
    CheckSystemCall( "close", ::close(sock_fd));
}

void UnixDomainSocket::send( const std::string & payload )
{
    const ssize_t bytes_sent = CheckSystemCall( "send", ::send( sock_fd,
                payload.data(),
                payload.size(),
                0 ) );

    if ( size_t( bytes_sent ) != payload.size() ) {
        throw unix_error( "datagram payload too big for send()" );
    }
}

std::string UnixDomainSocket::recv( void  )
{
    char buf[64];
    memset(buf, 0, 64);
    ssize_t recv_len = CheckSystemCall( "recv", ::recv( sock_fd, &buf, 63, 0 ) );
    if (recv_len > 0) {
        return std::string(buf);
    }
    return  std::string("");
}
