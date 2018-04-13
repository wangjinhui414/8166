#include "hal_process_DEBUG.h"
#include "hal_process_SERVES.h"

void hal_process_debug(void)
{
    u8 err = 0;
	  
	  while(1)			
		{
				char* pcmd = (char*)OSMboxPend(DebugMbox , 0 , &err);//µÈ´ýÍøÂçÃüÁî
        if(pcmd != NULL)
				{
					  myPrintf("(START) %s\r\n",pcmd);
						if(0 == strncmp((char*)pcmd , "TURNTEST" , strlen("TURNTEST")))
						{
								TestTurnMotor();
						}
						else if(0 == strncmp((char*)pcmd , "MOVETEST" , strlen("MOVETEST")))
						{
								TestMoveMotor();
						}
						else if(0 == strncmp((char*)pcmd , "BEDTEST" , strlen("BEDTEST")))
						{
								TestBedMotor();
							  //RunBed(950);
						}
						else if(0 == strncmp((char*)pcmd , "RESETZEROWHEEL" , strlen("RESETZEROWHEEL")))
						{
								ResetZeroWheel();
						}
						else if(0 == strncmp((char*)pcmd , "RESETZEROBEDUP" , strlen("RESETZEROBEDUP")))
						{
							ResetZeroBedUp();
						}
						else if(0 == strncmp((char*)pcmd , "RESETZEROBEDDOWN" , strlen("RESETZEROBEDDOWN")))
						{
							RunBed(1000);
						}						
						else if(0 == strncmp((char*)pcmd , "57du" , strlen("57du")))
						{
								TurnToAngleVal(57);
						}
						else if(0 == strncmp((char*)pcmd , "90du" , strlen("90du")))
						{
								TurnToAngleVal(90);
						}
						else if(0 == strncmp((char*)pcmd , "0du" , strlen("0du")))
						{
								TurnToAngleVal(0);
						}
						else if(0 == strncmp((char*)pcmd , "STOPALL" , strlen("STOPALL")))
						{
								MotorStopAll();
								MotorDisableAll();
						}
						else if(0 == strncmp((char*)pcmd , "ENABLE_KOER" , strlen("ENABLE_KOER")))
						{
							  BSP_Write_DOUT1_24(HC595_DOUT_9 | HC595_DOUT_10 ,0);
						}
						else if(0 == strncmp((char*)pcmd , "DISENABLE_KOER" , strlen("DISENABLE_KOER")))
						{
							  BSP_Write_DOUT1_24(HC595_DOUT_9 | HC595_DOUT_10 ,1);
						}
						else if(0 == strncmp((char*)pcmd , "CMDREADBED" , strlen("CMDREADBED")))
						{
							  CMDReadBed();
						}
						else if(0 == strncmp((char*)pcmd , "CMDCLEARBED" , strlen("CMDCLEARBED")))
						{
							  CMDClearBed();
						}
						else if(0 == strncmp((char*)pcmd , "CMDREADMOVE" , strlen("CMDREADMOVE")))
						{
							  TYPEDEF_MOVE_MOTRO_POS movepos;
							  RevMovePosition(&movepos);
							  NetPrintf("MOVE-->2:%d  4:%d  6:%d  8:%d\r\n",movepos.pos2,movepos.pos4,movepos.pos6,movepos.pos8);
						}
						else if(0 == strncmp((char*)pcmd , "CMDREADTURN" , strlen("CMDREADTURN")))
						{
							  TYPEDEF_TURN_MOTRO_POS turnepos;
							  RevTurnPosition(&turnepos);
							  NetPrintf("Turn-->1:%d  3:%d  5:%d  7:%d\r\n",turnepos.pos1,turnepos.pos3,turnepos.pos5,turnepos.pos7);
						}						
						
						else if(0 == strncmp((char*)pcmd , "READMOTORERR" , strlen("READMOTORERR")))
						{
							  u32 err[10] = {0};
								MotorReadError(ID_TX_MOTOR_TURN1 , ID_RX_MOTOR_TURN1 ,&err[0]);
								MotorReadError(ID_TX_MOTOR_TURN3 , ID_RX_MOTOR_TURN3 ,&err[1]);
								MotorReadError(ID_TX_MOTOR_TURN5 , ID_RX_MOTOR_TURN5 ,&err[2]);
								MotorReadError(ID_TX_MOTOR_TURN7 , ID_RX_MOTOR_TURN7 ,&err[3]);
								MotorReadError(ID_TX_MOTOR_MOVE2 , ID_RX_MOTOR_MOVE2 ,&err[4]);	
								MotorReadError(ID_TX_MOTOR_MOVE4 , ID_RX_MOTOR_MOVE4 ,&err[5]);	
								MotorReadError(ID_TX_MOTOR_MOVE6 , ID_RX_MOTOR_MOVE6 ,&err[6]);		
								MotorReadError(ID_TX_MOTOR_MOVE8 , ID_RX_MOTOR_MOVE8 ,&err[7]);
								MotorReadError(ID_TX_MOTOR_BED9  , ID_TX_MOTOR_BED9  ,&err[8]);		
								MotorReadError(ID_TX_MOTOR_BED10 , ID_TX_MOTOR_BED10 ,&err[9]);
								NetPrintf("1:%08X,2:%08X,3:%08X,4:%08X,5:%08X,6:%08X,7:%08X,8:%08X,9:%08X,10:%08X\r\n",err[0],err[1],err[2],err[3],err[4],err[5],err[6],err[7],err[8],err[9]);
						}
						else if(0 == strncmp((char*)pcmd , "CLEARMOTORERR" , strlen("CLEARMOTORERR")))
						{
								MotorClearError(ID_TX_MOTOR_TURN1 , ID_RX_MOTOR_TURN1 );
								MotorClearError(ID_TX_MOTOR_TURN3 , ID_RX_MOTOR_TURN3 );
								MotorClearError(ID_TX_MOTOR_TURN5 , ID_RX_MOTOR_TURN5 );
								MotorClearError(ID_TX_MOTOR_TURN7 , ID_RX_MOTOR_TURN7 );
								MotorClearError(ID_TX_MOTOR_MOVE2 , ID_RX_MOTOR_MOVE2 );	
								MotorClearError(ID_TX_MOTOR_MOVE4 , ID_RX_MOTOR_MOVE4 );	
								MotorClearError(ID_TX_MOTOR_MOVE6 , ID_RX_MOTOR_MOVE6 );		
								MotorClearError(ID_TX_MOTOR_MOVE8 , ID_RX_MOTOR_MOVE8 );
								MotorClearError(ID_TX_MOTOR_BED9  , ID_TX_MOTOR_BED9  );		
								MotorClearError(ID_TX_MOTOR_BED10 , ID_TX_MOTOR_BED10 );							
						}
						else if(0 == strncmp((char*)pcmd , "RUNBED:" , strlen("RUNBED:")))
						{
								s32 para[8] = {0};
								StringToParas(strstr((char*)pcmd , ":")+1 , para);
							  RunBed(para[0]);
						}						
						else if(0 == strncmp((char*)pcmd , "TURNDU:" , strlen("TURNDU:")))
						{
							  s32 para[8] = {0};
								StringToParas(strstr((char*)pcmd , ":")+1 , para);
								TurnToAngleVal(para[0]);
						}
						else if(0 == strncmp((char*)pcmd , "RORATERUN:" , strlen("RORATERUN:")))
						{
  							 s32 para[8] = {0};
								 StringToParas(strstr((char*)pcmd , ":")+1 , para);
                 ContrlRunRorate(para[0],para[1],para[2],para[3]);
						}
						else if(0 == strncmp((char*)pcmd , "RUNFB:" , strlen("RUNFB:")))
						{
  							 s32 para[8] = {0};
								 StringToParas(strstr((char*)pcmd , ":")+1 , para);
                 ContrlRunFB(para[0],para[1],para[2],para[3]);
						}
						else if(0 == strncmp((char*)pcmd , "RUNLR:" , strlen("RUNLR:")))
						{
  							 s32 para[8] = {0};
								 StringToParas(strstr((char*)pcmd , ":")+1 , para);
                 ContrlRunLR(para[0],para[1],para[2],para[3]);
						}						
						else if(0 == strncmp((char*)pcmd , "RUNLIMITFB:" , strlen("RUNLIMITFB:")))
						{
  							 s32 para[8] = {0};
								 StringToParas(strstr((char*)pcmd , ":")+1 , para);				
                 ContrlRunLimitedFB(para[0],para[1],para[2],para[3],para[4]);
						}
						else if(0 == strncmp((char*)pcmd , "RUNLIMITLR:" , strlen("RUNLIMITLR:")))
						{
  							 s32 para[8] = {0};
								 StringToParas(strstr((char*)pcmd , ":")+1 , para);				
                 ContrlRunLimitedLR(para[0],para[1],para[2],para[3],para[4]);
						}
						else if(0 == strncmp((char*)pcmd , "READINFO" , strlen("READINFO")))
						{
                  BSP_Read_Para(&g_system_para);
							    myPrintf("version:%s\r\nsize:%d\r\nspiversin:%s\r\nspisize:%d\r\nkm:%f\r\n",g_system_para.version,g_system_para.size,g_system_para.spiversion,g_system_para.spisize,g_system_para.km);
						}
						else if(0 == strncmp((char*)pcmd , "REDON" , strlen("REDON")))
						{						
								BSP_Write_DOUT1_24(HC595_DOUT_1 ,1);
						}
						else if(0 == strncmp((char*)pcmd , "GREENON" , strlen("GREENON")))
						{						
								BSP_Write_DOUT1_24(HC595_DOUT_1 ,0);
						}						
						else if(0 == strncmp((char*)pcmd , "CPURESET" , strlen("CPURESET")))
						{						
               BSP_CPU_Reset();
						}
						
            myPrintf("(END) %s\r\n",pcmd);						
				}
				OSTimeDly(10);
		}

	
}
