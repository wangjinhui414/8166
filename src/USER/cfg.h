#ifndef _CFG_H_
#define _CFG_H_

/*soft version*/
#define    DEF_VERSION        1.00

/*debug config*/
  
#define    DEF_DEBUG_CAN     0x00000001
#define    DEF_DEBUG_232     0x00000010
#define    DEF_DEBUF_485     0x00000100
#define    DEF_DEBUF_NET     0x00001000
#define    DEF_DEBUG_CFG    DEF_DEBUF_485 |DEF_DEBUG_232// DEF_DEBUG_CAN | DEF_DEBUF_485////DEF_DEBUG_232//| DEF_DEBUF_NET																									 
/*net config*/
#define   DEF_NET_IP           IP4_ADDR(&ipaddr, 192, 168, 1, 197);\
                               IP4_ADDR(&netmask, 255, 255, 255, 0);\
                               IP4_ADDR(&gw, 192, 168, 1, 1);

#define   DEF_NET_IP1           IP4_ADDR(&ipaddr, 192, 168, 33, 197);\
                               IP4_ADDR(&netmask, 255, 255, 255, 0);\
                               IP4_ADDR(&gw, 192, 168, 33, 11);

#define   DEF_NET_IP2           IP4_ADDR(&ipaddr, 192, 168, 31, 197);\
                               IP4_ADDR(&netmask, 255, 255, 255, 0);\
                               IP4_ADDR(&gw, 192, 168, 31, 20);
/*IWG config*/
#define  IWG_ISENABLE         0
#define	 SYSWDG_TIMEOUT			  12						




#endif

