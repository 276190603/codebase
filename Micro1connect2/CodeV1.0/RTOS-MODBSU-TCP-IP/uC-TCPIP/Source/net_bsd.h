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
* Filename      : net_bsd.h
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


/*$PAGE*/
/*
*********************************************************************************************************
*                                               MODULE
*
* Note(s) : (1) BSD 4.x Layer module is required for :
*
*               (a) Network sockets
*               (b) Applications that require BSD 4.x application interface (API) :
*                   (1) Data Types
*                   (2) Macro's
*                   (3) Functions
*
*                       (A) BSD 4.x API MUST be able to be disabled for certain environments, 
*                           operating systems, &/or project builds that may already include 
*                           BSD libraries &/or API definitions that cannot be re-defined.
*
*               See also 'net_sock.h  Note #1'
*                      & 'net_cfg.h   BSD 4.x LAYER CONFIGURATION'.
*
*           (2) The following BSD-module-present configuration value MUST be pre-#define'd in 
*               'net_cfg_net.h' PRIOR to all other network modules that require BSD 4.x Layer 
*               configuration (see 'net_cfg_net.h  BSD 4.x LAYER CONFIGURATION  Note #2b') :
*
*                   NET_BSD_MODULE_PRESENT
*********************************************************************************************************
*/

#ifdef   NET_BSD_MODULE_PRESENT                                 /* See Note #2.                                         */


/*
*********************************************************************************************************
*                                               EXTERNS
*********************************************************************************************************
*/

#ifdef   NET_BSD_MODULE
#define  NET_BSD_EXT
#else
#define  NET_BSD_EXT  extern
#endif


/*$PAGE*/
/*
*********************************************************************************************************
*                                               DEFINES
*********************************************************************************************************
*/

#define  NET_BSD_ASCII_LEN_MAX_ADDR_IP                   NET_ASCII_LEN_MAX_ADDR_IP


/*
*********************************************************************************************************
*                              BSD 4.x SOCKET FAMILY & PROTOCOL DEFINES
*
* Note(s) : (1) The following socket values MUST be pre-#define'd in 'net_def.h' PRIOR to 'net_cfg.h'
*               so that the developer can configure sockets for the correct socket family values (see
*               'net_def.h  BSD 4.x & NETWORK SOCKET LAYER DEFINES  Note #1' & 'net_cfg_net.h  NETWORK 
*               SOCKET LAYER CONFIGURATION') :
*
*               (a) AF_INET
*               (b) PF_INET
*********************************************************************************************************
*/

#if 0                                                           /* See Note #1.                                         */
                                                                /* ------------ SOCK FAMILY/PROTOCOL TYPES ------------ */
#ifdef   AF_INET
#undef   AF_INET
#endif
#define  AF_INET                                           2

#ifdef   PF_INET
#undef   PF_INET
#endif
#define  PF_INET                                     AF_INET

#endif


                                                                /* -------------------- SOCK TYPES -------------------- */
#ifdef   SOCK_STREAM
#undef   SOCK_STREAM
#endif
#define  SOCK_STREAM                                       1

#ifdef   SOCK_DGRAM
#undef   SOCK_DGRAM
#endif
#define  SOCK_DGRAM                                        2


                                                                /* -------------- TCP/IP SOCK PROTOCOLS --------------- */
#ifdef   IPPROTO_TCP
#undef   IPPROTO_TCP
#endif
#define  IPPROTO_TCP                                       6    /*  = NET_IP_HDR_PROTOCOL_TCP                           */

#ifdef   IPPROTO_UDP
#undef   IPPROTO_UDP
#endif
#define  IPPROTO_UDP                                      17    /*  = NET_IP_HDR_PROTOCOL_UDP                           */


/*
*********************************************************************************************************
*                                   BSD 4.x SOCKET ADDRESS DEFINES
*********************************************************************************************************
*/

#ifdef   INADDR_ANY
#undef   INADDR_ANY
#endif
#define  INADDR_ANY                                      NET_IP_ADDR_NONE

#ifdef   INADDR_BROADCAST
#undef   INADDR_BROADCAST
#endif
#define  INADDR_BROADCAST                                NET_IP_ADDR_BROADCAST


/*$PAGE*/
/*
*********************************************************************************************************
*                                     BSD 4.x SOCKET FLAG DEFINES
*
* Note(s) : (1) #### Some socket flags NOT currently supported.
*********************************************************************************************************
*/

#ifdef   MSG_OOB
#undef   MSG_OOB
#endif
#define  MSG_OOB                                  DEF_BIT_00    /* See Note #1.                                         */

#ifdef   MSG_PEEK
#undef   MSG_PEEK
#endif
#define  MSG_PEEK                                 DEF_BIT_01

#ifdef   MSG_DONTROUTE
#undef   MSG_DONTROUTE
#endif
#define  MSG_DONTROUTE                            DEF_BIT_02    /* See Note #1.                                         */

#ifdef   MSG_EOR
#undef   MSG_EOR
#endif
#define  MSG_EOR                                  DEF_BIT_03    /* See Note #1.                                         */

#ifdef   MSG_TRUNC
#undef   MSG_TRUNC
#endif
#define  MSG_TRUNC                                DEF_BIT_04    /* See Note #1.                                         */

#ifdef   MSG_CTRUNC
#undef   MSG_CTRUNC
#endif
#define  MSG_CTRUNC                               DEF_BIT_05    /* See Note #1.                                         */

#ifdef   MSG_WAITALL
#undef   MSG_WAITALL
#endif
#define  MSG_WAITALL                              DEF_BIT_06    /* See Note #1.                                         */

#ifdef   MSG_DONTWAIT
#undef   MSG_DONTWAIT
#endif
#define  MSG_DONTWAIT                             DEF_BIT_07


/*$PAGE*/
/*
*********************************************************************************************************
*                                 BSD 4.x RETURN CODE / ERROR DEFINES
*
* Note(s) : (1) IEEE Std 1003.1, 2004 Edition states ... :
*
*               (a) ... to "return 0" :
*
*                   (1) "Upon successful completion" of the following BSD socket functions :
*
*                       (A) close()
*                       (B) shutdown()
*                       (C) bind()
*                       (D) connect()
*                       (E) listen()
*
*                   (2) "If no messages are available ... and the peer has performed an orderly shutdown"
*                        for the following BSD socket functions :
*
*                       (A) recvfrom()
*                       (B) recv()
*
*                   (3) Stevens/Fenner/Rudoff, UNIX Network Programming, Volume 1, 3rd Edition, 6th Printing,
*                       Section 6.3, Page 161 states that BSD select() function "returns ... 0 on timeout".
*
*               (b) "Otherwise, -1 shall be returned and 'errno' set to indicate the error" ... for the 
*                    following BSD functions :
*
*                   (1) socket()
*                   (2) close()
*                   (3) shutdown()
*                   (4) bind()
*                   (5) listen()
*                   (6) accept()
*                   (7) connect()
*                   (8) recvfrom()
*                   (9) recv()
*                  (10) sendto()
*                  (11) send()
*                  (12) select()
*                  (13) inet_addr()
*                  (14) inet_ntoa()
*
*                  #### 'errno' NOT currently supported (see 'net_bsd.h  Note #1b').
*********************************************************************************************************
*/

#define  NET_BSD_ERR_NONE                                  0                    /* See Note #1a.                        */
#define  NET_BSD_ERR_DFLT                                 -1                    /* See Note #1b.                        */

#define  NET_BSD_RTN_CODE_OK                             NET_BSD_ERR_NONE       /* See Note #1a1.                       */
#define  NET_BSD_RTN_CODE_TIMEOUT                        NET_BSD_RTN_CODE_OK    /* See Note #1a3.                       */
#define  NET_BSD_RTN_CODE_CONN_CLOSED                    NET_BSD_RTN_CODE_OK    /* See Note #1a2.                       */


/*$PAGE*/
/*
*********************************************************************************************************
*                                             DATA TYPES
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*                                      BSD 4.x SOCKET DATA TYPES
*
* Note(s) : (1) BSD 4.x data types are required only for applications that reference BSD 4.x data types.
*
*               See also 'MODULE  Note #1b2'.
*
*           (2) (a) (1) IEEE Std 1003.1, 2004 Edition, Section 'sys/socket.h : DESCRIPTION' states that :
*
*                       (A) "sa_family_t ... shall define [as an] unsigned integer type."
*
*                       (B) "socklen_t ... is an integer type of width of at least 32 bits."
*
*                           (1) IEEE Std 1003.1, 2004 Edition, Section 'sys/socket.h : APPLICATION USAGE' 
*                               states that "it is recommended that applications not use values larger
*                               than (2^31 - 1) for the socklen_t type".
*
*                   (2) IEEE Std 1003.1, 2004 Edition, Section 'netinet/in.h : DESCRIPTION' states that :
*
*                       (A) "in_port_t - Equivalent to the type uint16_t."
*
*                       (B) "in_addr_t - Equivalent to the type uint32_t."
*
*               (b) (1) (A) IEEE Std 1003.1, 2004 Edition, Section 'sys/socket.h : DESCRIPTION' states 
*                           that "the sockaddr structure ... includes at least the following members" :
*
*                           (1) sa_family_t    sa_family    Address family
*                           (2) char           sa_data[]    Socket address
*
*                       (B) (1) Socket address structure 'sa_family' member MUST be configured in host-
*                               order & MUST NOT be converted to/from network-order.
*
*                           (2) Socket address structure addresses MUST be configured/converted from host-
*                               order to network-order.
*
*                           See also 'net_sock.h  NETWORK SOCKET ADDRESS DATA TYPES  Note #2'.
*
*                   (2) IEEE Std 1003.1, 2004 Edition, Section 'netinet/in.h : DESCRIPTION' states that :
*
*                       (A) "The in_addr structure ... includes at least the following member" :
*
*                           (1) in_addr_t    s_addr
*
*                       (B) (1) "The sockaddr_in structure ... includes at least the following members" :
*
*                               (a) sa_family_t       sin_family    Address family (AF_INET)
*                               (b) in_port_t         sin_port      Port number
*                               (c) struct in_addr    sin_addr      IP   address
*
*                           (2) (a) "The sin_port and sin_addr members shall be in network byte order."
*
*                               (b) "The sin_zero member was removed from the sockaddr_in structure."
*
*                                   (1) However, this does NOT preclude the structure from including a 
*                                       'sin_zero' member.
*
*           (3) IEEE Std 1003.1, 2004 Edition, Section 'sys/types.h : DESCRIPTION' states that :
*
*               (a) (1) (A) "size_t - Used for sizes of objects."
*
*                       (B) "size_t shall be an unsigned integer type."
*
*                       (C) To avoid possible namespace conflict with commonly-defined 'size_t' data type,
*                           '_size_t' data type is prefixed with a single underscore.
*
*                   (2) (A) "ssize_t - Used for a count of bytes or an error indication."
*
*                       (B) "ssize_t shall be [a] signed integer type ... capable of storing values at 
*                            least in the range [-1, {SSIZE_MAX}]."
*
*               (b) (1) (A) "time_t - Used for time in seconds."
*
*                       (B) "time_t ... shall be integer or real-floating types."
*
*                       (C) To avoid possible namespace conflict with commonly-defined 'time_t' data type,
*                           '_time_t' data type is prefixed with a single underscore.
*
*                   (2) (A) "suseconds_t - Used for time in microseconds."
*
*                       (B) "suseconds_t shall be a signed integer type capable of storing values at least
*                            in the range [-1, 1000000]."
*
*               (c) "The implementation shall support ... size_t, ssize_t, suseconds_t ... widths ... no
*                    greater than the width of type long."
*$PAGE*
*           (4) (a) IEEE Std 1003.1, 2004 Edition, Section 'sys/select.h : DESCRIPTION' states that "the 
*                   'timeval' structure ... includes at least the following members" :
*
*                   (1) time_t         tv_sec     Seconds
*                   (2) suseconds_t    tv_usec    Microseconds
*
*               (b) Ideally, the BSD 4.x Layer's 'timeval' data type would be the basis for the Network 
*                   Socket Layer's 'NET_SOCK_TIMEOUT' data type definition.  However, since the BSD 4.x 
*                   Layer application interface (API) is NOT guaranteed to be present in the project 
*                   build (see 'MODULE  Note #1bA'); the Network Socket Layer's 'NET_SOCK_TIMEOUT' data 
*                   type MUST be independently defined.
*
*                   However, for correct interoperability between the BSD 4.x Layer 'timeval' data type
*                   & the Network Socket Layer's 'NET_SOCK_TIMEOUT' data type; ANY modification to either 
*                   of these data types MUST be appropriately synchronized.
*
*                   See also 'net_sock.h  NETWORK SOCKET TIMEOUT DATA TYPE  Note #1b'.
*
*           (5) (a) (1) IEEE Std 1003.1, 2004 Edition, Section 'sys/select.h : DESCRIPTION' states that 
*                       the "'fd_set' type ... shall [be] define[d] ... as a structure".
*
*                       (A) "The requirement for the 'fd_set' structure to have a member 'fds_bits' has 
*                            been removed."
*
*                           (1) However, this does NOT preclude the descriptor structure from including
*                               an 'fds_bits' member.
*
*                       (B) Stevens/Fenner/Rudoff, UNIX Network Programming, Volume 1, 3rd Edition, 
*                           6th Printing, Section 6.3, Pages 162-163 states that "descriptor sets [are] 
*                           typically an array of integers, with each bit in each integer corresponding 
*                           to a descriptor".
*
*                   (2) (A) IEEE Std 1003.1, 2004 Edition, Section 'sys/select.h : DESCRIPTION' states 
*                           that "FD_SETSIZE ... shall be defined as a macro ... [as the] maximum number 
*                           of file descriptors in an 'fd_set' structure."
*
*                       (B) Stevens/Fenner/Rudoff, UNIX Network Programming, Volume 1, 3rd Edition, 
*                           6th Printing, Section 6.3, Page 163 states that "descriptors start at 0".
*
*               (b) Ideally, the BSD 4.x Layer's 'fd_set' data type would be the basis for the Network 
*                   Socket Layer's 'NET_SOCK_DESC' data type definition.  However, since the BSD 4.x 
*                   Layer application interface (API) is NOT guaranteed to be present in the project 
*                   build (see 'MODULE  Note #1bA'); the Network Socket Layer's 'NET_SOCK_DESC' data 
*                   type MUST be independently defined.
*
*                   However, for correct interoperability between the BSD 4.x Layer 'fd_set' data type
*                   & the Network Socket Layer's 'NET_SOCK_DESC' data type; ANY modification to either 
*                   of these data types MUST be appropriately synchronized.
*
*               See also 'net_sock.h  NETWORK SOCKET (IDENTIFICATION) DESCRIPTOR SET DATA TYPE  Note #1'.
*********************************************************************************************************
*/
/*$PAGE*/
#define  NET_BSD_ADDR_LEN_MAX                                     14    /*  = sizeof(sockaddr.sa_data)                  */
#define  NET_BSD_ADDR_IP_NBR_OCTETS_UNUSED                         8    /*  = sizeof(sockaddr_in.sin_zero)              */

#if (NET_BSD_CFG_API_EN == DEF_ENABLED)                                 /* See Note #1.                                 */

                                                                        /* ---------------- SOCK ADDR ----------------- */
typedef  CPU_INT16U       sa_family_t;                                  /* Sock addr family type (see Note #2a1A).      */

typedef  CPU_INT32S       socklen_t;                                    /* Sock addr len    type (see Note #2a1B).      */

typedef  CPU_INT16U       in_port_t;                                    /* Sock IP port nbr type (see Note #2a2A).      */
typedef  CPU_INT32U       in_addr_t;                                    /* Sock IP addr     type (see Note #2a2B).      */


struct  sockaddr {                                                      /* See Note #2b1.                               */
    sa_family_t           sa_family;                                    /* Sock family.                                 */
    CPU_CHAR              sa_data[NET_BSD_ADDR_LEN_MAX];                /* Sock addr.                                   */
};


struct  in_addr {                                                       /* See Note #2b2A.                              */
    in_addr_t             s_addr;
};


struct  sockaddr_in {                                                   /* See Note #2b2B.                              */
            sa_family_t   sin_family;                                   /* AF_INET family.                              */
            in_port_t     sin_port;                                     /* Port nbr              [see Note #2b2B2a].    */
    struct  in_addr       sin_addr;                                     /* IP   addr             [see Note #2b2B2a].    */
            CPU_CHAR      sin_zero[NET_BSD_ADDR_IP_NBR_OCTETS_UNUSED];  /* Unused (MUST be zero) [see Note #2b2B2b].    */
};


                                                                        /* ----------- SOCK DATA/VAL SIZES ------------ */
typedef    unsigned  int  _size_t;                                      /* Sock app data buf size type (see Note #3a1). */
typedef      signed  int  ssize_t;                                      /* Sock rtn data/val size type (see Note #3a2). */

                                                                        /* -------------- SOCK TIME VALS -------------- */
typedef  CPU_INT32S       _time_t;                                      /* Signed time val in  sec (see Note #3b1).     */
typedef  CPU_INT32S       suseconds_t;                                  /* Signed time val in usec (see Note #3b2).     */


struct  timeval {                                                       /* See Note #4a.                                */
   _time_t                tv_sec;                                       /* Time val in  sec (see Note #4a1).            */
    suseconds_t           tv_usec;                                      /* Time val in usec (see Note #4a2).            */
};


                                                                        /* -------------- FILE DESC SETS -------------- */
#define  FD_SETSIZE                NET_SOCK_CFG_NBR_SOCK                /* See Note #5a2A.                              */
#define  FD_MIN                                 0                       /* See Note #5a2B.                              */
#define  FD_MAX                   (FD_SETSIZE - 1)
#define  FD_ARRAY_SIZE          (((FD_SETSIZE - 1) / (sizeof(CPU_DATA) * DEF_OCTET_NBR_BITS)) + 1)

struct  fd_set {                                                        /* See Note #5a1.                               */
    CPU_DATA              fds_bits[FD_ARRAY_SIZE];
};

#endif


/*$PAGE*/
/*
*********************************************************************************************************
*                                          GLOBAL VARIABLES
*
* Note(s) : (1) BSD 4.x global variables are required only for applications that call BSD 4.x functions.
*
*               See also 'MODULE  Note #1b'
*                      & 'STANDARD BSD 4.x FUNCTION PROTOTYPES  Note #1'.
*********************************************************************************************************
*/

#if    (NET_BSD_CFG_API_EN  == DEF_ENABLED)                             /* See Note #1.                                 */

#ifdef  NET_SOCK_MODULE_PRESENT
#if    (NET_SOCK_CFG_FAMILY == NET_SOCK_FAMILY_IP_V4)
NET_BSD_EXT  CPU_CHAR   NetBSD_IP_to_Str_Array[NET_BSD_ASCII_LEN_MAX_ADDR_IP];
#endif
#endif

#endif


/*$PAGE*/
/*
*********************************************************************************************************
*                                               MACRO'S
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*                       BSD 4.x NETWORK WORD ORDER - TO - CPU WORD ORDER MACRO'S
*
* Description : Convert data values to & from network word order to host CPU word order.
*
* Argument(s) : val       Data value to convert (see Note #2).
*
* Return(s)   : Converted data value (see Note #2).
*
* Caller(s)   : Application.
*
*               These macro's are network protocol suite application interface (API) macro's & MAY be 
*               called by application function(s).
*
* Note(s)     : (1) BSD 4.x macro's are required only for applications that call BSD 4.x macro's.
*
*                   See also 'MODULE  Note #1b1'.
*
*               (2) 'val' data value to convert & any variable to receive the returned conversion MUST 
*                   start on appropriate CPU word-aligned addresses.  This is required because most word-
*                   aligned processors are more efficient & may even REQUIRE that multi-octet words start 
*                   on CPU word-aligned addresses.
*
*                   (a) For 16-bit word-aligned processors, this means that
*
*                           all 16- & 32-bit words MUST start on addresses that are multiples of 2 octets
*
*                   (b) For 32-bit word-aligned processors, this means that
*
*                           all 16-bit       words MUST start on addresses that are multiples of 2 octets
*                           all 32-bit       words MUST start on addresses that are multiples of 4 octets
*
*                   See also 'net_util.h  NETWORK WORD ORDER - TO - CPU WORD ORDER MACRO'S  Note #1'.
*********************************************************************************************************
*/

#if (NET_BSD_CFG_API_EN == DEF_ENABLED)                                 /* See Note #1.                                 */

#define  ntohs(val)                      NET_UTIL_NET_TO_HOST_16(val)
#define  ntohl(val)                      NET_UTIL_NET_TO_HOST_32(val)

#define  htons(val)                      NET_UTIL_HOST_TO_NET_16(val)
#define  htonl(val)                      NET_UTIL_HOST_TO_NET_32(val)

#endif


/*$PAGE*/
/*
*********************************************************************************************************
*                                   BSD 4.x FILE DESCRIPTOR MACRO'S
*
* Description : Initialize, modify, & check file descriptor sets for multiplexed I/O functions.
*
* Argument(s) : fd          File descriptor number to initialize, modify, or check; when applicable.
*
*               fdsetp      Pointer to a file descriptor set.
*
* Return(s)   : Return values macro-dependent :
*
*                   none, for    file descriptor initialization & modification macro's (see Note #2a2A).
*
*                   1,    if any file descriptor condition(s) satisfied                (see Note #2a2B1).
*
*                   0,    otherwise                                                    (see Note #2a2B2).
*
* Caller(s)   : Application.
*
*               These macro's are network protocol suite application interface (API) macro's & MAY be called
*               by application function(s).
*
* Note(s)     : (1) BSD 4.x macro's are required only for applications that call BSD 4.x macro's.
*
*                   See also 'MODULE  Note #1b3'.
*
*               (2) (a) (1) (A) IEEE Std 1003.1, 2004 Edition, Section 'sys/select.h : DESCRIPTION' states that 
*                               "each of the following may be declared as a function, or defined as a macro, or
*                                both" :
*
*                               (1) void  FD_ZERO (fd_set *fdset);
*
*                                  "Initializes the file descriptor set 'fdset' to have zero bits for all file 
*                                   descriptors."
*
*                               (2) void  FD_CLR  (int fd, fd_set *fdset);
*
*                                  "Clears the bit for the file descriptor 'fd' in the file descriptor set 'fdset'."
*
*                               (3) void  FD_SET  (int fd, fd_set *fdset);
*
*                                  "Sets   the bit for the file descriptor 'fd' in the file descriptor set 'fdset'."
*
*                               (4) int   FD_ISSET(int fd, fd_set *fdset);
*
*                                  "Returns a non-zero value if the bit for the file descriptor 'fd' is set in the 
*                                   file descriptor set by 'fdset', and 0 otherwise."
*
*                           (B) IEEE Std 1003.1, 2004 Edition, Section 'select() : DESCRIPTION' reiterates that
*                               "file descriptor masks of type 'fd_set' can be initialized and tested with" :
*                                    
*                               (1) "FD_ZERO(fdsetp) shall initialize the descriptor set pointed to by 'fdsetp' 
*                                    to the null set.  No error is returned if the set is not empty at the time 
*                                    FD_ZERO() is invoked."
*
*                               (2) "FD_CLR(fd, fdsetp) shall remove the file descriptor 'fd' from the set pointed 
*                                    to by 'fdsetp'.  If 'fd' is not a member of this set, there shall be no effect 
*                                    on the set, nor will an error be returned."
*
*                               (3) "FD_SET(fd, fdsetp) shall add the file descriptor 'fd' to the set pointed to by 
*                                    'fdsetp'.  If the file descriptor 'fd' is already in this set, there shall be 
*                                    no effect on the set, nor will an error be returned."
*
*                               (4) "FD_ISSET(fd, fdsetp) shall evaluate to non-zero if the file descriptor 'fd' is a
*                                    member of the set pointed to by 'fdsetp', and shall evaluate to zero otherwise."
*
*                       (2) IEEE Std 1003.1, 2004 Edition, Section 'select() : RETURN VALUE' states that :
*
*                           (A) The following macro's "do not return a value" :
*
*                               (1) "FD_CLR()," ...
*                               (2) "FD_SET()," ...
*                               (3) "FD_ZERO()."
*
*                           (B) "FD_ISSET() shall return" :
*
*                               (1) "a non-zero value if the bit for the file descriptor 'fd' is set in the file 
*                                    descriptor set pointed to by 'fdset'," ...
*                               (2) "0 otherwise."
*$PAGE*
*                   (b) (1) IEEE Std 1003.1, 2004 Edition, Section 'sys/select.h : DESCRIPTION' states that :
*
*                           (A) "If implemented as macros, these may evaluate their arguments more than once, so 
*                                applications should ensure that the arguments they supply are never expressions 
*                                with side effects."
*
*                       (2) IEEE Std 1003.1, 2004 Edition, Section 'select() : DESCRIPTION' also states that :
*
*                           (A) "It is unspecified whether each of these is a macro or a function.  If a macro 
*                                definition is suppressed in order to access an actual function, or a program 
*                                defines an external identifier with any of these names, the behavior is undefined."
*
*                           (B) "The behavior of these macros is undefined" :
*
*                               (1) "if the 'fd' argument is" :
*                                   (a) "less than 0"                             ...
*                                   (b) "or greater than or equal to FD_SETSIZE," ...
*
*                               (2) "or if 'fd' is not a valid file descriptor,"  ...
*                               (3) "or if any of the arguments are expressions with side effects."
*
*                       (3) Stevens/Fenner/Rudoff, UNIX Network Programming, Volume 1, 3rd Edition, 6th Printing,
*                           Section 6.3, Page 163 adds that "it is important to initialize [a descriptor] set,
*                           since unpredictable results can occur if the set is allocated as an automatic variable
*                           and not initialized".
*
*                   See also 'BSD 4.x SOCKET DATA TYPES  Note #5b'.
*
*               (3) Ideally, the BSD 4.x Layer's file descriptor macro's ('FD_???()') would be the basis for 
*                   the Network Socket Layer's socket descriptor macro ('NET_SOCK_DESC_???()') definitions.  
*                   However, since the BSD 4.x Layer application interface (API) is NOT guaranteed to be 
*                   present in the project build (see 'MODULE  Note #1bA'); the Network Socket Layer's socket 
*                   descriptor macro's MUST be independently defined.
*
*                   However, for correct interoperability between the BSD 4.x Layer file descriptor macro's
*                   & the Network Socket Layer's socket descriptor macro's; ANY modification to any of these 
*                   macro definitions MUST be appropriately synchronized.
*
*                   See also 'net_sock.h  NETWORK SOCKET DESCRIPTOR MACRO'S  Note #1'.
*********************************************************************************************************
*/

#if (NET_BSD_CFG_API_EN == DEF_ENABLED)                                 /* See Note #1.                                 */

#define  FD_ZERO(fdsetp)     { if (((struct fd_set *)(fdsetp)) != (struct fd_set *)0) {                      \
                                   Mem_Clr ((void     *)      (&(((struct fd_set *)(fdsetp))->fds_bits[0])), \
                                            (CPU_SIZE_T) (sizeof(((struct fd_set *)(fdsetp))->fds_bits)));   \
                               }                                                                             }


#define  FD_CLR(fd, fdsetp)               { if (((fd) >= FD_MIN) && ((fd) <= FD_MAX) &&                                                                  \
                                                                (((struct fd_set *)(fdsetp)) != (struct fd_set *)0)) {                                   \
                                                 DEF_BIT_CLR   ((((struct fd_set *)(fdsetp))->fds_bits[(fd) / (sizeof(CPU_DATA) * DEF_OCTET_NBR_BITS)]), \
                                                 DEF_BIT                                              ((fd) % (sizeof(CPU_DATA) * DEF_OCTET_NBR_BITS))); \
                                            }                                                                                                            }

#define  FD_SET(fd, fdsetp)               { if (((fd) >= FD_MIN) && ((fd) <= FD_MAX) &&                                                                  \
                                                                (((struct fd_set *)(fdsetp)) != (struct fd_set *)0)) {                                   \
                                                 DEF_BIT_SET   ((((struct fd_set *)(fdsetp))->fds_bits[(fd) / (sizeof(CPU_DATA) * DEF_OCTET_NBR_BITS)]), \
                                                 DEF_BIT                                              ((fd) % (sizeof(CPU_DATA) * DEF_OCTET_NBR_BITS))); \
                                            }                                                                                                            }

#define  FD_ISSET(fd, fdsetp)                 ((((fd) >= FD_MIN) && ((fd) <= FD_MAX) &&                                                                  \
                                                                (((struct fd_set *)(fdsetp)) != (struct fd_set *)0)) ?                                   \
                                              (((DEF_BIT_IS_SET((((struct fd_set *)(fdsetp))->fds_bits[(fd) / (sizeof(CPU_DATA) * DEF_OCTET_NBR_BITS)]), \
                                                 DEF_BIT                                              ((fd) % (sizeof(CPU_DATA) * DEF_OCTET_NBR_BITS)))) \
                                              == DEF_YES) ? 1 : 0)                                                                                       \
                                                              : 0)

#endif


/*$PAGE*/
/*
*********************************************************************************************************
*                                         FUNCTION PROTOTYPES
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*                                 STANDARD BSD 4.x FUNCTION PROTOTYPES
*
* Note(s) : (1) BSD 4.x function prototypes are required only for applications that call BSD 4.x functions.
*
*               See also 'MODULE  Note #1b3'.
*********************************************************************************************************
*/

#if (NET_BSD_CFG_API_EN == DEF_ENABLED)                                 /* See Note #1.                                 */

                                                                        /* ------------- SOCK ALLOC FNCTS ------------- */
int         socket   (        int         protocol_family,
                              int         sock_type,
                              int         protocol);

int         close    (        int         sock_id);

int         shutdown (        int         sock_id,
                              int         type);


                                                                        /* ------------- LOCAL CONN FCNTS ------------- */
int         bind     (        int         sock_id,
                      struct  sockaddr   *paddr_local,
                              socklen_t   addr_len);


                                                                        /* ------------ CLIENT CONN FCNTS ------------- */
int         connect  (        int         sock_id,
                      struct  sockaddr   *paddr_remote,
                              socklen_t   addr_len);


                                                                        /* ------------ SERVER CONN FCNTS ------------- */
int         listen   (        int         sock_id,
                              int         sock_q_size);

int         accept   (        int         sock_id,
                      struct  sockaddr   *paddr_remote,
                              socklen_t  *paddr_len);


/*$PAGE*/
                                                                        /* ----------------- RX FNCTS ----------------- */
ssize_t     recvfrom (        int         sock_id,
                              void       *pdata_buf,
                             _size_t      data_buf_len,
                              int         flags,
                      struct  sockaddr   *paddr_remote,
                              socklen_t  *paddr_len);

ssize_t     recv     (        int         sock_id,
                              void       *pdata_buf,
                             _size_t      data_buf_len,
                              int         flags);


                                                                        /* ----------------- TX FNCTS ----------------- */
ssize_t     sendto   (        int         sock_id,
                              void       *p_data,
                             _size_t      data_len,
                              int         flags,
                      struct  sockaddr   *paddr_remote,
                              socklen_t   addr_len);

ssize_t     send     (        int         sock_id,
                              void       *p_data,
                             _size_t      data_len,
                              int         flags);


                                                                        /* ------------ MULTIPLEX I/O FNCTS ----------- */
int         select   (        int         desc_nbr_max,
                      struct  fd_set     *pdesc_rd,
                      struct  fd_set     *pdesc_wr,
                      struct  fd_set     *pdesc_err,
                      struct  timeval    *ptimeout);


                                                                        /* ---------------- CONV FCNTS ---------------- */
in_addr_t   inet_addr(        char       *paddr);

char       *inet_ntoa(struct  in_addr     addr);


#endif


/*$PAGE*/
/*
*********************************************************************************************************
*                                        CONFIGURATION ERRORS
*********************************************************************************************************
*/


/*
*********************************************************************************************************
*                                             MODULE END
*
* Note(s) : (1) See 'net_bsd.h  MODULE'.
*********************************************************************************************************
*/

#endif                                                          /* End of BSD module include (see Note #1).             */

