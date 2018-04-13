#ifndef  _HAL_PROCESS_SERVERS_
#define  _HAL_PROCESS_SERVERS_

#include "bsp.h"
#include "ucos_ii.h"
#include "lwip/err.h"
#include "lwip/tcp.h"
#include "lwip/api.h"
#include "lwip/inet.h"

#define           NETCMD_NEXT               0x01
#define           NETCMD_HNEXT              0x02
#define           NETCMD_90DU               0x03 
#define           NETCMD_180DU              0x04
#define 					NETCMD_RUN_LF             0x05
#define 					NETCMD_RUN_FB             0x06
#define 					NETCMD_BEDUD              0x07

typedef struct TYPEDEF_NETCMD
{
	 u8 cmd;
	 s32 para[8];
}TYPEDEF_NETCMD;
typedef struct netconn NETCONN;


void tcp_server_process(void);

void StringToParas(char* src , s32 *array);
s8 NetPrintf(const char * format, ...);

s8 Mute_Net_Write(u8* txdtmp , u16 lret);
u32 Check7D7E7FNum(u8 *data, u32 len);
u32 PackData(u8* dataSrc, u32 lenSrc, u8* dataDest);
u8 CheckFrame(u8* dataSrc, u32 lenSrc);
u32 UnPackData(u8* dataSrc, u32 lenSrc, u8* dataDest);


#endif


