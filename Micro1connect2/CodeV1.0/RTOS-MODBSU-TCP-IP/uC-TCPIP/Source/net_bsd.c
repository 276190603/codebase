/*
*********************************************************************************************************
*                                              uC/TCP-IP
*                                      The Embedded TCP/IP Suite
*
*                          (c) Copyright 2003-2008; Micrium, Inc.; Weston, FL
*
*               All rights reserved.  Protected by international copyright laws.
*
*               uC/TCP-IP is provided in source form for FREE evaluation, for educational
*               use or peaceful research.  If you plan on using uC/TCP-IP in a commercial
*               product you need to contact Micrium to properly license its use in your
*               product.  We provide ALL the source code for your convenience and to help
*               you experience uC/TCP-IP.  The fact that the source code is provided does
*               NOT mean that you can use it without paying a licensing fee.
*
*               Knowledge of the source code may NOT be used to develop a similar product.
*
*               Please help us continue to provide the Embedded community with the finest
*               software available.  Your honesty is greatly appreciated.
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*
*                                            BSD 4.x LAYER
*
* Filename      : net_bsd.c
* Version       : V1.92
* Programmer(s) : ITJ
*********************************************************************************************************
* Note(s)       : (1) Supports BSD 4.x Layer API with the following restrictions/constraints :
*
*                     (a) ONLY supports the following BSD layer functionality :
*                         (1) BSD sockets                                           See 'net_sock.h  Note #1'
*
*                     (b) Return variable 'errno' NOT currently supported
*
*********************************************************************************************************
* Notice(s)     : (1) The Institute of Electrical and Electronics Engineers and The Open Group, have given 
*                     us permission to reprint portions of their documentation.  Portions of this text are 
*                     reprinted and reproduced in electronic form from the IEEE Std 1003.1, 2004 Edition, 
*                     Standard for Information Technology -- Portable Operating System Interface (POSIX), 
*                     The Open Group Base Specifications Issue 6, Copyright (C) 2001-2004 by the Institute 
*                     of Electrical and Electronics Engineers, Inc and The Open Group.  In the event of any 
*                     discrepancy between these versions and the original IEEE and The Open Group Standard, 
*                     the original IEEE and The Open Group Standard is the referee document.  The original 
*                     Standard can be obtained online at http://www.opengroup.org/unix/online.html.
*********************************************************************************************************
*/


/*
*********************************************************************************************************
*                                            INCLUDE FILES
*********************************************************************************************************
*/

#define    NET_BSD_MODULE
#include  <net.h>


/*
*********************************************************************************************************
*                                               MODULE
*
* Note(s) : (1) See 'net_bsd.h  MODULE'.
*********************************************************************************************************
*/

#ifdef  NET_BSD_MODULE_PRESENT


/*$PAGE*/
/*
*********************************************************************************************************
*                                            LOCAL DEFINES
*********************************************************************************************************
*/


/*
*********************************************************************************************************
*                                           LOCAL CONSTANTS
*********************************************************************************************************
*/


/*
*********************************************************************************************************
*                                          LOCAL DATA TYPES
*********************************************************************************************************
*/


/*
*********************************************************************************************************
*                                            LOCAL TABLES
*********************************************************************************************************
*/


/*
*********************************************************************************************************
*                                       LOCAL GLOBAL VARIABLES
*********************************************************************************************************
*/


/*
*********************************************************************************************************
*                                      LOCAL FUNCTION PROTOTYPES
*********************************************************************************************************
*/


/*
*********************************************************************************************************
*                                     LOCAL CONFIGURATION ERRORS
*********************************************************************************************************
*/


/*$PAGE*/
/*
*********************************************************************************************************
*                                     STANDARD BSD 4.x FUNCTIONS
*
* Note(s) : (1) BSD 4.x function definitions are required only for applications that call BSD 4.x functions.
*
*               See 'net_bsd.h  MODULE  Note #1b3'
*                 & 'net_bsd.h  STANDARD BSD 4.x FUNCTION PROTOTYPES  Note #1'.
*********************************************************************************************************
*/

#if (NET_BSD_CFG_API_EN == DEF_ENABLED)                         /* See Note #1.                                         */


/*
*********************************************************************************************************
*                                              socket()
*
* Description : Create a socket.
*
* Argument(s) : protocol_family     Socket protocol family :
*
*                                       PF_INET                 Internet Protocol version 4 (IPv4).
*
*                                   See also 'net_sock.c  Note #1a'.
*
*               sock_type           Socket type :
*
*                                       SOCK_DGRAM              Datagram-type socket.
*                                       SOCK_STREAM             Stream  -type socket.
*
*                                   See also 'net_sock.c  Note #1b'.
*
*               protocol            Socket protocol :
*
*                                       0                       Default protocol for socket type.
*                                       IPPROTO_UDP             User Datagram        Protocol (UDP).
*                                       IPPROTO_TCP             Transmission Control Protocol (TCP).
*
*                                   See also 'net_sock.c  Note #1c'.
*
* Return(s)   : Socket descriptor/handle identifier, if NO errors.
*
*               -1,                                  otherwise.
*
* Caller(s)   : Application.
*
*               This function is a network protocol suite application interface (API) function & MAY be 
*               called by application function(s).
*
* Note(s)     : none.
*********************************************************************************************************
*/

#ifdef  NET_SOCK_MODULE_PRESENT
int  socket (int  protocol_family,
             int  sock_type,
             int  protocol)
{
    int      rtn_code;
    NET_ERR  err;


    rtn_code = (int)NetSock_Open((NET_SOCK_PROTOCOL_FAMILY) protocol_family,
                                 (NET_SOCK_TYPE           ) sock_type,
                                 (NET_SOCK_PROTOCOL       ) protocol,
                                 (NET_ERR                *)&err);

    return (rtn_code);
}
#endif


/*$PAGE*/
/*
*********************************************************************************************************
*                                               close()
*
* Description : Close a socket.
*
* Argument(s) : sock_id     Socket descriptor/handle identifier of socket to close.
*
* Return(s)   :  0, if NO errors.
*
*               -1, otherwise.
*
* Caller(s)   : Application.
*
*               This function is a network protocol suite application interface (API) function & MAY be 
*               called by application function(s).
*
* Note(s)     : (1) Once an application closes its socket, NO further operations on the socket are allowed
*                   & the application MUST NOT continue to access the socket.
*                   
*                   #### Continued access to the closed socket by the application layer will likely corrupt
*                   the network socket layer.
*********************************************************************************************************
*/

#ifdef  NET_SOCK_MODULE_PRESENT
int  close (int  sock_id)
{
    int      rtn_code;
    NET_ERR  err;


    rtn_code = (int)NetSock_Close((NET_SOCK_ID) sock_id,
                                  (NET_ERR   *)&err);

    return (rtn_code);
}
#endif


/*$PAGE*/
/*
*********************************************************************************************************
*                                               bind()
*
* Description : Bind a socket to a local address.
*
* Argument(s) : sock_id         Socket descriptor/handle identifier of socket to bind to a local address.
*
*               paddr_local     Pointer to socket address structure             (see Notes #1b1B, #1b2, & #2).
*
*               addr_len        Length  of socket address structure (in octets) [see Note  #1b1C].
*
* Return(s)   :  0, if NO errors (see Note #1c1).
*
*               -1, otherwise    (see Note #1c2A).
*
* Caller(s)   : Application.
*
*               This function is a network protocol suite application interface (API) function & MAY be 
*               called by application function(s).
*
* Note(s)     : (1) (a) (1) IEEE Std 1003.1, 2004 Edition, Section 'bind() : DESCRIPTION' states that "the bind() 
*                           function shall assign a local socket address ... to a socket".
*
*                       (2) Stevens/Fenner/Rudoff, UNIX Network Programming, Volume 1, 3rd Edition, 6th Printing, 
*                           Section 4.4, Page 102 states that "bind() lets us specify the ... address, the port, 
*                           both, or neither".
*
*                   (b) (1) IEEE Std 1003.1, 2004 Edition, Section 'select() : DESCRIPTION' states that "the bind() 
*                           function takes the following arguments" :
*
*                           (A) 'socket' - "Specifies the file descriptor of the socket to be bound."
*
*                           (B) 'address' - "Points to a 'sockaddr' structure containing the address to be bound 
*                                to the socket.  The length and format of the address depend on the address family 
*                                of the socket."
*
*                           (C) 'address_len' - "Specifies the length of the 'sockaddr' structure pointed to by 
*                                the address argument."
*
*                       (2) Stevens/Fenner/Rudoff, UNIX Network Programming, Volume 1, 3rd Edition, 6th Printing, 
*                           Section 4.4, Page 102 states that "if ... bind() is called" with :
*
*                           (A) "A port number of 0, the kernel chooses an ephemeral port."
*
*                               (1) "bind() does not return the chosen value ... [of] an ephemeral port ... Call 
*                                    getsockname() to return the protocol address ... to obtain the value of the 
*                                    ephemeral port assigned by the kernel."
*
*                           (B) "A wildcard ... address, the kernel does not choose the local ... address until 
*                                either the socket is connected (TCP) or a datagram is sent on the socket (UDP)."
*
*                               (1) "With IPv4, the wildcard address is specified by the constant INADDR_ANY, 
*                                    whose value is normally 0."
*
*                   (c) IEEE Std 1003.1, 2004 Edition, Section 'bind() : RETURN VALUE' states that :
*
*                       (1) "Upon successful completion, bind() shall return 0;" ...
*
*                       (2) (A) "Otherwise, -1 shall be returned," ...
*                           (B) "and 'errno' shall be set to indicate the error."
*                               #### 'errno' NOT currently supported (see 'net_bsd.h  Note #1b').
*
*                   (d) (1) IEEE Std 1003.1, 2004 Edition, Section 'bind() : ERRORS' states that "the bind() 
*                           function shall fail if" :
*
*                           (A) "[EBADF] - The 'socket' argument is not a valid file descriptor."
*
*                           (B) "[EAFNOSUPPORT]  - The specified address is not a valid address for the address 
*                                family of the specified socket."
*
*                           (C) "[EADDRNOTAVAIL] - The specified address is not available from the local machine."
*
*                           (D) "[EADDRINUSE]    - The specified address is already in use."
*
*                           (E) "[EINVAL]" - 
*
*                               (1) (a) "The socket is already bound to an address,"                  ...
*                                   (b) "and the protocol does not support binding to a new address;" ...
*
*                               (2) "or the socket has been shut down."
*
*                       (2) IEEE Std 1003.1, 2004 Edition, Section 'bind() : ERRORS' states that "the bind() 
*                           function may fail if" :
*
*                           (A) "[EINVAL]  - The 'address_len' argument is not a valid length for the address 
*                                family."
*
*                           (B) "[EISCONN] - The socket is already connected."
*
*                           (C) "[ENOBUFS] - Insufficient resources were available to complete the call."
*
*                   See also 'net_sock.c  NetSock_BindHandler()  Note #2'.
*$PAGE*
*               (2) (a) Socket address structure 'sa_family' member MUST be configured in host-order & 
*                       MUST NOT be converted to/from network-order.
*
*                   (b) Socket address structure addresses MUST be configured/converted from host-order
*                       to network-order.
*
*                   See also 'net_sock.h  NETWORK SOCKET ADDRESS DATA TYPES  Note #2'
*                          & 'net_sock.c  NetSock_BindHandler()  Note #3'.
*********************************************************************************************************
*/

#ifdef  NET_SOCK_MODULE_PRESENT
int  bind (        int         sock_id,
           struct  sockaddr   *paddr_local,
                   socklen_t   addr_len)
{
    int      rtn_code;
    NET_ERR  err;


    rtn_code = (int)NetSock_Bind((NET_SOCK_ID      ) sock_id,
                                 (NET_SOCK_ADDR   *) paddr_local,
                                 (NET_SOCK_ADDR_LEN) addr_len,
                                 (NET_ERR         *)&err);

    return (rtn_code);
}
#endif


/*$PAGE*/
/*
*********************************************************************************************************
*                                              connect()
*
* Description : Connect a socket to a remote server.
*
* Argument(s) : sock_id         Socket descriptor/handle identifier of socket to connect.
*
*               paddr_remote    Pointer to socket address structure (see Note #1).
*
*               addr_len        Length  of socket address structure (in octets).
*
* Return(s)   :  0, if NO errors.
*
*               -1, otherwise.
*
* Caller(s)   : Application.
*
*               This function is a network protocol suite application interface (API) function & MAY be 
*               called by application function(s).
*
* Note(s)     : (1) (a) Socket address structure 'sa_family' member MUST be configured in host-order & 
*                       MUST NOT be converted to/from network-order.
*
*                   (b) Socket address structure addresses MUST be configured/converted from host-order
*                       to network-order.
*
*                   See also 'net_sock.h  NETWORK SOCKET ADDRESS DATA TYPES  Note #2'.
*********************************************************************************************************
*/

#ifdef  NET_SOCK_MODULE_PRESENT
int  connect (        int         sock_id,
              struct  sockaddr   *paddr_remote,
                      socklen_t   addr_len)
{
    int      rtn_code;
    NET_ERR  err;


    rtn_code = (int)NetSock_Conn((NET_SOCK_ID      ) sock_id,
                                 (NET_SOCK_ADDR   *) paddr_remote,
                                 (NET_SOCK_ADDR_LEN) addr_len,
                                 (NET_ERR         *)&err);

    return (rtn_code);
}
#endif


/*$PAGE*/
/*
*********************************************************************************************************
*                                              listen()
*
* Description : Set socket to listen for connection requests.
*
* Argument(s) : sock_id         Socket descriptor/handle identifier of socket to listen.
*
*               sock_q_size     Number of connection requests to queue on listen socket.
*
* Return(s)   :  0, if NO errors.
*
*               -1, otherwise.
*
* Caller(s)   : Application.
*
*               This function is a network protocol suite application interface (API) function & MAY be 
*               called by application function(s).
*
* Note(s)     : none.
*********************************************************************************************************
*/

#ifdef  NET_SOCK_MODULE_PRESENT
#if    (NET_SOCK_CFG_TYPE_STREAM_EN == DEF_ENABLED)
int  listen (int  sock_id,
             int  sock_q_size)
{
    int      rtn_code;
    NET_ERR  err;


    rtn_code = (int)NetSock_Listen((NET_SOCK_ID    ) sock_id,
                                   (NET_SOCK_Q_SIZE) sock_q_size,
                                   (NET_ERR       *)&err);

    return (rtn_code);
}
#endif
#endif


/*$PAGE*/
/*
*********************************************************************************************************
*                                              accept()
*
* Description : Get a new socket accepted from a socket set to listen for connection requests.
*
* Argument(s) : sock_id         Socket descriptor/handle identifier of listen socket.
*
*               paddr_remote    Pointer to an address buffer that will receive the socket address structure
*                                   of the accepted socket's remote address (see Note #1), if NO errors.
*
*               paddr_len       Pointer to a variable to ... :
*
*                                   (a) Pass the size of the address buffer pointed to by 'paddr_remote'.
*                                   (b) (1) Return the actual size of socket address structure with the 
*                                               accepted socket's remote address, if NO errors;
*                                       (2) Return 0,                             otherwise.
*
* Return(s)   : Socket descriptor/handle identifier of new accepted socket, if NO errors.
*
*               -1,                                                         otherwise.
*
* Caller(s)   : Application.
*
*               This function is a network protocol suite application interface (API) function & MAY be 
*               called by application function(s).
*
* Note(s)     : (1) (a) Socket address structure 'sa_family' member returned in host-order & SHOULD NOT 
*                       be converted to network-order.
*
*                   (b) Socket address structure addresses MUST be converted from host-order to network-
*                       order.
*
*                   See also 'net_sock.h  NETWORK SOCKET ADDRESS DATA TYPES  Note #2'.
*********************************************************************************************************
*/

#ifdef  NET_SOCK_MODULE_PRESENT
#if    (NET_SOCK_CFG_TYPE_STREAM_EN == DEF_ENABLED)
int  accept (        int         sock_id,
             struct  sockaddr   *paddr_remote,
                     socklen_t  *paddr_len)
{
    int                rtn_code;
    NET_SOCK_ADDR_LEN  addr_len;
    NET_ERR            err;


    addr_len  = (NET_SOCK_ADDR_LEN)*paddr_len;
    rtn_code  = (int)NetSock_Accept((NET_SOCK_ID        ) sock_id,
                                    (NET_SOCK_ADDR     *) paddr_remote,
                                    (NET_SOCK_ADDR_LEN *)&addr_len,
                                    (NET_ERR           *)&err);

   *paddr_len = (socklen_t)addr_len;

    return (rtn_code);
}
#endif
#endif


/*$PAGE*/
/*
*********************************************************************************************************
*                                             recvfrom()
*
* Description : Receive data from a socket.
*
* Argument(s) : sock_id         Socket descriptor/handle identifier of socket to receive data.
*
*               pdata_buf       Pointer to an application data buffer that will receive the socket's received
*                                   data.
*
*               data_buf_len    Size of the   application data buffer (in octets) [see Note #1].
*
*               flags           Flags to select receive options (see Note #2); bit-field flags logically OR'd :
*
*                                   0                           No socket flags selected.
*                                   MSG_PEEK                    Receive socket data without consuming
*                                                                   the socket data.
*                                   MSG_DONTWAIT                Receive socket data without blocking.
*
*               paddr_remote    Pointer to an address buffer that will receive the socket address structure 
*                                   with the received data's remote address (see Note #3), if NO errors.
*
*               paddr_len       Pointer to a variable to ... :
*
*                                   (a) Pass the size of the address buffer pointed to by 'paddr_remote'.
*                                   (b) (1) Return the actual size of socket address structure with the 
*                                               received data's remote address, if NO errors;
*                                       (2) Return 0,                           otherwise.
*
* Return(s)   : Number of positive data octets received, if NO errors                (see Note #4a).
*
*                0,                                      if socket connection closed (see Note #4b).
*
*               -1,                                      otherwise                   (see Note #4c1).
*
* Caller(s)   : Application.
*
*               This function is a network protocol suite application interface (API) function & MAY be 
*               called by application function(s).
*
* Note(s)     : (1) (a) (1) (A) Datagram-type sockets send & receive all data atomically -- i.e. every single, 
*                               complete datagram transmitted MUST be received as a single, complete datagram.
*
*                           (B) IEEE Std 1003.1, 2004 Edition, Section 'recvfrom() : DESCRIPTION' summarizes 
*                               that "for message-based sockets, such as ... SOCK_DGRAM ... the entire message 
*                               shall be read in a single operation.  If a message is too long to fit in the 
*                               supplied buffer, and MSG_PEEK is not set in the flags argument, the excess 
*                               bytes shall be discarded".
*
*                       (2) Thus, if the socket's type is datagram & the receive data buffer size is NOT
*                           large enough for the received data, the receive data buffer is maximally filled
*                           with receive data but the remaining data octets are silently discarded & NO 
*                           error is returned.
*
*                           (A) IEEE Std 1003.1, 2004 Edition, Section 'send() : ERRORS' states to return an
*                               'EMSGSIZE' error when "the message is too large to be sent all at once".
*
*                               ???? Similarly, a socket receive whose receive data buffer size is NOT large
*                               enough for the received data could return an 'EMSGSIZE' error.
*
*                   (b) (1) (A) (1) Stream-type sockets send & receive all data octets in one or more non-
*                                   distinct packets.  In other words, the application data is NOT bounded
*                                   by any specific packet(s); rather, it is contiguous & sequenced from 
*                                   one packet to the next.
*
*                               (2) IEEE Std 1003.1, 2004 Edition, Section 'recv() : DESCRIPTION' summarizes
*                                   that "for stream-based sockets, such as SOCK_STREAM, message boundaries 
*                                   shall be ignored.  In this case, data shall be returned to the user as
*                                   soon as it becomes available, and no data shall be discarded".
*
*                           (B) Thus, if the socket's type is stream & the receive data buffer size is NOT 
*                               large enough for the received data, the receive data buffer is maximally 
*                               filled with receive data & the remaining data octets remain queued for 
*                               later application-socket receives.
*                   
*                       (2) Thus, it is typical -- but NOT absolutely required -- that a single application
*                           task ONLY receive or request to receive data from a stream-type socket.
*
*                   See also 'net_sock.c  NetSock_RxDataHandler()  Note #2'.
*$PAGE*
*               (2) Only some socket receive flag options are implemented.  If other flag options are requested,
*                   socket receive handler function(s) abort & return appropriate error codes so that requested
*                   flag options are NOT silently ignored.
*
*               (3) (a) Socket address structure 'sa_family' member returned in host-order & SHOULD NOT 
*                       be converted to network-order.
*
*                   (b) Socket address structure addresses MUST be converted from host-order to network-
*                       order.
*
*                   See also 'net_sock.h  NETWORK SOCKET ADDRESS DATA TYPES  Note #2'.
*
*               (4) IEEE Std 1003.1, 2004 Edition, Section 'recvfrom() : RETURN VALUE' states that :
*
*                   (a) "Upon successful completion, recvfrom() shall return the length of the message in
*                        bytes."
*
*                   (b) "If no messages are available to be received and the peer has performed an orderly
*                        shutdown, recvfrom() shall return 0."
*
*                   (c) (1) "Otherwise, [-1 shall be returned]" ...
*                       (2) "and 'errno' set to indicate the error."
*                           #### 'errno' NOT currently supported 
*                                (see 'net_bsd.c  Note #1b').
*
*                   See also 'net_sock.c  NetSock_RxDataHandler()  Note #7'.
*********************************************************************************************************
*/

#ifdef  NET_SOCK_MODULE_PRESENT
ssize_t  recvfrom (        int         sock_id,
                           void       *pdata_buf,
                          _size_t      data_buf_len,
                           int         flags,
                   struct  sockaddr   *paddr_remote,
                           socklen_t  *paddr_len)
{
    ssize_t            rtn_code;
    NET_SOCK_ADDR_LEN  addr_len;
    NET_ERR            err;


    addr_len  = (NET_SOCK_ADDR_LEN)*paddr_len;
    rtn_code  = (ssize_t)NetSock_RxDataFrom((NET_SOCK_ID        ) sock_id,
                                            (void              *) pdata_buf,
                                            (CPU_INT16U         ) data_buf_len,
                                            (CPU_INT16S         ) flags,
                                            (NET_SOCK_ADDR     *) paddr_remote,
                                            (NET_SOCK_ADDR_LEN *)&addr_len,
                                            (void              *) 0,
                                            (CPU_INT08U         ) 0,
                                            (CPU_INT08U        *) 0,
                                            (NET_ERR           *)&err);

   *paddr_len = (socklen_t)addr_len;

    return (rtn_code);
}
#endif


/*$PAGE*/
/*
*********************************************************************************************************
*                                               recv()
*
* Description : Receive data from a socket.
*
* Argument(s) : sock_id         Socket descriptor/handle identifier of socket to receive data.
*
*               pdata_buf       Pointer to an application data buffer that will receive the socket's received
*                                   data.
*
*               data_buf_len    Size of the   application data buffer (in octets) [see Note #1].
*
*               flags           Flags to select receive options (see Note #2); bit-field flags logically OR'd :
*
*                                   0                           No socket flags selected.
*                                   MSG_PEEK                    Receive socket data without consuming
*                                                                   the socket data.
*                                   MSG_DONTWAIT                Receive socket data without blocking.
*
* Return(s)   : Number of positive data octets received, if NO errors                (see Note #3a).
*
*                0,                                      if socket connection closed (see Note #3b).
*
*               -1,                                      otherwise                   (see Note #3c1).
*
* Caller(s)   : Application.
*
*               This function is a network protocol suite application interface (API) function & MAY be 
*               called by application function(s).
*
* Note(s)     : (1) (a) (1) (A) Datagram-type sockets send & receive all data atomically -- i.e. every single, 
*                               complete datagram transmitted MUST be received as a single, complete datagram.
*
*                           (B) IEEE Std 1003.1, 2004 Edition, Section 'recv() : DESCRIPTION' summarizes that
*                               "for message-based sockets, such as SOCK_DGRAM ... the entire message shall be
*                               read in a single operation.  If a message is too long to fit in the supplied
*                               buffer, and MSG_PEEK is not set in the flags argument, the excess bytes shall
*                               be discarded".
*
*                       (2) Thus, if the socket's type is datagram & the receive data buffer size is NOT 
*                           large enough for the received data, the receive data buffer is maximally filled
*                           with receive data but the remaining data octets are silently discarded & NO 
*                           error is returned.
*
*                           (A) IEEE Std 1003.1, 2004 Edition, Section 'send() : ERRORS' states to return an
*                               'EMSGSIZE' error when "the message is too large to be sent all at once".
*
*                               ???? Similarly, a socket receive whose receive data buffer size is NOT large
*                               enough for the received data could return an 'EMSGSIZE' error.
*
*                   (b) (1) (A) (1) Stream-type sockets send & receive all data octets in one or more non-
*                                   distinct packets.  In other words, the application data is NOT bounded
*                                   by any specific packet(s); rather, it is contiguous & sequenced from 
*                                   one packet to the next.
*
*                               (2) IEEE Std 1003.1, 2004 Edition, Section 'recv() : DESCRIPTION' summarizes
*                                   that "for stream-based sockets, such as SOCK_STREAM, message boundaries 
*                                   shall be ignored.  In this case, data shall be returned to the user as
*                                   soon as it becomes available, and no data shall be discarded".
*
*                           (B) Thus, if the socket's type is stream & the receive data buffer size is NOT 
*                               large enough for the received data, the receive data buffer is maximally 
*                               filled with receive data & the remaining data octets remain queued for 
*                               later application-socket receives.
*                   
*                       (2) Thus, it is typical -- but NOT absolutely required -- that a single application
*                           task ONLY receive or request to receive data from a stream-type socket.
*
*                   See also 'net_sock.c  NetSock_RxDataHandler()  Note #2'.
*$PAGE*
*               (2) Only some socket receive flag options are implemented.  If other flag options are requested,
*                   socket receive handler function(s) abort & return appropriate error codes so that requested
*                   flag options are NOT silently ignored.
*
*               (3) IEEE Std 1003.1, 2004 Edition, Section 'recv() : RETURN VALUE' states that :
*
*                   (a) "Upon successful completion, recv() shall return the length of the message in bytes."
*
*                   (b) "If no messages are available to be received and the peer has performed an orderly
*                        shutdown, recv() shall return 0."
*
*                   (c) (1) "Otherwise, -1 shall be returned" ...
*                       (2) "and 'errno' set to indicate the error."
*                           #### 'errno' NOT currently supported 
*                                (see 'net_bsd.c  Note #1b').
*
*                   See also 'net_sock.c  NetSock_RxDataHandler()  Note #7'.
*********************************************************************************************************
*/

#ifdef  NET_SOCK_MODULE_PRESENT
ssize_t  recv (int      sock_id,
               void    *pdata_buf,
              _size_t   data_buf_len,
               int      flags)
{
    ssize_t  rtn_code;
    NET_ERR  err;


    rtn_code = (ssize_t)NetSock_RxData((NET_SOCK_ID  ) sock_id,
                                       (void        *) pdata_buf,
                                       (CPU_INT16U   ) data_buf_len,
                                       (CPU_INT16S   ) flags,
                                       (NET_ERR     *)&err);

    return (rtn_code);
}
#endif


/*$PAGE*/
/*
*********************************************************************************************************
*                                              sendto()
*
* Description : Send data through a socket.
*
* Argument(s) : sock_id         Socket descriptor/handle identifier of socket to send data.
*
*               p_data          Pointer to application data to send.
*
*               data_len        Length of  application data to send (in octets) [see Note #1].
*
*               flags           Flags to select send options (see Note #2); bit-field flags logically OR'd :
*
*                                   0                           No socket flags selected.
*                                   MSG_DONTWAIT                Send socket data without blocking.
*
*               paddr_remote    Pointer to destination address buffer (see Note #3).
*
*               addr_len        Length of  destination address buffer (in octets).
*
* Return(s)   : Number of positive data octets sent, if NO errors                (see Note #4a1).
*
*                0,                                  if socket connection closed (see Note #4b).
*
*               -1,                                  otherwise                   (see Note #4a2A).
*
* Caller(s)   : Application.
*
*               This function is a network protocol suite application interface (API) function & MAY be 
*               called by application function(s).
*
* Note(s)     : (1) (a) (1) (A) Datagram-type sockets send & receive all data atomically -- i.e. every single, 
*                               complete datagram  sent      MUST be received    as a single, complete datagram.
*                               Thus, each call to send data MUST be transmitted in a single, complete datagram.
*
*                           (B) (1) IEEE Std 1003.1, 2004 Edition, Section 'send() : DESCRIPTION' states that
*                                   "if the message is too long to pass through the underlying protocol, send() 
*                                   shall fail and no data shall be transmitted".
*
*                               (2) Since IP transmit fragmentation is NOT currently supported (see 'net_ip.h 
*                                   Note #1e'), if the socket's type is datagram & the requested send data
*                                   length is greater than the socket/transport layer MTU, then NO data is 
*                                   sent & NET_SOCK_ERR_INVALID_DATA_SIZE error is returned.
*
*                       (2) (A) (1) Stream-type sockets send & receive all data octets in one or more non-
*                                   distinct packets.  In other words, the application data is NOT bounded 
*                                   by any specific packet(s); rather, it is contiguous & sequenced from 
*                                   one packet to the next.
*
*                               (2) Thus, if the socket's type is stream & the socket's send data queue(s) are
*                                   NOT large enough for the send data, the send data queue(s) are maximally
*                                   filled with send data & the remaining data octets are discarded but may be
*                                   re-sent by later application-socket sends.
*
*                               (3) Therefore, NO stream-type socket send data length should be "too long to
*                                   pass through the underlying protocol" & cause the socket send to "fail ... 
*                                   [with] no data ... transmitted" (see Note #1a1B1).
*
*                           (B) Thus, it is typical -- but NOT absolutely required -- that a single application
*                               task ONLY send or request to send data to a stream-type socket.
*
*                   (b) 'data_len' of 0 octets NOT allowed.
*
*                   See also 'net_sock.c  NetSock_TxDataHandler()  Note #2'.
*$PAGE*
*               (2) Only some socket send flag options are implemented.  If other flag options are requested,
*                   socket send handler function(s) abort & return appropriate error codes so that requested
*                   flag options are NOT silently ignored.
*
*               (3) (a) Socket address structure 'sa_family' member MUST be configured in host-order & 
*                       MUST NOT be converted to/from network-order.
*
*                   (b) Socket address structure addresses MUST be configured/converted from host-order
*                       to network-order.
*
*                   See also 'net_sock.h  NETWORK SOCKET ADDRESS DATA TYPES  Note #2'.
*
*               (4) (a) IEEE Std 1003.1, 2004 Edition, Section 'sendto() : RETURN VALUE' states that :
*
*                       (1) "Upon successful completion, sendto() shall return the number of bytes sent."
*
*                           (A) Section 'sendto() : DESCRIPTION' elaborates that "successful completion 
*                               of a call to sendto() does not guarantee delivery of the message".
*
*                           (B) (1) Thus, applications SHOULD verify the actual returned number of data 
*                                   octets transmitted &/or prepared for transmission.
*
*                               (2) In addition, applications MAY desire verification of receipt &/or
*                                   acknowledgement of transmitted data to the remote host -- either 
*                                   inherently by the transport layer or explicitly by the application.
*
*                       (2) (A) "Otherwise, -1 shall be returned" ...
*                               (1) Section 'sendto() : DESCRIPTION' elaborates that "a return value of 
*                                   -1 indicates only locally-detected errors".
*
*                           (B) "and 'errno' set to indicate the error."
*                               #### 'errno' NOT currently supported (see 'net_bsd.c  Note #1b').
*
*                   (b) ???? Although NO socket send() specification states to return '0' when the socket's
*                       connection is closed, it seems reasonable to return '0' since it is possible for the
*                       socket connection to be close()'d or shutdown() by the remote host.
*
*                   See also 'net_sock.c  NetSock_TxDataHandler()  Note #5'.
*********************************************************************************************************
*/

#ifdef  NET_SOCK_MODULE_PRESENT
ssize_t  sendto (        int         sock_id,
                         void       *p_data,
                        _size_t      data_len,
                         int         flags,
                 struct  sockaddr   *paddr_remote,
                         socklen_t   addr_len)
{
    ssize_t  rtn_code;
    NET_ERR  err;


    rtn_code = (ssize_t)NetSock_TxDataTo((NET_SOCK_ID      ) sock_id,
                                         (void            *) p_data,
                                         (CPU_INT16U       ) data_len,
                                         (CPU_INT16S       ) flags,
                                         (NET_SOCK_ADDR   *) paddr_remote,
                                         (NET_SOCK_ADDR_LEN) addr_len,
                                         (NET_ERR         *)&err);

    return (rtn_code);
}
#endif


/*$PAGE*/
/*
*********************************************************************************************************
*                                               send()
*
* Description : Send data through a socket.
*
* Argument(s) : sock_id         Socket descriptor/handle identifier of socket to send data.
*
*               p_data          Pointer to application data to send.
*
*               data_len        Length of  application data to send (in octets) [see Note #1].
*
*               flags           Flags to select send options (see Note #2); bit-field flags logically OR'd :
*
*                                   0                           No socket flags selected.
*                                   MSG_DONTWAIT                Send socket data without blocking.
*
* Return(s)   : Number of positive data octets sent, if NO errors                (see Note #3a1).
*
*                0,                                  if socket connection closed (see Note #3b).
*
*               -1,                                  otherwise                   (see Note #3a2A).
*
* Caller(s)   : Application.
*
*               This function is a network protocol suite application interface (API) function & MAY be 
*               called by application function(s).
*
* Note(s)     : (1) (a) (1) (A) Datagram-type sockets send & receive all data atomically -- i.e. every single, 
*                               complete datagram  sent      MUST be received    as a single, complete datagram.
*                               Thus, each call to send data MUST be transmitted in a single, complete datagram.
*
*                           (B) (1) IEEE Std 1003.1, 2004 Edition, Section 'send() : DESCRIPTION' states that
*                                   "if the message is too long to pass through the underlying protocol, send() 
*                                   shall fail and no data shall be transmitted".
*
*                               (2) Since IP transmit fragmentation is NOT currently supported (see 'net_ip.h 
*                                   Note #1e'), if the socket's type is datagram & the requested send data
*                                   length is greater than the socket/transport layer MTU, then NO data is 
*                                   sent & NET_SOCK_ERR_INVALID_DATA_SIZE error is returned.
*
*                       (2) (A) (1) Stream-type sockets send & receive all data octets in one or more non-
*                                   distinct packets.  In other words, the application data is NOT bounded 
*                                   by any specific packet(s); rather, it is contiguous & sequenced from 
*                                   one packet to the next.
*
*                               (2) Thus, if the socket's type is stream & the socket's send data queue(s) are
*                                   NOT large enough for the send data, the send data queue(s) are maximally
*                                   filled with send data & the remaining data octets are discarded but may be
*                                   re-sent by later application-socket sends.
*
*                               (3) Therefore, NO stream-type socket send data length should be "too long to
*                                   pass through the underlying protocol" & cause the socket send to "fail ... 
*                                   [with] no data ... transmitted" (see Note #1a1B1).
*
*                           (B) Thus, it is typical -- but NOT absolutely required -- that a single application
*                               task ONLY send or request to send data to a stream-type socket.
*
*                   (b) 'data_len' of 0 octets NOT allowed.
*
*                   See also 'net_sock.c  NetSock_TxDataHandler()  Note #2'.
*$PAGE*
*               (2) Only some socket send flag options are implemented.  If other flag options are requested,
*                   socket send handler function(s) abort & return appropriate error codes so that requested
*                   flag options are NOT silently ignored.
*
*               (3) (a) IEEE Std 1003.1, 2004 Edition, Section 'send() : RETURN VALUE' states that :
*
*                       (1) "Upon successful completion, send() shall return the number of bytes sent."
*
*                           (A) Section 'send() : DESCRIPTION' elaborates that "successful completion 
*                               of a call to sendto() does not guarantee delivery of the message".
*
*                           (B) (1) Thus, applications SHOULD verify the actual returned number of data 
*                                   octets transmitted &/or prepared for transmission.
*
*                               (2) In addition, applications MAY desire verification of receipt &/or
*                                   acknowledgement of transmitted data to the remote host -- either 
*                                   inherently by the transport layer or explicitly by the application.
*
*                       (2) (A) "Otherwise, -1 shall be returned" ...
*                               (1) Section 'send() : DESCRIPTION' elaborates that "a return value of 
*                                   -1 indicates only locally-detected errors".
*
*                           (B) "and 'errno' set to indicate the error."
*                               #### 'errno' NOT currently supported (see 'net_bsd.c  Note #1b').
*
*                   (b) ???? Although NO socket send() specification states to return '0' when the socket's
*                       connection is closed, it seems reasonable to return '0' since it is possible for the
*                       socket connection to be close()'d or shutdown() by the remote host.
*
*                   See also 'net_sock.c  NetSock_TxDataHandler()  Note #5'.
*********************************************************************************************************
*/

#ifdef  NET_SOCK_MODULE_PRESENT
ssize_t  send (int      sock_id,
               void    *p_data,
              _size_t   data_len,
               int      flags)
{
    ssize_t  rtn_code;
    NET_ERR  err;


    rtn_code = (ssize_t)NetSock_TxData((NET_SOCK_ID  ) sock_id,
                                       (void        *) p_data,
                                       (CPU_INT16U   ) data_len,
                                       (CPU_INT16S   ) flags,
                                       (NET_ERR     *)&err);

    return (rtn_code);
}
#endif


/*$PAGE*/
/*
*********************************************************************************************************
*                                              select()
*
* Description : Check multiple file descriptors for available resources &/or operations.
*
* Argument(s) : desc_nbr_max    Maximum number of file descriptors in the file descriptor sets 
*                                   (see Note #1b1).
*
*               pdesc_rd        Pointer to a set of file descriptors to :
*
*                                   (a) Check for available read  operation(s) [see Note #1b2A1].
*
*                                   (b) (1) Return the actual file descriptors ready for available 
*                                               read  operation(s), if NO errors (see Note #1b2B1a1); 
*                                       (2) Return the initial, non-modified set of file descriptors, 
*                                               on any errors (see Note #1c2A);
*                                       (3) Return a null-valued (i.e. zero-cleared) descriptor set, 
*                                               if any timeout expires (see Note #1c2B).
*
*               pdesc_wr        Pointer to a set of file descriptors to :
*
*                                   (a) Check for available write operation(s) [see Note #1b2A2].
*
*                                   (b) (1) Return the actual file descriptors ready for available 
*                                               write operation(s), if NO errors (see Note #1b2B1a1); 
*                                       (2) Return the initial, non-modified set of file descriptors, 
*                                               on any errors (see Note #1c2A);
*                                       (3) Return a null-valued (i.e. zero-cleared) descriptor set, 
*                                               if any timeout expires (see Note #1c2B).
*
*               pdesc_err       Pointer to a set of file descriptors to :
*
*                                   (a) Check for any error(s) &/or exception(s) [see Note #1b2A3].
*
*                                   (b) (1) Return the actual file descriptors flagged with any error(s) 
*                                               &/or exception(s), if NO non-descriptor-related errors 
*                                               (see Note #1b2B1a1);
*                                       (2) Return the initial, non-modified set of file descriptors, 
*                                               on any errors (see Note #1c2A);
*                                       (3) Return a null-valued (i.e. zero-cleared) descriptor set, 
*                                               if any timeout expires (see Note #1c2B).
*
*               ptimeout        Pointer to a timeout (see Note #1b3).
*
* Return(s)   : Number of file descriptors with available resources &/or operations, if any     (see Note #1c1A1).
*
*                0,                                                                  on timeout (see Note #1c1B).
*
*               -1,                                                                  otherwise  (see Note #1c1A2a).
*
* Caller(s)   : Application.
*
*               This function is a network protocol suite application interface (API) function & MAY be 
*               called by application function(s).
*
* Note(s)     : (1) (a) IEEE Std 1003.1, 2004 Edition, Section 'select() : DESCRIPTION' states that :
*
*                       (1) (A) "select() ... shall support" the following file descriptor types :
*
*                               (1) "regular files,"                        ...
*                               (2) "terminal and pseudo-terminal devices," ...
*                               (3) "STREAMS-based files,"                  ...
*                               (4) "FIFOs,"                                ...
*                               (5) "pipes,"                                ...
*                               (6) "sockets."
*                   
*                           (B) "The behavior of ... select() on ... other types of ... file descriptors
*                                ... is unspecified."
*
*                       (2) Network Socket Layer supports BSD 4.x select() functionality with the following 
*                           restrictions/constraints :
*
*                           (A) ONLY supports the following file descriptor types :
*                               (1) Sockets
*
*                   (b) IEEE Std 1003.1, 2004 Edition, Section 'select() : DESCRIPTION' states that :
*
*                       (1) (A) "The 'nfds' argument ('desc_nbr_max') specifies the range of descriptors to 
*                                be tested.  The first 'nfds' descriptors shall be checked in each set; that 
*                                is, the descriptors from zero through nfds-1 in the descriptor sets shall 
*                                be examined."
*
*                           (B) Stevens/Fenner/Rudoff, UNIX Network Programming, Volume 1, 3rd Edition, 
*                               6th Printing, Section 6.3, Page 163 states that "the ['nfds'] argument" 
*                               specifies :
*
*                               (1) "the number of descriptors," ...
*                               (2) "not the largest value."
*$PAGE*
*                       (2) "The select() function shall ... examine the file descriptor sets whose addresses 
*                            are passed in the 'readfds' ('pdesc_rd'), 'writefds' ('pdesc_wr'), and 'errorfds' 
*                            ('pdesc_err') parameters to see whether some of their descriptors are ready for 
*                            reading, are ready for writing, or have an exceptional condition pending, 
*                            respectively."
*
*                           (A) (1) (a) "If the 'readfds' argument ('pdesc_rd') is not a null pointer, it 
*                                        points to an object of type 'fd_set' that on input specifies the 
*                                        file descriptors to be checked for being ready to read, and on 
*                                        output indicates which file descriptors are ready to read."
*
*                                   (b) "A descriptor shall be considered ready for reading when a call to 
*                                        an input function ... would not block, whether or not the function 
*                                        would transfer data successfully.  (The function might return data, 
*                                        an end-of-file indication, or an error other than one indicating 
*                                        that it is blocked, and in each of these cases the descriptor shall 
*                                        be considered ready for reading.)" :
*
*                                       (1) "If the socket is currently listening, then it shall be marked 
*                                            as readable if an incoming connection request has been received, 
*                                            and a call to the accept() function shall complete without 
*                                            blocking."
*
*                                   (c) Stevens/Fenner/Rudoff, UNIX Network Programming, Volume 1, 3rd Edition, 
*                                       6th Printing, Section 6.3, Pages 164-165 states that "a socket is ready 
*                                       for reading if any of the following ... conditions is true" :
*
*                                       (1) "A read operation on the socket will not block and will return a 
*                                            value greater than 0 (i.e., the data that is ready to be read)."
*                                            
*                                       (2) "The read half of the connection is closed (i.e., a TCP connection 
*                                            that has received a FIN).  A read operation ... will not block and 
*                                            will return 0 (i.e., EOF)."
*
*                                       (3) "The socket is a listening socket and the number of completed 
*                                            connections is nonzero.  An accept() on the listening socket 
*                                            will ... not block."
*
*                                       (4) "A socket error is pending.  A read operation on the socket will 
*                                            not block and will return an error (-1) with 'errno' set to the 
*                                            specific error condition."
*
*                               (2) (a) "If the 'writefds' argument ('pdesc_wr') is not a null pointer, it 
*                                        points to an object of type 'fd_set' that on input specifies the 
*                                        file descriptors to be checked for being ready to write, and on 
*                                        output indicates which file descriptors are ready to write."
*
*                                   (b) "A descriptor shall be considered ready for writing when a call to 
*                                        an output function ... would not block, whether or not the function 
*                                        would transfer data successfully" :
*
*                                       (1) "If a non-blocking call to the connect() function has been made 
*                                            for a socket, and the connection attempt has either succeeded 
*                                            or failed leaving a pending error, the socket shall be marked 
*                                            as writable."
*
*                                   (c) Stevens/Fenner/Rudoff, UNIX Network Programming, Volume 1, 3rd Edition, 
*                                       6th Printing, Section 6.3, Page 165 states that "a socket is ready for 
*                                       writing if any of the following ... conditions is true" :
*
*                                       (1) "A write operation will not block and will return a positive value 
*                                            (e.g., the number of bytes accepted by the transport layer)."
*
*                                       (2) "The write half of the connection is closed."
*
*                                       (3) "A socket using a non-blocking connect() has completed the
*                                            connection, or the connect() has failed."
*
*                                       (4) "A socket error is pending.  A write operation on the socket will 
*                                            not block and will return an error (-1) with 'errno' set to the 
*                                            specific error condition."
*
*                               (3) (a) "If the 'errorfds' argument ('pdesc_err') is not a null pointer, it 
*                                        points to an object of type 'fd_set' that on input specifies the file 
*                                        descriptors to be checked for error conditions pending, and on output 
*                                        indicates which file descriptors have error conditions pending."
*
*                                   (b) "A file descriptor ... shall be considered to have an exceptional 
*                                        condition pending ... as noted below" :
*
*                                       (2) "If a socket has a pending error."
*
*                                       (3) "Other circumstances under which a socket may be considered to 
*                                            have an exceptional condition pending are protocol-specific 
*                                            and implementation-defined."
*
*                                   (d) Stevens/Fenner/Rudoff, UNIX Network Programming, Volume 1, 3rd Edition, 
*                                       6th Printing, Section 6.3, Page 165 states "that when an error occurs on
*                                       a socket, it is [also] marked as both readable and writeable by select()".
*$PAGE*
*                           (B) (1) (a) "Upon successful completion, ... select() ... shall" :
*                           
*                                       (1) "modify the objects pointed to by the 'readfds' ('pdesc_rd'), 
*                                            'writefds' ('pdesc_wr'), and 'errorfds' ('pdesc_err') arguments 
*                                            to indicate which file descriptors are ready for reading, ready 
*                                            for writing, or have an error condition pending, respectively," ...
*
*                                       (2) "and shall return the total number of ready descriptors in all
*                                            the output sets."
*
*                                   (b) (1) "For each file descriptor less than nfds ('desc_nbr_max'), the
*                                            corresponding bit shall be set on successful completion" :
*
*                                           (A) "if it was set on input" ...
*                                           (B) "and the associated condition is true for that file descriptor."
*
*                               (2) select() can NOT absolutely guarantee that descriptors returned as ready 
*                                   will still be ready during subsequent operations since any higher priority 
*                                   tasks or processes may asynchronously consume the descriptors' operations 
*                                   &/or resources.  This can occur since select() functionality & subsequent 
*                                   operations are NOT atomic operations protected by network, file system, 
*                                   or operating system mechanisms.
*
*                                   However, as long as no higher priority tasks or processes access any of 
*                                   the same descriptors, then a single task or process can assume that all 
*                                   descriptors returned as ready by select() will still be ready during 
*                                   subsequent operations.
*
*                       (3) (A) "The 'timeout' parameter ('ptimeout') controls how long ... select() ... shall
*                                take before timing out."
*                                
*                               (1) (a) "If the 'timeout' parameter ('ptimeout') is not a null pointer, it 
*                                        specifies a maximum interval to wait for the selection to complete."
*
*                                       (1) "If none of the selected descriptors are ready for the requested 
*                                            operation, ... select() ... shall block until at least one of the 
*                                            requested operations becomes ready ... or ... until the timeout 
*                                            occurs."
*
*                                       (2) "If the specified time interval expires without any requested 
*                                            operation becoming ready, the function shall return."
*
*                                       (3) "To effect a poll, the 'timeout' parameter ('ptimeout') should not be
*                                            a null pointer, and should point to a zero-valued timespec structure
*                                            ('timeval')."
*
*                                   (b) (1) (A) "If the 'readfds' ('pdesc_rd'), 'writefds' ('pdesc_wr'), and 
*                                                'errorfds' ('pdesc_err') arguments are"                         ...
*                                               (1) "all null pointers"                                          ...
*                                               (2) [or all null-valued (i.e. no file descriptors set)]          ...
*                                           (B) "and the 'timeout' argument ('ptimeout') is not a null pointer," ...
*
*                                       (2) ... then "select() ... shall block for the time specified".
*
*                               (2) "If the 'timeout' parameter ('ptimeout') is a null pointer, then the call to 
*                                    ... select() shall block indefinitely until at least one descriptor meets the
*                                    specified criteria."
*
*                           (B) (1) "For the select() function, the timeout period is given ... in an argument
*                                   ('ptimeout') of type struct 'timeval'" ... :
*
*                                   (a) "in seconds" ...
*                                   (b) "and microseconds."
*
*                               (2) (a) (1) "Implementations may place limitations on the maximum timeout interval 
*                                            supported" :
*
*                                           (A) "All implementations shall support a maximum timeout interval of 
*                                                at least 31 days."
*
*                                               (1) However, since maximum timeout interval values are dependent
*                                                   on the specific OS implementation; a maximum timeout interval
*                                                   can NOT be guaranteed.
*
*                                           (B) "If the 'timeout' argument ('ptimeout') specifies a timeout interval 
*                                                greater than the implementation-defined maximum value, the maximum 
*                                                value shall be used as the actual timeout value."
*
*                                       (2) "Implementations may also place limitations on the granularity of 
*                                            timeout intervals" :
*
*                                           (A) "If the requested 'timeout' interval requires a finer granularity 
*                                                than the implementation supports, the actual timeout interval 
*                                                shall be rounded up to the next supported value."
*$PAGE*
*                   (c) (1) (A) IEEE Std 1003.1, 2004 Edition, Section 'select() : RETURN VALUE' states that :
*
*                               (1) "Upon successful completion, ... select() ... shall return the total 
*                                    number of bits set in the bit masks."
*
*                               (2) (a) "Otherwise, -1 shall be returned," ...
*                                   (b) "and 'errno' shall be set to indicate the error."
*                                       #### 'errno' NOT currently supported (see 'net_bsd.c  Note #1b').
*
*                           (B) Stevens/Fenner/Rudoff, UNIX Network Programming, Volume 1, 3rd Edition,
*                               6th Printing, Section 6.3, Page 161 states that BSD select() function
*                               "returns ... 0 on timeout".
*
*                       (2) IEEE Std 1003.1, 2004 Edition, Section 'select() : DESCRIPTION' states that :
*
*                           (A) "On failure, the objects pointed to by the 'readfds' ('pdesc_rd'), 'writefds' 
*                                ('pdesc_wr'), and 'errorfds' ('pdesc_err') arguments shall not be modified."
*
*                           (B) "If the 'timeout' interval expires without the specified condition being 
*                                true for any of the specified file descriptors, the objects pointed to 
*                                by the 'readfds' ('pdesc_rd'), 'writefds' ('pdesc_wr'), and 'errorfds' 
*                                ('pdesc_err') arguments shall have all bits set to 0."
*
*                   (d) IEEE Std 1003.1, 2004 Edition, Section 'select() : ERRORS' states that "under the
*                       following conditions, ... select() shall fail and set 'errno' to" :
*
*                       (1) "[EBADF] - One or more of the file descriptor sets specified a file descriptor 
*                            that is not a valid open file descriptor."
*
*                       (2) "[EINVAL]" -
*
*                           (A) "The 'nfds' argument ('desc_nbr_max') is" :
*                               (1) "less than 0 or" ...
*                               (2) "greater than FD_SETSIZE."
*
*                           (B) "An invalid timeout interval was specified."
*
*                       #### 'errno' NOT currently supported (see 'net_bsd.c  Note #1b').
*
*                   See also 'net_sock.c  NetSock_Sel()  Note #3'.
*********************************************************************************************************
*/

#ifdef  NET_SOCK_MODULE_PRESENT
#if    (NET_SOCK_CFG_SEL_EN == DEF_ENABLED)
int  select (        int       desc_nbr_max,
             struct  fd_set   *pdesc_rd,
             struct  fd_set   *pdesc_wr,
             struct  fd_set   *pdesc_err,
             struct  timeval  *ptimeout)
{
    int      rtn_code;
    NET_ERR  err;


    rtn_code = (int)NetSock_Sel((NET_SOCK_QTY      ) desc_nbr_max,
                                (NET_SOCK_DESC    *) pdesc_rd,
                                (NET_SOCK_DESC    *) pdesc_wr,
                                (NET_SOCK_DESC    *) pdesc_err,
                                (NET_SOCK_TIMEOUT *) ptimeout,
                                (NET_ERR          *)&err);

    return (rtn_code);
}
#endif
#endif


/*$PAGE*/
/*
*********************************************************************************************************
*                                             inet_addr()
*
* Description : Convert an IPv4 address in ASCII dotted-decimal notation to a network protocol IPv4 address
*                   in network-order.
*
* Argument(s) : paddr       Pointer to an ASCII string that contains a dotted-decimal IPv4 address (see Note #2).
*
* Return(s)   : Network-order IPv4 address represented by ASCII string, if NO errors.
*
*               -1,                                                     otherwise.
*
* Caller(s)   : Application.
*
*               This function is a network protocol suite application interface (API) function & MAY be 
*               called by application function(s).
*
* Note(s)     : (1) RFC #1983 states that "dotted decimal notation ... refers [to] IP addresses of the
*                   form A.B.C.D; where each letter represents, in decimal, one byte of a four byte IP
*                   address".
*
*                   In other words, the dotted-decimal notation separates four decimal octet values by
*                   the dot, or period, character ('.').  Each decimal value represents one octet of 
*                   the IP address starting with the most significant octet in network-order.
*
*                       IP Address Examples :
*
*                              DOTTED DECIMAL NOTATION     HEXADECIMAL EQUIVALENT
*
*                                   127.0.0.1           =       0x7F000001
*                                   192.168.1.64        =       0xC0A80140
*                                   255.255.255.0       =       0xFFFFFF00
*                                   ---         -                 --    --
*                                    ^          ^                 ^      ^
*                                    |          |                 |      |
*                                   MSO        LSO               MSO    LSO
*
*                           where
*                                   MSO        Most  Significant Octet in Dotted Decimal IP Address
*                                   LSO        Least Significant Octet in Dotted Decimal IP Address
*
*               (2) The dotted-decimal ASCII string MUST :
*
*                   (a) Include ONLY decimal values & the dot, or period, character ('.') ; ALL other
*                       characters trapped as invalid, including any leading or trailing characters.
*
*                   (b) (1) Include EXACTLY four decimal values ...
*                       (2) ... separated ...
*                       (3) ... by EXACTLY three dot characters.
*
*                   (c) Ensure that each decimal value does NOT exceed the maximum octet value (i.e. 255).
*
*                   (d) Ensure that each decimal value does NOT include leading zeros.
*********************************************************************************************************
*/

#ifdef  NET_SOCK_MODULE_PRESENT
#if    (NET_SOCK_CFG_FAMILY == NET_SOCK_FAMILY_IP_V4)
in_addr_t  inet_addr (char  *paddr)
{
    in_addr_t  addr;
    NET_ERR    err;


    addr = (in_addr_t)NetASCII_Str_to_IP((CPU_CHAR *) paddr,
                                         (NET_ERR  *)&err);
    if (err !=  NET_ASCII_ERR_NONE) {
        addr = (in_addr_t)NET_BSD_ERR_DFLT;
    }
    addr =  NET_UTIL_HOST_TO_NET_32(addr);

    return (addr);
}
#endif
#endif


/*$PAGE*/
/*
*********************************************************************************************************
*                                             inet_ntoa()
*
* Description : Convert a network protocol IPv4 address into a dotted-decimal notation ASCII string.
*
* Argument(s) : addr        IPv4 address.
*
* Return(s)   : Pointer to ASCII string of converted IPv4 address (see Note #2), if NO errors.
*
*               Pointer to NULL,                                                 otherwise.
*
* Caller(s)   : Application.
*
*               This function is a network protocol suite application interface (API) function & MAY be 
*               called by application function(s).
*
* Note(s)     : (1) RFC #1983 states that "dotted decimal notation ... refers [to] IP addresses of the
*                   form A.B.C.D; where each letter represents, in decimal, one byte of a four byte IP
*                   address".
*
*                   In other words, the dotted-decimal notation separates four decimal octet values by
*                   the dot, or period, character ('.').  Each decimal value represents one octet of 
*                   the IP address starting with the most significant octet in network-order.
*
*                       IP Address Examples :
*
*                              DOTTED DECIMAL NOTATION     HEXADECIMAL EQUIVALENT
*
*                                   127.0.0.1           =       0x7F000001
*                                   192.168.1.64        =       0xC0A80140
*                                   255.255.255.0       =       0xFFFFFF00
*                                   ---         -                 --    --
*                                    ^          ^                 ^      ^
*                                    |          |                 |      |
*                                   MSO        LSO               MSO    LSO
*
*                           where
*                                   MSO        Most  Significant Octet in Dotted Decimal IP Address
*                                   LSO        Least Significant Octet in Dotted Decimal IP Address
*
*               (2) IEEE Std 1003.1, 2004 Edition, Section 'inet_ntoa() : DESCRIPTION' states that
*                   "inet_ntoa() ... need not be reentrant ... [and] is not required to be thread-safe".
*
*                   Since the character string is returned in a single, global character string array,
*                   this conversion function is NOT re-entrant.
*********************************************************************************************************
*/

#ifdef  NET_SOCK_MODULE_PRESENT
#if    (NET_SOCK_CFG_FAMILY == NET_SOCK_FAMILY_IP_V4)
char  *inet_ntoa (struct  in_addr  addr)
{
    in_addr_t  addr_ip;
    NET_ERR    err;


    addr_ip = addr.s_addr;
    addr_ip = NET_UTIL_NET_TO_HOST_32(addr_ip);

    NetASCII_IP_to_Str((NET_IP_ADDR) addr_ip,
                       (CPU_CHAR  *)&NetBSD_IP_to_Str_Array[0],
                       (CPU_BOOLEAN) DEF_NO,
                       (NET_ERR   *)&err);
    if (err != NET_ASCII_ERR_NONE) {
        return ((char *)0);
    }

    return ((char *)&NetBSD_IP_to_Str_Array[0]);
}
#endif
#endif


/*$PAGE*/
/*
*********************************************************************************************************
*                                    STANDARD BSD 4.x FUNCTIONS END
*
* Note(s) : (1) See 'STANDARD BSD 4.x FUNCTIONS  Note #1'.
*********************************************************************************************************
*/

#endif                                                          /* End of BSD fncts (see Note #1).                      */


/*
*********************************************************************************************************
*                                             MODULE END
*
* Note(s) : (1) See 'net_bsd.h  MODULE'.
*********************************************************************************************************
*/

#endif                                                          /* End of BSD module include (see Note #1).             */

