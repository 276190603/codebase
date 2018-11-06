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
*                                          NETWORK TCP LAYER
*                                   (TRANSMISSION CONTROL PROTOCOL)
*
* Filename      : net_tcp.h
* Version       : V1.92
* Programmer(s) : ITJ
*********************************************************************************************************
* Note(s)       : (1) Supports Transmission Control Protocol as described in RFC #793 with the following
*                     restrictions/constraints :
*
*                     (a) TCP Security & Precedence NOT supported               RFC # 793, Section 3.6
*
*                     (b) TCP Urgent Data           NOT supported               RFC # 793, Section 3.7
*                                                                                'The Communication of
*                                                                                  Urgent Information'
*
*                     (c) The following TCP options NOT supported :             
*
*                         (1) Window Scale                                      RFC #1072, Section 2
*                                                                               RFC #1323, Section 2
*                         (2) Selective Acknowledgement (SACK)                  RFC #1072, Section 3
*                                                                               RFC #2018
*                                                                               RFC #2883
*                         (3) TCP Echo                                          RFC #1072, Section 4
*                         (4) Timestamp                                         RFC #1323, Section 3.2
*                         (5) Protection Against Wrapped Sequences (PAWS)       RFC #1323, Section 4
*
*                     (d) #### IP-Options-to-TCP-Connection                     RFC #1122, Section 4.2.3.8
*                                Handling NOT           supported
*
*                     (e) #### ICMP-Error-Message-to-TCP-Connection             RFC #1122, Section 4.2.3.9
*                                Handling NOT currently supported
*
*                 (2) TCP Layer assumes/requires Network Socket Layer (see 'net_sock.h  MODULE  Note #1a2').
*********************************************************************************************************
*/


/*$PAGE*/
/*
*********************************************************************************************************
*                                               MODULE
*
* Note(s) : (1) TCP Layer module is NOT required for UDP-to-Application API configuration.
*
*               See also 'net_cfg.h  TRANSPORT LAYER CONFIGURATION'
*                      & 'net_cfg.h  USER DATAGRAM PROTOCOL LAYER CONFIGURATION'.
*
*               See also 'net_tcp.h  Note #2'.
*
*           (2) The following TCP-module-present configuration value MUST be pre-#define'd in 
*               'net_cfg_net.h' PRIOR to all other network modules that require TCP Layer
*               configuration (see 'net_cfg_net.h  TCP LAYER CONFIGURATION  Note #2b') :
*
*                   NET_TCP_MODULE_PRESENT
*********************************************************************************************************
*/

#ifdef   NET_TCP_MODULE_PRESENT                                 /* See Note #2.                                         */


/*
*********************************************************************************************************
*                                               EXTERNS
*********************************************************************************************************
*/

#ifdef   NET_TCP_MODULE
#define  NET_TCP_EXT
#else
#define  NET_TCP_EXT  extern
#endif


/*$PAGE*/
/*
*********************************************************************************************************
*                                               DEFINES
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*                                         TCP HEADER DEFINES
*
* Note(s) : (1) The following TCP value MUST be pre-#define'd in 'net_def.h' PRIOR to 'net_buf.h' so that
*               the Network Buffer Module can configure maximum buffer header size (see 'net_def.h  TCP
*               LAYER DEFINES' & 'net_buf.h  NETWORK BUFFER INDEX & SIZE DEFINES  Note #1') :
*
*               (a) NET_TCP_HDR_SIZE_MAX                  60        (NET_TCP_HDR_LEN_MAX
*                                                                  * NET_TCP_HDR_LEN_WORD_SIZE)
*
*           (2) Urgent pointer & data NOT supported (see 'net_tcp.h  Note #1b').
*********************************************************************************************************
*/

#define  NET_TCP_HDR_LEN_MASK                         0xF000
#define  NET_TCP_HDR_LEN_SHIFT                            12
#define  NET_TCP_HDR_LEN_NONE                              0
#define  NET_TCP_HDR_LEN_MIN                               5
#define  NET_TCP_HDR_LEN_MAX                              15
#define  NET_TCP_HDR_LEN_WORD_SIZE                       CPU_WORD_SIZE_32

#define  NET_TCP_HDR_SIZE_MIN                           (NET_TCP_HDR_LEN_MIN * NET_TCP_HDR_LEN_WORD_SIZE)
#if 0                                                           /* See Note #1a.                                        */
#define  NET_TCP_HDR_SIZE_MAX                           (NET_TCP_HDR_LEN_MAX * NET_TCP_HDR_LEN_WORD_SIZE)
#endif

#define  NET_TCP_HDR_SIZE_TOT_MIN                       (NET_IP_HDR_SIZE_TOT_MIN + NET_TCP_HDR_SIZE_MIN)
#define  NET_TCP_HDR_SIZE_TOT_MAX                       (NET_IP_HDR_SIZE_TOT_MAX + NET_TCP_HDR_SIZE_MAX)


#define  NET_TCP_PSEUDO_HDR_SIZE                          12    /*  = sizeof(NET_TCP_PSEUDO_HDR)                        */


#define  NET_TCP_PORT_NBR_RESERVED                       NET_PORT_NBR_RESERVED
#define  NET_TCP_PORT_NBR_NONE                           NET_TCP_PORT_NBR_RESERVED


#define  NET_TCP_HDR_URG_PTR_NONE                     0x0000    /* See Note #2.                                         */


/*$PAGE*/
/*
*********************************************************************************************************
*                                       TCP HEADER FLAG DEFINES
*
* Note(s) : (1) See 'TCP HEADER  Note #2' for flag fields.
*
*           (2) Urgent pointer & data NOT supported (see 'net_tcp.h  Note #1b').
*********************************************************************************************************
*/

#define  NET_TCP_HDR_FLAG_MASK                        0x0FFF

#define  NET_TCP_HDR_FLAG_NONE                    DEF_BIT_NONE
#define  NET_TCP_HDR_FLAG_RESERVED                    0x0FE0    /* MUST be '0'.                                         */
#define  NET_TCP_HDR_FLAG_URGENT                  DEF_BIT_05    /* See Note #2.                                         */
#define  NET_TCP_HDR_FLAG_ACK                     DEF_BIT_04
#define  NET_TCP_HDR_FLAG_PUSH                    DEF_BIT_03
#define  NET_TCP_HDR_FLAG_RESET                   DEF_BIT_02
#define  NET_TCP_HDR_FLAG_SYNC                    DEF_BIT_01
#define  NET_TCP_HDR_FLAG_FIN                     DEF_BIT_00
#define  NET_TCP_HDR_FLAG_CLOSE                   NET_TCP_HDR_FLAG_FIN


/*
*********************************************************************************************************
*                                          TCP FLAG DEFINES
*********************************************************************************************************
*/

                                                                /* ------------------ NET TCP FLAGS ------------------- */
#define  NET_TCP_FLAG_NONE                        DEF_BIT_NONE
#define  NET_TCP_FLAG_USED                        DEF_BIT_00    /* TCP conn cur used; i.e. NOT in free TCP conn pool.   */

                                                                /* ------------------ TCP TX  FLAGS ------------------- */
                                                                /* TCP tx flags copied from TCP hdr flags.              */
#define  NET_TCP_FLAG_TX_FIN                      NET_TCP_HDR_FLAG_FIN
#define  NET_TCP_FLAG_TX_CLOSE                    NET_TCP_FLAG_TX_FIN
#define  NET_TCP_FLAG_TX_SYNC                     NET_TCP_HDR_FLAG_SYNC
#define  NET_TCP_FLAG_TX_RESET                    NET_TCP_HDR_FLAG_RESET
#define  NET_TCP_FLAG_TX_PUSH                     NET_TCP_HDR_FLAG_PUSH
#define  NET_TCP_FLAG_TX_ACK                      NET_TCP_HDR_FLAG_ACK
#define  NET_TCP_FLAG_TX_URGENT                   NET_TCP_HDR_FLAG_URGENT

#define  NET_TCP_FLAG_TX_BLOCK                    DEF_BIT_07

                                                                /* ------------------ TCP RX  FLAGS ------------------- */
#define  NET_TCP_FLAG_RX_DATA_PEEK                DEF_BIT_08
#define  NET_TCP_FLAG_RX_BLOCK                    DEF_BIT_15


/*$PAGE*/
/*
*********************************************************************************************************
*                                          TCP TYPE DEFINES
*
* Note(s) : (1) NET_TCP_TYPE_??? #define values specifically chosen as ASCII representations of the TCP
*               types.  Memory displays of TCP types will display with their chosen ASCII names.
*********************************************************************************************************
*/

                                                                /* ------------------ NET TCP TYPES ------------------- */
#if     (CPU_CFG_ENDIAN_TYPE == CPU_ENDIAN_TYPE_BIG)
#define  NET_TCP_TYPE_NONE                        0x4E4F4E45    /* "NONE" in ASCII.                                     */
#define  NET_TCP_TYPE_CONN                        0x54435020    /* "TCP " in ASCII.                                     */

#else

#if     (CPU_CFG_DATA_SIZE   == CPU_WORD_SIZE_32)
#define  NET_TCP_TYPE_NONE                        0x454E4F4E    /* "NONE" in ASCII.                                     */
#define  NET_TCP_TYPE_CONN                        0x20504354    /* "TCP " in ASCII.                                     */

#elif   (CPU_CFG_DATA_SIZE   == CPU_WORD_SIZE_16)
#define  NET_TCP_TYPE_NONE                        0x4F4E454E    /* "NONE" in ASCII.                                     */
#define  NET_TCP_TYPE_CONN                        0x43542050    /* "TCP " in ASCII.                                     */

#else                                                           /* Dflt CPU_WORD_SIZE_08.                               */
#define  NET_TCP_TYPE_NONE                        0x4E4F4E45    /* "NONE" in ASCII.                                     */
#define  NET_TCP_TYPE_CONN                        0x54435020    /* "TCP " in ASCII.                                     */

#endif
#endif


/*
*********************************************************************************************************
*                                     TCP SEQUENCE NUMBER DEFINES
*
* Note(s) : (1) TCP initial transmit sequence number is incremented by a fixed value, preferably a large
*               prime value or a large value with multiple unique prime factors.
*
*               (a) One reasonable TCP initial transmit sequence number increment value example :
*
*                       65527  =  37 * 23 * 11 * 7
*
*
*               #### NET_TCP_TX_SEQ_NBR_CTR_INC could be developer-configured in 'net_cfg.h'.
*
*               See also 'NET_TCP_TX_GET_SEQ_NBR()  Notes #1b2 & #1c2'.
*********************************************************************************************************
*/

#define  NET_TCP_SEQ_NBR_NONE                              0
#define  NET_TCP_ACK_NBR_NONE                            NET_TCP_SEQ_NBR_NONE

#define  NET_TCP_TX_SEQ_NBR_CTR_INC                    65527uL  /* See Note #1.                                         */


#define  NET_TCP_ACK_NBR_DUP_WIN_SIZE_SCALE                4


/*$PAGE*/
/*
*********************************************************************************************************
*                   TCP DATA/TOTAL LENGTH & MAXIMUM TRANSMISSION UNIT (MTU) DEFINES
*
* Note(s) : (1) (a) TCP total length #define's (NET_TCP_TOT_LEN)  relate to the total size of a complete
*                   TCP packet, including the packet's TCP header.  Note that a complete TCP packet MAY
*                   be fragmented in multiple Internet Protocol packets.
*
*               (b) TCP data  length #define's (NET_TCP_DATA_LEN) relate to the data  size of a complete
*                   TCP packet, equal to the total TCP packet length minus its TCP header size.  Note 
*                   that a complete TCP packet MAY be fragmented in multiple Internet Protocol packets.
*
*               (c) TCP MTU #define's (NET_TCP_MTU) relate to the data size for any single, non-fragmented
*                   TCP packet, equal to a packet's Internet Protocol MTU minus its TCP header size.
*
*           (2) The 'NET_TCP_MTU_NET_RSRC' pre-processor 'else'-conditional code will never be compiled/
*               linked since 'net_buf.h' ensures that at least one of the two configuration constants 
*               (NET_BUF_CFG_NBR_SMALL or NET_BUF_CFG_NBR_LARGE) will be configured with a value greater
*               than zero (see 'net_buf.h  CONFIGURATION ERRORS').  The 'else'-conditional code is
*               included for completeness & as an extra precaution in case 'net_buf.h' is incorrectly
*               modified.
*********************************************************************************************************
*/

                                                                                /* See Notes #1a & #1b.                 */
#define  NET_TCP_DATA_LEN_MIN                              0

#define  NET_TCP_TOT_LEN_MIN                            (NET_TCP_HDR_SIZE_MIN + NET_TCP_DATA_LEN_MIN)
#define  NET_TCP_TOT_LEN_MAX                            (NET_IP_TOT_LEN_MAX   - NET_IP_HDR_SIZE_MIN )

#define  NET_TCP_DATA_LEN_MAX                           (NET_TCP_TOT_LEN_MAX  - NET_TCP_HDR_SIZE_MIN)


                                                                                /* See Note #1c.                        */
#define  NET_TCP_MTU_MIN                                (NET_IP_MTU_MIN       - NET_TCP_HDR_SIZE_MAX)
#define  NET_TCP_MTU_MAX                                (NET_IP_MTU_MAX       - NET_TCP_HDR_SIZE_MIN)
#define  NET_TCP_MTU                                     NET_TCP_MTU_MIN


#define  NET_TCP_MTU_NET_RSRC                            NET_TCP_MTU_MAX

#if     (NET_BUF_CFG_NBR_LARGE > 0)
#if    ((NET_BUF_CFG_DATA_SIZE_LARGE - NET_BUF_DATA_SIZE_MIN) < NET_TCP_MTU_NET_RSRC)
#undef   NET_TCP_MTU_NET_RSRC
#define  NET_TCP_MTU_NET_RSRC                           (NET_BUF_CFG_DATA_SIZE_LARGE - NET_BUF_DATA_SIZE_MIN)
#endif

#elif   (NET_BUF_CFG_NBR_SMALL > 0)
#if    ((NET_BUF_CFG_DATA_SIZE_SMALL - NET_BUF_DATA_SIZE_MIN) < NET_TCP_MTU_NET_RSRC)
#undef   NET_TCP_MTU_NET_RSRC
#define  NET_TCP_MTU_NET_RSRC                           (NET_BUF_CFG_DATA_SIZE_SMALL - NET_BUF_DATA_SIZE_MIN)
#endif

#else                                                                           /* See Note #2.                         */
#error  "NET_BUF_CFG_NBR_SMALL  illegally #defined in 'net_buf.h'      "
#error  "NET_BUF_CFG_NBR_LARGE  illegally #defined in 'net_buf.h'      "
#error  "                       [See 'net_buf.h  CONFIGURATION ERRORS']"
#endif


#define  NET_TCP_MTU_ACTUAL                     (DEF_MIN(NET_TCP_MTU, NET_TCP_MTU_NET_RSRC))


/*$PAGE*/
/*
*********************************************************************************************************
*                                      TCP SEGMENT SIZE DEFINES
*
* Note(s) : (1) (a) RFC # 879, Section 3 states that the TCP Maximum Segment Size "counts only
*                   data octets in the segment, ... not the TCP header or the IP header".
*
*               (b) RFC #1122, Section 4.2.2.6 requires that :
*
*                   (1) "The MSS value to be sent in an MSS option must be less than or equal to
*
*                        (A) MMS_R - 20
*
*                        where MMS_R is the maximum size for a transport-layer message that can
*                        be received."
*
*                   (2) "If an MSS option is not received at connection setup, TCP MUST assume a
*                        default send MSS of 536 (576 - 40)."
*
*                   See also 'net_ip.h  IP DATA/TOTAL LENGTH & MAXIMUM TRANSMISSION UNIT (MTU) DEFINES  
*                       Note #1'.
*********************************************************************************************************
*/

                                                                                        /* See Note #1.                 */
#define  NET_TCP_MAX_SEG_SIZE_DFLT                      (NET_IP_MAX_DATAGRAM_SIZE_DFLT - NET_IP_HDR_SIZE_MIN - NET_TCP_HDR_SIZE_MIN)
#define  NET_TCP_MAX_SEG_SIZE_DFLT_RX                    NET_TCP_MTU_NET_RSRC           /* See Note #1b1.               */
#define  NET_TCP_MAX_SEG_SIZE_DFLT_TX                    NET_TCP_MAX_SEG_SIZE_DFLT      /* See Note #1b2.               */


#define  NET_TCP_MAX_SEG_SIZE_NONE                         0
#define  NET_TCP_MAX_SEG_SIZE_MIN                        NET_TCP_MAX_SEG_SIZE_DFLT
#define  NET_TCP_MAX_SEG_SIZE_MAX                        NET_TCP_DATA_LEN_MAX


#define  NET_TCP_SEG_LEN_MIN                             NET_TCP_DATA_LEN_MIN
#define  NET_TCP_SEG_LEN_MAX                             NET_TCP_DATA_LEN_MAX

#define  NET_TCP_SEG_LEN_SYNC                              1
#define  NET_TCP_SEG_LEN_FIN                               1
#define  NET_TCP_SEG_LEN_CLOSE                           NET_TCP_SEG_LEN_FIN
#define  NET_TCP_SEG_LEN_ACK                               0
#define  NET_TCP_SEG_LEN_RESET                             0
#define  NET_TCP_SEG_LEN_PROBE                             0


#define  NET_TCP_DATA_LEN_TX_SYNC                          0
#define  NET_TCP_DATA_LEN_TX_FIN                           0
#define  NET_TCP_DATA_LEN_TX_CLOSE                       NET_TCP_DATA_LEN_TX_FIN
#define  NET_TCP_DATA_LEN_TX_ACK                           0
#define  NET_TCP_DATA_LEN_TX_PROBE_NO_DATA                 0
#define  NET_TCP_DATA_LEN_TX_PROBE_DATA                    1
#define  NET_TCP_DATA_LEN_TX_RESET                         0

#define  NET_TCP_TX_PROBE_DATA                          0x00


/*
*********************************************************************************************************
*                                       TCP WINDOW SIZE DEFINES
*
* Note(s) : (1) Although NO RFC specifies the absolute minimum TCP connection window size value allowed,
*               RFC #793, Section 3.7 'Data Communication : Managing the Window' states that for "the
*               window ... there is an assumption that this is related to the currently available data
*               buffer space available for this connection".
*********************************************************************************************************
*/

#define  NET_TCP_WIN_SIZE_NONE                             0

#define  NET_TCP_WIN_SIZE_MIN                            NET_TCP_MTU_NET_RSRC           /* See Note #1.                 */
#define  NET_TCP_WIN_SIZE_MAX                            DEF_INT_16U_MAX_VAL


/*$PAGE*/
/*
*********************************************************************************************************
*                                     TCP HEADER OPTIONS DEFINES
*
* Note(s) : (1) See the following RFC's for TCP options summary :
*
*               (a) RFC # 793, Section  3.1 'Header Format : Options'
*               (b) RFC #1122; Sections 4.2.2.5, 4.2.2.6
*
*           (2) TCP option types are coded in the first octet for each TCP option as follows :
*
*                           --------
*                           | TYPE |
*                           --------
*
*               The TCP option type value determines the TCP option format :
*
*               (a) The following TCP option types are single-octet TCP options -- i.e. the option type
*                   octet is the ONLY octet for the TCP option.
*
*                   (1) TYPE =  0   End of Options List
*                   (2) TYPE =  1   No Operation
*
*
*               (b) All other TCP options MUST be multi-octet TCP options (see RFC #1122, Section 4.2.2.5) :
*
*                           ------------------------------
*                           | TYPE | LEN  |   TCP OPT    |
*                           ------------------------------
*
*                       where 
*                               TYPE        Indicates the specific TCP option type
*                               LEN         Indicates the total    TCP option length, in octets, including 
*                                                the option type & the option length octets
*                               TCP OPT     Additional TCP option octets, if any, that contain the remaining
*                                                TCP option information
*
*                   The following TCP option types are multi-octet TCP options where the option's second
*                   octet specify the total TCP option length, in octets, including the option type & the
*                   option length octets :
*
*                   (1) TYPE =  2   Maximum Segment Size        See RFC # 793, Section  3.1 'Header Format :
*                                                                   Options : Maximum Segment Size';
*                                                                   RFC #1122, Section 4.2.2.6;
*                                                                   RFC # 879, Section 3
*
*                   (2) TYPE =  3   Window  Scale               See 'net_tcp.h  Note #1c1'
*                   (3) TYPE =  4   SACK Allowed                See 'net_tcp.h  Note #1c2'
*                   (4) TYPE =  5   SACK Option                 See 'net_tcp.h  Note #1c2'
*                   (5) TYPE =  6   Echo Request                See 'net_tcp.h  Note #1c3'
*                   (6) TYPE =  7   Echo Reply                  See 'net_tcp.h  Note #1c3'
*                   (7) TYPE =  8   Timestamp                   See 'net_tcp.h  Note #1c4'
*
*           (3) TCP header allows for a maximum option list length of 40 octets :
*
*                   NET_TCP_HDR_OPT_SIZE_MAX = NET_TCP_HDR_SIZE_MAX - NET_TCP_HDR_SIZE_MIN
*
*                                            = 60 - 20
*
*                                            = 40
*
*           (4) Data type MUST be globally declared PRIOR to certain dependent #define's.
*********************************************************************************************************
*/
/*$PAGE*/
#define  NET_TCP_HDR_OPT_END_LIST                          0
#define  NET_TCP_HDR_OPT_NOP                               1
#define  NET_TCP_HDR_OPT_MAX_SEG_SIZE                      2
#define  NET_TCP_HDR_OPT_WIN_SCALE                         3
#define  NET_TCP_HDR_OPT_SACK_PERMIT                       4
#define  NET_TCP_HDR_OPT_SACK                              5
#define  NET_TCP_HDR_OPT_ECHO_REQ                          6
#define  NET_TCP_HDR_OPT_ECHO_REPLY                        7
#define  NET_TCP_HDR_OPT_TS                                8

#define  NET_TCP_HDR_OPT_PAD                             NET_TCP_HDR_OPT_END_LIST


#define  NET_TCP_HDR_OPT_LEN_END_LIST                      1
#define  NET_TCP_HDR_OPT_LEN_NOP                           1
#define  NET_TCP_HDR_OPT_LEN_MAX_SEG_SIZE                  4
#define  NET_TCP_HDR_OPT_LEN_WIN_SCALE                     3
#define  NET_TCP_HDR_OPT_LEN_SACK_PERMIT                   2
#define  NET_TCP_HDR_OPT_LEN_ECHO_REQ                      6
#define  NET_TCP_HDR_OPT_LEN_ECHO_REPLY                    6
#define  NET_TCP_HDR_OPT_LEN_TS                           10

#define  NET_TCP_HDR_OPT_LEN_SACK_MIN                      6
#define  NET_TCP_HDR_OPT_LEN_SACK_MAX                     38

#define  NET_TCP_HDR_OPT_LEN_MIN                           1
#define  NET_TCP_HDR_OPT_LEN_MIN_LEN                       2
#define  NET_TCP_HDR_OPT_LEN_MAX                          38



typedef  CPU_INT32U  NET_TCP_OPT_SIZE;                          /* TCP opt size data type (see Note #4).                */

#define  NET_TCP_HDR_OPT_SIZE_WORD               (sizeof(NET_TCP_OPT_SIZE))
#define  NET_TCP_HDR_OPT_SIZE_MAX                       (NET_TCP_HDR_SIZE_MAX - NET_TCP_HDR_SIZE_MIN)


#define  NET_TCP_HDR_OPT_NBR_MIN                           0
#define  NET_TCP_HDR_OPT_NBR_MAX                        (NET_TCP_HDR_OPT_SIZE_MAX / NET_TCP_HDR_OPT_SIZE_WORD)


#define  NET_TCP_HDR_OPT_IX                              NET_TCP_HDR_SIZE_MIN


/*$PAGE*/
/*
*********************************************************************************************************
*                                TCP OPTION CONFIGURATION TYPE DEFINES
*
* Note(s) : (1) NET_TCP_OPT_CFG_TYPE_??? #define values specifically chosen as ASCII representations of 
*               the TCP option configuration types.  Memory displays of TCP option configuration buffers 
*               will display the TCP option configuration TYPEs with their chosen ASCII names.
*********************************************************************************************************
*/

                                                                /* ---------------- TCP OPT CFG TYPES ----------------- */
#if     (CPU_CFG_ENDIAN_TYPE == CPU_ENDIAN_TYPE_BIG)
#define  NET_TCP_OPT_CFG_TYPE_NONE                0x4E4F4E45    /* "NONE" in ASCII.                                     */
#define  NET_TCP_OPT_CFG_TYPE_MAX_SEG_SIZE        0x4D535320    /* "MSS " in ASCII.                                     */
#define  NET_TCP_OPT_CFG_TYPE_WIN_SCALE           0x57494E20    /* "WIN " in ASCII (see 'net_tcp.h  Note #1c1').        */
#define  NET_TCP_OPT_CFG_TYPE_SACK_PERMIT         0x53434B50    /* "SCKP" in ASCII (see 'net_tcp.h  Note #1c2').        */
#define  NET_TCP_OPT_CFG_TYPE_SACK                0x5341434B    /* "SACK" in ASCII (see 'net_tcp.h  Note #1c2').        */
#define  NET_TCP_OPT_CFG_TYPE_ECHO_REQ            0x45524551    /* "EREQ" in ASCII (see 'net_tcp.h  Note #1c3').        */
#define  NET_TCP_OPT_CFG_TYPE_ECHO_REPLY          0x4543484F    /* "ECHO" in ASCII (see 'net_tcp.h  Note #1c3').        */
#define  NET_TCP_OPT_CFG_TYPE_TS                  0x54532020    /* "TS  " in ASCII (see 'net_tcp.h  Note #1c4').        */

#else

#if     (CPU_CFG_DATA_SIZE   == CPU_WORD_SIZE_32)
#define  NET_TCP_OPT_CFG_TYPE_NONE                0x454E4F4E    /* "NONE" in ASCII.                                     */
#define  NET_TCP_OPT_CFG_TYPE_MAX_SEG_SIZE        0x2053534D    /* "MSS " in ASCII.                                     */
#define  NET_TCP_OPT_CFG_TYPE_WIN_SCALE           0x204E4957    /* "WIN " in ASCII (see 'net_tcp.h  Note #1c1').        */
#define  NET_TCP_OPT_CFG_TYPE_SACK_PERMIT         0x504B4353    /* "SCKP" in ASCII (see 'net_tcp.h  Note #1c2').        */
#define  NET_TCP_OPT_CFG_TYPE_SACK                0x4B434153    /* "SACK" in ASCII (see 'net_tcp.h  Note #1c2').        */
#define  NET_TCP_OPT_CFG_TYPE_ECHO_REQ            0x51455245    /* "EREQ" in ASCII (see 'net_tcp.h  Note #1c3').        */
#define  NET_TCP_OPT_CFG_TYPE_ECHO_REPLY          0x4F484345    /* "ECHO" in ASCII (see 'net_tcp.h  Note #1c3').        */
#define  NET_TCP_OPT_CFG_TYPE_TS                  0x20205354    /* "TS  " in ASCII (see 'net_tcp.h  Note #1c4').        */

#elif   (CPU_CFG_DATA_SIZE   == CPU_WORD_SIZE_16)
#define  NET_TCP_OPT_CFG_TYPE_NONE                0x4F4E454E    /* "NONE" in ASCII.                                     */
#define  NET_TCP_OPT_CFG_TYPE_MAX_SEG_SIZE        0x534D2053    /* "MSS " in ASCII.                                     */
#define  NET_TCP_OPT_CFG_TYPE_WIN_SCALE           0x4957204E    /* "WIN " in ASCII (see 'net_tcp.h  Note #1c1').        */
#define  NET_TCP_OPT_CFG_TYPE_SACK_PERMIT         0x4353504B    /* "SCKP" in ASCII (see 'net_tcp.h  Note #1c2').        */
#define  NET_TCP_OPT_CFG_TYPE_SACK                0x41534B43    /* "SACK" in ASCII (see 'net_tcp.h  Note #1c2').        */
#define  NET_TCP_OPT_CFG_TYPE_ECHO_REQ            0x52455145    /* "EREQ" in ASCII (see 'net_tcp.h  Note #1c3').        */
#define  NET_TCP_OPT_CFG_TYPE_ECHO_REPLY          0x43454F48    /* "ECHO" in ASCII (see 'net_tcp.h  Note #1c3').        */
#define  NET_TCP_OPT_CFG_TYPE_TS                  0x53542020    /* "TS  " in ASCII (see 'net_tcp.h  Note #1c4').        */

#else                                                           /* Dflt CPU_WORD_SIZE_08.                               */
#define  NET_TCP_OPT_CFG_TYPE_NONE                0x4E4F4E45    /* "NONE" in ASCII.                                     */
#define  NET_TCP_OPT_CFG_TYPE_MAX_SEG_SIZE        0x4D535320    /* "MSS " in ASCII.                                     */
#define  NET_TCP_OPT_CFG_TYPE_WIN_SCALE           0x57494E20    /* "WIN " in ASCII (see 'net_tcp.h  Note #1c1').        */
#define  NET_TCP_OPT_CFG_TYPE_SACK_PERMIT         0x53434B50    /* "SCKP" in ASCII (see 'net_tcp.h  Note #1c2').        */
#define  NET_TCP_OPT_CFG_TYPE_SACK                0x5341434B    /* "SACK" in ASCII (see 'net_tcp.h  Note #1c2').        */
#define  NET_TCP_OPT_CFG_TYPE_ECHO_REQ            0x45524551    /* "EREQ" in ASCII (see 'net_tcp.h  Note #1c3').        */
#define  NET_TCP_OPT_CFG_TYPE_ECHO_REPLY          0x4543484F    /* "ECHO" in ASCII (see 'net_tcp.h  Note #1c3').        */
#define  NET_TCP_OPT_CFG_TYPE_TS                  0x54532020    /* "TS  " in ASCII (see 'net_tcp.h  Note #1c4').        */

#endif
#endif


/*$PAGE*/
/*
*********************************************************************************************************
*                                   TCP CONNECTION TIMEOUT DEFINES
*
* Note(s) : (1) (a) (1) RFC #1122, Section 4.2.2.13 'DISCUSSION' states that "the graceful close algorithm
*                       of TCP requires that the connection state remain defined on (at least) one end of
*                       the connection, for a timeout period of 2xMSL ... During this period, the (remote 
*                       socket, local socket) pair that defines the connection is busy and cannot be reused".
*
*                   (2) The following sections reiterate that the TIME-WAIT state timeout scalar is two
*                       maximum segment lifetimes (2 MSL) :
*
*                       (A) RFC #793, Section 3.9 'Event Processing : SEGMENT ARRIVES :
*                               Check Sequence Number : TIME-WAIT STATE'
*                       (B) RFC #793, Section 3.9 'Event Processing : SEGMENT ARRIVES :
*                               Check FIN Bit         : TIME-WAIT STATE'
*
*               (b) (1) RFC #793, Section 3.3 'Sequence Numbers : Knowing When to Keep Quiet' states that
*                       "the Maximum Segment Lifetime (MSL) is ... to be 2 minutes.  This is an engineering
*                       choice, and may be changed if experience indicates it is desirable to do so".
*
*                   (2) Microsoft Corporation's Windows XP defaults MSL to 15 seconds.
*********************************************************************************************************
*/

                                                                                    /* Max seg timeout (see Note #1b) : */
#define  NET_TCP_CONN_TIMEOUT_MAX_SEG_MIN_SEC     (        0                     )  /* ... min  =  0 seconds            */
#define  NET_TCP_CONN_TIMEOUT_MAX_SEG_MAX_SEC     (  2 * DEF_TIME_NBR_SEC_PER_MIN)  /* ... max  =  2 minutes            */
#define  NET_TCP_CONN_TIMEOUT_MAX_SEG_DFLT_SEC    (       15                     )  /* ... dflt = 15 seconds            */

#define  NET_TCP_CONN_TIMEOUT_MAX_SEG_SCALAR               2                        /* ... scalar (see Note #1a).       */


#define  NET_TCP_CONN_TIMEOUT_CONN_DFLT_SEC       (120 * DEF_TIME_NBR_SEC_PER_MIN)  /* Dflt conn timeout = 120 minutes  */
#define  NET_TCP_CONN_TIMEOUT_USER_DFLT_SEC       ( 30 * DEF_TIME_NBR_SEC_PER_MIN)  /* Dflt user timeout =  30 minutes  */


/*$PAGE*/
/*
*********************************************************************************************************
*                                        TCP CONNECTION STATES
*
* Note(s) : (1) See the following RFC's for TCP state machine summary :
*
*               (a) RFC # 793; Sections 3.2, 3.4, 3.5, 3.9
*               (b) RFC #1122; Sections 4.2.2.8, 4.2.2.10, 4.2.2.11, 4.2.2.13, 4.2.2.18, 4.2.2.20
*
*           (2) (a) #### Additional closing-data-available state used for closing connections to allow the
*                   application layer to receive any remaining data.
*
*                   See also 'net_tcp.c  NetTCP_RxPktConnHandlerFinWait1()  Note #2f5A2',
*                            'net_tcp.c  NetTCP_RxPktConnHandlerFinWait2()  Note #2f5B',
*                            'net_tcp.c  NetTCP_RxPktConnHandlerClosing()   Note #2d2B2a1B',
*                          & 'net_tcp.c  NetTCP_RxPktConnHandlerLastAck()   Note #2d2A1b'.
*********************************************************************************************************
*/

#define  NET_TCP_CONN_STATE_NONE                           0
#define  NET_TCP_CONN_STATE_FREE                           1

#define  NET_TCP_CONN_STATE_CLOSED                        10

#define  NET_TCP_CONN_STATE_LISTEN                        20

#define  NET_TCP_CONN_STATE_SYNC_RXD                      30
#define  NET_TCP_CONN_STATE_SYNC_RXD_PASSIVE              31
#define  NET_TCP_CONN_STATE_SYNC_RXD_ACTIVE               32

#define  NET_TCP_CONN_STATE_SYNC_TXD                      35

#define  NET_TCP_CONN_STATE_CONN                          40

#define  NET_TCP_CONN_STATE_FIN_WAIT_1                    50
#define  NET_TCP_CONN_STATE_FIN_WAIT_2                    51
#define  NET_TCP_CONN_STATE_CLOSING                       52
#define  NET_TCP_CONN_STATE_TIME_WAIT                     53

#define  NET_TCP_CONN_STATE_CLOSE_WAIT                    55
#define  NET_TCP_CONN_STATE_LAST_ACK                      56

#define  NET_TCP_CONN_STATE_CLOSING_DATA_AVAIL            59    /* See Note #2a.                                        */


/*
*********************************************************************************************************
*                                     TCP CONNECTION QUEUE STATES
*********************************************************************************************************
*/

#define  NET_TCP_RX_Q_STATE_NONE                           0
#define  NET_TCP_RX_Q_STATE_CLOSED                       100
#define  NET_TCP_RX_Q_STATE_CONN_CLOSED                  110
#define  NET_TCP_RX_Q_STATE_CONN_CLOSING                 111
#define  NET_TCP_RX_Q_STATE_CONN_SYNC                    120
#define  NET_TCP_RX_Q_STATE_CONN                         121


#define  NET_TCP_TX_Q_STATE_NONE                           0
#define  NET_TCP_TX_Q_STATE_CLOSED                       200
#define  NET_TCP_TX_Q_STATE_CONN_CLOSED                  210
#define  NET_TCP_TX_Q_STATE_CONN_CLOSING                 211
#define  NET_TCP_TX_Q_STATE_CONN_SYNC                    220
#define  NET_TCP_TX_Q_STATE_CONN                         221
#define  NET_TCP_TX_Q_STATE_CONN_SUSPEND                 225


/*$PAGE*/
/*
*********************************************************************************************************
*                                     TCP CONNECTION CODE DEFINES
*********************************************************************************************************
*/

#define  NET_TCP_CONN_RX_SEQ_NONE                         10    /* NO        rx'd TCP conn seq.                         */
#define  NET_TCP_CONN_RX_SEQ_SYNC_INVALID                 11    /* Invalid   rx'd TCP conn sync.                        */
#define  NET_TCP_CONN_RX_SEQ_SYNC                         12    /*   Valid   rx'd TCP conn sync.                        */
#define  NET_TCP_CONN_RX_SEQ_INVALID                      15    /* Invalid   rx'd TCP conn seq.                         */
#define  NET_TCP_CONN_RX_SEQ_VALID                        16    /*   Valid   rx'd TCP conn seq.                         */

#define  NET_TCP_CONN_RX_ACK_NONE                         20    /* NO        rx'd TCP conn ack.                         */
#define  NET_TCP_CONN_RX_ACK_INVALID                      21    /* Invalid   rx'd TCP conn ack.                         */
#define  NET_TCP_CONN_RX_ACK_VALID                        22    /*   Valid   rx'd TCP conn ack.                         */
#define  NET_TCP_CONN_RX_ACK_DUP                          23    /* Duplicate rx'd TCP conn ack.                         */
#define  NET_TCP_CONN_RX_ACK_PREV                         24    /* Prev'ly   rx'd TCP conn ack.                         */

#define  NET_TCP_CONN_RX_RESET_NONE                       30    /* NO        rx'd TCP conn reset.                       */
#define  NET_TCP_CONN_RX_RESET_INVALID                    31    /* Invalid   rx'd TCP conn reset.                       */
#define  NET_TCP_CONN_RX_RESET_VALID                      32    /*   Valid   rx'd TCP conn reset.                       */

#define  NET_TCP_CONN_RX_WIN_RESET                        50    /* Reset cur rx win size.                               */
#define  NET_TCP_CONN_RX_WIN_SET                          51    /* Set   cur rx win size.                               */
#define  NET_TCP_CONN_RX_WIN_INC                          52    /* Inc   cur rx win size.                               */
#define  NET_TCP_CONN_RX_WIN_DEC                          53    /* Dec   cur rx win size.                               */


#define  NET_TCP_CONN_TX_ACK_NONE                        120    /* Do NOT tx TCP conn ack.                              */
#define  NET_TCP_CONN_TX_ACK                             121    /*        Tx TCP conn ack.                              */
#define  NET_TCP_CONN_TX_ACK_IMMED                       122    /*        Tx TCP conn ack immed'ly.                     */
#define  NET_TCP_CONN_TX_ACK_FAULT                       123    /*        Tx TCP conn ack on fault.                     */
#define  NET_TCP_CONN_TX_ACK_TIMEOUT                     124    /*        Tx TCP conn ack on timeout.                   */

#define  NET_TCP_CONN_TX_RESET_NONE                      130    /* Do NOT tx TCP conn reset.                            */
#define  NET_TCP_CONN_TX_RESET                           131    /*        Tx TCP conn reset.                            */
#define  NET_TCP_CONN_TX_RESET_FAULT                     133    /*        Tx TCP conn reset on fault.                   */

#define  NET_TCP_CONN_TX_WIN_RESET                       150    /* Reset   cur tx        win size.                      */
#define  NET_TCP_CONN_TX_WIN_SET                         151    /* Set     cur tx        win size.                      */
#define  NET_TCP_CONN_TX_WIN_INC                         152    /* Inc     cur tx        win size.                      */
#define  NET_TCP_CONN_TX_WIN_DEC                         153    /* Dec     cur tx        win size.                      */

#define  NET_TCP_CONN_TX_WIN_SEG_RXD                     155    /* Update  cur tx        win size based on rx'd seg.    */
#define  NET_TCP_CONN_TX_WIN_TIMEOUT                     156    /* Timeout cur tx        win size.                      */
#define  NET_TCP_CONN_TX_WIN_REMOTE_UPDATE               157    /* Update  cur tx remote win size.                      */

#define  NET_TCP_CONN_TX_WIN_CONG_INC_SLOW_START         160    /* Inc         tx cong   win size based on slow start.  */
#define  NET_TCP_CONN_TX_WIN_CONG_INC_CONG_AVOID         161    /* Inc         tx cong   win size based on cong avoid.  */
#define  NET_TCP_CONN_TX_WIN_CONG_INC_REM                162    /* Inc     rem tx cong   win size.                      */
#define  NET_TCP_CONN_TX_WIN_CONG_SET_SLOW_START         165    /* Set         tx cong   win size to slow start    th.  */
#define  NET_TCP_CONN_TX_WIN_CONG_SET_FAST_RECOVERY      166    /* Set         tx cong   win size to fast recovery th.  */
#define  NET_TCP_CONN_TX_WIN_CONG_SET_TIMEOUT            167    /* Set         tx cong   win size to timeout       th.  */

#define  NET_TCP_CONN_TX_RTT_INIT                        170    /* Init     tx RTT ctrls.                               */
#define  NET_TCP_CONN_TX_RTT_RESET                       171    /* Reset    tx RTT ctrls.                               */
#define  NET_TCP_CONN_TX_RTT_CALC                        175    /* Calc     tx RTT calcs.                               */
#define  NET_TCP_CONN_TX_RTT_BACKOFF                     176    /* Back-off tx RTT ctrls.                               */


/*$PAGE*/
/*
*********************************************************************************************************
*                                   TCP CONGESTION CONTROL DEFINES
*
* Note(s) : (1) See the following RFC's for TCP Congestion Control summary :
*
*               (a) (1) RFC #2001                       TCP Congestion Control
*                   (2) RFC #2581                           Slow Start                      See Note #2
*                                                           Congestion Avoidance            See Note #3
*                                                           Fast Re-transmit                See Note #4
*                                                           Fast Recovery                   See Note #4
*
*               (b) (1) RFC # 813                       Silly Window Syndrome (SWS)         See Note #5
*                   (2) RFC #1122
*                       (A) Section 4.2.2.14
*                       (B) Section 4.2.3.3             Receive  SWS (RSWS)                 See Note #5a
*                       (C) Section 4.2.3.4             Transmit SWS (TSWS)                 See Note #5b
*
*               (c) (1) RFC # 896                       Nagle's Algorithm
*                   (2) RFC #1122
*                       (A) Section 4.2.2.14
*                       (B) Section 4.2.3.4
*
*               (d) (1) RFC # 813, Section 5            Delayed TCP Acknowledgments         See Note #6
*                   (2) RFC #1122, Section 4.2.3.2
*                   (3) RFC #2581, Section 4.2
*
*               (e) RFC #1122
*                   (1) Section 4.2.2.2                 TCP Segment PUSH
*                   (2) Section 4.2.2.17                Probing Zero Windows /
*                                                       TCP  Persist Timer
*                   (3) Section 4.2.3.5                 TCP  Connection Failures            See  Note #7
*                   (4) Section 4.2.3.9                 ICMP Error Message   /              See 'net_tcp.h
*                                                       TCP  Congestion Recovery                 Note #1e'
*
*
*           (2) (a) RFC #2581, Section 3.1 states that "the initial value of ssthresh [slow start threshold]
*                   MAY be arbitrarily high (for example, some implementations use the size of the advertised
*                   window)".
*
*                   (1) This amends RFC #2001, Section 2.1 which previously stated that "initialization for
*                       a given connection sets ... ssthresh to 65535 bytes".
*
*               (b) RFC #2581, Section 3.1, states that "when a TCP sender detects segment loss using the
*                   retransmission timer, the value of ssthresh MUST be set to no more than" :
*
*                       (3) ssthresh  =  max (FlightSize / 2, 2 * SMSS)
*
*                               where
*                                   (A) FlightSize          Amount of outstanding data in the network
*                                   (B) SMSS                Sender Maximum Segment Size -- the size of the
*                                                               largest segment that the sender can transmit
*
*               (c) 'SLOW_START_TH' (i.e. 'Slow Start Threshold') abbreviated to 'SST' for some TCP control
*                    constants to enforce ANSI-compliance of 31-character symbol length uniqueness.
*
*               See also 'net_tcp.c  NetTCP_TxConnWinSizeHandlerCongCtrl()  Notes #2c1A1b & #2c2A1b'.
*
*           (3) RFC #2581, Section 3.1 states that :
*
*               (a) "The initial value of cwnd [congestion window], MUST be less than or equal to 2*SMSS 
*                    bytes and MUST NOT be more than 2 segments."
*
*                   (1) This amends RFC #2001, Section 2.1 which previously stated that "initialization
*                       for a given connection sets cwnd to one segment".
*
*               (b) "During congestion avoidance, cwnd MUST NOT be increased by more than ... 1 full-sized
*                    segment per RTT [TCP segment round-trip time]."  
*
*               (c) "Upon a timeout cwnd MUST be set to no more than ... 1 full-sized segment."
*
*               See also 'net_tcp.c  NetTCP_TxConnWinSizeHandlerCongCtrl()  Notes #2c1A1a, #2c1A4b2,
*                                                                                 #2c2A1a, & #2c2A4'.
*
*           (4) RFC #2581, Section 3.2 states that :
*
*               (a) "The fast retransmit algorithm uses the arrival of 3 duplicate ACKs ... as an indication
*                    ... [to perform] a retransmission".
*
*               (b) "When the third duplicate ACK is received" :
*
*                   (1) "Set ssthresh to no more than the value given in equation 3" (see Note #2b).
*
*                   (2) "Set cwnd to ssthresh plus 3*SMSS.  This artificially 'inflates' the congestion
*                        window by the number of segments (three) that have left the network."
*
*               (c) "For each additional duplicate ACK received, increment cwnd by SMSS."
*
*               See also 'net_tcp.c  NetTCP_TxConnWinSizeHandlerCongCtrl()  Notes #2c1B & #2c2B'.
*$PAGE*
*           (5) (a) RFC #1122, Section 4.2.3.3 states that "the suggested SWS avoidance algorithm for the 
*                   receiver is ... to avoid advancing the right window edge RCV.NXT+RCV.WND ... until the
*                   reduction satisfies" :
*
*                       (1) RCV.BUFF - RCV.USER - RCV.WND  >=  min(Fr * RCV.BUFF, Eff.snd.MSS)
*
*                               where
*                                   (A) RCV.BUFF        Total receive buffer space
*                                   (B) RCV.USER        Data  received but not yet consumed
*                                   (C) RCV.WND         Space advertised to sender
*                                   (D) Fr              Fraction whose recommended value is 1/2
*                                   (E) Eff.snd.MSS     Effective send MSS for the connection
*
*
*                   See also 'net_tcp.c  NetTCP_RxConnWinSizeCfgUpdateTh()  Note #1'.
*                          & 'net_tcp.c  NetTCP_RxConnWinSizeHandler()      Note #3'.
*
*               (b) RFC #1122, Section 4.2.3.4 states that "the sender's SWS avoidance algorithm is ... 
*                   [to] send data" :
*
*                   (3) "If at least a fraction Fs of the maximum window can be sent ... Fs is a fraction
*                        whose recommended value is 1/2."
*
*                   (4) "If data is PUSHed and the override timeout occurs ... The override timeout should
*                        be in the range 0.1 - 1.0 seconds."
*
*                   See also 'net_tcp.c  NetTCP_TxConnTxQ()  Note #7b2'.
*
*           (6) The following sections state that "a TCP SHOULD implement a delayed ACK" :
*
*                       (A) RFC # 813, Section 5
*                       (B) RFC #1122, Section 4.2.3.2
*                       (C) RFC #2581, Section 4.2
*
*               (a) (1) (A) RFC #1122, Section 4.2.3.2 states that "in a stream of full-sized segments 
*                           there SHOULD be an ACK for at least every second segment".
*
*                       (B) RFC #2581, Section 4.2 reiterates that "an ACK SHOULD be generated for at 
*                           least every second full-sized segment".
*
*                   (2) However, RFC #2581, Section 4.2 states that "an implementation is deemed to
*                       comply with this requirement ... by acknowledging at least every second segment,
*                       regardless of size".
*
*               (b) (1) RFC #1122, Section 4.2.3.2 states that "an ACK should not be excessively delayed;
*                       in particular, the delay MUST be less than 0.5 seconds".
*
*                   (2) RFC #2581, Section 4.2 reiterates that "an ACK ... MUST be generated within 500 ms
*                       of the arrival of the first unacknowledged packet".
*
*               See also 'net_tcp.c  NetTCP_TxConnAck()  Note #6'.
*$PAGE*
*           (7) RFC #1122, Section 4.2.3.5 states that "excessive retransmission of the same segment by TCP
*               indicates some failure of the remote host or the Internet path".
*
*               (a) "The following procedure MUST be used to handle excessive retransmissions of data segments" :
*
*                   (a) "There are ... thresholds ... measuring the amount of retransmission that has occurred
*                        for the same segment ... measured in time units or as a count of retransmissions."
*
*                   (c) "When the number of transmissions of the same segment reaches a threshold ... close the
*                        connection."
*
*                   (d) "An application MUST be able to set the [threshold] value ... for a particular connection.
*                        For example, an interactive application might set [the threshold value] to 'infinity'."
*
*                   See also 'net_tcp.c  NetTCP_TxConnReTxQ()  Note #3'.
*
*               (b) (1) (A) "The value of ... [the threshold] SHOULD correspond to at least 100 seconds."
*
*                       (B) Therefore, the minimum threshold value is calculated as follows :
*
*                                                [                                       ]         Minimum
*                           (1)    Summation     [ Exponential * (Exponential ^ (N - 1)) ]  >=  Retransmission
*                               i = 1 --> i = N  [   Scalar          Base                ]        Threshold
*
*                                  Summation     [ 3 * (2 ^ (N - 1))]  >=  100 seconds
*                               i = 1 --> i = N
*
*                                                                   N  >=  5.06
*
*                           (2)                                     N   =  6
*
*                                   where
*                                           N                           Minimum Excessive Retransmission Threshold
*                                                                           (in number of retransmissions)
*                                           Exponential Scalar          Exponential Scalar = 3 (see 'TCP ROUND-TRIP
*                                                                           TIME (RTT) / RE-TRANSMIT TIMEOUT (RTO) 
*                                                                           DEFINES  Note #3a1A1b')
*                                           Exponential Base            Exponential Base   = 2 (see 'TCP ROUND-TRIP
*                                                                           TIME (RTT) / RE-TRANSMIT TIMEOUT (RTO) 
*                                                                           DEFINES  Note #3b2')
*                                           Minimum Retransmission      Minimum Excessive Retransmission Threshold 
*                                                 Threshold                 (in seconds; see Note #7b1A)
*
*                   (2) (A) Stevens, TCP/IP Illustrated, Volume 1, 8th Printing, Section 21.2, Page 299 
*                           states that "on most implementations this total timeout value is not tunable
*                           ... and its default is ... the more common 9 minutes".
*
*                       (B) Therefore, the default threshold value is calculated as follows :
*
*                           (1)   [                                                                      ]
*                                 [ Exponential * (Exponential ^ (M - 1))  ,  when < Maximum Exponential ]
*                                 [   Scalar          Base                              Timeout Value    ]          Common
*                   Summation     [                                                                      ]  >=  Retransmission
*                i = 1 --> i = M  [ Maximum Exponential                                                  ]        Threshold
*                                 [    Timeout Value                       ,  otherwise                  ]
*                                 [                                                                      ]
*
*                                 [                                          ]
*                                 [  3 * (2 ^ (M - 1))  ,  when < 64 seconds ]
*                   Summation     [                                          ]  >=  9 minutes
*                i = 1 --> i = M  [ 64 seconds          ,  otherwise         ]
*                                 [                                          ]
*
*                                                                            M  >=  11.98
*
*                           (2)                                              M   =  12
*
*                                   where
*                                           M                           Default Excessive Retransmission Threshold
*                                                                           (in number of retransmissions)
*                                           Exponential Scalar          Exponential Scalar = 3 (see 'TCP ROUND-TRIP
*                                                                           TIME (RTT) / RE-TRANSMIT TIMEOUT (RTO) 
*                                                                           DEFINES  Note #3a1A1b')
*                                           Exponential Base            Exponential Base   = 2 (see 'TCP ROUND-TRIP
*                                                                           TIME (RTT) / RE-TRANSMIT TIMEOUT (RTO) 
*                                                                           DEFINES  Note #3b2')
*                                           Maximum Exponential         Maximum Exponential Timeout Value (see 
*                                              Timeout Value                'TCP ROUND-TRIP TIME (RTT) / RE-TRANSMIT 
*                                                                           TIMEOUT (RTO) DEFINES  Note #3b1B')
*                                           Common Retransmission       Common Default Retransmission Threshold 
*                                                Threshold                  (in minutes; see Note #7b2A)
*********************************************************************************************************
*/
/*$PAGE*/
                                                                                /* See Note #2b3.                       */
#define  NET_TCP_SST_UNACKD_DATA_NUMER                     1
#define  NET_TCP_SST_UNACKD_DATA_DENOM                     2

#define  NET_TCP_SST_MSS_SCALAR                            2



#define  NET_TCP_CONG_WIN_MSS_SCALAR_INIT                  2                    /* See Note  #3a.                       */
#define  NET_TCP_CONG_WIN_MSS_SCALAR_INC                   1                    /* See Notes #3b & #4c.                 */
#define  NET_TCP_CONG_WIN_MSS_SCALAR_TIMEOUT               1                    /* See Note  #3c.                       */



#define  NET_TCP_FAST_RE_TX_ACK_DUP_TH                     3                    /* See Note #4a.                        */
                                                                                /* See Note #4b2.                       */
#define  NET_TCP_FAST_RECOVERY_MSS_SCALAR                NET_TCP_FAST_RE_TX_ACK_DUP_TH  


                                                                                /* See Note #5a1D.                      */
#define  NET_TCP_RX_SILLY_WIN_NUMER                        1
#define  NET_TCP_RX_SILLY_WIN_DENOM                        2

                                                                                /* See Note #5b3.                       */
#define  NET_TCP_TX_SILLY_WIN_NUMER                        1
#define  NET_TCP_TX_SILLY_WIN_DENOM                        2
                                                                                /* See Note #5b4.                       */
#define  NET_TCP_TX_SILLY_WIN_TIMEOUT_MIN_MS             100
#define  NET_TCP_TX_SILLY_WIN_TIMEOUT_MAX_MS            1000
#define  NET_TCP_TX_SILLY_WIN_TIMEOUT_DFLT_MS            NET_TCP_TX_SILLY_WIN_TIMEOUT_MAX_MS



#define  NET_TCP_ACK_DLY_CNT_TH                            2                    /* See Note #6a.                        */

#define  NET_TCP_ACK_DLY_TIME_MIN_MS                       0            
#define  NET_TCP_ACK_DLY_TIME_MAX_MS                     500                    /* See Note #6b.                        */


#define  NET_TCP_RE_TX_TH_MIN                              6                    /* See Note #7b1B2.                     */
#define  NET_TCP_RE_TX_TH_MAX      (DEF_INT_16U_MAX_VAL -  1)                   /* See Note #7ad.                       */
#define  NET_TCP_RE_TX_TH_DFLT                            12                    /* See Note #7b2B2.                     */


/*$PAGE*/
/*
*********************************************************************************************************
*                    TCP ROUND-TRIP TIME (RTT) / RE-TRANSMIT TIMEOUT (RTO) DEFINES
*
* Note(s) : (1) See the following RFC's for TCP Round-Trip Times (RTT) & Re-transmit Timeout (RTO) summary :
*
*               (a) RFC #2988                               TCP Round-Trip Time Calculations (RTT) /
*               (b) RFC #1122                               TCP Retransmission Timeout       (RTO)
*                   (1) Section 4.2.2.15
*                   (2) Section 4.2.3.1
*               (c) RFC # 793, Section 3.7 'Retransmission Timeout'
*               (d) Jacobson/Karels, "Congestion Avoidance and Control"
*               (e) Karn/Partridge,  "Improving Round-Trip Time Estimates in Reliable Transport Protocols"
*
*
*           (2) (a) RFC #793, Section 3.7 'Data Communication : Retransmission Timeout' states that 
*                   "because of the variability of the networks that compose an internetwork system 
*                   and the wide range of uses of TCP connections the retransmission timeout must be 
*                   dynamically determined.  One procedure for determining a retransmission time out 
*                   is given here as an illustration".
*
*               (b) However, RFC #1122, Section 4.2.2.15 states that "the algorithm suggested in 
*                   RFC-793 for calculating the retransmission timeout is now known to be inadequate"
*                   & Section 4.2.3.1 states that "a host TCP MUST implement Karn's algorithm and
*                   Jacobson's algorithm for computing the retransmission timeout".
*
*               (c) Further, RFC #2988, Section 1 states that "this document codifies the algorithm 
*                   for setting the RTO ... expands on the discussion in section 4.2.3.1 of RFC 1122
*                    ... [but] does not alter the ... retransmission ... behavior outlined in RFC 2581".
*
*           (3) (a) (1) (A) (1) RFC #1122, Section 4.2.3.1 states that "the following values SHOULD be
*                               used to initialize the estimation parameters for a new connection" :
*
*                                   (a) RTT = 0 seconds
*                                   (b) RTO = 3 seconds
*
*                                           where
*                                                   RTO                 Retransmission Timeout
*                                                   RTT                 Round-Trip Time
*
*                               (A) RFC #2988, Section 2.1 reiterates that "until a round-trip time (RTT) 
*                                   measurement has been made ... the sender SHOULD set RTO <- 3 seconds".
*
*                               (B) Furthermore, RFC #1122, Section 4.2.3.1.(b) states that "the smoothed 
*                                   variance is to be initialized to the value that will result in" these
*                                   values.
*
*                                   Since computing the RTT retransmit timer      is given by the following 
*                                   equation (see RFC #2988, Section 2.3) :
*
*                                       (1) RTO     =  RTT_Avg + (RTT_RTO_Gain * RTT_Dev)
*
*                                   then the RTT deviation, or smoothed variance, is given by the following
*                                   equation :
*
*                                       (2) RTT_Dev = (RTO - RTT_Avg) / RTT_RTO_Gain
*
*                                               where
*                                                       RTT_Avg                 RTT Average
*                                                       RTT_Dev                 RTT Deviation
*                                                       RTT_RTO_Gain            RTT-RTO Gain
*
*
*                           (2) However, since RFC #2988, Section 2.2 amends the RFC #1122, Section 
*                               4.2.3.1 RTT initialization; the smoothed RTT average & deviation do
*                               NOT truly require explicit initialization.  Nonetheless, these RTT
*                               values are initialized to conform with RFC #1122, Section 4.2.3.1.
*
*                           See also 'net_tcp.c  NetTCP_TxConnRTT_Init()  Note #1'
*                                  & 'net_tcp.c  NetTCP_TxConnRTO_Init()  Note #2'.
*$PAGE*
*                       (B) RFC #2988, Section 2 states that "the rules governing the computation of SRTT
*                           (smoothed round-trip time), RTTVAR RTTVAR (round-trip time variation), and RTO
*                           are as follows" :
*
*                           (a) RFC #2988, Section 2.2 states that for "the first RTT measurement R ...
*                               the host MUST set" :
*
*                                   (1) SRTT    <-  R
*                                   (2) RTTVAR  <-  R/2
*                                   (3) RTO     <-  SRTT  +  max(G, K * RTTVAR)
*
*                                            where
*                                                   SRTT                    RTT Smoothed Average
*                                                   RTTVAR                  RTT Variance/Deviation
*                                                   RTO                         Retransmission Timeout
*                                                   R                       RTT First Measurement
*                                   (4)             R   = R * 1             RTT First Average   Gain
*                                   (5)             R/2 = R * 1/2           RTT First Deviation Gain
*                                                   G                       RTT Clock Granularity (resolution)
*                                   (6)             K = 4                   RTT-RTO Gain
*
*
*                           (b) RFC #2988, Section 2.3 states that for "subsequent RTT measurement R' ...
*                               a host MUST set" :
*
*                                   (1) RTTVAR  <-  (1 - beta ) * RTTVAR  +  beta  * |SRTT - R'|
*                                   (2) SRTT    <-  (1 - alpha) * SRTT    +  alpha * R'
*                                   (3) RTO     <-  SRTT  +  max(G, K * RTTVAR)
*
*                                            where
*                                                   SRTT                    RTT Smoothed Average
*                                                   RTTVAR                  RTT Variance/Deviation
*                                                   RTO                         Retransmission Timeout
*                                                   R'                      RTT Subsequent Measurement(s)
*                                   (4)             alpha = 1/8             RTT-Average   Gain
*                                   (5)             beta  = 1/4             RTT-Deviation Gain
*                                                   G                       RTT Clock Granularity (resolution)
*                                   (6)             K = 4                   RTT-RTO Gain
*
*
*                           See also 'net_tcp.c  NetTCP_TxConnRTT_RTO_Calc()  Note #2a1'.
*
*                   (2) (A) Jacobson/Karels, "Congestion Avoidance and Control", Appendix A.2 states that RTT
*                           calculations "should be done in integer arithmetic".  RFC #2988, Section 2.3 adds
*                           that RTT calculations "SHOULD be computed using ... 1/8 and ... 1/4" gains (see
*                           Notes #3a1Bb4 & #3a1Bb5).
*
*                       (B) (a) Therefore, the RTT integer calculations SHOULD be scaled by 8, the least 
*                               common denominator for the RTT fractional gains.
*
*                           (b) However, ONLY the RTT measurements, average, & deviation are scaled.  RTT
*                               gains MUST NOT be scaled since scaling the multiplicative gains incorrectly
*                               exponentiates the RTT calculations :
*
*                                   (1) (RTT_Gain * RTT)  =  (RTT_Gain * RTT) * (RTT_Scale / RTT_Scale)
*
*                                   (2)                   =  (RTT_Gain * RTT  * RTT_Scale) / RTT_Scale
*
*                                   (3)                   =  [RTT_Gain * (RTT * RTT_Scale)] / RTT_Scale
*
*                                   (4)                  !=  [(RTT_Gain * RTT_Scale) * (RTT * RTT_Scale)] / RTT_Scale
*
*
*                       See also 'net_tcp.c  NetTCP_TxConnRTT_RTO_Calc()  Note #2a3'.
*$PAGE*
*               (b) (1) (A) (1) RFC #2988, Section 2.4 states that "whenever RTO is computed, if it is 
*                               less than 1 second then the RTO SHOULD be rounded up to 1 second".
*
*                               (a) This amends RFC #1122, Section 4.2.3.1 which previously stated that 
*                                   "the recommended ... RTO ... lower bound ... SHOULD be measured in 
*                                   fractions of a second".
*
*                           (2) RFC #2988, Section 4 states that "there is no requirement for the clock
*                               granularity G used for computing RTT measurements ... However, if the
*                               K*RTTVAR term in the RTO calculation equals zero, the variance term MUST
*                               be rounded to G seconds".
*
*                               See also Notes #3a1Ba3 & #3a1Bb3.
*
*                           See also 'net_tcp.c  NetTCP_TxConnRTT_RTO_Calc()  Note #2b1A'.
*
*                       (B) (1) RFC #2988, Section 2.4 adds that "a maximum value MAY be placed on RTO
*                               provided it is at least 60 seconds".
*
*                           (2) RFC #1122, Section 4.2.3.1 states that "the recommended ... RTO ... upper
*                               bound should be 2*MSL".
*
*                           (3) Stevens, TCP/IP Illustrated, Volume 1, 8th Printing, Section 21.2, Page 299
*                               states that "the timeout value is doubled for each retransmission, with an
*                               upper limit of 64 seconds".
*
*                           See also 'net_tcp.c  NetTCP_TxConnRTO_CfgMaxTimeout()  Note #1',
*                                    'net_tcp.c  NetTCP_TxConnRTO_CalcBackOff()    Note #1b',
*                                  & 'net_tcp.c  NetTCP_TxConnRTT_RTO_Calc()       Note #2b1B'.
*
*                   (2) RFC #1122, Section 4.2.3.1 states that an "implementation MUST also include 
*                       'exponential backoff' for successive RTO values for the same segment".
*
*                       (A) (1) RFC #2988, Section 5.5 states that "when the retransmission timer expires
*                               ... the host MUST set RTO <- RTO * 2 ('back off the timer')".
*
*                           (2) Stevens, TCP/IP Illustrated, Volume 1, 8th Printing, Section 21.2, Page 299
*                               reiterates that "this doubling is called an 'exponential backoff'".
*
*                       (B) Thus, the TCP retransmission timer exponential back-off scalar value is 2.
*
*                       See also 'net_tcp.c  NetTCP_TxConnRTO_CalcBackOff()  Note #1a'.
*********************************************************************************************************
*/
/*$PAGE*/
#define  NET_TCP_TX_RTT_AVG_INIT_SEC                       0    /* RTT avg init = 0 sec     (see Note #3a1A1a).         */
#define  NET_TCP_TX_RTT_RTO_INIT_SEC                       3    /* RTT RTO init = 3 sec     (see Note #3a1A1b).         */


                                                                /* RTT avg init gain = 1    (see Note #3a1Ba4).         */
#define  NET_TCP_TX_RTT_GAIN_AVG_INIT                      1
#define  NET_TCP_TX_RTT_GAIN_AVG_INIT_NUMER                1
#define  NET_TCP_TX_RTT_GAIN_AVG_INIT_DENOM                1
                                                                /* RTT dev init gain = 1/2  (see Note #3a1Ba5).         */
#define  NET_TCP_TX_RTT_GAIN_DEV_INIT_NUMER                1
#define  NET_TCP_TX_RTT_GAIN_DEV_INIT_DENOM                2

                                                                /* RTT avg      gain = 1/8  (see Note #3a1Bb4).         */
#define  NET_TCP_TX_RTT_GAIN_AVG_NUMER                     1
#define  NET_TCP_TX_RTT_GAIN_AVG_DENOM                     8
                                                                /* RTT dev      gain = 1/4  (see Note #3a1Bb5).         */
#define  NET_TCP_TX_RTT_GAIN_DEV_NUMER                     1
#define  NET_TCP_TX_RTT_GAIN_DEV_DENOM                     4
                                                                /* RTT dev RTO  gain = 4    (see Note #3a1Bb6).         */
#define  NET_TCP_TX_RTT_GAIN_RTO                           4
#define  NET_TCP_TX_RTT_GAIN_RTO_NUMER                     4
#define  NET_TCP_TX_RTT_GAIN_RTO_DENOM                     1



#define  NET_TCP_TX_RTT_SCALE                              8    /* RTT vals int-scaled-by-8 (see Note #3a2Ba).          */
#define  NET_TCP_TX_RTT_MS_SCALE                        (DEF_TIME_NBR_mS_PER_SEC     * NET_TCP_TX_RTT_SCALE)

#define  NET_TCP_TX_RTT_AVG_INIT_SEC_SCALED             (NET_TCP_TX_RTT_AVG_INIT_SEC * NET_TCP_TX_RTT_SCALE)
#define  NET_TCP_TX_RTT_RTO_INIT_SEC_SCALED             (NET_TCP_TX_RTT_RTO_INIT_SEC * NET_TCP_TX_RTT_SCALE)

                                                                /* RTT dev init scaled      (see Note #3a1A1B2).        */
#define  NET_TCP_TX_RTT_DEV_INIT_SEC_SCALED           (((NET_TCP_TX_RTT_RTO_INIT_SEC - NET_TCP_TX_RTT_AVG_INIT_SEC) * NET_TCP_TX_RTT_SCALE) \
                                                                                                                    / NET_TCP_TX_RTT_GAIN_RTO)


#define  NET_TCP_TX_RTT_AVG_INIT_MS_SCALED              (NET_TCP_TX_RTT_AVG_INIT_SEC_SCALED * DEF_TIME_NBR_mS_PER_SEC)
#define  NET_TCP_TX_RTT_DEV_INIT_MS_SCALED              (NET_TCP_TX_RTT_DEV_INIT_SEC_SCALED * DEF_TIME_NBR_mS_PER_SEC)
#define  NET_TCP_TX_RTT_RTO_INIT_MS_SCALED              (NET_TCP_TX_RTT_RTO_INIT_SEC_SCALED * DEF_TIME_NBR_mS_PER_SEC)



#define  NET_TCP_TX_RTT_NONE                               0

#define  NET_TCP_TX_RTT_TS_NONE                            0L
#define  NET_TCP_TX_RTT_TS_MIN                           DEF_INT_32U_MIN_VAL
#define  NET_TCP_TX_RTT_TS_MAX                           DEF_INT_32U_MAX_VAL

                                                                /* RTT clk resolution (see Note #3b1A2).                */
#if     (NET_TMR_TASK_PERIOD_mS > 0)
#define  NET_TCP_TX_RTT_TS_CLK_MS                        NET_TMR_TASK_PERIOD_mS
#else
#define  NET_TCP_TX_RTT_TS_CLK_MS                        NET_TMR_TIME_TICK
#endif
#define  NET_TCP_TX_RTT_TS_CLK_MS_SCALED                (NET_TCP_TX_RTT_TS_CLK_MS * NET_TCP_TX_RTT_SCALE)



#define  NET_TCP_TX_RTO_NONE                               0

#define  NET_TCP_TX_RTO_MIN_TIMEOUT_SEC                    1    /* RTO min timeout      =  1 sec   (see Note #3b1A1).   */
#define  NET_TCP_TX_RTO_MIN_TIMEOUT_MS_SCALED           (NET_TCP_TX_RTO_MIN_TIMEOUT_SEC * NET_TCP_TX_RTT_MS_SCALE)


#define  NET_TCP_TX_RTO_MAX_TIMEOUT_MIN_SEC               60    /* RTO max timeout min  = 60 sec   (see Note #3b1B1).   */
                                                                /* RTO max timeout max  =  2 MSL   (see Note #3b1B2).   */
#define  NET_TCP_TX_RTO_MAX_TIMEOUT_MAX_SEC              NET_TCP_CONN_TIMEOUT_MAX_SEG_MAX_SEC
#define  NET_TCP_TX_RTO_MAX_TIMEOUT_DFLT_SEC              64    /* RTO max timeout dflt = 64 sec   (see Note #3b1B3).   */


#define  NET_TCP_TX_RTO_TIMEOUT_BACKOFF_SCALAR             2    /* RTO exponential back-off scalar (see Note #3b2B).    */



#define  NET_TCP_TX_RTT_RTO_STATE_NONE                     0
#define  NET_TCP_TX_RTT_RTO_STATE_INIT                    10
#define  NET_TCP_TX_RTT_RTO_STATE_RESET                   11
#define  NET_TCP_TX_RTT_RTO_STATE_CALC                    20
#define  NET_TCP_TX_RTT_RTO_STATE_RE_TX                   30


/*$PAGE*/
/*
*********************************************************************************************************
*                                             DATA TYPES
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*                                      TCP PORT NUMBER DATA TYPE
*********************************************************************************************************
*/

typedef  NET_PORT_NBR       NET_TCP_PORT_NBR;                   /* Defines TCP port nbr size.                           */


/*
*********************************************************************************************************
*                                    TCP SEQUENCE NUMBER DATA TYPE
*
* Note(s) : (1) 'NET_TCP_SEQ_NBR'  pre-defined in 'net_type.h' PRIOR to all other network modules that 
*                require TCP sequence number data type(s).
*********************************************************************************************************
*/

#if 0                                                           /* See Note #1.                                         */
typedef  CPU_INT32U         NET_TCP_SEQ_NBR;
#endif


/*
*********************************************************************************************************
*                                     TCP SEGMENT SIZE DATA TYPE
*
* Note(s) : (1) 'NET_TCP_SEG_SIZE' pre-defined in 'net_type.h' PRIOR to all other network modules that 
*                require TCP segment size data type(s).
*********************************************************************************************************
*/

#if 0                                                           /* See Note #1.                                         */
typedef  CPU_INT16U         NET_TCP_SEG_SIZE;
#endif


/*
*********************************************************************************************************
*                                      TCP WINDOW SIZE DATA TYPE
*
* Note(s) : (1) 'NET_TCP_WIN_SIZE' pre-defined in 'net_type.h' PRIOR to all other network modules that 
*                require TCP window size data type(s).
*********************************************************************************************************
*/

#if 0                                                           /* See Note #1.                                         */
typedef  CPU_INT16U         NET_TCP_WIN_SIZE;
#endif


/*$PAGE*/
/*
*********************************************************************************************************
*                                   TCP CONNECTION STATE DATA TYPE
*********************************************************************************************************
*/

typedef  CPU_INT08U         NET_TCP_CONN_STATE;


/*
*********************************************************************************************************
*                                  TCP RECEIVE QUEUE STATE DATA TYPE
*********************************************************************************************************
*/

typedef  CPU_INT08U         NET_TCP_RX_Q_STATE;


/*
*********************************************************************************************************
*                                 TCP TRANSMIT QUEUE STATE DATA TYPE
*********************************************************************************************************
*/

typedef  CPU_INT08U         NET_TCP_TX_Q_STATE;


/*
*********************************************************************************************************
*                                   TCP RTT MEASUREMENT DATA TYPES
*
* Note(s) : (1) RTT measurement data types MUST be defined to ensure sufficient range for both scaled
*               & un-scaled, signed & unsigned time measurement values.
*
*           (2) 'NET_TCP_TX_RTT_MS'/'NET_TCP_TX_RTT_TS_MS' pre-defined in 'net_type.h' PRIOR to all 
*                other network modules that require TCP Transmit Round-Trip Time data types.
*********************************************************************************************************
*/

typedef  CPU_INT08U         NET_TCP_TX_RTT_STATE;

                                                                /* See Note #1.                                         */
#if 0                                                           /* See Note #2.                                         */
typedef  CPU_INT32U         NET_TCP_TX_RTT_MS;
#endif
typedef  CPU_INT32S         NET_TCP_TX_RTT_MS_SCALED;

#if 0                                                           /* See Note #2.                                         */
typedef  NET_TCP_TX_RTT_MS  NET_TCP_TX_RTT_TS_MS;
#endif


/*
*********************************************************************************************************
*                                       TCP TIMEOUT DATA TYPES
*********************************************************************************************************
*/

typedef  CPU_INT32U         NET_TCP_TIMEOUT_MS;
typedef  CPU_INT16U         NET_TCP_TIMEOUT_SEC;


/*$PAGE*/
/*
*********************************************************************************************************
*                                  TCP CONNECTION QUANTITY DATA TYPE
*
* Note(s) : (1) See also 'TCP CONNECTION IDENTIFICATION DATA TYPE  Note #1'.
*********************************************************************************************************
*/

typedef  CPU_INT16S  NET_TCP_CONN_QTY;                          /* Defines max qty of TCP conns to support.             */


/*
*********************************************************************************************************
*                               TCP CONNECTION IDENTIFICATION DATA TYPE
*
* Note(s) : (1) (a) NET_TCP_CONN_NBR_MAX  SHOULD be #define'd based on 'NET_TCP_CONN_QTY' data type declared.
*
*               (b) However, since TCP connection handle identifiers are data-typed as 16-bit signed integers; 
*                   the maximum number of valid TCP connection identifiers, & therefore the maximum number 
*                   of valid TCP connections, is the total number of non-negative values that 16-bit signed
*                   integers support.
*********************************************************************************************************
*/

typedef  CPU_INT16S  NET_TCP_CONN_ID;

#define  NET_TCP_CONN_NBR_MIN                              1
#define  NET_TCP_CONN_NBR_MAX            DEF_INT_16S_MAX_VAL    /* See Note #1.                                         */

#define  NET_TCP_CONN_ID_NONE                             -1
#define  NET_TCP_CONN_ID_MIN                               0
#define  NET_TCP_CONN_ID_MAX       (NET_TCP_CFG_NBR_CONN - 1)


/*$PAGE*/
/*
*********************************************************************************************************
*                                             TCP HEADER
*
* Note(s) : (1) See RFC #793, Section 3.1 for TCP segment header format.
*
*           (2) TCP Header Length & Flags are coded in the thirteenth & fourteenth octets of an IP header
*               as follows :
*
*                        15 14 13 12 11 10 9 8 7 6  5   4   3   2   1   0
*                       ---------------------------------------------------
*                       |  H L E N  |   Z E R O   |URG|ACK|PSH|RST|SYN|FIN|
*                       ---------------------------------------------------
*
*                   where
*                           HLEN        TCP Headers' length in 32-bit words; MUST be at least 5 (20-octet header) 
*                                           & MUST be less than or equal to 15 (60-octet header)
*                           ZERO        MUST be zero; i.e. '000000'
*                           URG         Segment Urgent Data Pointer Valid (see Note #3) :
*                                           '0' - Urgent Data                NOT available (default)
*                                           '1' - Urgent Data                    available
*                           ACK         Segment Acknowledgement Number Valid :
*                                           '0' - Acknowledgement Number     NOT available
*                                           '1' - Acknowledgement Number         available
*                           PSH         Segment Push Data Request :
*                                           '0' - Push Data                  NOT requested (default)
*                                           '1' - Push Data                      requested
*                           RST         Connection Reset Request :
*                                           '0' - Connection Reset           NOT requested (default)
*                                           '1' - Connection Reset               requested
*                           SYN         Connection Synchronization Request :
*                                           '0' - Connection Synchronization NOT requested (default)
*                                           '1' - Connection Synchronization     requested
*                           FIN         Connection Close Request :
*                                           '0' - Connection Close           NOT requested (default)
*                                           '1' - Connection Close               requested
*
*           (3) Urgent pointer & data NOT supported (see 'net_tcp.h  Note #1b').
*********************************************************************************************************
*/

                                                                /* ------------------- NET TCP HDR -------------------- */
typedef  struct  net_tcp_hdr {
    NET_TCP_PORT_NBR        PortSrc;                            /* TCP seg  src  port.                                  */
    NET_TCP_PORT_NBR        PortDest;                           /* TCP seg  dest port.                                  */
    NET_TCP_SEQ_NBR         SeqNbr;                             /* TCP seg  seq  nbr.                                   */
    NET_TCP_SEQ_NBR         AckNbr;                             /* TCP seg  ack  nbr.                                   */
    CPU_INT16U              HdrLen_Flags;                       /* TCP seg  hdr  len/flags (see Note #2).               */
    NET_TCP_WIN_SIZE        WinSize;                            /* TCP conn win size.                                   */
    NET_CHK_SUM             ChkSum;                             /* TCP seg  chk sum.                                    */
    CPU_INT16U              UrgentPtr;                          /* TCP seg  urgent ptr     (see Note #3).               */
    NET_TCP_OPT_SIZE        Opts[NET_TCP_HDR_OPT_NBR_MAX];      /* TCP seg  opts (if any).                              */
} NET_TCP_HDR;


/*
*********************************************************************************************************
*                                          TCP PSEUDO-HEADER
*
* Note(s) : (1) See RFC #793, Section 3.1 'Header Format : Checksum' for TCP pseudo-header format.
*********************************************************************************************************
*/

                                                                /* ---------------- NET TCP PSEUDO-HDR ---------------- */
typedef  struct  net_tcp_pseudo_hdr {
    NET_IP_ADDR             AddrSrc;                            /* TCP seg src  addr.                                   */
    NET_IP_ADDR             AddrDest;                           /* TCP seg dest addr.                                   */
    CPU_INT08U              Zero;                               /* Field MUST be zero'd; i.e. ALL bits clr'd.           */
    CPU_INT08U              Protocol;                           /* TCP protocol.                                        */
    CPU_INT16U              TotLen;                             /* TCP seg tot len.                                     */
} NET_TCP_PSEUDO_HDR;


/*$PAGE*/
/*
*********************************************************************************************************
*                              TCP HEADER OPTION CONFIGURATION DATA TYPES
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*                       TCP MAXIMUM SEGMENT SIZE OPTION CONFIGURATION DATA TYPE
*********************************************************************************************************
*/

typedef  struct  net_tcp_opt_cfg_max_seg_size {
    NET_TYPE                Type;                               /* TCP opt type.                                        */
    NET_TCP_SEG_SIZE        MaxSegSize;                         /* TCP max seg size.                                    */
    void                   *NextOptPtr;                         /* Ptr to next TCP opt cfg.                             */
} NET_TCP_OPT_CFG_MAX_SEG_SIZE;


/*$PAGE*/
/*
*********************************************************************************************************
*                                      TCP CONNECTION DATA TYPE
*
*                                    NET_TCP_CONN
*                                   |-------------|
*                                   |  Conn Type  |
*                                   |-------------|      Next TCP
*                                   |      O----------> Connection
*                                   |-------------|
*                                   |     IDs     |
*                                   |-------------|
*                                   | Conn State  |
*                                   |-------------|
*                                   |   Rx Ctrl   |
*                                   |   Rx Q's    |
*                                   |-------------|
*                                   |   Tx Ctrl   |
*                                   |   Tx Q's    |
*                                   |   Tx Tmrs   |
*                                   |-------------|
*                                   |   Timeouts  |
*                                   |-------------|
*                                   |    Flags    |
*                                   |-------------|
*
*
* Note(s) : (1) Forced word-alignment at start of TCP connection NOT required since first data member 
*               'Type' is declared as 'CPU_INT32U'.
*
*           (2) #### 'TimeoutUser_sec' updates NOT fully implemented.
*********************************************************************************************************
*/

                                                                /* ------------------- NET TCP CONN ------------------- */
struct  net_tcp_conn {
    NET_TYPE                    Type;                           /* Type cfg'd @ init : NET_TCP_TYPE_CONN.               */

    NET_TCP_CONN               *NextPtr;                        /* Ptr to NEXT conn.                                    */


    NET_TCP_CONN_ID             ID;                             /* TCP conn id.                                         */
    NET_CONN_ID                 ID_Conn;                        /*     Conn id.                                         */


    NET_TCP_CONN_STATE          ConnState;                      /* TCP conn state.                                      */

    CPU_INT08U                  ConnCloseCode;                  /* Conn close code.                                     */
    CPU_BOOLEAN                 ConnCloseAppFlag;               /* Conn close app           flag.                       */
    CPU_BOOLEAN                 ConnCloseTimeoutFaultFlag;      /* Conn close timeout fault flag.                       */


                                                                /*      (All seg sizes in octets.)                      */
    NET_TCP_SEG_SIZE            MaxSegSizeLocal;                /* Local max seg size to advertise to remote host.      */
    NET_TCP_SEG_SIZE            MaxSegSizeRemote;               /*       Max seg size advertised   by remote host.      */
    NET_TCP_SEG_SIZE            MaxSegSizeConn;                 /*       Max seg size calc'd for conn.                  */



    NET_TCP_SEQ_NBR             RxSeqNbrSync;                   /* Sync  rx seq nbr.                                    */
    NET_TCP_SEQ_NBR             RxSeqNbrNext;                   /* Next  rx seq nbr.                                    */
    NET_TCP_SEQ_NBR             RxSeqNbrLast;                   /* Last  rx seq nbr.                                    */
    NET_TCP_SEQ_NBR             RxSeqNbrClose;                  /* Close rx seq nbr.                                    */

    NET_TCP_WIN_SIZE            RxWinSizeCfgd;                  /* Rx win size cfg'd (all win sizes in octets).         */
    NET_TCP_WIN_SIZE            RxWinSizeCfgdActual;            /* Rx win size cfg'd actual.                            */
    NET_TCP_WIN_SIZE            RxWinSizeCfgdActualUpdateRem;   /* Rx win size cfg'd actual rem'ing to update.          */
    NET_TCP_WIN_SIZE            RxWinSizeCalcd;                 /* Rx win size calc'd.                                  */
    NET_TCP_WIN_SIZE            RxWinSizeActual;                /* Rx win size actual/advertised.                       */

    NET_TCP_WIN_SIZE            RxWinSizeUpdateTh;              /* Rx win size update th (rx silly win ctrl).           */


    NET_TCP_RX_Q_STATE          RxQ_State;                      /*                   Rx Q state.                        */

    NET_BUF                    *RxQ_Transport_Head;             /* Head of transport rx Q to q TCP pkts until seq'd.    */
    NET_BUF                    *RxQ_Transport_Tail;             /* Tail of transport rx Q to q TCP pkts until seq'd.    */

    NET_BUF                    *RxQ_App_Head;                   /* Head of app data  rx Q to q TCP pkts until app rd(s).*/
    NET_BUF                    *RxQ_App_Tail;                   /* Tail of app data  rx Q to q TCP pkts until app rd(s).*/



/*$PAGE*/
    NET_TCP_SEQ_NBR             TxSeqNbrSync;                   /* Sync  tx seq nbr.                                    */
    NET_TCP_SEQ_NBR             TxSeqNbrNext;                   /* Next  tx seq nbr.                                    */
    NET_TCP_SEQ_NBR             TxSeqNbrNextQ;                  /* Next  tx seq nbr NOT yet tx'd; i.e. in tx Q.         */
    NET_TCP_SEQ_NBR             TxSeqNbrUnReTxd;                /* Last  tx seq nbr NOT re- tx'd.                       */
    NET_TCP_SEQ_NBR             TxSeqNbrUnAckd;                 /* Last  tx seq nbr NOT yet ack'd.                      */
    NET_TCP_SEQ_NBR             TxSeqNbrUnAckdPrev;             /* Prev  tx seq nbr NOT yet ack'd.                      */
    NET_TCP_SEQ_NBR             TxSeqNbrUnAckdAlignDelta;       /*       Tx seq nbr NOT yet ack'd alignment delta.      */
    NET_TCP_SEQ_NBR             TxSeqNbrLast;                   /* Last  tx seq nbr.                                    */
    NET_TCP_SEQ_NBR             TxSeqNbrClose;                  /* Close tx seq nbr.                                    */


    NET_TCP_SEQ_NBR             TxWinUpdateSeqNbr;              /* Last  rx seq nbr  that updated remote rx win size.   */
    NET_TCP_SEQ_NBR             TxWinUpdateAckNbr;              /* Last  rx ack nbr  that updated remote rx win size.   */
    NET_TCP_WIN_SIZE            TxWinUpdateWinSize;             /* Last  rx win size that updated remote rx win size.   */

    CPU_INT16U                  TxWinRxdAckDupCtr;              /*       Rx'd duplicate ack ctr.                        */
    NET_TCP_SEQ_NBR             TxWinRxdLastSeqNbr;             /* Last  rx'd seq nbr.                                  */
    NET_TCP_SEQ_NBR             TxWinRxdLastAckNbr;             /* Last  rx'd ack nbr.                                  */
    NET_TCP_WIN_SIZE            TxWinRxdLastWinSize;            /* Last  rx'd win size.                                 */

    NET_TCP_WIN_SIZE            TxWinSizeCfgd;                  /* Tx  win size cfg'd (all win sizes in octets).        */
    NET_TCP_WIN_SIZE            TxWinSizeCfgdRem;               /* Tx  win size rem'ing to q tx app data.               */

    NET_TCP_WIN_SIZE            TxWinSizeRemote;                /*     Win size advertised by  remote host.             */
    NET_TCP_WIN_SIZE            TxWinSizeRemoteMax;             /* Max win size advertised by  remote host.             */
    NET_TCP_WIN_SIZE            TxWinSizeRemoteActual;          /* Tx  win size actual     for remote host.             */
    NET_TCP_WIN_SIZE            TxWinSizeRemoteRem;             /* Tx  win size rem'ing    for remote host.             */

    NET_TCP_WIN_SIZE            TxWinSizeSlowStartTh;           /* Tx  win size slow start      th.                     */
    NET_TCP_WIN_SIZE            TxWinSizeSlowStartThInit;       /* Tx  win size slow start init th.                     */
    NET_TCP_WIN_SIZE            TxWinSizeCongInit;              /* Tx  win size        init'd by cong ctrl.             */
    NET_TCP_WIN_SIZE            TxWinSizeCongCalcdActual;       /* Tx  win size actual calc'd    cong ctrl.             */
    NET_TCP_WIN_SIZE            TxWinSizeCongCalcdCur;          /* Tx  win size cur    calc'd    cong ctrl.             */
    NET_TCP_WIN_SIZE            TxWinSizeCongRem;               /* Tx  win size rem'ing       by cong ctrl.             */

    NET_TCP_WIN_SIZE            TxWinSizeAvail;                 /* Tx  win size avail.                                  */

    NET_TCP_WIN_SIZE            TxWinSizeMinTh;                 /* Tx  win size min th (tx silly win ctrl).             */
    CPU_BOOLEAN                 TxWinSizeNagleEn;               /* Tx  win size Nagle alg en.                           */

    NET_TCP_TIMEOUT_MS          TxWinSillyWinTimeout_ms;        /* Tx  silly win timeout (in ms).                       */
    NET_TCP_TIMEOUT_SEC         TxWinZeroWinTimeout_ms;         /* Tx  zero  win timeout (in ms).                       */


    CPU_INT16U                  TxSegReTxTh;                    /* TCP conn nbr of re-tx th.                            */

    NET_TCP_TIMEOUT_MS          TxAckDlyTimeout_ms;             /* Tx ack dly timeout (in ms).                          */
    NET_TMR                    *TxAckDlyTmr;                    /* Tx ack dly tmr.                                      */
    CPU_INT16U                  TxAckDlyCnt;                    /* Tx ack dly cur cnt.                                  */

    CPU_BOOLEAN                 TxAckImmedRxdPushEn;            /* Tx immed ack for rx'd TCP push seg(s) en.            */


    NET_TCP_TX_RTT_MS_SCALED    TxRTT_Avg_ms_scaled;            /* RTT smoothed avg      (in int-scaled ms   ).         */
    NET_TCP_TX_RTT_MS_SCALED    TxRTT_Dev_ms_scaled;            /* RTT mean     dev      (in int-scaled ms   ).         */
    NET_TCP_TX_RTT_MS_SCALED    TxRTT_RTO_ms_scaled;            /* RTT re-tx timeout     (in int-scaled ms   ).         */
    NET_TCP_TX_RTT_MS_SCALED    TxRTT_RTO_Max_ms_scaled;        /* RTT re-tx timeout max (in int-scaled ms   ).         */
    NET_TCP_TIMEOUT_MS          TxRTT_Avg_ms;                   /* RTT smoothed avg      (in            ms   ).         */
    NET_TCP_TIMEOUT_MS          TxRTT_Dev_ms;                   /* RTT mean     dev      (in            ms   ).         */
    NET_TCP_TIMEOUT_MS          TxRTT_RTO_ms;                   /* RTT re-tx timeout     (in            ms   ).         */
    NET_TCP_TIMEOUT_MS          TxRTT_RTO_Max_ms;               /* RTT re-tx timeout max (in            ms   ).         */
    NET_TCP_TIMEOUT_SEC         TxRTT_RTO_sec;                  /* RTT re-tx timeout     (in            secs ).         */
    NET_TCP_TIMEOUT_SEC         TxRTT_RTO_Max_sec;              /* RTT re-tx timeout max (in            secs ).         */
    NET_TMR_TICK                TxRTT_RTO_tick;                 /* RTT re-tx timeout     (in            ticks).         */
    NET_TCP_TX_RTT_STATE        TxRTT_RTO_State;                /* RTT-RTO state.                                       */


    NET_IP_TOS                  TxIP_TOS;                       /* TCP conn tx IP TOS.                                  */
    NET_IP_TTL                  TxIP_TTL;                       /* TCP conn tx IP TTL.                                  */
    CPU_INT16U                  TxIP_Flags;                     /* TCP conn tx IP flags.                                */


    NET_TCP_TX_Q_STATE          TxQ_State;                      /*      Tx Q state.                                     */

    NET_BUF                    *TxQ_Head;                       /* Head of Q of TCP pkts to    tx.                      */
    NET_BUF                    *TxQ_Tail;                       /* Tail of Q of TCP pkts to    tx.                      */
    NET_TMR                    *TxQ_IdleTmr;                    /*      Tx Q idle              tmr.                     */
    NET_TMR                    *TxQ_SillyWinTmr;                /*      Tx Q silly win persist tmr.                     */
    NET_TMR                    *TxQ_ZeroWinTmr;                 /*      Tx Q zero  win persist tmr.                     */

    NET_BUF                    *ReTxQ_Head;                     /* Head of Q of TCP pkts to re-tx on timeout.           */
    NET_BUF                    *ReTxQ_Tail;                     /* Tail of Q of TCP pkts to re-tx on timeout.           */
    NET_TMR                    *ReTxQ_Tmr;                      /*   Re-tx Q         tmr.                               */



/*$PAGE*/
    NET_TMR                    *KeepAliveTmr;                   /* TCP conn keep-alive   tmr.                           */
    NET_TMR                    *TimeoutTmr;                     /* TCP conn      timeout tmr.                           */

    NET_TCP_TIMEOUT_SEC         TimeoutConn_sec;                /* TCP conn      timeout (in secs).                     */
    NET_TCP_TIMEOUT_SEC         TimeoutUser_sec;                /* TCP user      timeout (in secs) [see Note #2].       */
    NET_TCP_TIMEOUT_SEC         TimeoutMaxSeg_sec;              /* TCP max  seg  timeout (in secs).                     */


    CPU_INT16U                  Flags;                          /* TCP conn flags.                                      */
};


/*$PAGE*/
/*
*********************************************************************************************************
*                                          GLOBAL VARIABLES
*********************************************************************************************************
*/

NET_TCP_EXT  NET_TCP_CONN      NetTCP_ConnTbl[NET_TCP_CFG_NBR_CONN];
NET_TCP_EXT  NET_TCP_CONN     *NetTCP_ConnPoolPtr;              /* Ptr to pool of free TCP conns.                       */
NET_TCP_EXT  NET_STAT_POOL     NetTCP_ConnPoolStat;


NET_TCP_EXT  NET_TCP_SEQ_NBR   NetTCP_TxSeqNbrCtr;              /* Global tx seq nbr ctr.                               */


/*$PAGE*/
/*
*********************************************************************************************************
*                                               MACRO'S
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*                                      NET_TCP_TX_GET_SEQ_NBR()
*
* Description : Get next TCP transmit sequence number.
*
* Argument(s) : seq_nbr     Variable that will receive the returned TCP transmit sequence number.
*
* Return(s)   : none.
*
* Caller(s)   : NetTCP_TxConnSync().
*
*               This macro is an INTERNAL network protocol suite macro & SHOULD NOT be called by 
*               application function(s).
*
* Note(s)     : (1) (a) RFC #1122, Section 4.2.2.9 states that "a TCP MUST use the specified clock-driven 
*                       selection of initial sequence numbers".
*
*                   (b) However; Stevens, TCP/IP Illustrated, Volume 1, 8th Printing, Section 18.2, Page 232
*                       states that for "4.4BSD (and most Berkeley-derived implementations)" ... :
*
*                       (1) "When the system is initialized the initial send sequence number is initialized 
*                            to 1.  This practice violates the Host Requirements RFC [#1122]."
*
*                       (2) "This variable is then incremented by 64,000" ... :
*                           (A) "every half-second" and ...
*                           (B) "additionally, each time a connection is established."
*
*                   (c) As a compromise :
*
*                       (1) The developer is required to configure the TCP transmit initial sequence number
*                           counter (see 'net_bsp.c  NetTCP_InitTxSeqNbr()  Note #1').
*
*                       (2) However, the TCP initial transmit sequence number is incremented by a fixed 
*                           value each time a new TCP connection is established (see also Note #2b2B).
*
*               (2) Return TCP sequence number is NOT converted from host-order to network-order.
*********************************************************************************************************
*/

#if (NET_DBG_CFG_TEST_EN != DEF_ENABLED)

#define  NET_TCP_TX_GET_SEQ_NBR(seq_nbr)            { NET_UTIL_VAL_COPY_32(&(seq_nbr), &NetTCP_TxSeqNbrCtr); \
                                                      NetTCP_TxSeqNbrCtr += NET_TCP_TX_SEQ_NBR_CTR_INC;      }

#else

#define  NET_TCP_TX_GET_SEQ_NBR(seq_nbr)            { NET_UTIL_VAL_COPY_32(&(seq_nbr), &NetTCP_TxSeqNbrCtr); }

#endif


/*$PAGE*/
/*
*********************************************************************************************************
*                                         FUNCTION PROTOTYPES
*********************************************************************************************************
*/

void             NetTCP_Init                      (NET_ERR              *perr);



                                                                                    /* ----------- RX FNCTS ----------- */
void             NetTCP_Rx                        (NET_BUF              *pbuf,
                                                   NET_ERR              *perr);

CPU_INT16U       NetTCP_RxAppData                 (NET_TCP_CONN_ID       conn_id_tcp,
                                                   void                 *pdata_buf,
                                                   CPU_INT16U            data_buf_len,
                                                   CPU_INT16U            flags,
                                                   NET_ERR              *perr);



                                                                                    /* ----------- TX FNCTS ----------- */
void             NetTCP_TxConnReq                 (NET_TCP_CONN_ID       conn_id_tcp,
                                                   NET_IP_TOS            TOS,
                                                   NET_IP_TTL            TTL,
                                                   CPU_INT16U            flags_ip,
                                                   void                 *popts_ip,
                                                   NET_ERR              *perr);

void             NetTCP_TxConnReqClose            (NET_TCP_CONN_ID       conn_id_tcp,
                                                   CPU_INT08U            conn_close_code,
                                                   NET_ERR              *perr);


CPU_INT16U       NetTCP_TxConnAppData             (NET_TCP_CONN_ID       conn_id_tcp,
                                                   void                 *p_data,
                                                   CPU_INT16U            data_len,
                                                   CPU_INT16U            flags,
                                                   NET_ERR              *perr);



                                                                                    /* ---- TCP RX/TX STATUS FNCTS ---- */
CPU_BOOLEAN      NetTCP_ConnIsAvailRx             (NET_TCP_CONN_ID       conn_id_tcp,
                                                   NET_ERR              *perr);

CPU_BOOLEAN      NetTCP_ConnIsRdyTx               (NET_TCP_CONN_ID       conn_id_tcp,
                                                   NET_ERR              *perr);



/*$PAGE*/
                                                                                    /* ------ TCP CONN API FNCTS ------ */
NET_TCP_CONN_ID  NetTCP_ConnGet                   (NET_ERR              *perr);

void             NetTCP_ConnFree                  (NET_TCP_CONN_ID       conn_id_tcp);


void             NetTCP_ConnCloseFromConn         (NET_TCP_CONN_ID       conn_id_tcp);


void             NetTCP_ConnSetID_Conn            (NET_TCP_CONN_ID       conn_id_tcp,
                                                   NET_CONN_ID           conn_id,
                                                   NET_ERR              *perr);


void             NetTCP_ConnSetStateListen        (NET_TCP_CONN_ID       conn_id_tcp,
                                                   NET_ERR              *perr);



                                                                                    /* ------ TCP CONN CFG FNCTS ------ */
CPU_BOOLEAN      NetTCP_ConnCfgMaxSegSizeLocal    (NET_TCP_CONN_ID       conn_id_tcp,
                                                   NET_TCP_SEG_SIZE      max_seg_size);


CPU_BOOLEAN      NetTCP_ConnCfgRxWinSize          (NET_TCP_CONN_ID       conn_id_tcp,
                                                   NET_TCP_WIN_SIZE      win_size);


CPU_BOOLEAN      NetTCP_ConnCfgReTxMaxTh          (NET_TCP_CONN_ID       conn_id_tcp,
                                                   CPU_INT16U            nbr_max_re_tx);

CPU_BOOLEAN      NetTCP_ConnCfgReTxMaxTimeout     (NET_TCP_CONN_ID       conn_id_tcp,
                                                   NET_TCP_TIMEOUT_SEC   timeout_sec);


CPU_BOOLEAN      NetTCP_ConnCfgTxAckImmedRxdPushEn(NET_TCP_CONN_ID       conn_id_tcp,
                                                   CPU_BOOLEAN           tx_immed_ack_en);



                                                                                    /* ---- TCP CONN STATUS FNCTS ----- */
CPU_BOOLEAN      NetTCP_ConnIsUsed                (NET_TCP_CONN_ID       conn_id_tcp,
                                                   NET_ERR              *perr);


NET_STAT_POOL    NetTCP_ConnPoolStatGet           (void);

void             NetTCP_ConnPoolStatResetMaxUsed  (void);


/*$PAGE*/
/*
*********************************************************************************************************
*                                         FUNCTION PROTOTYPES
*                                      DEFINED IN OS'S  net_os.c
*********************************************************************************************************
*/


void        NetOS_TCP_Init             (NET_ERR          *perr);                    /* Create TCP objs.                 */


                                                                                    /* Clr      TCP conn rx Q signal.   */
void        NetOS_TCP_RxQ_Clr          (NET_TCP_CONN_ID   conn_id_tcp,
                                        NET_ERR          *perr);
                                                                                    /* Wait for TCP conn rx Q signal.   */
void        NetOS_TCP_RxQ_Wait         (NET_TCP_CONN_ID   conn_id_tcp,
                                        NET_ERR          *perr);
                                                                                    /* Signal   TCP conn rx Q.          */
void        NetOS_TCP_RxQ_Signal       (NET_TCP_CONN_ID   conn_id_tcp,
                                        NET_ERR          *perr);
                                                                                    /* Abort    TCP conn rx Q.          */
void        NetOS_TCP_RxQ_Abort        (NET_TCP_CONN_ID   conn_id_tcp,
                                        NET_ERR          *perr);

                                                                                    /* Set dflt TCP conn rx Q timeout.  */
void        NetOS_TCP_RxQ_TimeoutDflt  (NET_TCP_CONN_ID   conn_id_tcp,
                                        NET_ERR          *perr);
                                                                                    /* Set      TCP conn rx Q timeout.  */
void        NetOS_TCP_RxQ_TimeoutSet   (NET_TCP_CONN_ID   conn_id_tcp,
                                        CPU_INT32U        timeout_ms,
                                        NET_ERR          *perr);
                                                                                    /* Get      TCP conn rx Q timeout.  */
CPU_INT32U  NetOS_TCP_RxQ_TimeoutGet_ms(NET_TCP_CONN_ID   conn_id_tcp,
                                        NET_ERR          *perr);


                                                                                    /* Clr      TCP conn tx Q signal.   */
void        NetOS_TCP_TxQ_Clr          (NET_TCP_CONN_ID   conn_id_tcp,
                                        NET_ERR          *perr);
                                                                                    /* Wait for TCP conn tx Q signal.   */
void        NetOS_TCP_TxQ_Wait         (NET_TCP_CONN_ID   conn_id_tcp,
                                        NET_ERR          *perr);
                                                                                    /* Signal   TCP conn tx Q.          */
void        NetOS_TCP_TxQ_Signal       (NET_TCP_CONN_ID   conn_id_tcp,
                                        NET_ERR          *perr);
                                                                                    /* Abort    TCP conn tx Q.          */
void        NetOS_TCP_TxQ_Abort        (NET_TCP_CONN_ID   conn_id_tcp,
                                        NET_ERR          *perr);

                                                                                    /* Set dflt TCP conn tx Q timeout.  */
void        NetOS_TCP_TxQ_TimeoutDflt  (NET_TCP_CONN_ID   conn_id_tcp,
                                        NET_ERR          *perr);
                                                                                    /* Set      TCP conn tx Q timeout.  */
void        NetOS_TCP_TxQ_TimeoutSet   (NET_TCP_CONN_ID   conn_id_tcp,
                                        CPU_INT32U        timeout_ms,
                                        NET_ERR          *perr);
                                                                                    /* Get      TCP conn tx Q timeout.  */
CPU_INT32U  NetOS_TCP_TxQ_TimeoutGet_ms(NET_TCP_CONN_ID   conn_id_tcp,
                                        NET_ERR          *perr);


/*
*********************************************************************************************************
*                                         FUNCTION PROTOTYPES
*                                   DEFINED IN PRODUCT'S  net_bsp.c
*********************************************************************************************************
*/

void                  NetTCP_InitTxSeqNbr      (void);                              /* Init TCP tx seq nbr ctr.         */

NET_TCP_TX_RTT_TS_MS  NetTCP_TxConnRTT_GetTS_ms(void);                              /* Get  TCP tx RTT TS (in ms).      */


/*$PAGE*/
/*
*********************************************************************************************************
*                                        CONFIGURATION ERRORS
*********************************************************************************************************
*/

#ifndef  NET_TCP_CFG_NBR_CONN
#error  "NET_TCP_CFG_NBR_CONN                        not #define'd in 'net_cfg.h'"
#error  "                                      [MUST be  >= NET_TCP_CONN_NBR_MIN]"
#error  "                                      [     &&  <= NET_TCP_CONN_NBR_MAX]"

#elif  ((NET_TCP_CFG_NBR_CONN < NET_TCP_CONN_NBR_MIN) || \
        (NET_TCP_CFG_NBR_CONN > NET_TCP_CONN_NBR_MAX))
#error  "NET_TCP_CFG_NBR_CONN                  illegally #define'd in 'net_cfg.h'"
#error  "                                      [MUST be  >= NET_TCP_CONN_NBR_MIN]"
#error  "                                      [     &&  <= NET_TCP_CONN_NBR_MAX]"
#endif




#ifndef  NET_TCP_CFG_RX_WIN_SIZE_OCTET
#error  "NET_TCP_CFG_RX_WIN_SIZE_OCTET               not #define'd in 'net_cfg.h'"
#error  "                                      [MUST be  >= NET_TCP_WIN_SIZE_MIN]"
#error  "                                      [     &&  <= NET_TCP_WIN_SIZE_MAX]"

#elif  ((NET_TCP_CFG_RX_WIN_SIZE_OCTET < NET_TCP_WIN_SIZE_MIN) || \
        (NET_TCP_CFG_RX_WIN_SIZE_OCTET > NET_TCP_WIN_SIZE_MAX))
#error  "NET_TCP_CFG_RX_WIN_SIZE_OCTET         illegally #define'd in 'net_cfg.h'"
#error  "                                      [MUST be  >= NET_TCP_WIN_SIZE_MIN]"
#error  "                                      [     &&  <= NET_TCP_WIN_SIZE_MAX]"
#endif



#ifndef  NET_TCP_CFG_TX_WIN_SIZE_OCTET
#error  "NET_TCP_CFG_TX_WIN_SIZE_OCTET               not #define'd in 'net_cfg.h'"
#error  "                                      [MUST be  >= NET_TCP_WIN_SIZE_MIN]"
#error  "                                      [     &&  <= NET_TCP_WIN_SIZE_MAX]"

#elif  ((NET_TCP_CFG_TX_WIN_SIZE_OCTET < NET_TCP_WIN_SIZE_MIN) || \
        (NET_TCP_CFG_TX_WIN_SIZE_OCTET > NET_TCP_WIN_SIZE_MAX))
#error  "NET_TCP_CFG_TX_WIN_SIZE_OCTET         illegally #define'd in 'net_cfg.h'"
#error  "                                      [MUST be  >= NET_TCP_WIN_SIZE_MIN]"
#error  "                                      [     &&  <= NET_TCP_WIN_SIZE_MAX]"
#endif




/*$PAGE*/
#ifndef  NET_TCP_CFG_TIMEOUT_CONN_MAX_SEG_SEC
#error  "NET_TCP_CFG_TIMEOUT_CONN_MAX_SEG_SEC        not #define'd in 'net_cfg.h'                "
#error  "                                      [MUST be  >= NET_TCP_CONN_TIMEOUT_MAX_SEG_MIN_SEC]"
#error  "                                      [     &&  <= NET_TCP_CONN_TIMEOUT_MAX_SEG_MAX_SEC]"

#elif  ((NET_TCP_CFG_TIMEOUT_CONN_MAX_SEG_SEC < NET_TCP_CONN_TIMEOUT_MAX_SEG_MIN_SEC) || \
        (NET_TCP_CFG_TIMEOUT_CONN_MAX_SEG_SEC > NET_TCP_CONN_TIMEOUT_MAX_SEG_MAX_SEC))
#error  "NET_TCP_CFG_TIMEOUT_CONN_MAX_SEG_SEC  illegally #define'd in 'net_cfg.h'                "
#error  "                                      [MUST be  >= NET_TCP_CONN_TIMEOUT_MAX_SEG_MIN_SEC]"
#error  "                                      [     &&  <= NET_TCP_CONN_TIMEOUT_MAX_SEG_MAX_SEC]"
#endif



#ifndef  NET_TCP_CFG_TIMEOUT_CONN_ACK_DLY_MS
#error  "NET_TCP_CFG_TIMEOUT_CONN_ACK_DLY_MS         not #define'd in 'net_cfg.h'       "
#error  "                                      [MUST be  >= NET_TCP_ACK_DLY_TIME_MIN_MS]"
#error  "                                      [     &&  <= NET_TCP_ACK_DLY_TIME_MAX_MS]"

#elif  ((NET_TCP_CFG_TIMEOUT_CONN_ACK_DLY_MS < NET_TCP_ACK_DLY_TIME_MIN_MS) || \
        (NET_TCP_CFG_TIMEOUT_CONN_ACK_DLY_MS > NET_TCP_ACK_DLY_TIME_MAX_MS))
#error  "NET_TCP_CFG_TIMEOUT_CONN_ACK_DLY_MS   illegally #define'd in 'net_cfg.h'       "
#error  "                                      [MUST be  >= NET_TCP_ACK_DLY_TIME_MIN_MS]"
#error  "                                      [     &&  <= NET_TCP_ACK_DLY_TIME_MAX_MS]"
#endif




#ifndef  NET_TCP_CFG_TIMEOUT_CONN_RX_Q_MS
#error  "NET_TCP_CFG_TIMEOUT_CONN_RX_Q_MS            not #define'd in 'net_cfg.h' "
#error  "                                      [MUST be  >= NET_OS_TIMEOUT_MIN_mS]"
#error  "                                      [     &&  <= NET_OS_TIMEOUT_MAX_mS]"
#error  "                                      [     ||  == NET_TMR_TIME_INFINITE]"

#elif (((NET_TCP_CFG_TIMEOUT_CONN_RX_Q_MS <  NET_OS_TIMEOUT_MIN_mS)  || \
        (NET_TCP_CFG_TIMEOUT_CONN_RX_Q_MS >  NET_OS_TIMEOUT_MAX_mS)) && \
        (NET_TCP_CFG_TIMEOUT_CONN_RX_Q_MS != NET_TMR_TIME_INFINITE))
#error  "NET_TCP_CFG_TIMEOUT_CONN_RX_Q_MS      illegally #define'd in 'net_cfg.h' "
#error  "                                      [MUST be  >= NET_OS_TIMEOUT_MIN_mS]"
#error  "                                      [     &&  <= NET_OS_TIMEOUT_MAX_mS]"
#error  "                                      [     ||  == NET_TMR_TIME_INFINITE]"
#endif



#ifndef  NET_TCP_CFG_TIMEOUT_CONN_TX_Q_MS
#error  "NET_TCP_CFG_TIMEOUT_CONN_TX_Q_MS            not #define'd in 'net_cfg.h' "
#error  "                                      [MUST be  >= NET_OS_TIMEOUT_MIN_mS]"
#error  "                                      [     &&  <= NET_OS_TIMEOUT_MAX_mS]"
#error  "                                      [     ||  == NET_TMR_TIME_INFINITE]"

#elif (((NET_TCP_CFG_TIMEOUT_CONN_TX_Q_MS <  NET_OS_TIMEOUT_MIN_mS)  || \
        (NET_TCP_CFG_TIMEOUT_CONN_TX_Q_MS >  NET_OS_TIMEOUT_MAX_mS)) && \
        (NET_TCP_CFG_TIMEOUT_CONN_TX_Q_MS != NET_TMR_TIME_INFINITE))
#error  "NET_TCP_CFG_TIMEOUT_CONN_TX_Q_MS      illegally #define'd in 'net_cfg.h' "
#error  "                                      [MUST be  >= NET_OS_TIMEOUT_MIN_mS]"
#error  "                                      [     &&  <= NET_OS_TIMEOUT_MAX_mS]"
#error  "                                      [     ||  == NET_TMR_TIME_INFINITE]"
#endif


/*$PAGE*/
/*
*********************************************************************************************************
*                                             MODULE END
*
* Note(s) : (1) See 'net_tcp.h  MODULE'.
*********************************************************************************************************
*/

#endif                                                          /* End of TCP module include (see Note #1).             */

