#include "hal_process_SERVES.h"
#include "hal_process_CONTROL.h"
#include "hal_process_COLLECT.h"
#include "stdlib.h"
#include "bsp_usart.h"

/*
*********************************************************************************************************
*                                       LOCAL GLOBAL VARIABLES
*********************************************************************************************************
*/
#define TCPSERVER_PORT_NO   6000    /* tcpserver 使用端口号 */
#define TCP_SERVER_RX_BUFSIZE	  600		//定义tcp server最大接收数据长度
#define TCP_SERVER_TX_BUFSIZE   600

#define DEF_ADDR_FALSH_UPDATA   0

u8 tcp_server_recvbuf[TCP_SERVER_RX_BUFSIZE];	//TCP客户端接收数据缓冲区
u8 tcp_server_sendbuf[TCP_SERVER_TX_BUFSIZE];	//TCP客户端发送数据缓冲区

TYPEDEF_NETCMD     g_netcmd = {0};
u32 g_updata_timeout = 0;
/*
*********************************************************************************************************
*                                       LOCAL GLOBAL FUNCTION
*********************************************************************************************************
*/

static u8  NetCmdProcess(u8 *src);
static u8  NetProtocolProcess( u8 *src , u16 len);

/*
*********************************************************************************************************
*
*                       TCP server task function
*
**********************************************************************************************************
*/
static struct netconn *conn, *newconn;
void tcp_server_process(void)	
{
	  err_t err,recv_err;
	  static ip_addr_t ipaddr;
	  static u16_t 			port;
	  struct pbuf *q;
	  u32 data_len = 0;
	
		myPrintf("---------------------ip information-------------------\r\n");
		Display_IPAddress();
		myPrintf("------------------------------------------------------\r\n");
		conn = netconn_new(NETCONN_TCP);  //创建一个TCP链接
		netconn_bind(conn,IP_ADDR_ANY,TCPSERVER_PORT_NO);  //绑定端口 
		netconn_listen(conn);  //进入监听模式
		conn->recv_timeout = 10;  //禁止阻塞线程 等待10ms
		

		while (1) 
	  {
				err = netconn_accept(conn,&newconn);  //接收连接请求
				if(err==ERR_OK) 
					  newconn->recv_timeout = 100;
				if (err == ERR_OK)    //处理新连接的数据
				{ 
						struct netbuf *recvbuf;
						netconn_getaddr(newconn,&ipaddr,&port,0); //获取远端IP地址和端口号
					  Mute_Net_Write((u8*)"==welcome to stm32 contrl server!==\r\n",strlen("==welcome to stm32 contrl server!==\r\n"));
					  myPrintf("client connect----ip address: %s,port:%d\r\n", inet_ntoa(*((struct ip_addr_t*)&(ipaddr))) , port);
					 
					  while(1)
						{
							    //网络数据接收处理
									if((recv_err = netconn_recv(newconn,&recvbuf)) == ERR_OK)  	//接收到数据
									{
											memset(tcp_server_recvbuf,0,TCP_SERVER_RX_BUFSIZE);  //数据接收缓冲区清零
											for(q=recvbuf->p;q!=NULL;q=q->next)  //遍历完整个pbuf链表
											{
												//判断要拷贝到TCP_SERVER_RX_BUFSIZE中的数据是否大于TCP_SERVER_RX_BUFSIZE的剩余空间，如果大于
												//的话就只拷贝TCP_SERVER_RX_BUFSIZE中剩余长度的数据，否则的话就拷贝所有的数据
												if(q->len > (TCP_SERVER_RX_BUFSIZE-data_len)) 
													memcpy(tcp_server_recvbuf+data_len,q->payload,(TCP_SERVER_RX_BUFSIZE-data_len));//拷贝数据
												else 
													memcpy(tcp_server_recvbuf+data_len,q->payload,q->len);
												data_len += q->len;  	
												if(data_len > TCP_SERVER_RX_BUFSIZE) break; //超出TCP客户端接收数组,跳出	
											}
											if(CheckFrame(tcp_server_recvbuf,data_len))//协议数据
											{
												  NetProtocolProcess( tcp_server_recvbuf , data_len);
											}
											else//调试指令
											{
													NetCmdProcess(tcp_server_recvbuf);/*net debuf cmd process*/
											}
											data_len=0;  //复制完成后data_len要清零。	
											netbuf_delete(recvbuf);
									}							
									else if(recv_err == ERR_CLSD)  //关闭连接
									{
										netbuf_delete(recvbuf);
										netconn_close(newconn);
										netconn_delete(newconn);
										myPrintf("client closed\r\n");
										break;
									}
									if(g_updata_timeout!=0  && OSTmr_Count_Get(&g_updata_timeout) > 20000)//升级等待超时
									{
										 myPrintf("Timeout updata, reset cpu now!\r\n");
										 BSP_CPU_Reset();
									}
									OSTimeDly(1);
						}
				}
				OSTimeDly(300);		
		}
	
	
}

/*
*********************************************************************************************************
*                       网络调试命令解析函数
**********************************************************************************************************
*/
static u8  NetCmdProcess(u8 *src)
{
	  static char cstr[TCP_SERVER_RX_BUFSIZE] = {0};
		memset(cstr,0,TCP_SERVER_RX_BUFSIZE);
		strcpy(cstr,(char*)src); 

    if(0 == strncmp((char*)cstr , "STOPALL" , strlen("STOPALL:")))
		{
				MotorStopAll();
				MotorDisableAll();
		}
    else
		{			
				OSMboxPost(DebugMbox, (void *)&cstr);
		}			
		return 1;
}
/*
**********************************************************************
                 网络协议命令
**********************************************************************
*/

static u8  NetProtocolProcess( u8 *src , u16 len)
{
     static u8 revtmp[TCP_SERVER_RX_BUFSIZE] = {0};
     static u8 txdtmp[TCP_SERVER_TX_BUFSIZE] = {0};		 
		 static u32 size_bin = 0;
		 static u32 cout = 0;
		 u16 dl = 0;
		 u16 lret = 0;
		 
		 memset(revtmp, 0 , TCP_SERVER_RX_BUFSIZE);
		 memset(txdtmp, 0 , TCP_SERVER_TX_BUFSIZE);
     dl = UnPackData(src,len,revtmp);
     /*网络升级指令处理，先写到外部FLASH。重启后bootloader写入主FALSH*/
		 if(*(u16*)&revtmp[0] == 0x2700)//准备升级
		 {
					cout = 0; //清空字节计数
					size_bin = *(u32*)&revtmp[2];//获得文件大小
			    memcpy((u8*)&g_system_para.spiversion , &revtmp[6] , 6);
          OSTmr_Count_Start(&g_updata_timeout);//升级帧等待计时			
					myPrintf("Ready updata!\r\n");
			    /* 升级中挂起其他任务 */
			    OSTaskSuspend(APP_CFG_TASK_DATACOLLECT_PRIO);
			    OSTaskSuspend(APP_CFG_TASK_CONTROL_PRIO);		
			    OSTaskSuspend(APP_CFG_TASK_TOGGLELED_PRIO);
					lret = PackData(revtmp , 2 ,txdtmp);//组包应答 准备就绪
					Mute_Net_Write(txdtmp , lret );			 
		 }
		 else if(*(u16*)&revtmp[0] == 0x2701)//升级数据包
		 {
          OSTmr_Count_Start(&g_updata_timeout);//升级帧等待计时			 
					BSP_SPI_FLASH_BufferWrite(&revtmp[4] , DEF_ADDR_FALSH_UPDATA + cout, dl-4 );
					BSP_SPI_FLASH_BufferRead(&txdtmp[4] , DEF_ADDR_FALSH_UPDATA + cout, dl-4 );
			   if(memcmp(&revtmp[4],&txdtmp[4] ,dl-4) == 0)//正确写入
					{
						 cout+=(dl-4);
						 *(u16*)&revtmp[0]+=1;//应答命令2702
					}
					else//写入不正确 请求重发
					{
						 *(u16*)&revtmp[0]+=1;//应答命令2702
						 *(u16*)&revtmp[2] = 0;//应答包序号0表示接受失败					
					}
					lret = PackData(revtmp , 4 ,txdtmp);//组包应答帧	
					Mute_Net_Write(txdtmp , lret );
          if(cout == size_bin)//升级完成 重启进入boot
					{
						   g_system_para.upflag = 1;
						   g_system_para.spisize = size_bin;
					    
						   g_updata_timeout = 0;
							 if(BSP_Write_Para(&g_system_para))
							 {
								 myPrintf("Complete updata, reset cpu now!\r\n");
								 BSP_CPU_Reset();
							 }
					}						
		 }
		 /*无需等待回复的简单指令,此处添加指令可以和运动指令并行处理*/
		 else if(*(u16*)&revtmp[0] == 0x2902)//抱闸
		 {		 
					DEF_ENABLE_KOER;
					lret = PackData(revtmp , 2 ,txdtmp);
					Mute_Net_Write(txdtmp , lret );		
		 }
		 else if(*(u16*)&revtmp[0] == 0x2903)//松闸
		 {		 
					DEF_DISENABLE_KOER;
					lret = PackData(revtmp , 2 ,txdtmp);
					Mute_Net_Write(txdtmp , lret );	
		 }
		 else if(*(u16*)&revtmp[0] == 0x28FF)//紧急停止电机
		 {		 
					MotorStopAll();
					MotorDisableAll();
					BSP_CPU_Reset();		 
		 }
		 else if(*(u16*)&revtmp[0] == 0x28FE)//停止电机
		 {		 
			    g_stopflag = TRUE;
					while(!g_activerdy)
					{
						OSTimeDly(200);
					}
			    g_stopflag = FALSE;
		 }
		  /*复杂需要等待回复的指令*/
		 else
		 {
				g_stopflag = TRUE;
			  while(!g_activerdy)
				{
					OSTimeDly(200);
				}
				g_stopflag = FALSE;
			  OSMboxPost(NetCmdMbox, (void *)&revtmp);
		 }	
	 return 0;
}


void StringToParas(char* src , s32 *array)
{
	  u8 j = 0;
	  char *ptmp = NULL;
	  char *pcsrc = NULL;
    char ctmp[80] = {0};
		memset(ctmp , 0, 80);
		strcpy(ctmp , src);
		pcsrc = ctmp;
    while((ptmp = strstr(pcsrc , ",")) != NULL || (ptmp = strstr(pcsrc , "\r")) != NULL)
		{
        *ptmp = '\0';
			  array[j++] = atoi(pcsrc);
			  pcsrc = ptmp +1;
		}
}


s8 Mute_Net_Write(u8* txdtmp , u16 lret)
{
		u8 err = 0;
		if(newconn == NULL)
			return -2;
		OSSemPend(NetMutexSem,	3000,	&err);		
		if(err != OS_ERR_NONE)
			return	-1;

		netconn_write(newconn ,txdtmp , lret ,NETCONN_COPY);	
		OSSemPost(NetMutexSem);		
		return 0;
}



s8 NetPrintf(const char * format, ...)
{
  	va_list arg;
	  static char tmp[100];
		memset(tmp,0,100);
			
		va_start(arg,format);
		vsprintf((char*)tmp,format,arg);
		va_end(arg);
		Mute_Net_Write((u8*)tmp,strlen(tmp));

		return 0;
}


/*******************************************************************************************************************
                                   以下是网络协议函数
*******************************************************************************************************************/
/*
*********************************************************************************
             检测一个内存快中0x7d,0x7e,0x7f个数
data：		检测的内存指针
len：			内存有效数据长度
返回值：	出现目标字节的次数
*********************************************************************************
*/
u32 Check7D7E7FNum(u8 *data, u32 len)
{
		u32 iret = 0;
		int i = 0;
		for (; i != len; i++)
		{
			if (data[i] == 0x7F || data[i] == 0x7D || data[i] == 0x7E)
				iret++;
		}
		return iret;
}
/*
*********************************************************************************
                  按协议打包一个需要发送的DATA域
dataSrc：		源内存快指针
lenSrc：		源有效数据长度
dataDest：	打包后的内存快指针
返回值：		打包后内存快有效长度
*********************************************************************************
*/
u32 PackData(u8* dataSrc, u32 lenSrc, u8* dataDest)
{
		u32 ipos = 3;
		u8 sum = 0;
		int i = 0;
		for (; i != lenSrc; i++)
		{
			sum += dataSrc[i];
		}
		for (i = 0; i != lenSrc; i++)
		{
			if (dataSrc[i] == 0x7D)
			{
				dataDest[ipos++] = 0x7F;
				dataDest[ipos++] = 0x00;
			}
			else if (dataSrc[i] == 0x7E)
			{
				dataDest[ipos++] = 0x7F;
				dataDest[ipos++] = 0x01;
			}
			else if (dataSrc[i] == 0x7F)
			{
				dataDest[ipos++] = 0x7F;
				dataDest[ipos++] = 0x02;
			}
			else
				dataDest[ipos++] = dataSrc[i];
		}
		if (sum == 0x7D)
		{
			dataDest[ipos++] = 0x7F;
			dataDest[ipos++] = 0x00;
		}
		else if (sum == 0x7E)
		{
			dataDest[ipos++] = 0x7F;
			dataDest[ipos++] = 0x01;
		}
		else if (sum == 0x7F)
		{
			dataDest[ipos++] = 0x7F;
			dataDest[ipos++] = 0x02;
		}
		else
			dataDest[ipos++] = sum;
		dataDest[0] = 0x7D;
		dataDest[1] = (((ipos - 3) >> 2) & 0xF0);
		dataDest[2] = (ipos - 3 & 0x3F);
		dataDest[ipos] = 0x7E;
		return ipos + 1;
}
/*
*********************************************************************************
                  检测一段数据是否为指定协议帧
dataSrc：		源内存快指针
lenSrc：		源有效数据长度
返回值：		1：有效帧  0：无效帧
*********************************************************************************
*/
u8 CheckFrame(u8* dataSrc, u32 lenSrc)
{
		u32 len = 0;
		if (lenSrc <= 4)	return 0;
		if (dataSrc[0] == 0x7D && dataSrc[lenSrc - 1] == 0x7E)
		{
			len = ((u16)(dataSrc[1]&0xF0)<<2) | (dataSrc[2] & 0x003F);
		}
		if (lenSrc == (len + 4))
			return 1;
		return 0;
}
/*
*********************************************************************************
                  按协议解包一个需要发送的DATA域
dataSrc：		源内存快指针
lenSrc：		源有效数据长度
dataDest：	解包后的内存快指针
返回值：		解包后内存快有效长度
*********************************************************************************
*/
u32 UnPackData(u8* dataSrc, u32 lenSrc, u8* dataDest)
{
		u8 sum = 0;
		u32 ipos = 0;
		int i = 0;
		int j = 0;
		if (CheckFrame(dataSrc, lenSrc) == 0) return 0;
		dataSrc += 3;
		lenSrc -= 4;
	
		for (i = 0; i != lenSrc; i++)
		{
			if (dataSrc[i] == 0x7F && dataSrc[i + 1] == 0x00)
			{
				dataDest[ipos++] = 0x7D; i++;
			}
			else if (dataSrc[i] == 0x7F && dataSrc[i + 1] == 0x01)
			{
				dataDest[ipos++] = 0x7E; i++;
			}
			else if (dataSrc[i] == 0x7F && dataSrc[i + 1] == 0x02)
			{
				dataDest[ipos++] = 0x7F; i++;
			}
			else
				dataDest[ipos++] = dataSrc[i];
		}
		dataSrc -= 3;	
		for (j = 0; j != ipos - 1; j++)
		{
			sum += dataDest[j];
		}
		if (sum != dataDest[ipos - 1])
			return 0;
		return ipos - 1;
}
