/**
 * @file CXBindingsSocket.h
 * @brief Main file for defining a socket
 * @author Nicolas Macherey (nicolas.macherey@gmail.com)
 */
#ifndef CXB_SOCKET_H
#define CXB_SOCKET_H

#include <sys/types.h>
#include <time.h>

#ifdef HAVE_SSL
#include <openssl/ssl.h>
#endif

#ifdef WIN32
#include <winsock2.h>
#else
#include <sys/socket.h>
#include <netinet/in.h>
#endif

#ifdef WIN32
    #define CXBSOCKET SOCKET
    #define CXBSSL void*
#else
    #define CXBSOCKET int
    #define CXBSSL void*
#endif

typedef struct sockaddr_in CXBSOCKADDR;

/**
 * This is the main class for handling a socket in our http server
 * @author Nicolas Macherey (nicolas.macherey@gmail.com)
 */
class CXBindingsSocket {
    /** define the internal socket handler */
    CXBSOCKET m_sock;
    /** Store the socket address */
    CXBSOCKADDR m_addr;
    /** Internal storage for ssl transformations */
    CXBSSL m_ssl;
public:
    

    /* ACCESSORS */
    inline const CXBSOCKET& GetSocket() const {return m_sock;};
    inline void SetSocket( const CXBSOCKET& sock ) {m_sock = sock;}
    inline CXBSSL GetSSL() const {return m_ssl;};
    inline void SetSSL( CXBSSL ssl ) {m_ssl = ssl;}

};

#endif // CXB_SOCKET_H
