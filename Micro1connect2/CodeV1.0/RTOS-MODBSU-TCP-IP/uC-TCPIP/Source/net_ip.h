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
*                                          NETWORK IP LAYER
*                                         (INTERNET PROTOCOL)
*
* Filename      : net_ip.h
* Version       : V1.92
* Programmer(s) : ITJ
*********************************************************************************************************
* Note(s)       : (1) Supports Internet Protocol as described in RFC #791, also known as IPv4, with the
*                     following restrictions/constraints :
*
*                     (a) ONLY supports a single network interface/             RFC # 791, Section 2.3
*                            host IP address                                                'Addressing'
*
*                     (b) ONLY supports a single default gateway                RFC #1122, Section 3.3.1
*
*                     (c) IP forwarding/routing  NOT           supported        RFC #1122, Sections 3.3.1,
*                                                                                           3.3.4 & 3.3.5
*
*                     (d) IP multicasting        NOT currently supported        RFC #1122, Section 3.3.7
*
*                     (e) Transmit fragmentation NOT currently supported        RFC # 791, Section 2.3
*                                                                                           'Fragmentation &
*                                                                                              Reassembly'
*                     (f) IP Security options    NOT           supported        RFC #1108
*********************************************************************************************************
*/


/*
*********************************************************************************************************
*                                               EXTERNS
*********************************************************************************************************
*/

#ifdef   NET_IP_MODULE
#define  NET_IP_EXT
#else
#define  NET_IP_EXT  extern
#endif


/*$PAGE*/
/*
*********************************************************************************************************
*                                               DEFINES
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*                                          IP HEADER DEFINES
*
* Note(s) : (1) The following IP value MUST be pre-#define'd in 'net_def.h' PRIOR to 'net_buf.h' so that
*               the Network Buffer Module can configure maximum buffer header size (see 'net_def.h  IP
*               LAYER DEFINES' & 'net_buf.h  NETWORK BUFFER INDEX & SIZE DEFINES  Note #1') :
*
*               (a) NET_IP_HDR_SIZE_MAX                   60        (NET_IP_HDR_LEN_MAX
*                                                                  * NET_IP_HDR_LEN_WORD_SIZE)
*********************************************************************************************************
*/

#define  NET_IP_HDR_VER_MASK                            0xF0
#define  NET_IP_HDR_VER_SHIFT                              4
#define  NET_IP_HDR_VER                                    4    /* Supports IPv4 ONLY (see 'net_ip.h  Note #1').        */


#define  NET_IP_HDR_LEN_MASK                            0x0F
#define  NET_IP_HDR_LEN_MIN                                5
#define  NET_IP_HDR_LEN_MAX                               15
#define  NET_IP_HDR_LEN_WORD_SIZE                        CPU_WORD_SIZE_32

#define  NET_IP_HDR_SIZE_MIN                            (NET_IP_HDR_LEN_MIN * NET_IP_HDR_LEN_WORD_SIZE)
#if 0                                                           /* See Note #1a.                                        */
#define  NET_IP_HDR_SIZE_MAX                            (NET_IP_HDR_LEN_MAX * NET_IP_HDR_LEN_WORD_SIZE)
#endif

#define  NET_IP_HDR_SIZE_TOT_MIN                        (NET_IF_HDR_SIZE_TOT_MIN + NET_IP_HDR_SIZE_MIN)
#define  NET_IP_HDR_SIZE_TOT_MAX                        (NET_IF_HDR_SIZE_TOT_MAX + NET_IP_HDR_SIZE_MAX)


#define  NET_IP_ID_NONE                                    0
#define  NET_IP_ID_INIT                                  NET_IP_ID_NONE


/*$PAGE*/
/*
*********************************************************************************************************
*                               IP HEADER TYPE OF SERVICE (TOS) DEFINES
*
* Note(s) : (1) (a) See 'IP HEADER  Note #3'                     for TOS            fields.
*
*               (b) See RFC # 791, Section 3.1 'Type of Service' for TOS Precedence values.
*
*               (c) See RFC #1349, Section 4                     for TOS            values.
*
*           (2) RFC #1122, Section 3.2.1.6 states that "the default ... TOS field ... is all zero bits."
*********************************************************************************************************
*/

#define  NET_IP_HDR_TOS_PRECEDNCE_ROUTINE               0x00
#define  NET_IP_HDR_TOS_PRECEDNCE_PRIO                  0x20
#define  NET_IP_HDR_TOS_PRECEDNCE_IMMED                 0x40
#define  NET_IP_HDR_TOS_PRECEDNCE_FLASH                 0x60
#define  NET_IP_HDR_TOS_PRECEDNCE_FLASH_OVERRIDE        0x80
#define  NET_IP_HDR_TOS_PRECEDNCE_CRITIC_ECP            0xA0
#define  NET_IP_HDR_TOS_PRECEDNCE_INTERNET_CTRL         0xC0
#define  NET_IP_HDR_TOS_PRECEDNCE_NET_CTRL              0xE0

#define  NET_IP_HDR_TOS_PRECEDNCE_MASK                  0xE0
#define  NET_IP_HDR_TOS_PRECEDNCE_DFLT                   NET_IP_HDR_TOS_PRECEDNCE_ROUTINE


#define  NET_IP_HDR_TOS_NONE                            0x00
#define  NET_IP_HDR_TOS_LO_DLY                          0x10
#define  NET_IP_HDR_TOS_HI_THRUPUT                      0x08
#define  NET_IP_HDR_TOS_HI_RELIABILITY                  0x04
#define  NET_IP_HDR_TOS_LO_COST                         0x02

#define  NET_IP_HDR_TOS_MASK                            0x1E
#define  NET_IP_HDR_TOS_DFLT                             NET_IP_HDR_TOS_NONE


#define  NET_IP_HDR_TOS_MBZ_MASK                        0x01
#define  NET_IP_HDR_TOS_MBZ_DFLT                        0x00

#define  NET_IP_HDR_TOS_RESERVED                         NET_IP_HDR_TOS_MBZ_DFLT

                                                                /* See Note #2.                                         */
#define  NET_IP_TOS_DFLT                                (NET_IP_HDR_TOS_PRECEDNCE_DFLT | \
                                                         NET_IP_HDR_TOS_DFLT           | \
                                                         NET_IP_HDR_TOS_MBZ_DFLT)


/*
*********************************************************************************************************
*                                IP HEADER TIME-TO-LIVE (TTL) DEFINES
*
* Note(s) : (1) RFC #1122, Section 3.2.1.7 states that :
*
*               (a) "A host MUST NOT send a datagram with a Time-to-Live (TTL) value of zero."
*
*               (b) "When a fixed TTL value is used, it MUST be configurable."
*
*                   #### NOT yet implemented.
*********************************************************************************************************
*/

#define  NET_IP_HDR_TTL_NONE                               0    /* On IP Tx, subst _DFLT for _NONE (see Note #1a).      */
#define  NET_IP_HDR_TTL_MIN                                1
#define  NET_IP_HDR_TTL_MAX                              255
#define  NET_IP_HDR_TTL_DFLT                             128    /* See Note #1b.                                        */

#define  NET_IP_TTL_NONE                                 NET_IP_HDR_TTL_NONE
#define  NET_IP_TTL_DFLT                                 NET_IP_HDR_TTL_DFLT


/*$PAGE*/
/*
*********************************************************************************************************
*                   IP DATA/TOTAL LENGTH & MAXIMUM TRANSMISSION UNIT (MTU) DEFINES
*
* Note(s) : (1) (a) IP total length #define's (NET_IP_TOT_LEN)  relate to the total size of a complete IP
*                   datagram, including the packet's IP header.  Note that a complete IP datagram MAY be
*                   fragmented in multiple IP packets.
*
*               (b) IP data  length #define's (NET_IP_DATA_LEN) relate to the data  size of a complete IP
*                   datagram, equal to the total IP datagram length minus its IP header size.  Note that
*                   a complete IP datagram MAY be fragmented in multiple IP packets.
*
*               (c) IP MTU #define's (NET_IP_MTU) relate to the data size for any single, non-fragmented
*                   IP packet, equal to a packet's Network Interface MTU minus its IP header size.
*
*           (2) RFC #791, Section 3.1 'Total Length' "recommend[s] that hosts only send datagrams larger
*               than 576 octets if ... the destination is prepared to accept the larger datagrams"; while
*               RFC #879, Section 1 requires that "HOSTS MUST NOT SEND DATAGRAMS LARGER THAN 576 OCTETS
*               UNLESS ... THE DESTINATION HOST IS PREPARED TO ACCEPT LARGER DATAGRAMS".
*********************************************************************************************************
*/

                                                                                /* See Notes #1a & #1b.                 */
#define  NET_IP_DATA_LEN_MIN                               0

#define  NET_IP_TOT_LEN_MIN                             (NET_IP_HDR_SIZE_MIN + NET_IP_DATA_LEN_MIN)
#define  NET_IP_TOT_LEN_MAX                              DEF_INT_16U_MAX_VAL

#define  NET_IP_DATA_LEN_MAX                            (NET_IP_TOT_LEN_MAX  - NET_IP_HDR_SIZE_MIN)


                                                                                /* See Note #1c.                        */
#define  NET_IP_MTU_MIN                                 (NET_IF_MTU          - NET_IP_HDR_SIZE_MAX)
#define  NET_IP_MTU_MAX                                 (NET_IF_MTU          - NET_IP_HDR_SIZE_MIN)
#define  NET_IP_MTU                                      NET_IP_MTU_MIN



#define  NET_IP_MAX_DATAGRAM_SIZE_DFLT                   576                    /* See Note #2.                         */


/*$PAGE*/
/*
*********************************************************************************************************
*                                       IP HEADER FLAG DEFINES
*
* Note(s) : (1) See 'IP HEADER  Note #4' for flag fields.
*********************************************************************************************************
*/

#define  NET_IP_HDR_FLAG_MASK                         0xE000

#define  NET_IP_HDR_FLAG_NONE                     DEF_BIT_NONE
#define  NET_IP_HDR_FLAG_RESERVED                 DEF_BIT_15    /* MUST be '0'.                                         */
#define  NET_IP_HDR_FLAG_FRAG_DONT                DEF_BIT_14
#define  NET_IP_HDR_FLAG_FRAG_MORE                DEF_BIT_13


/*
*********************************************************************************************************
*                                      IP FRAGMENTATION DEFINES
*
* Note(s) : (1) (a) (1) RFC #791, Section 3.2 'Fragmentation and Reassembly' states that :
*
*                       (A) "Fragments are counted in units of 8 octets."
*                       (B) "The minimum fragment is 8 octets."
*
*                   (2) However, this CANNOT apply to the last fragment in a fragmented datagram :
*
*                       (A) Which may be of ANY size; ...
*                       (B) But SHOULD be at least one octet.
*
*               (b) RFC #791, Section 3.2 'Fragmentation and Reassembly : An Example Reassembly Procedure' 
*                   states that "the current recommendation for the [IP fragmentation reassembly] timer 
*                   setting is 15 seconds ... [but] this may be changed ... with ... experience".
*********************************************************************************************************
*/

#define  NET_IP_HDR_FRAG_OFFSET_MASK                   0x7FF
#define  NET_IP_HDR_FRAG_OFFSET_NONE                       0
#define  NET_IP_HDR_FRAG_OFFSET_MIN                        0
#define  NET_IP_HDR_FRAG_OFFSET_MAX                     2047


#define  NET_IP_FRAG_SIZE_UNIT                             2    /* Frag size unit = 8 octets (see Note #1a1).           */

#define  NET_IP_FRAG_SIZE_NONE                           DEF_INT_16U_MAX_VAL
#define  NET_IP_FRAG_SIZE_MIN_FRAG_MORE                  NET_IP_FRAG_SIZE_UNIT      /* See Note #1a1B.                  */
#define  NET_IP_FRAG_SIZE_MIN_FRAG_LAST                    1                        /* See Note #1a2B.                  */
#define  NET_IP_FRAG_SIZE_MAX                         (((NET_IP_TOT_LEN_MAX - NET_IP_HDR_SIZE_MIN) / NET_IP_FRAG_SIZE_UNIT) \
                                                                                                   * NET_IP_FRAG_SIZE_UNIT)

                                                                /* IP frag reasm timeout (see Note #1b) :               */
#define  NET_IP_FRAG_REASM_TIMEOUT_MIN_SEC                 1    /* IP frag reasm timeout min  =  1 seconds              */
#define  NET_IP_FRAG_REASM_TIMEOUT_MAX_SEC                 2    /* IP frag reasm timeout max  = 10 seconds              */
#define  NET_IP_FRAG_REASM_TIMEOUT_DFLT_SEC                1    /* IP frag reasm timeout dflt =  5 seconds              */


/*
*********************************************************************************************************
*                                  IP HEADER PROTOCOL FIELD DEFINES
*
* Note(s) : (1) See 'IP HEADER  Note #5'.
*********************************************************************************************************
*/

#define  NET_IP_HDR_PROTOCOL_ICMP                          1
#define  NET_IP_HDR_PROTOCOL_TCP                           6
#define  NET_IP_HDR_PROTOCOL_UDP                          17


/*$PAGE*/
/*
*********************************************************************************************************
*                                         IP ADDRESS DEFINES
*
* Note(s) : (1) See the following RFC's for IP address summary :
*
*               (a) RFC # 791, Section 3.2     'Addressing'
*               (b) RFC # 950, Section 2.1
*               (c) RFC #1122, Section 3.2.1.3
*               (d) RFC #3927, Section 2.1
*
*           (2) (a) (1) Supports IP Class A, B, & C Network addresses ONLY as specified by RFC #791,
*                       Section 3.2 'Addressing : Address Format' :
*              
*                           Class  High Order Bits   
*                           -----  ---------------   
*                            (A)         0
*                            (B)         10
*                            (C)         110
*
*                   (2) IP Class-D Multicast addresses NOT currently supported (see 'net_ip.h  Note #1d').
*              
*               (b) (1) RFC #1519 states that "the concept of network 'class' needs to be deprecated" 
*                       (Section 4.1) in order to "support classless network destinations" which will 
*                       "permit arbitrary super/subnetting of the remaining class A and class B [and 
*                       class C] address space (the assumption being that classless ... non-contiguous 
*                       subnets ... will be contained within a single ... domain)" (Section 2.2).
*
*                       However, despite the aggregated, classless network address space; individual 
*                       class definitions MUST still be used to validate IP addresses as within a 
*                       network address space -- versus multicast, reserved, or experimental addresses.
*
*                   (2) RFC #950, Section 2.1 states that "the bits that identify the subnet ... need 
*                       not be adjacent in the address.  However, we recommend that the subnet bits be 
*                       contiguous and located as the most significant bits of the local address".
*
*                       #### Therefore, it is assumed that at least the most significant bit of the 
*                       network portion of the subnet address SHOULD be set.
*
*           (3) (a) Currently supports 'This Host' initialization address ONLY :
*
*                   (1) This      Host                      0.0.0.0         See RFC #1122, Section 3.2.1.3.(a)
*
*                  #### Specified host initialization addresses NOT currently supported :
*
*                   (2) Specified Host                      0.<host>        See RFC #1122, Section 3.2.1.3.(b)
*
*               (b) Supports 'Localhost' loopback address :
*
*                   (1) Localhost                         127.<host>        See RFC #1122, Section 3.2.1.3.(g)
*
*                   (2) Stevens, TCP/IP Illustrated, Volume 1, 8th Printing, Section 2.7, Page 28 states 
*                       that "most implementations support a 'loopback interface' that allows a ... host 
*                       to communicate with" itself :
*
*                       (A) "The class A network ID 127 is reserved for the loopback interface."
*                       (B) For "this [loopback] interface ... most systems assign" ... :
*                           (1) "the IP address of 127.0.0.1 ... and" ...
*                           (2) "the name 'localhost'."
*
*               (c) Supports auto-configured, link-local IP addresses :
*
*                   (1) Link-local Hosts                169.254.<host>      See RFC #3927, Section 2.1
*
*                   (2) (A) RFC #3927, Section 2.1 specifies the "IPv4 Link-Local address ... range ... [as]
*                           inclusive" ... :
*
*                           (1) "from 169.254.1.0" ...
*                           (2) "to   169.254.254.255".
*
*                       (B) RFC #3927, Section 2.6.2 states that "the broadcast  address for the Link-Local
*                           prefix ... is ... 169.254.255.255".
*
*                       (C) RFC #3927, Section 2.8   states that "the 169.254/16 address prefix MUST NOT be 
*                           subnetted".
*
*               (d) Currently supports limited & directed-network broadcasts ONLY :
*
*                   (1) Limited          Broadcast      255.255.255.255     See RFC #1122, Section 3.2.1.3.(c)
*                   (2) Directed-Network Broadcast        <net>.<-1>        See RFC #1122, Section 3.2.1.3.(d)
*                   (3) Directed-Subnet  Broadcast   <net>.<subnet >.<-1>   See RFC #1122, Section 3.2.1.3.(e)
*
*                  #### Directed-subnets broadcasts NOT currently supported :
*
*                   (4) Directed-Subnets Broadcast   <net>.<subnets>.<-1>   See RFC #1122, Section 3.2.1.3.(f)
*
*           (4) IP addresses expressed in IPv4 dotted-decimal notation, ww.xx.yy.zz, #define'd as :
*
*                    (((NET_IP_ADDR) ww << (3 * DEF_OCTET_NBR_BITS)) | \
*                     ((NET_IP_ADDR) xx << (2 * DEF_OCTET_NBR_BITS)) | \
*                     ((NET_IP_ADDR) yy << (1 * DEF_OCTET_NBR_BITS)) | \
*                     ((NET_IP_ADDR) zz << (0 * DEF_OCTET_NBR_BITS)))
*********************************************************************************************************
*/
/*$PAGE*/
#define  NET_IP_ADDR_CLASS_A                      0x00000000            /* Class-A IP addr (see Note #2a1A).            */
#define  NET_IP_ADDR_CLASS_A_MASK                 0x80000000
#define  NET_IP_ADDR_CLASS_A_MASK_NET             0xFF000000
#define  NET_IP_ADDR_CLASS_A_MASK_HOST            0x00FFFFFF

#define  NET_IP_ADDR_CLASS_B                      0x80000000            /* Class-B IP addr (see Note #2a1B).            */
#define  NET_IP_ADDR_CLASS_B_MASK                 0xC0000000
#define  NET_IP_ADDR_CLASS_B_MASK_NET             0xFFFF0000
#define  NET_IP_ADDR_CLASS_B_MASK_HOST            0x0000FFFF

#define  NET_IP_ADDR_CLASS_C                      0xC0000000            /* Class-C IP addr (see Note #2a1C).            */
#define  NET_IP_ADDR_CLASS_C_MASK                 0xE0000000
#define  NET_IP_ADDR_CLASS_C_MASK_NET             0xFFFFFF00
#define  NET_IP_ADDR_CLASS_C_MASK_HOST            0x000000FF


#define  NET_IP_ADDR_CLASS_SUBNET_MASK_MIN        0x80000000            /* See Note #2b2.                               */



                                                                        /* 'This Host' IP init addr (see Note #3a1).    */
#define  NET_IP_ADDR_THIS_HOST            (((NET_IP_ADDR)  0 << (3 * DEF_OCTET_NBR_BITS)) | \
                                           ((NET_IP_ADDR)  0 << (2 * DEF_OCTET_NBR_BITS)) | \
                                           ((NET_IP_ADDR)  0 << (1 * DEF_OCTET_NBR_BITS)) | \
                                           ((NET_IP_ADDR)  0 << (0 * DEF_OCTET_NBR_BITS)))

#define  NET_IP_ADDR_NONE                    NET_IP_ADDR_THIS_HOST


                                                                        /* Localhost net           (see Note #3b1).     */
#define  NET_IP_ADDR_LOCAL_HOST_NET       (((NET_IP_ADDR)127 << (3 * DEF_OCTET_NBR_BITS)) | \
                                           ((NET_IP_ADDR)  0 << (2 * DEF_OCTET_NBR_BITS)) | \
                                           ((NET_IP_ADDR)  0 << (1 * DEF_OCTET_NBR_BITS)) | \
                                           ((NET_IP_ADDR)  0 << (0 * DEF_OCTET_NBR_BITS)))
                                                                        /* Localhost typical  addr (see Note #3b2B1).   */
#define  NET_IP_ADDR_LOCAL_HOST_ADDR      (((NET_IP_ADDR)127 << (3 * DEF_OCTET_NBR_BITS)) | \
                                           ((NET_IP_ADDR)  0 << (2 * DEF_OCTET_NBR_BITS)) | \
                                           ((NET_IP_ADDR)  0 << (1 * DEF_OCTET_NBR_BITS)) | \
                                           ((NET_IP_ADDR)  1 << (0 * DEF_OCTET_NBR_BITS)))
                                                                        /* Localhost min host addr.                     */
#define  NET_IP_ADDR_LOCAL_HOST_MIN       (((NET_IP_ADDR)127 << (3 * DEF_OCTET_NBR_BITS)) | \
                                           ((NET_IP_ADDR)  0 << (2 * DEF_OCTET_NBR_BITS)) | \
                                           ((NET_IP_ADDR)  0 << (1 * DEF_OCTET_NBR_BITS)) | \
                                           ((NET_IP_ADDR)  1 << (0 * DEF_OCTET_NBR_BITS)))
                                                                        /* Localhost max host addr.                     */
#define  NET_IP_ADDR_LOCAL_HOST_MAX       (((NET_IP_ADDR)127 << (3 * DEF_OCTET_NBR_BITS)) | \
                                           ((NET_IP_ADDR)255 << (2 * DEF_OCTET_NBR_BITS)) | \
                                           ((NET_IP_ADDR)255 << (1 * DEF_OCTET_NBR_BITS)) | \
                                           ((NET_IP_ADDR)254 << (0 * DEF_OCTET_NBR_BITS)))

#define  NET_IP_ADDR_LOCAL_HOST_MASK_NET          0xFF000000
#define  NET_IP_ADDR_LOCAL_HOST_MASK_HOST         0x00FFFFFF


                                                                        /* Link-local net           (see Note #3c1).    */
#define  NET_IP_ADDR_LOCAL_LINK_NET       (((NET_IP_ADDR)169 << (3 * DEF_OCTET_NBR_BITS)) | \
                                           ((NET_IP_ADDR)254 << (2 * DEF_OCTET_NBR_BITS)) | \
                                           ((NET_IP_ADDR)  0 << (1 * DEF_OCTET_NBR_BITS)) | \
                                           ((NET_IP_ADDR)  0 << (0 * DEF_OCTET_NBR_BITS)))
                                                                        /* Link-local min host addr (see Note #3c2A1).  */
#define  NET_IP_ADDR_LOCAL_LINK_HOST_MIN  (((NET_IP_ADDR)169 << (3 * DEF_OCTET_NBR_BITS)) | \
                                           ((NET_IP_ADDR)254 << (2 * DEF_OCTET_NBR_BITS)) | \
                                           ((NET_IP_ADDR)  1 << (1 * DEF_OCTET_NBR_BITS)) | \
                                           ((NET_IP_ADDR)  0 << (0 * DEF_OCTET_NBR_BITS)))
                                                                        /* Link-local max host addr (see Note #3c2A2).  */
#define  NET_IP_ADDR_LOCAL_LINK_HOST_MAX  (((NET_IP_ADDR)169 << (3 * DEF_OCTET_NBR_BITS)) | \
                                           ((NET_IP_ADDR)254 << (2 * DEF_OCTET_NBR_BITS)) | \
                                           ((NET_IP_ADDR)254 << (1 * DEF_OCTET_NBR_BITS)) | \
                                           ((NET_IP_ADDR)255 << (0 * DEF_OCTET_NBR_BITS)))

#define  NET_IP_ADDR_LOCAL_LINK_MASK_NET          0xFFFF0000
#define  NET_IP_ADDR_LOCAL_LINK_MASK_HOST         0x0000FFFF


                                                                        /* Limited broadcast addr (see Note #3d1).      */
#define  NET_IP_ADDR_BROADCAST            (((NET_IP_ADDR)255 << (3 * DEF_OCTET_NBR_BITS)) | \
                                           ((NET_IP_ADDR)255 << (2 * DEF_OCTET_NBR_BITS)) | \
                                           ((NET_IP_ADDR)255 << (1 * DEF_OCTET_NBR_BITS)) | \
                                           ((NET_IP_ADDR)255 << (0 * DEF_OCTET_NBR_BITS)))


/*$PAGE*/
/*
*********************************************************************************************************
*                                      IP HEADER OPTIONS DEFINES
*
* Note(s) : (1) See the following RFC's for IP options summary :
*
*               (a) RFC # 791, Section 3.1     'Options'
*               (b) RFC #1122, Section 3.2.1.8
*               (c) RFC #1108
*
*           (2) IP option types are coded in the first octet for each IP option as follows :
*
*                         7   6 5  4 3 2 1 0
*                       ---------------------
*                       |CPY|CLASS|  N B R  |
*                       ---------------------
*
*                   where
*                           CPY         Indicates whether option is copied into all fragments :
*                                           '0' - IP option NOT copied into fragments
*                                           '1' - IP option     copied into fragments
*                           CLASS       Indicates options class :
*                                           '00' - Control
*                                           '01' - Reserved
*                                           '10' - Debug / Measurement
*                                           '11' - Reserved
*                           NBR         Option Number :
*                                           '00000' - End of Options List
*                                           '00001' - No Operation
*                                           '00010' - Security
*                                           '00011' - Loose  Source Routing
*                                           '00100' - Internet Timestamp
*                                           '00111' - Record Route
*                                           '01001' - Strict Source Routing
*
*           (3) IP header allows for a maximum option list length of ten (10) 32-bit options :
*
*                   NET_IP_HDR_OPT_SIZE_MAX = (NET_IP_HDR_SIZE_MAX - NET_IP_HDR_SIZE_MIN) / NET_IP_HDR_OPT_SIZE_WORD
*
*                                           = (60 - 20) / (32-bits)
*
*                                           =  Ten (10) 32-bit options
*
*           (4) Data type MUST be globally declared PRIOR to certain dependent #define's.
*********************************************************************************************************
*/
/*$PAGE*/
#define  NET_IP_HDR_OPT_COPY_FLAG                 DEF_BIT_07

#define  NET_IP_HDR_OPT_CLASS_MASK                      0x60
#define  NET_IP_HDR_OPT_CLASS_CTRL                      0x00
#define  NET_IP_HDR_OPT_CLASS_RESERVED_1                0x20
#define  NET_IP_HDR_OPT_CLASS_DBG                       0x40
#define  NET_IP_HDR_OPT_CLASS_RESERVED_2                0x60

#define  NET_IP_HDR_OPT_NBR_MASK                        0x1F
#define  NET_IP_HDR_OPT_NBR_END_LIST                    0x00
#define  NET_IP_HDR_OPT_NBR_NOP                         0x01
#define  NET_IP_HDR_OPT_NBR_SECURITY                    0x02    /* See 'net_ip.h  Note #1f'.                            */
#define  NET_IP_HDR_OPT_NBR_ROUTE_SRC_LOOSE             0x03
#define  NET_IP_HDR_OPT_NBR_SECURITY_EXTENDED           0x05    /* See 'net_ip.h  Note #1f'.                            */
#define  NET_IP_HDR_OPT_NBR_TS                          0x04
#define  NET_IP_HDR_OPT_NBR_ROUTE_REC                   0x07
#define  NET_IP_HDR_OPT_NBR_ROUTE_SRC_STRICT            0x09

#define  NET_IP_HDR_OPT_END_LIST             (                           NET_IP_HDR_OPT_CLASS_CTRL | NET_IP_HDR_OPT_NBR_END_LIST         )
#define  NET_IP_HDR_OPT_NOP                  (                           NET_IP_HDR_OPT_CLASS_CTRL | NET_IP_HDR_OPT_NBR_NOP              )
#define  NET_IP_HDR_OPT_SECURITY             (NET_IP_HDR_OPT_COPY_FLAG | NET_IP_HDR_OPT_CLASS_CTRL | NET_IP_HDR_OPT_NBR_SECURITY         )
#define  NET_IP_HDR_OPT_ROUTE_SRC_LOOSE      (NET_IP_HDR_OPT_COPY_FLAG | NET_IP_HDR_OPT_CLASS_CTRL | NET_IP_HDR_OPT_NBR_ROUTE_SRC_LOOSE  )
#define  NET_IP_HDR_OPT_SECURITY_EXTENDED    (NET_IP_HDR_OPT_COPY_FLAG | NET_IP_HDR_OPT_CLASS_CTRL | NET_IP_HDR_OPT_NBR_SECURITY_EXTENDED)
#define  NET_IP_HDR_OPT_TS                   (                           NET_IP_HDR_OPT_CLASS_DBG  | NET_IP_HDR_OPT_NBR_TS               )
#define  NET_IP_HDR_OPT_ROUTE_REC            (                           NET_IP_HDR_OPT_CLASS_CTRL | NET_IP_HDR_OPT_NBR_ROUTE_REC        )
#define  NET_IP_HDR_OPT_ROUTE_SRC_STRICT     (NET_IP_HDR_OPT_COPY_FLAG | NET_IP_HDR_OPT_CLASS_CTRL | NET_IP_HDR_OPT_NBR_ROUTE_SRC_STRICT )

#define  NET_IP_HDR_OPT_PAD                             0x00

                                                                /* ---------------- SRC/REC ROUTE OPTS ---------------- */
#define  NET_IP_OPT_ROUTE_PTR_OPT                          0    /* Ptr ix to       route opt itself.                    */
#define  NET_IP_OPT_ROUTE_PTR_ROUTE                        4    /* Ptr ix to first route (min legal ptr val).           */

                                                                /* --------------------- TS OPTS ---------------------- */
#define  NET_IP_OPT_TS_PTR_OPT                             0    /* Ptr ix to       TS    opt itself.                    */
#define  NET_IP_OPT_TS_PTR_TS                              4    /* Ptr ix to first TS    (min legal ptr val).           */

#define  NET_IP_OPT_TS_OVF_MASK                         0xF0
#define  NET_IP_OPT_TS_OVF_SHIFT                           4
#define  NET_IP_OPT_TS_OVF_MAX                            15

#define  NET_IP_OPT_TS_FLAG_MASK                        0x0F
#define  NET_IP_OPT_TS_FLAG_TS_ONLY                        0
#define  NET_IP_OPT_TS_FLAG_TS_ROUTE_REC                   1
#define  NET_IP_OPT_TS_FLAG_TS_ROUTE_SPEC                  3



typedef  CPU_INT32U  NET_IP_OPT_SIZE;                           /* IP opt size data type (see Note #4).                 */

#define  NET_IP_HDR_OPT_SIZE_WORD                (sizeof(NET_IP_OPT_SIZE))
#define  NET_IP_HDR_OPT_SIZE_MAX                        (NET_IP_HDR_SIZE_MAX - NET_IP_HDR_SIZE_MIN)

#define  NET_IP_HDR_OPT_SIZE_ROUTE                       NET_IP_HDR_OPT_SIZE_WORD
#define  NET_IP_HDR_OPT_SIZE_TS                          NET_IP_HDR_OPT_SIZE_WORD
#define  NET_IP_HDR_OPT_SIZE_SECURITY                      3


#define  NET_IP_HDR_OPT_NBR_MIN                            0
#define  NET_IP_HDR_OPT_NBR_MAX                         (NET_IP_HDR_OPT_SIZE_MAX / NET_IP_HDR_OPT_SIZE_WORD)

#define  NET_IP_OPT_PARAM_NBR_MIN                          1

#define  NET_IP_OPT_PARAM_NBR_MAX_ROUTE                    9
#define  NET_IP_OPT_PARAM_NBR_MAX_TS_ONLY                  9
#define  NET_IP_OPT_PARAM_NBR_MAX_TS_ROUTE                 4

#define  NET_IP_OPT_PARAM_NBR_MAX                          9    /* Max nbr of 'max nbr opts'.                           */


#define  NET_IP_HDR_OPT_IX                               NET_IP_HDR_SIZE_MIN
#define  NET_IP_OPT_IX_RX                                  0


/*$PAGE*/
/*
*********************************************************************************************************
*                                IP OPTION CONFIGURATION TYPE DEFINES
*
* Note(s) : (1) NET_IP_OPT_CFG_TYPE_??? #define values specifically chosen as ASCII representations of
*               the IP option configuration types.  Memory displays of IP option configuration buffers
*               will display the IP option configuration TYPEs with their chosen ASCII names.
*********************************************************************************************************
*/

                                                                /* ----------------- IP OPT CFG TYPES ----------------- */
#if     (CPU_CFG_ENDIAN_TYPE == CPU_ENDIAN_TYPE_BIG)
#define  NET_IP_OPT_CFG_TYPE_NONE                 0x4E4F4E45    /* "NONE" in ASCII.                                     */
#define  NET_IP_OPT_CFG_TYPE_ROUTE_STRICT         0x52545354    /* "RTST" in ASCII.                                     */
#define  NET_IP_OPT_CFG_TYPE_ROUTE_LOOSE          0x52544C53    /* "RTLS" in ASCII.                                     */
#define  NET_IP_OPT_CFG_TYPE_ROUTE_REC            0x52545243    /* "RTRC" in ASCII.                                     */
#define  NET_IP_OPT_CFG_TYPE_TS_ONLY              0x54532020    /* "TS  " in ASCII.                                     */
#define  NET_IP_OPT_CFG_TYPE_TS_ROUTE_REC         0x54535243    /* "TSRC" in ASCII.                                     */
#define  NET_IP_OPT_CFG_TYPE_TS_ROUTE_SPEC        0x54535254    /* "TSRT" in ASCII.                                     */
#define  NET_IP_OPT_CFG_TYPE_SECURITY             0x53454320    /* "SEC " in ASCII (see 'net_ip.h  Note #1f').          */
#define  NET_IP_OPT_CFG_TYPE_SECURITY_EXTENDED    0x53454358    /* "SECX" in ASCII (see 'net_ip.h  Note #1f').          */

#else

#if     (CPU_CFG_DATA_SIZE   == CPU_WORD_SIZE_32)
#define  NET_IP_OPT_CFG_TYPE_NONE                 0x454E4F4E    /* "NONE" in ASCII.                                     */
#define  NET_IP_OPT_CFG_TYPE_ROUTE_STRICT         0x54535452    /* "RTST" in ASCII.                                     */
#define  NET_IP_OPT_CFG_TYPE_ROUTE_LOOSE          0x534C5452    /* "RTLS" in ASCII.                                     */
#define  NET_IP_OPT_CFG_TYPE_ROUTE_REC            0x43525452    /* "RTRC" in ASCII.                                     */
#define  NET_IP_OPT_CFG_TYPE_TS_ONLY              0x20205354    /* "TS  " in ASCII.                                     */
#define  NET_IP_OPT_CFG_TYPE_TS_ROUTE_REC         0x43525354    /* "TSRC" in ASCII.                                     */
#define  NET_IP_OPT_CFG_TYPE_TS_ROUTE_SPEC        0x54525354    /* "TSRT" in ASCII.                                     */
#define  NET_IP_OPT_CFG_TYPE_SECURITY             0x20434553    /* "SEC " in ASCII (see 'net_ip.h  Note #1f').          */
#define  NET_IP_OPT_CFG_TYPE_SECURITY_EXTENDED    0x58434553    /* "SECX" in ASCII (see 'net_ip.h  Note #1f').          */

#elif   (CPU_CFG_DATA_SIZE   == CPU_WORD_SIZE_16)
#define  NET_IP_OPT_CFG_TYPE_NONE                 0x4F4E454E    /* "NONE" in ASCII.                                     */
#define  NET_IP_OPT_CFG_TYPE_ROUTE_STRICT         0x54525453    /* "RTST" in ASCII.                                     */
#define  NET_IP_OPT_CFG_TYPE_ROUTE_LOOSE          0x5452534C    /* "RTLS" in ASCII.                                     */
#define  NET_IP_OPT_CFG_TYPE_ROUTE_REC            0x54524352    /* "RTRC" in ASCII.                                     */
#define  NET_IP_OPT_CFG_TYPE_TS_ONLY              0x53542020    /* "TS  " in ASCII.                                     */
#define  NET_IP_OPT_CFG_TYPE_TS_ROUTE_REC         0x53544352    /* "TSRC" in ASCII.                                     */
#define  NET_IP_OPT_CFG_TYPE_TS_ROUTE_SPEC        0x53545452    /* "TSRT" in ASCII.                                     */
#define  NET_IP_OPT_CFG_TYPE_SECURITY             0x45532043    /* "SEC " in ASCII (see 'net_ip.h  Note #1f').          */
#define  NET_IP_OPT_CFG_TYPE_SECURITY_EXTENDED    0x45535843    /* "SECX" in ASCII (see 'net_ip.h  Note #1f').          */

#else                                                           /* Dflt CPU_WORD_SIZE_08.                               */
#define  NET_IP_OPT_CFG_TYPE_NONE                 0x4E4F4E45    /* "NONE" in ASCII.                                     */
#define  NET_IP_OPT_CFG_TYPE_ROUTE_STRICT         0x52545354    /* "RTST" in ASCII.                                     */
#define  NET_IP_OPT_CFG_TYPE_ROUTE_LOOSE          0x52544C53    /* "RTLS" in ASCII.                                     */
#define  NET_IP_OPT_CFG_TYPE_ROUTE_REC            0x52545243    /* "RTRC" in ASCII.                                     */
#define  NET_IP_OPT_CFG_TYPE_TS_ONLY              0x54532020    /* "TS  " in ASCII.                                     */
#define  NET_IP_OPT_CFG_TYPE_TS_ROUTE_REC         0x54535243    /* "TSRC" in ASCII.                                     */
#define  NET_IP_OPT_CFG_TYPE_TS_ROUTE_SPEC        0x54535254    /* "TSRT" in ASCII.                                     */
#define  NET_IP_OPT_CFG_TYPE_SECURITY             0x53454320    /* "SEC " in ASCII (see 'net_ip.h  Note #1f').          */
#define  NET_IP_OPT_CFG_TYPE_SECURITY_EXTENDED    0x53454358    /* "SECX" in ASCII (see 'net_ip.h  Note #1f').          */

#endif
#endif


/*
*********************************************************************************************************
*                                           IP FLAG DEFINES
*********************************************************************************************************
*/

                                                                /* ------------------- NET IP FLAGS ------------------- */
#define  NET_IP_FLAG_NONE                             DEF_BIT_NONE

                                                                /* IP tx flags copied from IP hdr flags.                */
#define  NET_IP_FLAG_TX_DONT_FRAG                     NET_IP_HDR_FLAG_FRAG_DONT


/*
*********************************************************************************************************
*                                    IP INTERNET TIMESTAMP DEFINES
*********************************************************************************************************
*/

#define  NET_IP_TS_NONE                           ((NET_TS)0)


/*$PAGE*/
/*
*********************************************************************************************************
*                                             DATA TYPES
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*                                 IP TYPE OF SERVICE (TOS) DATA TYPE
*********************************************************************************************************
*/

typedef  CPU_INT08U  NET_IP_TOS;


/*
*********************************************************************************************************
*                                   IP TIME TO LIVE (TTL) DATA TYPE
*********************************************************************************************************
*/

typedef  CPU_INT08U  NET_IP_TTL;


/*
*********************************************************************************************************
*                                        IP ADDRESS DATA TYPE
*
* Note(s) : (1) 'NET_IP_ADDR' pre-defined in 'net_type.h' PRIOR to all other network modules that require 
*                IP address data type.
*********************************************************************************************************
*/

#if 0                                                           /* See Note #1.                                         */
typedef  CPU_INT32U  NET_IP_ADDR;                               /* Defines IPv4 IP addr size.                           */
#endif


/*$PAGE*/
/*
*********************************************************************************************************
*                                             IP HEADER
*
* Note(s) : (1) See RFC #791, Section 3.1 for IP datagram header format.
*
*           (2) IP Version Number & Header Length are coded in the first octet of an IP header as follows :
*
*                         7 6 5 4   3 2 1 0
*                       ---------------------
*                       |  V E R  | H L E N |
*                       ---------------------
*
*                   where
*                           VER         IP version; currently 4 (see 'net_ip.h  Note #1')
*                           HLEN        IP Headers' length in 32-bit words; MUST be at least 5 (20-octet header) 
*                                           & MUST be less than or equal to 15 (60-octet header)
*
*           (3) Type of Service (TOS) is coded in the second octet of an IP header as follows (see 'RFC #1349
*               Type of Service in the Internet Protocol Suite' for required TOS implementation) :
*
*                        7   6   5   4   3   2   1   0
*                       --------------------------------
*                       |PRECEDENCE| D | T | R | C | 0 |
*                       --------------------------------
*
*                   where
*                           PRECEDENCE      Datagram Priority (see 'IP HEADER TYPE OF SERVICE (TOS) DEFINES') :
*                                               '000' - Lowest  "Routine"         Priority (default)
*                                               '111' - Highest "Network Control" Priority
*                           D               Datagram Delay Request :
*                                               '0' - Normal Delay       requested (default)
*                                               '1' - Low    Delay       requested
*                           T               Datagram Throughput Request :
*                                               '0' - Normal Throughput  requested (default)
*                                               '1' - High   Throughput  requested
*                           R               Datagram Reliability Request :
*                                               '0' - Normal Reliability requested (default)
*                                               '1' - High   Reliability requested
*                           C               Datagram Cost Request :
*                                               '0' - Normal Cost        requested (default)
*                                               '1' - Low    Cost        requested
*                           0               MUST be zero; i.e. '0'
*
*           (4) Flags & Fragment Offset are coded in the seventh & eighth octets of an IP header as follows :
*
*                        15 14 13  12 11 10 9 8 7 6 5 4 3 2 1 0
*                       ----------------------------------------
*                       | 0 DF MF |      FRAGMENT OFFSET       |
*                       ----------------------------------------
*
*                   where
*                           0                   MUST be zero; i.e. '0'
*                           DF                  'Do Not Fragment' Flag :
*                                                   '0' - Datagram fragmentation     allowed
*                                                   '1' - Datagram fragmentation NOT allowed
*                           MF                  'More Fragments' Flag :
*                                                   '0' - No more fragments for datagram; i.e. last fragment
*                                                   '1' -    More fragments for datagram
*                           FRAGMENT OFFSET     Offset of fragment in original datagram, measured in units of
*                                                   8 octets (64 bits)
*
*           (5) Supports ONLY a sub-set of allowed protocol numbers :
*
*               (a) ICMP
*               (b) UDP
*               (c) TCP
*
*               See also 'net.h  Note #1a';
*                  & see 'RFC #1340  Assigned Numbers' for a complete list of protocol numbers.
*********************************************************************************************************
*/

                                                                /* -------------------- NET IP HDR -------------------- */
typedef  struct  net_ip_hdr {
    CPU_INT08U          Ver_HdrLen;                             /* IP datagram ver nbr/hdr len   (see Note #2).         */
    NET_IP_TOS          TOS;                                    /* IP datagram TOS               (see Note #3).         */
    CPU_INT16U          TotLen;                                 /* IP datagram tot len.                                 */
    CPU_INT16U          ID;                                     /* IP datagram ID.                                      */
    CPU_INT16U          Flags_FragOffset;                       /* IP datagram flags/frag offset (see Note #4).         */
    NET_IP_TTL          TTL;                                    /* IP datagram TTL.                                     */
    CPU_INT08U          Protocol;                               /* IP datagram protocol          (see Note #5).         */
    NET_CHK_SUM         ChkSum;                                 /* IP datagram chk sum.                                 */
    NET_IP_ADDR         AddrSrc;                                /* IP datagram src  addr.                               */
    NET_IP_ADDR         AddrDest;                               /* IP datagram dest addr.                               */
    NET_IP_OPT_SIZE     Opts[NET_IP_HDR_OPT_NBR_MAX];           /* IP datagram opts (if any).                           */
} NET_IP_HDR;


/*$PAGE*/
/*
*********************************************************************************************************
*                                     IP HEADER OPTION DATA TYPES
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*                                  IP SOURCE ROUTE OPTION DATA TYPE
*
* Note(s) : (1) See the following RFC's for Source Route options summary :
*
*               (a) RFC # 791, Section 3.1         'Options : Loose/Strict Source & Record Route'
*               (b) RFC #1122, Section 3.2.1.8.(c)
*
*           (2) Used for both Source Route options & Record Route options :
*
*               (a) NET_IP_HDR_OPT_ROUTE_SRC_LOOSE
*               (b) NET_IP_HDR_OPT_ROUTE_SRC_STRICT
*               (c) NET_IP_HDR_OPT_ROUTE_REC
*
*           (3) 'Route' declared with 1 entry; prevents removal by compiler optimization.
*********************************************************************************************************
*/

typedef  struct  net_ip_opt_src_route {
    CPU_INT08U          Type;                                   /* Src route type (see Note #2).                        */
    CPU_INT08U          Len;                                    /* Len of   src route opt (in octets).                  */
    CPU_INT08U          Ptr;                                    /* Ptr into src route opt (octet-ix'd).                 */
    CPU_INT08U          Pad;                                    /* ???? Forced word-alignment pad octet.                */
    NET_IP_ADDR         Route[1];                               /* Src route IP addrs (see Note #3).                    */
} NET_IP_OPT_SRC_ROUTE;


/*
*********************************************************************************************************
*                               IP INTERNET TIMESTAMP OPTION DATA TYPE
*
* Note(s) : (1) See the following RFC's for Internet Timestamp option summary :
*
*               (a) RFC # 791, Section 3.1         'Options : Internet Timestamp'
*               (b) RFC #1122, Section 3.2.1.8.(e)
*
*           (2) 'TS'/'Route'/'Route_TS' declared with 1 entry; prevents removal by compiler optimization.
*********************************************************************************************************
*/

typedef  struct  net_ip_opt_ts {
    CPU_INT08U          Type;                                   /* TS type.                                             */
    CPU_INT08U          Len;                                    /* Len of   src route opt (in octets).                  */
    CPU_INT08U          Ptr;                                    /* Ptr into src route opt (octet-ix'd).                 */
    CPU_INT08U          Ovf_Flags;                              /* Ovf/Flags.                                           */
    NET_TS              TS[1];                                  /* Timestamps (see Note #2).                            */
} NET_IP_OPT_TS;



typedef  struct  net_ip_route_ts {
    NET_IP_ADDR         Route[1];                               /* Route IP addrs (see Note #2).                        */
    NET_TS              TS[1];                                  /* Timestamps     (see Note #2).                        */
} NET_IP_ROUTE_TS;

#define  NET_IP_OPT_TS_ROUTE_SIZE                (sizeof(NET_IP_ROUTE_TS))


typedef  struct  net_ip_opt_ts_route {
    CPU_INT08U          Type;                                   /* TS type.                                             */
    CPU_INT08U          Len;                                    /* Len of   src route opt (in octets).                  */
    CPU_INT08U          Ptr;                                    /* Ptr into src route opt (octet-ix'd).                 */
    CPU_INT08U          Ovf_Flags;                              /* Ovf/Flags.                                           */
    NET_IP_ROUTE_TS     Route_TS[1];                            /* Route IP addrs / TS (see Note #2).                   */
} NET_IP_OPT_TS_ROUTE;


/*$PAGE*/
/*
*********************************************************************************************************
*                              IP HEADER OPTION CONFIGURATION DATA TYPES
*
* Note(s) : (1) RFC #1122, Section 3.2.1.8 states that "there MUST be a means ... to specify IP options 
*               to included in transmitted IP datagrams".
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*                    IP ROUTE & INTERNET TIMESTAMP OPTIONS CONFIGURATION DATA TYPE
*
* Note(s) : (1) 'NET_IP_OPT_CFG_ROUTE_TS' data type used to configure IP Route & Internet Timestamp transmit 
*               options :
*
*               (a) Type        specifies the desired IP option configuration type
*               (b) Nbr         specifies the desired number of option entries to allocate
*               (c) Route       specifies the desired IP addresses for Route or Internet Timestamp
*               (d) TS          specifies the desired Internet Timestamps
*********************************************************************************************************
*/

typedef  struct  net_ip_opt_cfg_route_ts {
    NET_TYPE            Type;                                   /* IP opt type.                                         */
    CPU_INT08U          Nbr;                                    /* IP opt nbr.                                          */
    NET_IP_ADDR         Route[NET_IP_OPT_PARAM_NBR_MAX];        /* IP route addrs.                                      */
    NET_TS              TS[NET_IP_OPT_PARAM_NBR_MAX];           /* IP TS's.                                             */
    void               *NextOptPtr;                             /* Ptr to next IP opt cfg.                              */
} NET_IP_OPT_CFG_ROUTE_TS;


/*
*********************************************************************************************************
*                             IP SECURITY OPTIONS CONFIGURATION DATA TYPE
*
* Note(s) : (1) IP Security options NOT currently supported (see 'net_ip.h  Note #1f').
*********************************************************************************************************
*/

typedef  struct  net_ip_opt_cfg_security {
    NET_TYPE            Type;                                   /* IP opt type.                                         */

    void               *NextOptPtr;                             /* Ptr to next IP cfg opt.                              */
} NET_IP_OPT_CFG_SECURITY;


/*$PAGE*/
/*
*********************************************************************************************************
*                                          GLOBAL VARIABLES
*********************************************************************************************************
*/

NET_IP_EXT  NET_IP_ADDR    NetIP_AddrThisHost;                  /* This host's IP addr      (see 'net_ip.h  Note #1a'). */
NET_IP_EXT  NET_IP_ADDR    NetIP_AddrThisHostNetOrder;          /* This host's IP addr in net-order.                    */
NET_IP_EXT  NET_IP_ADDR    NetIP_AddrThisHostSubnetMask;        /* This host's IP subnet net  mask.                     */
NET_IP_EXT  NET_IP_ADDR    NetIP_AddrThisHostSubnetMaskHost;    /* This host's IP subnet host mask.                     */
NET_IP_EXT  NET_IP_ADDR    NetIP_AddrThisHostSubnetNet;         /* This host's IP subnet      net.                      */
NET_IP_EXT  NET_IP_ADDR    NetIP_AddrDfltGateway;               /* This host's dflt gateway (see 'net_ip.h  Note #1c'). */


NET_IP_EXT  NET_BUF       *NetIP_FragReasmListsHead;            /* Ptr to head of frag reasm lists.                     */
NET_IP_EXT  NET_BUF       *NetIP_FragReasmListsTail;            /* Ptr to tail of frag reasm lists.                     */

NET_IP_EXT  CPU_INT08U     NetIP_FragReasmTimeout_sec;          /* IP frag reasm timeout (in secs ).                    */
NET_IP_EXT  NET_TMR_TICK   NetIP_FragReasmTimeout_tick;         /* IP frag reasm timeout (in ticks).                    */


NET_IP_EXT  CPU_INT16U     NetIP_TxID_Ctr;                      /* Global tx ID field ctr.                              */


/*$PAGE*/
/*
*********************************************************************************************************
*                                               MACRO'S
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*                                          NET_IP_TX_GET_ID()
*
* Description : Get next IP transmit identification number.
*
* Argument(s) : id      Variable that will receive the returned IP transmit identification number.
*
* Return(s)   : none.
*
* Caller(s)   : NetIP_TxPktPrepareHdr(),
*               NetIP_ReTxPktPrepareHdr().
*
*               This macro is an INTERNAL network protocol suite macro & SHOULD NOT be called by 
*               application function(s).
*
* Note(s)     : (1) Return IP identification number is NOT converted from host-order to network-order.
*********************************************************************************************************
*/

#define  NET_IP_TX_GET_ID(id)                       { NET_UTIL_VAL_COPY_16(&(id), &NetIP_TxID_Ctr); \
                                                      NetIP_TxID_Ctr++;                             }


/*$PAGE*/
/*
*********************************************************************************************************
*                                         FUNCTION PROTOTYPES
*********************************************************************************************************
*/

void          NetIP_Init                  (void);



                                                                            /* -------------- CFG FNCTS --------------- */
CPU_BOOLEAN   NetIP_CfgAddrThisHost       (NET_IP_ADDR   addr_host,
                                           NET_IP_ADDR   addr_subnet_mask);

CPU_BOOLEAN   NetIP_CfgAddrDfltGateway    (NET_IP_ADDR   addr_dflt_gateway);


CPU_BOOLEAN   NetIP_CfgFragReasmTimeout   (CPU_INT08U    timeout_sec);



                                                                            /* -------------- GET FNCTS --------------- */
NET_IP_ADDR   NetIP_GetAddrThisHost       (void);

NET_IP_ADDR   NetIP_GetAddrThisHostHandler(NET_ERR      *perr);


NET_IP_ADDR   NetIP_GetAddrSubnetMask     (void);

NET_IP_ADDR   NetIP_GetAddrDfltGateway    (void);



CPU_CHAR     *NetIP_GetAddrStrThisHost    (CPU_CHAR     *paddr);

CPU_CHAR     *NetIP_GetAddrStrSubnetMask  (CPU_CHAR     *paddr);

CPU_CHAR     *NetIP_GetAddrStrDfltGateway (CPU_CHAR     *paddr);



                                                                            /* ------------- STATUS FNCTS ------------- */
CPU_BOOLEAN   NetIP_IsAddrClassA          (NET_IP_ADDR   addr);

CPU_BOOLEAN   NetIP_IsAddrClassB          (NET_IP_ADDR   addr);

CPU_BOOLEAN   NetIP_IsAddrClassC          (NET_IP_ADDR   addr);

CPU_BOOLEAN   NetIP_IsAddrThisHost        (NET_IP_ADDR   addr);

CPU_BOOLEAN   NetIP_IsAddrLocalHost       (NET_IP_ADDR   addr);

CPU_BOOLEAN   NetIP_IsAddrLocalLink       (NET_IP_ADDR   addr);

CPU_BOOLEAN   NetIP_IsAddrBroadcast       (NET_IP_ADDR   addr);


CPU_BOOLEAN   NetIP_IsAddrHost            (NET_IP_ADDR   addr);

CPU_BOOLEAN   NetIP_IsAddrHostHandler     (NET_IP_ADDR   addr);


CPU_BOOLEAN   NetIP_IsAddrHostCfgd        (NET_IP_ADDR   addr);

CPU_BOOLEAN   NetIP_IsAddrHostCfgdHandler (NET_IP_ADDR   addr);



CPU_BOOLEAN   NetIP_IsValidAddrHost       (NET_IP_ADDR   addr_host);        /* Validate an IP host addr.                */

CPU_BOOLEAN   NetIP_IsValidAddrThisHost   (NET_IP_ADDR   addr_host,         /* Validate this  host's IP addr.           */
                                           NET_IP_ADDR   addr_subnet_mask);
                                                                            /* Validate an IP addr subnet mask.         */
CPU_BOOLEAN   NetIP_IsValidAddrSubnetMask (NET_IP_ADDR   addr_subnet_mask);



/*$PAGE*/
                                                                            /* --------------- RX FNCTS --------------- */
void          NetIP_Rx                    (NET_BUF      *pbuf,
                                           NET_ERR      *perr);



                                                                            /* --------------- TX FNCTS --------------- */
void          NetIP_Tx                    (NET_BUF      *pbuf,              /* Prepare & tx IP pkts.                    */
                                           NET_IP_ADDR   addr_src,
                                           NET_IP_ADDR   addr_dest,
                                           NET_IP_TOS    TOS,
                                           NET_IP_TTL    TTL,
                                           CPU_INT16U    flags,
                                           void         *popts,
                                           NET_ERR      *perr);

void          NetIP_ReTx                  (NET_BUF      *pbuf,              /* Prepare & re-tx IP pkts.                 */
                                           NET_ERR      *perr);


/*$PAGE*/
/*
*********************************************************************************************************
*                                        CONFIGURATION ERRORS
*********************************************************************************************************
*/

