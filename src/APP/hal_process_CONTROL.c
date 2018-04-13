#include "hal_process_CONTROL.h"
#include "hal_process_SERVES.h"
#include "hal_process_COLLECT.h"
#include "Patrol_control.h"

bool g_stopflag = FALSE;
bool g_activerdy = FALSE;
u32 g_limitruntimeout = 60000;

void hal_process_control(void)
{
		u8 err = 0;
	  u8 *pcmd = NULL;
    u8 slen = 0;
    static u8 btmp[50]={0};	
	  while(1)			
		{
			  memset(btmp,0,50);
        g_stopflag = FALSE;
        g_activerdy = TRUE;			
				pcmd = (u8*)OSMboxPend(NetCmdMbox,	0,	&err);//µ»¥˝Õ¯¬Á√¸¡
			  g_activerdy = FALSE;
			  memcpy(btmp,pcmd,50);
        switch(*(u16*)btmp)
				{
					case 0x2800:
						ResetZeroWheel();
						slen = PackData(btmp , 2 ,btmp);
						Mute_Net_Write(btmp , slen );		
						break;
					case 0x2801:
						TurnToAngleVal(*(u16*)&btmp[2]);
					  slen = PackData(btmp , 2 ,btmp);
						Mute_Net_Write(btmp , slen );						
						break;
					case 0x2811:
						ContrlRunFB(*(s8*)&btmp[2], *(u32*)&btmp[3] , *(u16*)&btmp[7] , *(u16*)&btmp[9]);
					  slen = PackData(btmp , 2 ,btmp);
						Mute_Net_Write(btmp , slen );						
						break;
					case 0x2812:
						ContrlRunLR(*(s8*)&btmp[2], *(u32*)&btmp[3] , *(u16*)&btmp[7] , *(u16*)&btmp[9]);
						slen = PackData(btmp , 2 ,btmp);
						Mute_Net_Write(btmp , slen );						
						break;
					case 0x2813:
						ContrlRunLimitedFB(*(s8*)&btmp[2], *(u32*)&btmp[3] , *(u16*)&btmp[7] , *(u16*)&btmp[9] , *(u16*)&btmp[11]);
						slen = PackData(btmp , 2 ,btmp);
						Mute_Net_Write(btmp , slen );						
						break;
					case 0x2814:
						ContrlRunLimitedLR(*(s8*)&btmp[2], *(u32*)&btmp[3] , *(u16*)&btmp[7] , *(u16*)&btmp[9] , *(u16*)&btmp[11]);
						slen = PackData(btmp , 2 ,btmp);
						Mute_Net_Write(btmp , slen );						
						break;
					case 0x2815:
						ContrlRunRorate(*(s8*)&btmp[2], *(u32*)&btmp[3] , *(u16*)&btmp[7] , *(u16*)&btmp[9]);
						slen = PackData(btmp , 2 ,btmp);
						Mute_Net_Write(btmp , slen );						
						break;
					case 0x2900:
						ResetZeroBedUp();
						slen = PackData(btmp , 2 ,btmp);
						Mute_Net_Write(btmp , slen );						
						break;
					case 0x2901:
						RunBed(*(u16*)&btmp[2]);
						slen = PackData(btmp , 2 ,btmp);
						Mute_Net_Write(btmp , slen );						
						break;
					default:
						break;					
				}

		}
}

/*****************************************************************************************************************************************************************
                                              NETCMD_RUN_LF        —∞«∞∫Û‘À∂Ø            
*****************************************************************************************************************************************************************/
bool  ContrlRunLimitedFB(s8 dir , u32 runlength , s32 maxspeed , s32 minspeed ,u16 golimit)
{
    u8 step = 0;
	  TYPEDEF_MOVE_MOTRO_POS movepos = {0};
		long tmplength = 0;
		u32 timeout = 0;
			
	  while(!g_stopflag)
		{
				switch(step)
				{
					case 0:
						OSTmr_Count_Start(&timeout);
					  g_movelimitcout = 0;//«Â≥˝œﬁŒªº∆ ˝
						if (!CheckTurnAngleVal(0))
						{
								TurnToAngleVal(0);
						}
						ZeroMoveMotorEncoder();//ÀÆ∆Ω‘À∂ØµÁª˙¬Î≈Ã«Â¡„
						ZeroMoveMotorEncoder();
						ZeroMoveMotorEncoder();
						step++;
						break;
					case 1:
						if (CheckTurnAngleVal(0))
						{ 	
								if(dir==1)
								{
										Tracking(Forward);
								}
								else
								{
										Tracking(Backward);
								}
								MoveFBSpeedSet(maxspeed*dir*(-1));
								step++;
						}
						break;
					case 2:
							 if(dir==1)
							 {
										Tracking(Forward);
							 }
							 else
							 {
									 Tracking(Backward);
							 }
							 RevMovePosition(&movepos);				   
							 tmplength = (ABS(movepos.pos2) + ABS(movepos.pos4) + ABS(movepos.pos6) + ABS(movepos.pos8) )/4;
							 if((runlength - tmplength) <  DecelerationDistance(maxspeed) )
							 {
									MoveFBSpeedChange(dir * minspeed*(-1));
									step++;
							 }

							 break;
					case 3:
							 if(dir==1)
							 {
										Tracking(Forward);
							 }
							 else
							 {
									 Tracking(Backward);
							 }
							 RevMovePosition(&movepos);				   
							 tmplength = (ABS(movepos.pos2) + ABS(movepos.pos4) + ABS(movepos.pos6) + ABS(movepos.pos8) )/4;						
							 if(g_movelimitcout >= golimit || tmplength>runlength)
							 {
									MotorStopAll();
									MotorDisableAll();
									ClearFpidBpid();
                  return TRUE;								 
							 }
							 break;
					default:
							 break;
				}
			  if(OSTmr_Count_Get(&timeout) > g_limitruntimeout)
				{
						MotorStopAll();
						MotorDisableAll();
						ClearFpidBpid();								
					  break;;
				}
			  OSTimeDly(5);
	  }
		MotorStopAll();
		MotorDisableAll();			 		   
	  return FALSE;
}

/*****************************************************************************************************************************************************************
                                              NETCMD_RUN_LF        —∞«∞∫Û‘À∂Ø            
*****************************************************************************************************************************************************************/
bool  ContrlRunLimitedLR(s8 dir , u32 runlength , s32 maxspeed , s32 minspeed ,u16 golimit)
{
    u8 step = 0;
	  TYPEDEF_MOVE_MOTRO_POS movepos = {0};
		long tmplength = 0;
		u32 timeout = 0;
			
	  while(!g_stopflag)
		{
				switch(step)
				{
					case 0:
						OSTmr_Count_Start(&timeout);
					  g_movelimitcout = 0;//«Â≥˝œﬁŒªº∆ ˝
						if (!CheckTurnAngleVal(90))
						{
								TurnToAngleVal(90);
						}
						ZeroMoveMotorEncoder();//ÀÆ∆Ω‘À∂ØµÁª˙¬Î≈Ã«Â¡„
						ZeroMoveMotorEncoder();
						ZeroMoveMotorEncoder();
						step++;
						break;
					case 1:
						if (CheckTurnAngleVal(90))
						{ 	
								if(dir==1)
								{
										TrackingLR(Right);
								}
								else
								{
										TrackingLR(Left);
								}
								MoveLRSpeedSet(maxspeed*dir*(-1));
								step++;
						}
						break;
					case 2:
							 RevMovePosition(&movepos);				   
							 tmplength = (ABS(movepos.pos2) + ABS(movepos.pos4) + ABS(movepos.pos6) + ABS(movepos.pos8) )/4;
							 if((runlength - tmplength) <  DecelerationDistance(maxspeed) )
							 {
									MoveLRSpeedChange(dir * minspeed*(-1));
									step++;
							 }
							 if(dir==1)
							 {
										TrackingLR(Right);
							 }
							 else
							 {
									 TrackingLR(Left);
							 }
							 break;
					case 3:
							 RevMovePosition(&movepos);				   
							 tmplength = (ABS(movepos.pos2) + ABS(movepos.pos4) + ABS(movepos.pos6) + ABS(movepos.pos8) )/4;						
							 if(g_movelimitcout >= golimit || tmplength>runlength)
							 {
									MotorStopAll();
									MotorDisableAll();
									ClearFpidBpid();
                  return TRUE;								 
							 }
							 break;
					default:
							 break;
				}
			  if(OSTmr_Count_Get(&timeout) > g_limitruntimeout)
				{
						MotorStopAll();
						MotorDisableAll();
						ClearFpidBpid();								
					  break;;
				}
			   OSTimeDly(5);
	  }
		MotorStopAll();
		MotorDisableAll();			 		   
	  return FALSE;
}

/*****************************************************************************************************************************************************************
                                              NETCMD_RUN_LF        ≤ª—∞–˝◊™               
*****************************************************************************************************************************************************************/
bool  ContrlRunRorate(s8 dir , u32 runlength , s32 maxspeed , s32 minspeed)
{
    u8 step = 0;
	  TYPEDEF_MOVE_MOTRO_POS movepos = {0};
		long tmplength = 0;
		u32 timeout = 0;
		
	  while(!g_stopflag)
		{
	       switch(step)
				 {
					 case 0:
						 OSTmr_Count_Start(&timeout);
						 if(!CheckTurnAngleVal(57))  
						     TurnToAngleVal(57);
						 step++;
						 break;
					 case 1:
						 ZeroMoveMotorEncoder();
					   ZeroMoveMotorEncoder();
					   step++;
					   break;
					 case 2:
						 if (CheckTurnAngleVal(57))
						 { 
								MoveRorateSpeedSet(dir * maxspeed);		 
								step++;
						 } 
						 break;
           case 3:
						 RevMovePosition(&movepos);				   
						 tmplength = (ABS(movepos.pos2) + ABS(movepos.pos4) + ABS(movepos.pos6) + ABS(movepos.pos8) )/4;
					   myPrintf("3runlength:%d  tmplength:%d  dec:%d\r\n",runlength,tmplength,DecelerationDistance(maxspeed));
					   if((runlength - tmplength) <  DecelerationDistance(maxspeed) )
						 {
							  MoveRorateSpeedChange(dir * minspeed);
							  step++;
						 }
             break;
           case 4:
						 RevMovePosition(&movepos);
					   tmplength = (ABS(movepos.pos2) + ABS(movepos.pos4) + ABS(movepos.pos6) + ABS(movepos.pos8) )/4;
					   myPrintf("4runlength:%d  tmplength:%d  dec:%d\r\n",runlength,tmplength,DecelerationDistance(maxspeed));
						 if (tmplength>= runlength)//11400
						 {
						     MotorStopAll();
								 MotorDisableAll();
							   return TRUE;
						 }
						 break;
					 default:
						 break;
				 }
         //±‹’œ¥¶¿Ì
         if(step == 3 || step == 4)
				 {

				 }
				 //≥¨ ±¥¶¿Ì
         if(OSTmr_Count_Get(&timeout) > g_limitruntimeout)
				 {
					   myPrintf("ContrlRunRorate timeout\r\n");
						 MotorStopAll();
						 MotorDisableAll();
					   break;
				 }					 
			   OSTimeDly(5);
		}
		MotorStopAll();
		MotorDisableAll();		
	  return FALSE;
}

void MoveRorateSpeedSet(long speed)
{
		if(speed > MaxMovingSpeed || speed < -MaxMovingSpeed)
	  {
			 //±®∏Ê∑˛ŒÒ∆˜ÀŸ∂»≥¨≥ˆ
		}
		MotorSetMode(ID_TX_MOTOR_MOVE2 , ID_RX_MOTOR_MOVE2 , MODE_SPEED);
		MotorSetMode(ID_TX_MOTOR_MOVE4 , ID_RX_MOTOR_MOVE4 , MODE_SPEED);
		MotorSetMode(ID_TX_MOTOR_MOVE6 , ID_RX_MOTOR_MOVE6 , MODE_SPEED);
		MotorSetMode(ID_TX_MOTOR_MOVE8 , ID_RX_MOTOR_MOVE8 , MODE_SPEED);
		OSTimeDlyHMSM(0,0,0,20);
		//2°¢ πƒ‹
		MotorEnable(ID_TX_MOTOR_MOVE2 , ID_RX_MOTOR_MOVE2);
		MotorEnable(ID_TX_MOTOR_MOVE4 , ID_RX_MOTOR_MOVE4);
		MotorEnable(ID_TX_MOTOR_MOVE6 , ID_RX_MOTOR_MOVE6);
		MotorEnable(ID_TX_MOTOR_MOVE8 , ID_RX_MOTOR_MOVE8);		
		//3°¢‘À––
		MotorVelocityRun(ID_TX_MOTOR_MOVE2 , ID_RX_MOTOR_MOVE2 , -speed*33.33333);
		MotorVelocityRun(ID_TX_MOTOR_MOVE4 , ID_RX_MOTOR_MOVE4 , -speed*33.33333);
		MotorVelocityRun(ID_TX_MOTOR_MOVE6 , ID_RX_MOTOR_MOVE6 , -speed*33.33333);
		MotorVelocityRun(ID_TX_MOTOR_MOVE8 , ID_RX_MOTOR_MOVE8 , -speed*33.33333);	
}
void MoveRorateSpeedChange(long speed)
{
		if(speed > MaxMovingSpeed || speed < -MaxMovingSpeed)
	  {
			 //±®∏Ê∑˛ŒÒ∆˜ÀŸ∂»≥¨≥ˆ
		}
		//1°¢‘À––
		MotorVelocityRun(ID_TX_MOTOR_MOVE2 , ID_RX_MOTOR_MOVE2 , -speed*33.33333);
		MotorVelocityRun(ID_TX_MOTOR_MOVE4 , ID_RX_MOTOR_MOVE4 , -speed*33.33333);
		MotorVelocityRun(ID_TX_MOTOR_MOVE6 , ID_RX_MOTOR_MOVE6 , -speed*33.33333);
		MotorVelocityRun(ID_TX_MOTOR_MOVE8 , ID_RX_MOTOR_MOVE8 , -speed*33.33333);	
}


/*****************************************************************************************************************************************************************
                                              NETCMD_RUN_LF        ≤ª—∞œﬂ◊Û”“‘À∂Ø               
*****************************************************************************************************************************************************************/
bool  ContrlRunLR(s8 dir , u32 runlength , u16 maxspeed , u16 minspeed)
{
    u8 step = 0;
	  TYPEDEF_MOVE_MOTRO_POS movepos = {0};
		long tmplength = 0;
		u32 timeout = 0;
		
	  while(!g_stopflag)
		{
	       switch(step)
				 {
					 case 0:
						 OSTmr_Count_Start(&timeout);
						 if(!CheckTurnAngleVal(90))  
						     TurnToAngleVal(90);
						 step++;
						 break;
					 case 1:
						 ZeroMoveMotorEncoder();
					   ZeroMoveMotorEncoder();
					   ZeroMoveMotorEncoder();
					   step++;
					   break;
					 case 2:
						 if (CheckTurnAngleVal(90))
						 { 
								MoveLRSpeedSet(dir * maxspeed);		 
								step++;
						 } 
						 break;
           case 3:
						 RevMovePosition(&movepos);				   
						 tmplength = (ABS(movepos.pos2) + ABS(movepos.pos4) + ABS(movepos.pos6) + ABS(movepos.pos8) )/4;
					   if((runlength - tmplength) <  DecelerationDistance(maxspeed) )
						 {
							  MoveLRSpeedChange(dir * minspeed);
							  step++;
						 }
             break;
           case 4:
						 RevMovePosition(&movepos);
					   tmplength = (ABS(movepos.pos2) + ABS(movepos.pos4) + ABS(movepos.pos6) + ABS(movepos.pos8) )/4;
						 if (tmplength>= runlength)//11400
						 {
								 MotorStopAll();
								 MotorDisableAll();
							   return TRUE;
						 }
						 break;
					 default:
						 break;
				 }
         //±‹’œ¥¶¿Ì
         if(step == 3 || step == 4)
				 {

				 }
				 //≥¨ ±¥¶¿Ì
         if(OSTmr_Count_Get(&timeout) > g_limitruntimeout)
				 {
					  myPrintf("ContrlRunLR timeout\r\n");
						 MotorStopAll();
						 MotorDisableAll();			 
					   break;
				 }					 
			   OSTimeDly(5);
		}
		MotorStopAll();
		MotorDisableAll();			 	
	  return FALSE;
}
void MoveLRSpeedSet(long speed)
{
		if(speed > MaxMovingSpeed || speed < -MaxMovingSpeed)
	  {
			 //±®∏Ê∑˛ŒÒ∆˜ÀŸ∂»≥¨≥ˆ
		}
		MotorSetMode(ID_TX_MOTOR_MOVE2 , ID_RX_MOTOR_MOVE2 , MODE_SPEED);
		MotorSetMode(ID_TX_MOTOR_MOVE4 , ID_RX_MOTOR_MOVE4 , MODE_SPEED);
		MotorSetMode(ID_TX_MOTOR_MOVE6 , ID_RX_MOTOR_MOVE6 , MODE_SPEED);
		MotorSetMode(ID_TX_MOTOR_MOVE8 , ID_RX_MOTOR_MOVE8 , MODE_SPEED);
		OSTimeDlyHMSM(0,0,0,20);
		//2°¢ πƒ‹
		MotorEnable(ID_TX_MOTOR_MOVE2 , ID_RX_MOTOR_MOVE2);
		MotorEnable(ID_TX_MOTOR_MOVE4 , ID_RX_MOTOR_MOVE4);
		MotorEnable(ID_TX_MOTOR_MOVE6 , ID_RX_MOTOR_MOVE6);
		MotorEnable(ID_TX_MOTOR_MOVE8 , ID_RX_MOTOR_MOVE8);		
		//3°¢‘À––
		if(FALSE == MotorVelocityRun(ID_TX_MOTOR_MOVE4 , ID_RX_MOTOR_MOVE4 , speed*33.33333) ||
		FALSE == MotorVelocityRun(ID_TX_MOTOR_MOVE6 , ID_RX_MOTOR_MOVE6 , speed*33.33333)||
		FALSE == MotorVelocityRun(ID_TX_MOTOR_MOVE2 , ID_RX_MOTOR_MOVE2 , -speed*33.33333) ||
		FALSE == MotorVelocityRun(ID_TX_MOTOR_MOVE8 , ID_RX_MOTOR_MOVE8 , -speed*33.33333))
		{
				 MotorStopAll();
				 MotorDisableAll();
		}	
}
void MoveLRSpeedChange(long speed)
{
		if(speed > MaxMovingSpeed || speed < -MaxMovingSpeed)
	  {
			 //±®∏Ê∑˛ŒÒ∆˜ÀŸ∂»≥¨≥ˆ
		}
		//1°¢‘À––
		if(FALSE == MotorVelocityRun(ID_TX_MOTOR_MOVE4 , ID_RX_MOTOR_MOVE4 , speed*33.33333) ||
		FALSE == MotorVelocityRun(ID_TX_MOTOR_MOVE6 , ID_RX_MOTOR_MOVE6 , speed*33.33333)||
		FALSE == MotorVelocityRun(ID_TX_MOTOR_MOVE2 , ID_RX_MOTOR_MOVE2 , -speed*33.33333) ||
		FALSE == MotorVelocityRun(ID_TX_MOTOR_MOVE8 , ID_RX_MOTOR_MOVE8 , -speed*33.33333))
		{
				 MotorStopAll();
				 MotorDisableAll();
		}	
}
/*****************************************************************************************************************************************************************
                                              NETCMD_RUN_FB        ≤ª—∞œﬂ«∞∫Û‘À∂Ø                  
*****************************************************************************************************************************************************************/
bool  ContrlRunFB(s8 dir , u32 runlength , u16 maxspeed , u16 minspeed)
{
    u8 step = 0;
	  TYPEDEF_MOVE_MOTRO_POS movepos = {0};
		long tmplength = 0;
		u32 timeout = 0;
		//u8 icout = 0;
		
	  while(!g_stopflag)
		{
	       switch(step)
				 {
					 case 0:
						 OSTmr_Count_Start(&timeout);
						 if(!CheckTurnAngleVal(0))  
						     TurnToAngleVal(0);
						 step++;
						 break;
					 case 1:
						 ZeroMoveMotorEncoder();
					   ZeroMoveMotorEncoder();
					   step++;
					   break;
					 case 2:
						 if (CheckTurnAngleVal(0))
						 { 
								MoveFBSpeedSet(dir * maxspeed);		 
								step++;
						 } 
						 break;
           case 3:
						 RevMovePosition(&movepos);				   
						 tmplength = (ABS(movepos.pos2) + ABS(movepos.pos4) + ABS(movepos.pos6) + ABS(movepos.pos8) )/4;
					   if((runlength - tmplength) <  DecelerationDistance(maxspeed) )
						 {
							  MoveFBSpeedChange(dir * minspeed);
							  step++;
						 }
             break;
           case 4:
						 RevMovePosition(&movepos);
					   tmplength = (ABS(movepos.pos2) + ABS(movepos.pos4) + ABS(movepos.pos6) + ABS(movepos.pos8) )/4;
						 if (tmplength>= runlength)//11400
						 {
								 MotorStopAll();
								 MotorDisableAll();
							   return TRUE;
						 }
						 break;
					 default:
						 break;
				 }
//         //±‹’œ¥¶¿Ì
//         if((step == 3 || step == 4) && biflag)
//				 {
//					   static u8 yflag = 0;
//             if(Barrier(0,4,0,4) && icout<20)    icout++;//≈–∂œ’œ∞≠¥Œ ˝Õ≥º∆ ¡¨–¯20¥Œ  100∫¡√Î“ª÷±¥Ê‘⁄’œ∞≠ ≤≈»œŒ™”–’œ∞≠
//             else if(!Barrier(0,4,0,4)) icout = 0;
//             if(icout>=20 && yflag!=1)//≈–∂œ”–’œ∞≠ŒÔ …Ë÷√ÀŸ∂»Œ™0 yflag ∑¿÷π∆µ∑±…Ë÷√
//						 {
//                MoveFBSpeedChange(0);yflag = 1;
//						 }
//             else if(yflag!=2)//≈–∂œ√ª”–’œ∞≠ŒÔ …Ë÷√ÀŸ∂»Œ™÷Æ«∞ÀŸ∂» 
//						 {
//							  MoveFBSpeedChange(dir * (step==3?maxspeed:minspeed));yflag = 2;
//						 }							 
//					 
//				 }
				 //≥¨ ±¥¶¿Ì
         if(OSTmr_Count_Get(&timeout) > g_limitruntimeout)
				 {
					  myPrintf("ContrlRunFB timeout\r\n");
						MotorStopAll();
						MotorDisableAll();
						break;
				 }					 
			   OSTimeDly(5);
		}
		MotorStopAll();
		MotorDisableAll();			 		
	  return FALSE;
}
void MoveFBSpeedSet(long speed)
{
		if(speed > MaxMovingSpeed || speed < -MaxMovingSpeed)
	  {
			 //±®∏Ê∑˛ŒÒ∆˜ÀŸ∂»≥¨≥ˆ
		}
		MotorSetMode(ID_TX_MOTOR_MOVE2 , ID_RX_MOTOR_MOVE2 , MODE_SPEED);
		MotorSetMode(ID_TX_MOTOR_MOVE4 , ID_RX_MOTOR_MOVE4 , MODE_SPEED);
		MotorSetMode(ID_TX_MOTOR_MOVE6 , ID_RX_MOTOR_MOVE6 , MODE_SPEED);
		MotorSetMode(ID_TX_MOTOR_MOVE8 , ID_RX_MOTOR_MOVE8 , MODE_SPEED);
		//2°¢ πƒ‹
		MotorEnable(ID_TX_MOTOR_MOVE2 , ID_RX_MOTOR_MOVE2);
		MotorEnable(ID_TX_MOTOR_MOVE4 , ID_RX_MOTOR_MOVE4);
		MotorEnable(ID_TX_MOTOR_MOVE6 , ID_RX_MOTOR_MOVE6);
		MotorEnable(ID_TX_MOTOR_MOVE8 , ID_RX_MOTOR_MOVE8);		
		//3°¢‘À––
		if(FALSE == MotorVelocityRun(ID_TX_MOTOR_MOVE2 , ID_RX_MOTOR_MOVE2 , speed*33.33333) || 
		FALSE == MotorVelocityRun(ID_TX_MOTOR_MOVE4 , ID_RX_MOTOR_MOVE4 , speed*33.33333) ||
		FALSE == MotorVelocityRun(ID_TX_MOTOR_MOVE6 , ID_RX_MOTOR_MOVE6 , -speed*33.33333) ||
		FALSE == MotorVelocityRun(ID_TX_MOTOR_MOVE8 , ID_RX_MOTOR_MOVE8 , -speed*33.33333))
		{
				MotorStopAll();
				MotorDisableAll();		
		}
}
void MoveFBSpeedChange(long speed)
{
		if(speed > MaxMovingSpeed || speed < -MaxMovingSpeed)
	  {
			 //±®∏Ê∑˛ŒÒ∆˜ÀŸ∂»≥¨≥ˆ
		}
		//1°¢‘À––
		if(FALSE == MotorVelocityRun(ID_TX_MOTOR_MOVE2 , ID_RX_MOTOR_MOVE2 , speed*33.33333) || 
		FALSE == MotorVelocityRun(ID_TX_MOTOR_MOVE4 , ID_RX_MOTOR_MOVE4 , speed*33.33333) ||
		FALSE == MotorVelocityRun(ID_TX_MOTOR_MOVE6 , ID_RX_MOTOR_MOVE6 , -speed*33.33333) ||
		FALSE == MotorVelocityRun(ID_TX_MOTOR_MOVE8 , ID_RX_MOTOR_MOVE8 , -speed*33.33333))
		{
				MotorStopAll();
				MotorDisableAll();		
		}	
}
void TurnToAngleVal(u16 angle)
{
		long position =  angle * 240000 / 90;
		//1°¢…Ë÷√‘À––ƒ£ Ω
		MotorSetMode(ID_TX_MOTOR_TURN1 , ID_RX_MOTOR_TURN1 , MODE_POS);
		MotorSetMode(ID_TX_MOTOR_TURN3 , ID_RX_MOTOR_TURN3 , MODE_POS);
		MotorSetMode(ID_TX_MOTOR_TURN5 , ID_RX_MOTOR_TURN5 , MODE_POS);
		MotorSetMode(ID_TX_MOTOR_TURN7 , ID_RX_MOTOR_TURN7 , MODE_POS);
		//OSTimeDlyHMSM(0,0,0,20);
		//2°¢ πƒ‹
		MotorEnable(ID_TX_MOTOR_TURN1 , ID_RX_MOTOR_TURN1);
		MotorEnable(ID_TX_MOTOR_TURN3 , ID_RX_MOTOR_TURN3);
		MotorEnable(ID_TX_MOTOR_TURN5 , ID_RX_MOTOR_TURN5);
		MotorEnable(ID_TX_MOTOR_TURN7 , ID_RX_MOTOR_TURN7);
		//3°¢‘À––
		//OSTimeDlyHMSM(0,0,0,20);
		MotorPositionRun(ID_TX_MOTOR_TURN1 , ID_RX_MOTOR_TURN1 , position ,1200*33.33333);
		MotorPositionRun(ID_TX_MOTOR_TURN3 , ID_RX_MOTOR_TURN3 , -position ,1200*33.33333);
		MotorPositionRun(ID_TX_MOTOR_TURN5 , ID_RX_MOTOR_TURN5 , position ,1200*33.33333);
		MotorPositionRun(ID_TX_MOTOR_TURN7 , ID_RX_MOTOR_TURN7 , -position ,1200*33.33333);
	
}

void ZeroMoveMotorEncoder(void)
{
		MotorClearEncoder(ID_TX_MOTOR_MOVE2 , ID_RX_MOTOR_MOVE2);
   	MotorClearEncoder(ID_TX_MOTOR_MOVE4 , ID_RX_MOTOR_MOVE4);
	  MotorClearEncoder(ID_TX_MOTOR_MOVE6 , ID_RX_MOTOR_MOVE6);
	  MotorClearEncoder(ID_TX_MOTOR_MOVE8 , ID_RX_MOTOR_MOVE8);
}
void RevTurnPosition(TYPEDEF_TURN_MOTRO_POS * turnpos)
{
		MotorReadEncoder(ID_TX_MOTOR_TURN1 , ID_RX_MOTOR_TURN1 , &turnpos->pos1);
   	MotorReadEncoder(ID_TX_MOTOR_TURN3 , ID_RX_MOTOR_TURN3 , &turnpos->pos3);
	  MotorReadEncoder(ID_TX_MOTOR_TURN5 , ID_RX_MOTOR_TURN5 , &turnpos->pos5);
	  MotorReadEncoder(ID_TX_MOTOR_TURN7 , ID_RX_MOTOR_TURN7 , &turnpos->pos7);
}
void RevMovePosition(TYPEDEF_MOVE_MOTRO_POS * movepos)
{
		MotorReadEncoder(ID_TX_MOTOR_MOVE2 , ID_RX_MOTOR_MOVE2 , &movepos->pos2);
   	MotorReadEncoder(ID_TX_MOTOR_MOVE4 , ID_RX_MOTOR_MOVE4 , &movepos->pos4);
	  MotorReadEncoder(ID_TX_MOTOR_MOVE6 , ID_RX_MOTOR_MOVE6 , &movepos->pos6);
	  MotorReadEncoder(ID_TX_MOTOR_MOVE8 , ID_RX_MOTOR_MOVE8 , &movepos->pos8);
	 // myPrintf("MOVE-->2:%d  4:%d  6:%d  8:%d\r\n",movepos->pos2,movepos->pos4,movepos->pos6,movepos->pos8);
}

bool CheckTurnAngleVal(u16 angle)
{
	  TYPEDEF_TURN_MOTRO_POS turnpos = {0};
		long std =  angle * 240000 / 90;
		
		RevTurnPosition(&turnpos);
		if(ALLOWERR(turnpos.pos1 , std ,TRUNERR) && ALLOWERR(-turnpos.pos3 , std ,TRUNERR) && ALLOWERR(turnpos.pos5 , std ,TRUNERR) && ALLOWERR(-turnpos.pos7 , std ,TRUNERR))
			  return TRUE;
		else
			  return FALSE;
		
}
long DecelerationDistance(long maxspeed)
{
		int tmp = 0;
		if(BELONGTO(ABS(maxspeed) , 0 , 500))
				tmp=100;
		else if(BELONGTO(ABS(maxspeed) , 500 , 1000))
				tmp=150-10;
		else if(BELONGTO(ABS(maxspeed) , 1000 , 1300))
				tmp=130;
		else if(BELONGTO(ABS(maxspeed) , 1300 , 1500))
				tmp=220;
		else if(BELONGTO(ABS(maxspeed) , 1500 , 1700))
				tmp=250;
		else if(BELONGTO(ABS(maxspeed) , 1700 , 1900))
				tmp=300;
		else if(ABS(maxspeed)>=1900)
				tmp=500;
		return (long) (2*WheelRoundCount * tmp/WheelRoundLenght) ;	
}

/*****************************************************************************************************************************************************************
                                              NETCMD_BEDUD        ¥≤∞Âøÿ÷∆∫Ø ˝                   
*****************************************************************************************************************************************************************/
bool  ContrlBedUpDown(u32 position)
{
		u8 step = 0;
	  long curhight = 0;
	  u32 timeout = 0;
	
		while(1)
		{  
				switch(step)
				{
					case 0://∂¡»°µ±«∞¥≤∞Â∏ﬂ∂»
						OSTmr_Count_Start(&timeout);				
						curhight = ReadBedPos();
						step++;
						break;
					case 1://øÿ÷∆¥≤∞Â‘À∂Ø
						RunBed(position);
						step++;
						break;
					case 2://ºÏ≤È «∑ÒµΩ÷∏∂®Œª÷√  µΩ¡ÀÕ£÷πµÁª˙
						curhight = ReadBedPos();
						if (curhight >= (position - 1)  && curhight <= (position + 1) )
						{
								StopBed();
							  return TRUE;
						}	
					default:
						break;
				}
		
				//≈–∂œ¥≤∞Â…œœ¬¥´∏–∆˜◊¥Ã¨
//				if(!GPIO_ReadInputDataBit( GPIOD , BSP_GPIOD_EDIN5))
//				{
//						long l_temp= position - curhight;
//						if(l_temp>0) //…œ…˝
//						{
//							;
//						}
//						if(l_temp<0) //œ¬Ωµ
//						{
//							StopBed();
//							return FALSE;
//						}
//				}			
//				//≈–∂œ‘À∂Ø≤Ω÷Ë «∑ÒÕÍ≥…
//				if(OSTmr_Count_Get(&timeout) > 34000)//’˝≥£ÕÍ≥…ªÚ’ﬂ≥¨ ±∂º”¶∏√ÕÀ≥ˆ÷¥––π˝≥Ã
//				{
//						return FALSE;
//				}
				OSTimeDly(3);
		}
}

void StopBed(void)
{
	MotorBedStop(ID_TX_MOTOR_BED9 , ID_RX_MOTOR_BED9);
	MotorBedStop(ID_TX_MOTOR_BED10 , ID_RX_MOTOR_BED10);
	OSTimeDlyHMSM(0,0,0,100);
	DEF_ENABLE_KOER; 
}

/*
***************************************************************
∂¡¥≤∞Â∏ﬂ∂»
***************************************************************
*/
long ReadBedPos(void)
{
	  long mt9pos = 0,mt10pos = 0;
		long curhight = 0;
		MotorReadEncoder(ID_TX_MOTOR_BED9 , ID_RX_MOTOR_BED9 , &mt9pos);
		MotorReadEncoder(ID_TX_MOTOR_BED10 , ID_RX_MOTOR_BED10 , &mt10pos);
		curhight= (((mt9pos + mt10pos)/2)/19305)*(-1)+1090; 	
    return curhight;
}
/*
***************************************************************
‘À––¥≤∞ÂµΩ÷∏∂®∏ﬂ∂»
***************************************************************
*/
void RunBed(long pos)
{
		long bed9pos = 0 , bed10pos = 0;
		int cout = 0;  	
		long m_position1;	 
    if(pos<905) pos = 905;
	  else if(pos>1095)  pos = 1095;
		m_position1 = pos - 900;  
		m_position1 = 190 - m_position1; 

		//Õ£÷πµÁª˙
		MotorBedStop(ID_TX_MOTOR_BED9 , ID_RX_MOTOR_BED9);
		MotorBedStop(ID_TX_MOTOR_BED10 , ID_RX_MOTOR_BED10);
		//…Ë÷√µÁª˙ƒ£ Ω
		MotorSetMode(ID_TX_MOTOR_BED9 , ID_RX_MOTOR_BED9 , MODE_POS);
		MotorSetMode(ID_TX_MOTOR_BED10 , ID_RX_MOTOR_BED10 , MODE_POS);
		// πƒ‹µÁª˙
		if(FALSE == MotorEnable(ID_TX_MOTOR_BED9 , ID_RX_MOTOR_BED9)|| FALSE == MotorEnable(ID_TX_MOTOR_BED10 , ID_RX_MOTOR_BED10))
		{
			MotorBedStop(ID_TX_MOTOR_BED9 , ID_RX_MOTOR_BED9);
			MotorBedStop(ID_TX_MOTOR_BED10 , ID_RX_MOTOR_BED10);
			return;
		}
		
		DEF_DISENABLE_KOER;//±ß’¢À…ø™
		
		//…˝Ωµ÷˘1000000∂‘”¶µƒæ‡¿Î «51.8mm,œ÷‘⁄º”ÀŸ∂»Ã´–°,“ª∫¡√◊ «19305 
		m_position1 = m_position1 * 19305;
		//…Ë÷√µÁª˙‘À––≤Œ ˝
		if(MotroBedPositionRun(ID_TX_MOTOR_BED9 , ID_RX_MOTOR_BED9 ,m_position1,120)==FALSE || MotroBedPositionRun(ID_TX_MOTOR_BED10 , ID_RX_MOTOR_BED10 ,m_position1,120)==FALSE)
		{
				DEF_ENABLE_KOER;
				MotorBedStop(ID_TX_MOTOR_BED9 , ID_RX_MOTOR_BED9);
				MotorBedStop(ID_TX_MOTOR_BED10 , ID_RX_MOTOR_BED10);
				return;
		}
	  do
		{
			  OSTimeDly(20);
		   	MotorReadEncoder(ID_TX_MOTOR_BED9 , ID_RX_MOTOR_BED9 , &bed9pos);
				MotorReadEncoder(ID_TX_MOTOR_BED10 , ID_RX_MOTOR_BED10 , &bed10pos);
			  //myPrintf("Bed9:%d  Bed10:%d\r\n",bed9pos , bed10pos);
        if(ALLOWERR(bed9pos , m_position1 , 40000) && ALLOWERR(bed9pos , m_position1 , 40000))
			  {
					  break;
			  }
        cout++;
        if(cout>750 || g_stopflag)
				{
					 break;
				}					
		}
   	while(1);
		
		DEF_ENABLE_KOER;
		MotorBedStop(ID_TX_MOTOR_BED9 , ID_RX_MOTOR_BED9);
		MotorBedStop(ID_TX_MOTOR_BED10 , ID_RX_MOTOR_BED10);

}

/*
****************************************************************************************
¥≤∞Âøÿ÷∆
****************************************************************************************
*/


void TestBedMotor(void)
{
//	  long bed9pos = 0 , bed10pos = 0;
//    int cout = 0;  

		MotorBedStop(ID_TX_MOTOR_BED9 , ID_RX_MOTOR_BED9);
		MotorBedStop(ID_TX_MOTOR_BED10 , ID_RX_MOTOR_BED10);	

		MotorSetMode(ID_TX_MOTOR_BED9 , ID_RX_MOTOR_BED9 , MODE_POS);
		MotorSetMode(ID_TX_MOTOR_BED10 , ID_RX_MOTOR_BED10 , MODE_POS);

		if(FALSE == MotorEnable(ID_TX_MOTOR_BED9 , ID_RX_MOTOR_BED9) || FALSE == MotorEnable(ID_TX_MOTOR_BED10 , ID_RX_MOTOR_BED10))
		{
				MotorBedStop(ID_TX_MOTOR_BED9 , ID_RX_MOTOR_BED9);
				MotorBedStop(ID_TX_MOTOR_BED10 , ID_RX_MOTOR_BED10);
				return ;
		}	

	  DEF_DISENABLE_KOER;
	
		if(FALSE == MotroBedPositionRun(ID_TX_MOTOR_BED9 , ID_RX_MOTOR_BED9 , 19305*100 , 120) || FALSE == MotroBedPositionRun(ID_TX_MOTOR_BED10 , ID_RX_MOTOR_BED10 , 19305*100 , 120))
		{
				MotorBedStop(ID_TX_MOTOR_BED9 , ID_RX_MOTOR_BED9);
				MotorBedStop(ID_TX_MOTOR_BED10 , ID_RX_MOTOR_BED10);
				return;
		}
		OSTimeDly(9000);
		
		DEF_ENABLE_KOER;
		
		MotorBedStop(ID_TX_MOTOR_BED9 , ID_RX_MOTOR_BED9);
		MotorBedStop(ID_TX_MOTOR_BED10 , ID_RX_MOTOR_BED10);			
}
/*
*******************************************************************************
¥≤∞Â…œ…˝—≤¡„
*******************************************************************************
*/
void ResetZeroBedUp(void)
{
    long bed9pos = 0 , bed10pos = 0;
    u16 cout = 0;  
	
  	MotorBedStop(ID_TX_MOTOR_BED9 , ID_RX_MOTOR_BED9);
		MotorBedStop(ID_TX_MOTOR_BED10 , ID_RX_MOTOR_BED10);	

		MotorSetMode(ID_TX_MOTOR_BED9 , ID_RX_MOTOR_BED9 , MODE_HOME);
		MotorSetMode(ID_TX_MOTOR_BED10 , ID_RX_MOTOR_BED10 , MODE_HOME);

		MotorEnable(ID_TX_MOTOR_BED9 , ID_RX_MOTOR_BED9);
		MotorEnable(ID_TX_MOTOR_BED10 , ID_RX_MOTOR_BED10);
		
	  DEF_DISENABLE_KOER;

		if(FALSE == MotorHomeRun(ID_TX_MOTOR_BED9 , ID_RX_MOTOR_BED9) || FALSE == MotorHomeRun(ID_TX_MOTOR_BED10 , ID_RX_MOTOR_BED10))
		{
				MotorBedStop(ID_TX_MOTOR_BED9 , ID_RX_MOTOR_BED9);
				MotorBedStop(ID_TX_MOTOR_BED10 , ID_RX_MOTOR_BED10);
				return ;
		}
    
	  do
		{
			  OSTimeDly(100);
		   	MotorReadEncoder(ID_TX_MOTOR_BED9 , ID_RX_MOTOR_BED9 , &bed9pos);
				MotorReadEncoder(ID_TX_MOTOR_BED10 , ID_RX_MOTOR_BED10 , &bed10pos);
			  //myPrintf("Bed9:%d  Bed10:%d\r\n",bed9pos , bed10pos);
        if(BELONGTO(bed9pos , 0 , 8000) && BELONGTO(bed10pos , 0 , 8000))
			  {
					  OSTimeDly(300);
            //myPrintf("comple\r\n");
					  break;
			  }
        cout++;
        if(cout>300|| g_stopflag)
				{
					 break;
				}					
		}
   	while(1);
		DEF_ENABLE_KOER;
		
    MotorBedStop(ID_TX_MOTOR_BED9 , ID_RX_MOTOR_BED9);
		MotorBedStop(ID_TX_MOTOR_BED10 , ID_RX_MOTOR_BED10);
}
/*
*******************************************************************************
¥≤∞Âœ¬Ωµ—≤¡„
*******************************************************************************
*/
void ResetZeroBedDown(void)
{
  	MotorBedStop(ID_TX_MOTOR_BED9 , ID_RX_MOTOR_BED9);
		MotorBedStop(ID_TX_MOTOR_BED10 , ID_RX_MOTOR_BED10);	

		MotorSetMode(ID_TX_MOTOR_BED9 , ID_RX_MOTOR_BED9 , MODE_SPEED);
		MotorSetMode(ID_TX_MOTOR_BED10 , ID_RX_MOTOR_BED10 , MODE_SPEED);
	
		MotorEnable(ID_TX_MOTOR_BED9 , ID_RX_MOTOR_BED9);
		MotorEnable(ID_TX_MOTOR_BED10 , ID_RX_MOTOR_BED10);

		if(FALSE == MotorVelocityRun(ID_TX_MOTOR_BED9 , ID_RX_MOTOR_BED9 , 10000) || FALSE == MotorVelocityRun(ID_TX_MOTOR_BED10 , ID_RX_MOTOR_BED10 , 10000))
		{
				MotorBedStop(ID_TX_MOTOR_BED9 , ID_RX_MOTOR_BED9);
				MotorBedStop(ID_TX_MOTOR_BED10 , ID_RX_MOTOR_BED10);
				return ;
		}			
	
	  DEF_DISENABLE_KOER;
	
	  OSTimeDly(3000);
	  
    DEF_ENABLE_KOER;
		
    MotorBedStop(ID_TX_MOTOR_BED9 , ID_RX_MOTOR_BED9);
		MotorBedStop(ID_TX_MOTOR_BED10 , ID_RX_MOTOR_BED10);
}
/*
*******************************************************************************
∂¡¥≤∞Â¬Î≈Ã
*******************************************************************************
*/
void CMDReadBed(void)
{
    long bed9pos = 0 , bed10pos = 0;
		MotorReadEncoder(ID_TX_MOTOR_BED9 , ID_RX_MOTOR_BED9 , &bed9pos);
		MotorReadEncoder(ID_TX_MOTOR_BED10 , ID_RX_MOTOR_BED10 , &bed10pos);
		myPrintf("Bed9:%d  Bed10:%d\r\n",bed9pos , bed10pos);
}
/*
*******************************************************************************
«Â≥˝¥≤∞Â¬Î≈Ã
*******************************************************************************
*/
void CMDClearBed(void)
{
	 u8 i = 3;
	 MotorBedStop(ID_TX_MOTOR_BED9 , ID_RX_MOTOR_BED9 );
	 MotorBedStop(ID_TX_MOTOR_BED10 , ID_RX_MOTOR_BED10 );
	
	 while(i--)
	 {
		 OSTimeDly(100);
			MotorClearEncoder(ID_TX_MOTOR_BED9 , ID_RX_MOTOR_BED9 );
			MotorClearEncoder(ID_TX_MOTOR_BED10 , ID_RX_MOTOR_BED10 );
	 }
}




/*****************************************************************************************************************************************************************
                                                      µÁª˙øÿ÷∆µ•‘™∫Ø ˝                    
*****************************************************************************************************************************************************************/

/***********************************************************************
∫Ø ˝π¶ƒ‹£∫∂¡IO
msgid£∫«˝∂Ø∆˜ID
∑µªÿ÷µ£∫TRUE≥…π¶
        FALSE ß∞‹
***********************************************************************/
bool MotorReadIO(u32 sendid , u32 filterid , u32* pvalue)
{
    const u8 CanSendData_velocity[8]={0x40,0xFD,0x60,0x00,0x00,0x00,0x00,0x00};
		u8 err = 0;
		CanRxMsg *prxmsg = NULL;
		
		if(BSP_CAN_SendMsg(sendid ,filterid , (u8*)CanSendData_velocity) == TRUE)
		{
				prxmsg = (CanRxMsg*) OSMboxPend(CANItMbox , 100 , &err);
			
				if(err == OS_ERR_NONE)
				{
#if DEF_DEBUG_CFG & DEF_DEBUG_CAN
						myPrintf("Recv->  ID:%04X  DATA:%02X %02X %02X %02X %02X %02X %02X %02X \r\n",prxmsg->StdId , prxmsg->Data[0] , prxmsg->Data[1] , prxmsg->Data[2] , prxmsg->Data[3] ,
					                                                                      prxmsg->Data[4] , prxmsg->Data[5] , prxmsg->Data[6] , prxmsg->Data[7]  );		
#endif							
						if(0x40 ==	prxmsg->Data[1] && 0x60 ==	prxmsg->Data[2])
						{
							 *pvalue = *(u8*)&prxmsg->Data[4];
								return TRUE;//÷¥––≥…π¶
						}
						else
						{
								//∏Ê÷™clientµÁª˙÷¥–– ß∞‹
								return FALSE;
						}
				}
				else
				{
						//∏Ê÷™clientµÁª˙ŒﬁcanœÏ”¶
						return FALSE;
				}
		}
		else
		{
				//∏Ê÷™clientµÁª˙can∑¢ÀÕ ß∞‹
				return FALSE;
		}
}
/***********************************************************************
∫Ø ˝π¶ƒ‹£∫Õ£÷π√¸¡Ó
msgid£∫«˝∂Ø∆˜ID
∑µªÿ÷µ£∫TRUE≥…π¶
        FALSE ß∞‹
***********************************************************************/
bool MotorStop(u32 sendid , u32 filterid)
{
		const u8 CanSendData_Stop[8]={0x2b,0x40,0x60,0x00,0x0F,0x01,0x00,0x00};
		u8 err = 0;
		CanRxMsg *prxmsg = NULL;
		
		if(BSP_CAN_SendMsg(sendid ,filterid , (u8*)CanSendData_Stop) == TRUE)
		{
				prxmsg = (CanRxMsg*) OSMboxPend(CANItMbox , 100 , &err);
			
				if(err == OS_ERR_NONE)
				{
#if DEF_DEBUG_CFG & DEF_DEBUG_CAN	
						myPrintf("Recv->  ID:%04X  DATA:%02X %02X %02X %02X %02X %02X %02X %02X \r\n",prxmsg->StdId , prxmsg->Data[0] , prxmsg->Data[1] , prxmsg->Data[2] , prxmsg->Data[3] ,
					                                                                      prxmsg->Data[4] , prxmsg->Data[5] , prxmsg->Data[6] , prxmsg->Data[7]  );		
#endif							
						if(0x40 ==	prxmsg->Data[1] && 0x60 ==	prxmsg->Data[2])
						{
								return TRUE;//÷¥––≥…π¶
						}
						else
						{
								//∏Ê÷™clientµÁª˙÷¥–– ß∞‹
								return FALSE;
						}
				}
				else
				{
						//∏Ê÷™clientµÁª˙ŒﬁcanœÏ”¶
						return FALSE;
				}
		}
		else
		{
				//∏Ê÷™clientµÁª˙can∑¢ÀÕ ß∞‹
				return FALSE;
		}
}

/*********************************************************************
…˝ΩµµÁª˙Õ£÷π
**********************************************************************/
bool MotorBedStop(u32 sendid , u32 filterid)
{
		u8 err = 0;
		CanRxMsg *prxmsg = NULL;
		const u8 CanSendData_Stop[8]={0x2b,0x40,0x60,0x00,0x02,0x00,0x00,0x00};

		if(BSP_CAN_SendMsg(sendid ,filterid , (u8*)CanSendData_Stop) == TRUE)
		{
				prxmsg = (CanRxMsg*) OSMboxPend(CANItMbox , 100 , &err);
			
				if(err == OS_ERR_NONE)
				{
#if DEF_DEBUG_CFG & DEF_DEBUG_CAN	
						myPrintf("Recv->  ID:%04X  DATA:%02X %02X %02X %02X %02X %02X %02X %02X \r\n",prxmsg->StdId , prxmsg->Data[0] , prxmsg->Data[1] , prxmsg->Data[2] , prxmsg->Data[3] ,
					                                                                      prxmsg->Data[4] , prxmsg->Data[5] , prxmsg->Data[6] , prxmsg->Data[7]  );		
#endif							
						if(0x40 ==	prxmsg->Data[1] && 0x60 ==	prxmsg->Data[2])
						{
								return TRUE;//÷¥––≥…π¶
						}
						else
						{
								//∏Ê÷™clientµÁª˙÷¥–– ß∞‹
								return FALSE;
						}
				}
				else
				{
						//∏Ê÷™clientµÁª˙ŒﬁcanœÏ”¶
						return FALSE;
				}
		}
		else
		{
				//∏Ê÷™clientµÁª˙can∑¢ÀÕ ß∞‹
				return FALSE;
		}
}

/*********************************************************************
µÁª˙π§◊˜ƒ£ Ω…Ë÷√
**********************************************************************/
bool MotorSetMode(u32 sendid , u32 filterid , TYPEDEF_MOTER_MODE  mode)
{
		static const u8 CanSendData_position[8]={ 0x2F,0x60,0x60,0x00,0x01,0x00,0x00,0x00};
		static const u8 CanSendData_velocity[8]={ 0x2F,0x60,0x60,0x00,0x03,0x00,0x00,0x00};
		static const u8 CanSendData_homing[8]={ 0x2F,0x60,0x60,0x00,0x06,0x00,0x00,0x00};
		static const u8 *modemsg[] = {CanSendData_position , CanSendData_velocity , CanSendData_homing};

		u8 err = 0;
		CanRxMsg *prxmsg = NULL;

		if(BSP_CAN_SendMsg(sendid ,filterid , (u8*)modemsg[mode]) == TRUE)
		{
				prxmsg = (CanRxMsg*) OSMboxPend(CANItMbox , 100 , &err);
			
				if(err == OS_ERR_NONE)
				{
#if DEF_DEBUG_CFG & DEF_DEBUG_CAN	
						myPrintf("Recv->  ID:%04X  DATA:%02X %02X %02X %02X %02X %02X %02X %02X \r\n",prxmsg->StdId , prxmsg->Data[0] , prxmsg->Data[1] , prxmsg->Data[2] , prxmsg->Data[3] ,
					                                                                      prxmsg->Data[4] , prxmsg->Data[5] , prxmsg->Data[6] , prxmsg->Data[7]  );		
#endif	
						if(0x60 ==	prxmsg->Data[1] && 0x60 ==	prxmsg->Data[2])
						{
								return TRUE;//÷¥––≥…π¶
						}
						else
						{
								//∏Ê÷™clientµÁª˙÷¥–– ß∞‹
								return FALSE;
						}
				}
				else
				{
						//∏Ê÷™clientµÁª˙ŒﬁcanœÏ”¶
						return FALSE;
				}
		}
		else
		{
				//∏Ê÷™clientµÁª˙can∑¢ÀÕ ß∞‹
				return FALSE;
		}
}


/***********************************************************************
∫Ø ˝π¶ƒ‹£∫µÁª˙ πƒ‹
msgid£∫«˝∂Ø∆˜ID
∑µªÿ÷µ£∫TRUE≥…π¶
        FALSE ß∞‹
***********************************************************************/
bool MotorEnable(u32 sendid , u32 filterid)
{
		static const u8 CanSendData1[8]={0x2b,0x40,0x60,0x00,0x06,0x00,0x00,0x00};// πƒ‹∂œø™
		static const u8 CanSendData2[8]={0x2b,0x40,0x60,0x00,0x07,0x00,0x00,0x00};//Swich ON
		static const u8 CanSendData3[8]={0x2b,0x40,0x60,0x00,0x0f,0x00,0x00,0x00};// πƒ‹
		static const u8 *msgarr[] = { CanSendData1 , CanSendData2 ,CanSendData3};
	
		u8 err = 0 , i = 0;
		CanRxMsg *prxmsg = NULL;
    
		for(i = 0 ;i!=3 ;i++)
		{
				if(BSP_CAN_SendMsg(sendid ,filterid , (u8*)msgarr[i]) == TRUE)
				{
					
						prxmsg = (CanRxMsg*) OSMboxPend(CANItMbox , 100 , &err);
					
						if(err == OS_ERR_NONE)
						{
#if DEF_DEBUG_CFG & DEF_DEBUG_CAN	
						myPrintf("Recv->  ID:%04X  DATA:%02X %02X %02X %02X %02X %02X %02X %02X \r\n",prxmsg->StdId , prxmsg->Data[0] , prxmsg->Data[1] , prxmsg->Data[2] , prxmsg->Data[3] ,
					                                                                      prxmsg->Data[4] , prxmsg->Data[5] , prxmsg->Data[6] , prxmsg->Data[7]  );		
#endif								
								if(0x40 ==	prxmsg->Data[1] && 0x60 ==	prxmsg->Data[2])
								{
										continue;
								}
								else
								{
										//∏Ê÷™clientµÁª˙÷¥–– ß∞‹
										return FALSE;
								}
						}
						else
						{
								//∏Ê÷™clientµÁª˙ŒﬁcanœÏ”¶
								return FALSE;
						}
				}
				else
				{
						//∏Ê÷™clientµÁª˙can∑¢ÀÕ ß∞‹
						return FALSE;
				}				
		}
		return TRUE;//÷¥––≥…π¶
}
/***********************************************************************
∫Ø ˝π¶ƒ‹£∫µÁª˙∂œø™ πƒ‹
msgid£∫«˝∂Ø∆˜ID
∑µªÿ÷µ£∫TRUE≥…π¶
        FALSE ß∞‹
***********************************************************************/
bool MotorDisEnable(u32 sendid , u32 filterid )
{
	  const u8 CanSendData1[8]={0x2b,0x40,0x60,0x00,0x06,0x00,0x00,0x00};// πƒ‹∂œø™
		u8 err = 0;	
		CanRxMsg *prxmsg = NULL;
		
		if(BSP_CAN_SendMsg(sendid ,filterid , (u8*)CanSendData1) == TRUE)
		{
				prxmsg = (CanRxMsg*) OSMboxPend(CANItMbox , 100 , &err);
			
				if(err == OS_ERR_NONE)
				{
#if DEF_DEBUG_CFG & DEF_DEBUG_CAN	
						myPrintf("Recv->  ID:%04X  DATA:%02X %02X %02X %02X %02X %02X %02X %02X \r\n",prxmsg->StdId , prxmsg->Data[0] , prxmsg->Data[1] , prxmsg->Data[2] , prxmsg->Data[3] ,
					                                                                      prxmsg->Data[4] , prxmsg->Data[5] , prxmsg->Data[6] , prxmsg->Data[7]  );		
#endif						
						if(0x63 ==	prxmsg->Data[1] && 0x60 ==	prxmsg->Data[2])
						{
								return TRUE;//÷¥––≥…π¶
						}
						else
						{
								//∏Ê÷™clientµÁª˙÷¥–– ß∞‹
								return FALSE;
						}
				}
				else
				{
						//∏Ê÷™clientµÁª˙ŒﬁcanœÏ”¶
						return FALSE;
				}
		}
		else
		{
				//∏Ê÷™clientµÁª˙can∑¢ÀÕ ß∞‹
				return FALSE;
		}

}
/***********************************************************************
∫Ø ˝π¶ƒ‹£∫∂¡¬Î≈Ã
msgid£∫«˝∂Ø∆˜ID
∑µªÿ÷µ£∫TRUE≥…π¶
        FALSE ß∞‹
***********************************************************************/ 
bool MotorReadEncoder (u32 sendid , u32 filterid , long* rpos)
{
    const u8 CanSendData_velocity[8]={0x40,0x63,0x60,0x00,0x00,0x00,0x00,0x00};
		u8 err = 0;
		CanRxMsg *prxmsg = NULL;

		if(BSP_CAN_SendMsg(sendid ,filterid , (u8*)CanSendData_velocity) == TRUE)
		{
				prxmsg = (CanRxMsg*) OSMboxPend(CANItMbox , 100 , &err);
			
				if(err == OS_ERR_NONE)
				{
#if DEF_DEBUG_CFG & DEF_DEBUG_CAN	
						myPrintf("Recv->  ID:%04X  DATA:%02X %02X %02X %02X %02X %02X %02X %02X \r\n",prxmsg->StdId , prxmsg->Data[0] , prxmsg->Data[1] , prxmsg->Data[2] , prxmsg->Data[3] ,
					                                                                      prxmsg->Data[4] , prxmsg->Data[5] , prxmsg->Data[6] , prxmsg->Data[7]  );		
#endif						
						if(0x63 ==	prxmsg->Data[1] && 0x60 ==	prxmsg->Data[2])
						{
							  *rpos = *(long*)&prxmsg->Data[4];
								return TRUE;//÷¥––≥…π¶
						}
						else
						{
								//∏Ê÷™clientµÁª˙÷¥–– ß∞‹
								return FALSE;
						}
				}
				else
				{
						//∏Ê÷™clientµÁª˙ŒﬁcanœÏ”¶
						return FALSE;
				}
		}
		else
		{
				//∏Ê÷™clientµÁª˙can∑¢ÀÕ ß∞‹
				return FALSE;
		}
}

/***********************************************************************
∫Ø ˝π¶ƒ‹£∫–¥¬Î≈Ã
msgid£∫«˝∂Ø∆˜ID
∑µªÿ÷µ£∫TRUE≥…π¶
        FALSE ß∞‹
***********************************************************************/
bool MotorClearEncoder(u32 sendid , u32 filterid)
{	  
    static const u8 CanSendData1[8]={0x2b,0x40,0x60,0x00,0x06,0x00,0x00,0x00};// πƒ‹∂œø™
		static const u8 CanSendData2[8]={0x2f,0x60,0x60,0x00,0x06,0x00,0x00,0x00 };
		static const u8 CanSendData3[8]={0x2f,0x98,0x60,0x00,0x23,0x00,0x00,0x00};
		static const u8 CanSendData4[8]={0x2b,0x40,0x60,0x00,0x10,0x00,0x00,0x00};
		static const u8 *msgarr[] = {CanSendData1 , CanSendData2 , CanSendData3 ,CanSendData4};
	
		u8 err = 0 , i = 0;
		CanRxMsg *prxmsg = NULL;
    
		for(i = 0 ;i!=4 ;i++)
		{
				if(BSP_CAN_SendMsg(sendid ,filterid , (u8*)msgarr[i]) == TRUE)
				{
						prxmsg = (CanRxMsg*) OSMboxPend(CANItMbox , 100 , &err);
					
						if(err == OS_ERR_NONE)
						{
#if DEF_DEBUG_CFG & DEF_DEBUG_CAN	
						myPrintf("Recv->  ID:%04X  DATA:%02X %02X %02X %02X %02X %02X %02X %02X \r\n",prxmsg->StdId , prxmsg->Data[0] , prxmsg->Data[1] , prxmsg->Data[2] , prxmsg->Data[3] ,
					                                                                      prxmsg->Data[4] , prxmsg->Data[5] , prxmsg->Data[6] , prxmsg->Data[7]  );		
#endif								
								if(msgarr[i][1] ==	prxmsg->Data[1] && msgarr[i][2]  ==	prxmsg->Data[2])
								{
										continue;
								}
								else
								{
										//∏Ê÷™clientµÁª˙÷¥–– ß∞‹
										return FALSE;
								}
						}
						else
						{
								//∏Ê÷™clientµÁª˙ŒﬁcanœÏ”¶
								return FALSE;
						}
				}
				else
				{
						//∏Ê÷™clientµÁª˙can∑¢ÀÕ ß∞‹
						return FALSE;
				}				
		}
		return TRUE;//÷¥––≥…π¶
	
	
}
/***********************************************************************
∫Ø ˝π¶ƒ‹£∫…Ë÷√µÁª˙ÀŸ∂»ƒ£ ΩµƒÀŸ∂»¥Û–°
msgid£∫«˝∂Ø∆˜ID
vl£∫ÀŸ∂»¥Û–°
∑µªÿ÷µ£∫TRUE≥…π¶
        FALSE ß∞‹
***********************************************************************/
bool MotorVelocityRun(u32 sendid , u32 filterid , long speed)
{
    u8 CanSendData_velocity[8]={0x23,0xff,0x60,0x00,0x30,0xF8,0xFF,0xFF};

		u8 err = 0;
		CanRxMsg *prxmsg = NULL;

    *(long*)&CanSendData_velocity[4] = speed;
		
		if(BSP_CAN_SendMsg(sendid ,filterid , CanSendData_velocity) == TRUE)
		{
				prxmsg = (CanRxMsg*) OSMboxPend(CANItMbox , 100 , &err);
			
				if(err == OS_ERR_NONE)
				{
#if DEF_DEBUG_CFG & DEF_DEBUG_CAN	
						myPrintf("Recv->  ID:%04X  DATA:%02X %02X %02X %02X %02X %02X %02X %02X \r\n",prxmsg->StdId , prxmsg->Data[0] , prxmsg->Data[1] , prxmsg->Data[2] , prxmsg->Data[3] ,
					                                                                      prxmsg->Data[4] , prxmsg->Data[5] , prxmsg->Data[6] , prxmsg->Data[7]  );		
#endif							
						if(0xff ==	prxmsg->Data[1] && 0x60 ==	prxmsg->Data[2])
						{
								return TRUE;//÷¥––≥…π¶
						}
						else
						{
								//∏Ê÷™clientµÁª˙÷¥–– ß∞‹
								return FALSE;
						}
				}
				else
				{
						//∏Ê÷™clientµÁª˙ŒﬁcanœÏ”¶
						return FALSE;
				}
		}
		else
		{
				//∏Ê÷™clientµÁª˙can∑¢ÀÕ ß∞‹
				return FALSE;
		}
}

/***********************************************************************
∫Ø ˝π¶ƒ‹£∫…Ë÷√µÁª˙◊ﬂŒª÷√
msgid£∫«˝∂Ø∆˜ID
po£∫Œª÷√
vl£∫ÀŸ∂»¥Û–°
∑µªÿ÷µ£∫TRUE≥…π¶
        FALSE ß∞‹
***********************************************************************/
bool MotorPositionRun(u32 sendid , u32 filterid , long position , long speed)
{

		static u8 CanSendData_position[8]={0x23,0x7A,0x60,0x00,0x00,0x20,0x00,0x00};// Œª÷√…Ë÷√
		static u8 CanSendData_speed[8]={0x23,0x81,0x60,0x00,0x00,0x20,0x00,0x00};// Œª÷√…Ë÷√
		static u8 CanSendData_velocity3[8]={0x2b,0x40,0x60,0x00,0x1F,0x00,0x00,0x00};
		static u8 CanSendData_velocity4[8]={0x2b,0x40,0x60,0x00,0x0F,0x00,0x00,0x00};
		static const u8 *msgarr[] = { CanSendData_position , CanSendData_speed ,CanSendData_velocity3 ,CanSendData_velocity4};
	
		u8 err = 0 , i = 0;
		CanRxMsg *prxmsg = NULL;
    
		*(long*)&CanSendData_position[4] = position;
    *(long*)&CanSendData_speed[4] = speed;
		
		for(i = 0 ;i!=4 ;i++)
		{
				if(BSP_CAN_SendMsg(sendid ,filterid , (u8*)msgarr[i]) == TRUE)
				{
						prxmsg = (CanRxMsg*) OSMboxPend(CANItMbox , 100 , &err);
					
						if(err == OS_ERR_NONE)
						{
#if DEF_DEBUG_CFG & DEF_DEBUG_CAN	
						myPrintf("Recv->  ID:%04X  DATA:%02X %02X %02X %02X %02X %02X %02X %02X \r\n",prxmsg->StdId , prxmsg->Data[0] , prxmsg->Data[1] , prxmsg->Data[2] , prxmsg->Data[3] ,
					                                                                      prxmsg->Data[4] , prxmsg->Data[5] , prxmsg->Data[6] , prxmsg->Data[7]  );		
#endif								
							 switch(i)
							 {
								 case 0:
										if(0x7A ==	prxmsg->Data[1] && 0x60 ==	prxmsg->Data[2])
										{
												continue;
										}
										else
										{
												//∏Ê÷™clientµÁª˙÷¥–– ß∞‹
												return FALSE;
										}
								 case 1:
										if(0x81 ==	prxmsg->Data[1] && 0x60 ==	prxmsg->Data[2])
										{
												continue;
										}
										else
										{
												//∏Ê÷™clientµÁª˙÷¥–– ß∞‹
												return FALSE;
										}

								 case 2:
								 case 3:
										if(0x40 ==	prxmsg->Data[1] && 0x60 ==	prxmsg->Data[2])
										{
												continue;
										}
										else
										{
												//∏Ê÷™clientµÁª˙÷¥–– ß∞‹
												return FALSE;
										}

								 default:return FALSE;
							 }

						}
						else
						{
								//∏Ê÷™clientµÁª˙ŒﬁcanœÏ”¶
								return FALSE;
						}
				}
				else
				{
						//∏Ê÷™clientµÁª˙can∑¢ÀÕ ß∞‹
						return FALSE;
				}				
		}
		return TRUE;//÷¥––≥…π¶	
}

/***********************************************************************
∫Ø ˝π¶ƒ‹£∫…Ë÷√º”ÀŸ∂»
msgid£∫«˝∂Ø∆˜ID
vl£∫ºıÀŸ∂»¥Û–°
∑µªÿ÷µ£∫TRUE≥…π¶
        FALSE ß∞‹
***********************************************************************/
bool MotorSetACC(u32 sendid , u32 filterid ,long acc)
{
	  static u8 CanSendData_velocity[8]={0x23,0x83,0x60,0x00,0x00,0x08,0x00,0x00};
		u8 err = 0;	
		CanRxMsg *prxmsg = NULL;
		
		*(long*)&CanSendData_velocity[4] = acc;
		
		if(BSP_CAN_SendMsg(sendid ,filterid , (u8*)CanSendData_velocity) == TRUE)
		{
				prxmsg = (CanRxMsg*) OSMboxPend(CANItMbox , 100 , &err);
			
				if(err == OS_ERR_NONE)
				{
#if DEF_DEBUG_CFG & DEF_DEBUG_CAN	
						myPrintf("Recv->  ID:%04X  DATA:%02X %02X %02X %02X %02X %02X %02X %02X \r\n",prxmsg->StdId , prxmsg->Data[0] , prxmsg->Data[1] , prxmsg->Data[2] , prxmsg->Data[3] ,
					                                                                      prxmsg->Data[4] , prxmsg->Data[5] , prxmsg->Data[6] , prxmsg->Data[7]  );		
#endif							
						if(0x83 ==	prxmsg->Data[1] && 0x60 ==	prxmsg->Data[2])
						{
								return TRUE;//÷¥––≥…π¶
						}
						else
						{
								//∏Ê÷™clientµÁª˙÷¥–– ß∞‹
								return FALSE;
						}
				}
				else
				{
						//∏Ê÷™clientµÁª˙ŒﬁcanœÏ”¶
						return FALSE;
				}
		}
		else
		{
				//∏Ê÷™clientµÁª˙can∑¢ÀÕ ß∞‹
				return FALSE;
		}
}
/***********************************************************************
∫Ø ˝π¶ƒ‹£∫…Ë÷√ºıÀŸ∂»
msgid£∫«˝∂Ø∆˜ID
vl£∫ºıÀŸ∂»¥Û–°
∑µªÿ÷µ£∫TRUE≥…π¶
        FALSE ß∞‹
***********************************************************************/
bool MotorSetDCC(u32 sendid , u32 filterid ,long dcc)
{
	  static u8 CanSendData_velocity[8]={0x23,0x84,0x60,0x00,0x00,0x08,0x00,0x00};
		u8 err = 0;	
		CanRxMsg *prxmsg = NULL;
		
		*(long*)&CanSendData_velocity[4] = dcc ;
		if(BSP_CAN_SendMsg(sendid ,filterid , (u8*)CanSendData_velocity) == TRUE)
		{
				prxmsg = (CanRxMsg*) OSMboxPend(CANItMbox , 100 , &err);
			
				if(err == OS_ERR_NONE)
				{
#if DEF_DEBUG_CFG & DEF_DEBUG_CAN	
						myPrintf("Recv->  ID:%04X  DATA:%02X %02X %02X %02X %02X %02X %02X %02X \r\n",prxmsg->StdId , prxmsg->Data[0] , prxmsg->Data[1] , prxmsg->Data[2] , prxmsg->Data[3] ,
					                                                                      prxmsg->Data[4] , prxmsg->Data[5] , prxmsg->Data[6] , prxmsg->Data[7]  );		
#endif							
						if(0x84==	prxmsg->Data[1] && 0x60 ==	prxmsg->Data[2])
						{
								return TRUE;//÷¥––≥…π¶
						}
						else
						{
								//∏Ê÷™clientµÁª˙÷¥–– ß∞‹
								return FALSE;
						}
				}
				else
				{
						//∏Ê÷™clientµÁª˙ŒﬁcanœÏ”¶
						return FALSE;
				}
		}
		else
		{
				//∏Ê÷™clientµÁª˙can∑¢ÀÕ ß∞‹
				return FALSE;
		}

}
/***********************************************************************
∫Ø ˝π¶ƒ‹£∫∂¡µÁª˙¥ÌŒÛ
msgid£∫«˝∂Ø∆˜ID
∑µªÿ÷µ£∫TRUE≥…π¶
        FALSE ß∞‹
***********************************************************************/
bool MotorReadError(u32 sendid , u32 filterid , u32 *perr)
{
	  static const  u8 CanSendData[8]={0x40,0x01,0x10,0x00,0x00,0x00,0x00,0x00};
		u8 err = 0;	
		CanRxMsg *prxmsg = NULL; 
	

		if(BSP_CAN_SendMsg( sendid ,filterid , (u8*)CanSendData) == TRUE)
		{
				prxmsg = (CanRxMsg*) OSMboxPend(CANItMbox , 100 , &err);
			
				if(err == OS_ERR_NONE)
				{
#if DEF_DEBUG_CFG & DEF_DEBUG_CAN	
						myPrintf("Recv->  ID:%04X  DATA:%02X %02X %02X %02X %02X %02X %02X %02X \r\n",prxmsg->StdId , prxmsg->Data[0] , prxmsg->Data[1] , prxmsg->Data[2] , prxmsg->Data[3] ,
					                                                                      prxmsg->Data[4] , prxmsg->Data[5] , prxmsg->Data[6] , prxmsg->Data[7]  );		
#endif							
						if(0x01==	prxmsg->Data[1] && 0x10 ==	prxmsg->Data[2])
						{
							  *perr = *(u8*)&prxmsg->Data[4];
								return TRUE;//÷¥––≥…π¶
						}
						else
						{
								//∏Ê÷™clientµÁª˙÷¥–– ß∞‹
								return FALSE;
						}
				}
				else
				{
						//∏Ê÷™clientµÁª˙ŒﬁcanœÏ”¶
						return FALSE;
				}
		}
		else
		{
				//∏Ê÷™clientµÁª˙can∑¢ÀÕ ß∞‹
				return FALSE;
		}
}

/***********************************************************************
∫Ø ˝π¶ƒ‹£∫«Â≥˝µÁª˙¥ÌŒÛ
msgid£∫«˝∂Ø∆˜ID
∑µªÿ÷µ£∫TRUE≥…π¶
        FALSE ß∞‹
***********************************************************************/
bool MotorClearError(u32 sendid , u32 filterid)
{
	  static const u8 CanSendData[8]={0x2b,0x40,0x60,0x00,0x80,0x00,0x00,0x00};
		u8 err = 0;	
		CanRxMsg *prxmsg = NULL; 
	
		if(BSP_CAN_SendMsg( sendid ,filterid , (u8*)CanSendData) == TRUE)
		{
				prxmsg = (CanRxMsg*) OSMboxPend(CANItMbox , 100 , &err);
			
				if(err == OS_ERR_NONE)
				{
#if DEF_DEBUG_CFG & DEF_DEBUG_CAN	
						myPrintf("Recv->  ID:%04X  DATA:%02X %02X %02X %02X %02X %02X %02X %02X \r\n",prxmsg->StdId , prxmsg->Data[0] , prxmsg->Data[1] , prxmsg->Data[2] , prxmsg->Data[3] ,
					                                                                      prxmsg->Data[4] , prxmsg->Data[5] , prxmsg->Data[6] , prxmsg->Data[7]  );		
#endif							
						if(0x01==	prxmsg->Data[1] && 0x10 ==	prxmsg->Data[2])
						{
								return TRUE;//÷¥––≥…π¶
						}
						else
						{
								//∏Ê÷™clientµÁª˙÷¥–– ß∞‹
								return FALSE;
						}
				}
				else
				{
						//∏Ê÷™clientµÁª˙ŒﬁcanœÏ”¶
						return FALSE;
				}
		}
		else
		{
				//∏Ê÷™clientµÁª˙can∑¢ÀÕ ß∞‹
				return FALSE;
		}

}


/***********************************************************************
∫Ø ˝π¶ƒ‹£∫Œª÷√ƒ£ Ω…Ë÷√µÁª˙“™◊ﬂµƒŒª÷√∫ÕÀŸ∂»
msgid£∫«˝∂Ø∆˜ID
po£∫…Ë÷√µƒŒª÷√
vl£∫ºıÀŸ∂»¥Û–°
∑µªÿ÷µ£∫TRUE≥…π¶
        FALSE ß∞‹
***********************************************************************/
bool MotroBedPositionRun(u32 sendid , u32 filterid , long position,long speed)
{

		static u8 CanSendData_velocity1[8]={0x23,0x7A,0x60,0x00,0x00,0x20,0x00,0x00};//Œª÷√…Ë÷√
		static u8 CanSendData_velocity2[8]={0x23,0x81,0x60,0x00,0x00,0x20,0x00,0x00};// ÀŸ∂»…Ë÷√
		static u8 CanSendData_velocity3[8]={0x23,0x83,0x60,0x00,0x20,0x00,0x00,0x00};// º”ÀŸ∂»…Ë÷√  £®…Ë÷√π˝¥Ûª· π…˝Ωµ÷˘ÀŸ∂»π˝øÏ£©
		static u8 CanSendData_velocity4[8]={0x23,0x84,0x60,0x00,0xf1,0x00,0x00,0x00};// ºıÀŸ∂»…Ë÷√
		static u8 CanSendData_velocity5[8]={0x2b,0x40,0x60,0x00,0x1F,0x00,0x00,0x00};
		static u8 CanSendData_velocity6[8]={0x2b,0x40,0x60,0x00,0x0F,0x00,0x00,0x00};
		static const u8 *msgarr[] = { CanSendData_velocity1 , CanSendData_velocity2 ,CanSendData_velocity3 , CanSendData_velocity4 ,CanSendData_velocity5 ,CanSendData_velocity6};

		u8 err = 0 , i = 0;
		CanRxMsg *prxmsg = NULL;
		
		*(long*)&CanSendData_velocity1[4] = position;
    *(long*)&CanSendData_velocity2[4] = speed;
		
		for(i = 0 ;i!=6 ;i++)
		{
				if(BSP_CAN_SendMsg(sendid ,filterid , (u8*)msgarr[i]) == TRUE)
				{
						prxmsg = (CanRxMsg*) OSMboxPend(CANItMbox , 1000 , &err);
					
						if(err == OS_ERR_NONE)
						{
#if DEF_DEBUG_CFG & DEF_DEBUG_CAN	
						myPrintf("Recv->  ID:%04X  DATA:%02X %02X %02X %02X %02X %02X %02X %02X \r\n",prxmsg->StdId , prxmsg->Data[0] , prxmsg->Data[1] , prxmsg->Data[2] , prxmsg->Data[3] ,
					                                                                      prxmsg->Data[4] , prxmsg->Data[5] , prxmsg->Data[6] , prxmsg->Data[7]  );		
#endif									
								if(msgarr[i][1] ==	prxmsg->Data[1] && msgarr[i][2]  ==	prxmsg->Data[2])
								{
										continue;
								}
								else
								{
										//∏Ê÷™clientµÁª˙÷¥–– ß∞‹
										return FALSE;
								}
						}
						else
						{
								//∏Ê÷™clientµÁª˙ŒﬁcanœÏ”¶
								return FALSE;
						}
				}
				else
				{
						//∏Ê÷™clientµÁª˙can∑¢ÀÕ ß∞‹
						return FALSE;
				}				
		}
		return TRUE;//÷¥––≥…π¶
}

/********************************************************
∫Ø ˝π¶ƒ‹£∫HOMEŒª÷√‘À––
i		 £∫«˝∂Ø∆˜ID∫≈
Velocity £∫
Position £∫
*********************************************************/
bool MotorHomeRun(u32 sendid , u32 filterid)
{
	  static const u8 CanSendData[8]={0x2b,0x40,0x60,0x00,0x1f,0x00,0x00,0x00};
		u8 err = 0;	
		CanRxMsg *prxmsg = NULL; 
	
		if(BSP_CAN_SendMsg( sendid ,filterid , (u8*)CanSendData) == TRUE)
		{
				prxmsg = (CanRxMsg*) OSMboxPend(CANItMbox , 100 , &err);
			
				if(err == OS_ERR_NONE)
				{
#if DEF_DEBUG_CFG & DEF_DEBUG_CAN	
						myPrintf("Recv->  ID:%04X  DATA:%02X %02X %02X %02X %02X %02X %02X %02X \r\n",prxmsg->StdId , prxmsg->Data[0] , prxmsg->Data[1] , prxmsg->Data[2] , prxmsg->Data[3] ,
					                                                                      prxmsg->Data[4] , prxmsg->Data[5] , prxmsg->Data[6] , prxmsg->Data[7]  );		
#endif								
						if(CanSendData[1]==	prxmsg->Data[1] && CanSendData[2] ==	prxmsg->Data[2])
						{
								return TRUE;//÷¥––≥…π¶
						}
						else
						{
								//∏Ê÷™clientµÁª˙÷¥–– ß∞‹
								return FALSE;
						}
				}
				else
				{
						//∏Ê÷™clientµÁª˙ŒﬁcanœÏ”¶
						return FALSE;
				}
		}
		else
		{
				//∏Ê÷™clientµÁª˙can∑¢ÀÕ ß∞‹
				return FALSE;
		}	
}

/********************************************************
∫Ø ˝π¶ƒ‹£∫◊™œÚ¬÷◊”◊ﬂŒª÷√
i		 £∫«˝∂Ø∆˜ID∫≈
Velocity £∫Œª÷√ƒ£ ΩÀŸ∂»
Position £∫“™◊ﬂµƒŒª÷√
*********************************************************/
bool MotionTurningPosition(u32 sendid , u32 filterid, long speed, long position) 
{
		//1°¢…Ë÷√ƒ£ Ω
		MotorSetMode(sendid , filterid ,MODE_POS); 
		//2°¢ πƒ‹
		MotorEnable(sendid , filterid);
		//3°¢‘À––
		MotorPositionRun(sendid , filterid , position,speed*33.33333);
		return TRUE;
}

void MotorMoveStop(void)
{
	MotorStop(ID_TX_MOTOR_MOVE2 , ID_RX_MOTOR_MOVE2);
	MotorStop(ID_TX_MOTOR_MOVE4 , ID_RX_MOTOR_MOVE4);
	MotorStop(ID_TX_MOTOR_MOVE6 , ID_RX_MOTOR_MOVE6);
	MotorStop(ID_TX_MOTOR_MOVE8 , ID_RX_MOTOR_MOVE8);
}
void MotorTurnStop(void)
{
	MotorStop(ID_TX_MOTOR_TURN1 , ID_RX_MOTOR_TURN1);
	MotorStop(ID_TX_MOTOR_TURN3 , ID_RX_MOTOR_TURN3);
	MotorStop(ID_TX_MOTOR_TURN5 , ID_RX_MOTOR_TURN5);
	MotorStop(ID_TX_MOTOR_TURN7 , ID_RX_MOTOR_TURN7);
}
void MotorStopAll(void)
{
	MotorMoveStop();
	MotorTurnStop();
}
void MotorDisableAll(void)
{
	MotorDisEnable(ID_TX_MOTOR_TURN1 , ID_RX_MOTOR_TURN1);
	MotorDisEnable(ID_TX_MOTOR_TURN3 , ID_RX_MOTOR_TURN3);
	MotorDisEnable(ID_TX_MOTOR_TURN5 , ID_RX_MOTOR_TURN5);
	MotorDisEnable(ID_TX_MOTOR_TURN7 , ID_RX_MOTOR_TURN7);
	MotorDisEnable(ID_TX_MOTOR_MOVE2 , ID_RX_MOTOR_MOVE2);
	MotorDisEnable(ID_TX_MOTOR_MOVE4 , ID_RX_MOTOR_MOVE4);
	MotorDisEnable(ID_TX_MOTOR_MOVE6 , ID_RX_MOTOR_MOVE6);
	MotorDisEnable(ID_TX_MOTOR_MOVE8 , ID_RX_MOTOR_MOVE8);	
}
/****************************************************************************
∫Ø ˝π¶ƒ‹£∫«∞√Ê◊™œÚµÁª˙◊ﬂŒª÷√–˝◊™
turnpos £∫◊™œÚµÁª˙“™‘À––µƒŒª÷√÷µ
****************************************************************************/
void ForwardWheel(double turnpos)  
{
 
	if(MotionTurningPosition(ID_TX_MOTOR_TURN1 , ID_RX_MOTOR_TURN1,100*33.33333,turnpos)==FALSE)
	{
		MotorStopAll();
		BSP_Write_DOUT1_24(HC595_DOUT_1 ,1);
		//RedLight();// πæØ æµ∆¡¡∫Ïµ∆
	}
 	if(MotionTurningPosition(ID_TX_MOTOR_TURN7 , ID_RX_MOTOR_TURN7,100*33.33333,turnpos)==FALSE)
	{
		MotorStopAll();
		BSP_Write_DOUT1_24(HC595_DOUT_1 ,1);
		//RedLight();// «æØ æµ∆¡¡∫Ïµ∆
	}
}
/****************************************************************************
∫Ø ˝π¶ƒ‹£∫∫Û√Ê◊™œÚµÁª˙◊ﬂŒª÷√–˝◊™
turnpos £∫◊™œÚµÁª˙“™‘À––µƒŒª÷√÷µ
****************************************************************************/
void BackwardWheel(double turnpos)
{
	
 
	if(MotionTurningPosition(ID_TX_MOTOR_TURN3 , ID_RX_MOTOR_TURN3,100*33.33333,turnpos)==FALSE)
	{
		MotorStopAll();
		BSP_Write_DOUT1_24(HC595_DOUT_1 ,1);
		//RedLight();// πæØ æµ∆¡¡∫Ïµ∆
	}
 	if(MotionTurningPosition(ID_TX_MOTOR_TURN5 , ID_RX_MOTOR_TURN5,100*33.33333,turnpos)==FALSE)
	{
		MotorStopAll();
		BSP_Write_DOUT1_24(HC595_DOUT_1 ,1);
		//RedLight();	// πæØ æµ∆¡¡∫Ïµ∆
	}
}
/****************************************************************************
∫Ø ˝π¶ƒ‹£∫”“√Ê◊™œÚµÁª˙◊ﬂŒª÷√–˝◊™
turnpos £∫◊™œÚµÁª˙“™‘À––µƒŒª÷√÷µ
****************************************************************************/
void RightWheel(double turnpos)
{

	if(MotionTurningPosition(ID_TX_MOTOR_TURN1 , ID_RX_MOTOR_TURN1,100*33.33333,turnpos+240000)==FALSE)
	{
		MotorStopAll();
		BSP_Write_DOUT1_24(HC595_DOUT_1 ,1);
		//RedLight();// πæØ æµ∆¡¡∫Ïµ∆
	}
 	if(MotionTurningPosition(ID_TX_MOTOR_TURN3 , ID_RX_MOTOR_TURN3,100*33.33333,turnpos-240000)==FALSE)
	{
		MotorStopAll();
		BSP_Write_DOUT1_24(HC595_DOUT_1 ,1);
		//RedLight();// πæØ æµ∆¡¡∫Ïµ∆
	}
}
/****************************************************************************
∫Ø ˝π¶ƒ‹£∫◊Û√Ê◊™œÚµÁª˙◊ﬂŒª÷√–˝◊™
turnpos £∫◊™œÚµÁª˙“™‘À––µƒŒª÷√÷µ
****************************************************************************/
void LeftWheel(double turnpos)
{
	if(MotionTurningPosition(ID_TX_MOTOR_TURN5 , ID_RX_MOTOR_TURN5,100*33.33333,turnpos+240000)==FALSE)
	{
		MotorStopAll();
		BSP_Write_DOUT1_24(HC595_DOUT_1 ,1);
		//RedLight();// πæØ æµ∆¡¡∫Ïµ∆
	}
 	if(MotionTurningPosition(ID_TX_MOTOR_TURN7 , ID_RX_MOTOR_TURN7,100*33.33333,turnpos-240000)==FALSE)
	{
		MotorStopAll();
		BSP_Write_DOUT1_24(HC595_DOUT_1 ,1);
		//RedLight();// πæØ æµ∆¡¡∫Ïµ∆
	}
}


bool ElmoMotorHome(u32 sendid , u32 filterid)
{
		static u8 CanSendData1[8]={0x23,0x99,0x60,0x01,0x10,0x87,0x00,0x00};  //vref=10000 counts/s until limit switch is reached
		static u8 CanSendData2[8]={0x23,0x99,0x60,0x02,0x88,0x13,0x00,0x00};  //vref=5000 counts/s from limit switch to zero mark
		static u8 CanSendData3[8]={0x23,0x9A,0x60,0x00,0x10,0xa7,0x00,0x00}; // Decel. and Accel. ramp  1000counts/s
		static u8 CanSendData4[8]={0x23,0x7C,0x60,0x00,0x00,0x00,0x00,0x00};  //Reference offset 25000counts
		static u8 CanSendData5[8]={0x2b,0x98,0x60,0x00,0x1c,0x00,0x00,0x00};	
		static const u8 *msgarr[] = { CanSendData1 , CanSendData2 ,CanSendData3 , CanSendData4 ,CanSendData5 };

		u8 err = 0 , i = 0;
		CanRxMsg *prxmsg = NULL;
		
		for(i = 0 ;i!=5 ;i++)
		{
				if(BSP_CAN_SendMsg(sendid ,filterid , (u8*)msgarr[i]) == TRUE)
				{
						prxmsg = (CanRxMsg*) OSMboxPend(CANItMbox , 100 , &err);
					
						if(err == OS_ERR_NONE)
						{
#if DEF_DEBUG_CFG & DEF_DEBUG_CAN	
						myPrintf("Recv->  ID:%04X  DATA:%02X %02X %02X %02X %02X %02X %02X %02X \r\n",prxmsg->StdId , prxmsg->Data[0] , prxmsg->Data[1] , prxmsg->Data[2] , prxmsg->Data[3] ,
					                                                                      prxmsg->Data[4] , prxmsg->Data[5] , prxmsg->Data[6] , prxmsg->Data[7]  );		
#endif										
								if(msgarr[i][1] ==	prxmsg->Data[1] && msgarr[i][2]  ==	prxmsg->Data[2])
								{
										continue;
								}
								else
								{
										//∏Ê÷™clientµÁª˙÷¥–– ß∞‹
										return FALSE;
								}
						}
						else
						{
								//∏Ê÷™clientµÁª˙ŒﬁcanœÏ”¶
								return FALSE;
						}
				}
				else
				{
						//∏Ê÷™clientµÁª˙can∑¢ÀÕ ß∞‹
						return FALSE;
				}				
		}
		return TRUE;//÷¥––≥…π¶	
}


void ResetZeroWheel(void)
{
		TYPEDEF_TURN_MOTRO_POS turnpos;
    u8 i = 3;u16 cout = 0;

	  /*turn motor HOME run*/
	  MotorSetMode(ID_TX_MOTOR_TURN1 , ID_RX_MOTOR_TURN1 , MODE_HOME);
		MotorSetMode(ID_TX_MOTOR_TURN3 , ID_RX_MOTOR_TURN3 , MODE_HOME);
		MotorSetMode(ID_TX_MOTOR_TURN5 , ID_RX_MOTOR_TURN5 , MODE_HOME);
		MotorSetMode(ID_TX_MOTOR_TURN7 , ID_RX_MOTOR_TURN7 , MODE_HOME);	
	
	  ElmoMotorHome(ID_TX_MOTOR_TURN1 , ID_RX_MOTOR_TURN1 );
		ElmoMotorHome(ID_TX_MOTOR_TURN3 , ID_RX_MOTOR_TURN3 );
		ElmoMotorHome(ID_TX_MOTOR_TURN5 , ID_RX_MOTOR_TURN5 );
		ElmoMotorHome(ID_TX_MOTOR_TURN7 , ID_RX_MOTOR_TURN7 );		

		MotorEnable(ID_TX_MOTOR_TURN1 , ID_RX_MOTOR_TURN1);	
		MotorEnable(ID_TX_MOTOR_TURN3 , ID_RX_MOTOR_TURN3);	
		MotorEnable(ID_TX_MOTOR_TURN5 , ID_RX_MOTOR_TURN5);	
		MotorEnable(ID_TX_MOTOR_TURN7 , ID_RX_MOTOR_TURN7);

		MotorHomeRun(ID_TX_MOTOR_TURN1 , ID_RX_MOTOR_TURN1);	
		MotorHomeRun(ID_TX_MOTOR_TURN3 , ID_RX_MOTOR_TURN3);	
		MotorHomeRun(ID_TX_MOTOR_TURN5 , ID_RX_MOTOR_TURN5);	
		MotorHomeRun(ID_TX_MOTOR_TURN7 , ID_RX_MOTOR_TURN7);

    do
		{
			 OSTimeDly(100);
			 RevTurnPosition(&turnpos);
			 //myPrintf("ResetZeroWheel home: pos1:%d  pos3:%d  pos5:%d  pos7:%d\r\n",turnpos.pos1 , turnpos.pos3 , turnpos.pos5 ,turnpos.pos7);
       cout++;
       if(cout > 600 || g_stopflag)
       {
				   	MotorTurnStop();
           return;
			 }				 
					
		}
		while(!ALLOWERR(turnpos.pos1 , 0 , ERRVALUE0) || !ALLOWERR(turnpos.pos3 , 0 , ERRVALUE0) || !ALLOWERR(turnpos.pos5 , 0 , ERRVALUE0) || !ALLOWERR(turnpos.pos7 , 0 , ERRVALUE0));
   
		/*turn motor POSITION run to -80000 */
		MotorSetMode(ID_TX_MOTOR_TURN1 , ID_RX_MOTOR_TURN1 , MODE_POS);	
		MotorSetMode(ID_TX_MOTOR_TURN3 , ID_RX_MOTOR_TURN3 , MODE_POS);	
		MotorSetMode(ID_TX_MOTOR_TURN5 , ID_RX_MOTOR_TURN5 , MODE_POS);	
		MotorSetMode(ID_TX_MOTOR_TURN7 , ID_RX_MOTOR_TURN7 , MODE_POS);
	
		MotorEnable(ID_TX_MOTOR_TURN1 , ID_RX_MOTOR_TURN1);	
		MotorEnable(ID_TX_MOTOR_TURN3 , ID_RX_MOTOR_TURN3);	
		MotorEnable(ID_TX_MOTOR_TURN5 , ID_RX_MOTOR_TURN5);	
		MotorEnable(ID_TX_MOTOR_TURN7 , ID_RX_MOTOR_TURN7);
	
		MotorPositionRun(ID_TX_MOTOR_TURN1 , ID_RX_MOTOR_TURN1 , -80000 , 500*33.333);
		MotorPositionRun(ID_TX_MOTOR_TURN3 , ID_RX_MOTOR_TURN3 , -80000 , 500*33.333);
		MotorPositionRun(ID_TX_MOTOR_TURN5 , ID_RX_MOTOR_TURN5 , -80000 , 500*33.333);
		MotorPositionRun(ID_TX_MOTOR_TURN7 , ID_RX_MOTOR_TURN7 , -80000 , 500*33.333);
		
		cout = 0;
		do
		{
			 OSTimeDly(100);
			 RevTurnPosition(&turnpos);
			 myPrintf("ResetZeroWheel compensate: pos1:%d  pos3:%d  pos5:%d  pos7:%d\r\n",turnpos.pos1 , turnpos.pos3 , turnpos.pos5 ,turnpos.pos7);
			 if(cout > 600|| g_stopflag)
			 {
					 myPrintf("ResetZeroWheel home  timeout\r\n");
				 	 MotorTurnStop();
					 return;
			 }						
					
		}
		while(!ALLOWERR(turnpos.pos1 , -80000 , ERRVALUE1) || !ALLOWERR(turnpos.pos3 , -80000 , ERRVALUE1) || !ALLOWERR(turnpos.pos5 , -80000 , ERRVALUE1) || !ALLOWERR(turnpos.pos7 , -80000 , ERRVALUE1));

	 /*turn motor clear encoder to zero */ 
		while(i--)
		{
				OSTimeDly(100);
				MotorClearEncoder(ID_TX_MOTOR_TURN1 , ID_RX_MOTOR_TURN1);	
				MotorClearEncoder(ID_TX_MOTOR_TURN3 , ID_RX_MOTOR_TURN3);	
				MotorClearEncoder(ID_TX_MOTOR_TURN5 , ID_RX_MOTOR_TURN5);	
				MotorClearEncoder(ID_TX_MOTOR_TURN7 , ID_RX_MOTOR_TURN7);	
		}
		
		cout = 0;
		do
		{
				OSTimeDly(100);
				RevTurnPosition(&turnpos);
				myPrintf("ResetZeroWheel clearencoder: pos1:%d  pos3:%d  pos5:%d  pos7:%d\r\n",turnpos.pos1 , turnpos.pos3 , turnpos.pos5 ,turnpos.pos7);
				if(cout > 200|| g_stopflag)
				{
					 //myPrintf("ResetZeroWheel home  timeout\r\n");
					 MotorTurnStop();
					 return;
				}		
		}
		while(!ALLOWERR(turnpos.pos1 , 0 , ERRVALUE0) || !ALLOWERR(turnpos.pos3 , 0 , ERRVALUE0) || !ALLOWERR(turnpos.pos5 , 0 , ERRVALUE0) || !ALLOWERR(turnpos.pos7 , 0 , ERRVALUE0));

    /*turn motor acc set */
		MotorSetACC(ID_TX_MOTOR_TURN1 , ID_RX_MOTOR_TURN1 , 60000);	
		MotorSetACC(ID_TX_MOTOR_TURN3 , ID_RX_MOTOR_TURN3 , 60000);	
		MotorSetACC(ID_TX_MOTOR_TURN5 , ID_RX_MOTOR_TURN5 , 60000);		
		MotorSetACC(ID_TX_MOTOR_TURN7 , ID_RX_MOTOR_TURN7 , 60000);	
    /*turn motor dcc set */		
		MotorSetDCC(ID_TX_MOTOR_TURN1 , ID_RX_MOTOR_TURN1 , 60000);	
		MotorSetDCC(ID_TX_MOTOR_TURN3 , ID_RX_MOTOR_TURN3 , 60000);		
		MotorSetDCC(ID_TX_MOTOR_TURN5 , ID_RX_MOTOR_TURN5 , 60000);		
		MotorSetDCC(ID_TX_MOTOR_TURN7 , ID_RX_MOTOR_TURN7 , 60000);

		MotorDisEnable(ID_TX_MOTOR_MOVE2 , ID_RX_MOTOR_MOVE2);	
		MotorDisEnable(ID_TX_MOTOR_MOVE4 , ID_RX_MOTOR_MOVE4);	
		MotorDisEnable(ID_TX_MOTOR_MOVE6 , ID_RX_MOTOR_MOVE6);	
		MotorDisEnable(ID_TX_MOTOR_MOVE8 , ID_RX_MOTOR_MOVE8);
    /*move motor acc set */
		MotorSetACC(ID_TX_MOTOR_MOVE2 , ID_RX_MOTOR_MOVE2 , 15000);	
		MotorSetACC(ID_TX_MOTOR_MOVE4 , ID_RX_MOTOR_MOVE4 , 15000);	
		MotorSetACC(ID_TX_MOTOR_MOVE6 , ID_RX_MOTOR_MOVE6 , 15000);		
		MotorSetACC(ID_TX_MOTOR_MOVE8 , ID_RX_MOTOR_MOVE8 , 15000);	
    /*move motor dcc set */
		MotorSetDCC(ID_TX_MOTOR_MOVE2 , ID_RX_MOTOR_MOVE2 , 45000);	
		MotorSetDCC(ID_TX_MOTOR_MOVE4 , ID_RX_MOTOR_MOVE4 , 45000);		
		MotorSetDCC(ID_TX_MOTOR_MOVE6 , ID_RX_MOTOR_MOVE6 , 45000);		
		MotorSetDCC(ID_TX_MOTOR_MOVE8 , ID_RX_MOTOR_MOVE8 , 45000);

		
}


void TestTurnMotor(void)
{	
		MotorDisEnable(ID_TX_MOTOR_TURN1 , ID_RX_MOTOR_TURN1);
		MotorDisEnable(ID_TX_MOTOR_TURN3 , ID_RX_MOTOR_TURN3);	
		MotorDisEnable(ID_TX_MOTOR_TURN5 , ID_RX_MOTOR_TURN5);	
		MotorDisEnable(ID_TX_MOTOR_TURN7 , ID_RX_MOTOR_TURN7);
	
		MotorClearEncoder(ID_TX_MOTOR_TURN1 , ID_RX_MOTOR_TURN1);	
		MotorClearEncoder(ID_TX_MOTOR_TURN3 , ID_RX_MOTOR_TURN3);	
		MotorClearEncoder(ID_TX_MOTOR_TURN5 , ID_RX_MOTOR_TURN5);	
		MotorClearEncoder(ID_TX_MOTOR_TURN7 , ID_RX_MOTOR_TURN7);

		MotorEnable(ID_TX_MOTOR_TURN1 , ID_RX_MOTOR_TURN1);	
		MotorEnable(ID_TX_MOTOR_TURN3 , ID_RX_MOTOR_TURN3);	
		MotorEnable(ID_TX_MOTOR_TURN5 , ID_RX_MOTOR_TURN5);	
		MotorEnable(ID_TX_MOTOR_TURN7 , ID_RX_MOTOR_TURN7);

	  MotorSetMode(ID_TX_MOTOR_TURN1 , ID_RX_MOTOR_TURN1 , MODE_POS);
		MotorSetMode(ID_TX_MOTOR_TURN3 , ID_RX_MOTOR_TURN3 , MODE_POS);
		MotorSetMode(ID_TX_MOTOR_TURN5 , ID_RX_MOTOR_TURN5 , MODE_POS);
		MotorSetMode(ID_TX_MOTOR_TURN7 , ID_RX_MOTOR_TURN7 , MODE_POS);
	
	  MotorPositionRun(ID_TX_MOTOR_TURN1 , ID_RX_MOTOR_TURN1 , 50000 , 300*333);
		MotorPositionRun(ID_TX_MOTOR_TURN3 , ID_RX_MOTOR_TURN3 , 50000 , 300*333);
		MotorPositionRun(ID_TX_MOTOR_TURN5 , ID_RX_MOTOR_TURN5 , 50000 , 300*333);
		MotorPositionRun(ID_TX_MOTOR_TURN7 , ID_RX_MOTOR_TURN7 , 50000 , 300*333);

	  OSTimeDly(3000);

	  MotorPositionRun(ID_TX_MOTOR_TURN1 , ID_RX_MOTOR_TURN1 , 0 , 300*333);
		MotorPositionRun(ID_TX_MOTOR_TURN3 , ID_RX_MOTOR_TURN3 , 0 , 300*333);
		MotorPositionRun(ID_TX_MOTOR_TURN5 , ID_RX_MOTOR_TURN5 , 0 , 300*333);
		MotorPositionRun(ID_TX_MOTOR_TURN7 , ID_RX_MOTOR_TURN7 , 0 , 300*333);
		
		OSTimeDly(3000);
		
		MotorStop(ID_TX_MOTOR_TURN1 , ID_RX_MOTOR_TURN1);	
		MotorStop(ID_TX_MOTOR_TURN3 , ID_RX_MOTOR_TURN3);	
		MotorStop(ID_TX_MOTOR_TURN5 , ID_RX_MOTOR_TURN5);	
		MotorStop(ID_TX_MOTOR_TURN7 , ID_RX_MOTOR_TURN7);	
		
		MotorDisEnable(ID_TX_MOTOR_TURN1 , ID_RX_MOTOR_TURN1);
		MotorDisEnable(ID_TX_MOTOR_TURN3 , ID_RX_MOTOR_TURN3);	
		MotorDisEnable(ID_TX_MOTOR_TURN5 , ID_RX_MOTOR_TURN5);	
		MotorDisEnable(ID_TX_MOTOR_TURN7 , ID_RX_MOTOR_TURN7);
}

void TestMoveMotor(void)
{
		MotorDisEnable(ID_TX_MOTOR_MOVE2 , ID_RX_MOTOR_MOVE2);
		MotorDisEnable(ID_TX_MOTOR_MOVE4 , ID_RX_MOTOR_MOVE4);	
		MotorDisEnable(ID_TX_MOTOR_MOVE6 , ID_RX_MOTOR_MOVE6);	
		MotorDisEnable(ID_TX_MOTOR_MOVE8 , ID_RX_MOTOR_MOVE8);
	
		MotorClearEncoder(ID_TX_MOTOR_MOVE2 , ID_RX_MOTOR_MOVE2);
		MotorClearEncoder(ID_TX_MOTOR_MOVE4 , ID_RX_MOTOR_MOVE4);	
		MotorClearEncoder(ID_TX_MOTOR_MOVE6 , ID_RX_MOTOR_MOVE6);	
		MotorClearEncoder(ID_TX_MOTOR_MOVE8 , ID_RX_MOTOR_MOVE8);

		MotorEnable(ID_TX_MOTOR_MOVE2 , ID_RX_MOTOR_MOVE2);
		MotorEnable(ID_TX_MOTOR_MOVE4 , ID_RX_MOTOR_MOVE4);	
		MotorEnable(ID_TX_MOTOR_MOVE6 , ID_RX_MOTOR_MOVE6);	
		MotorEnable(ID_TX_MOTOR_MOVE8 , ID_RX_MOTOR_MOVE8);

		MotorSetMode(ID_TX_MOTOR_MOVE2 , ID_RX_MOTOR_MOVE2 , MODE_POS);
		MotorSetMode(ID_TX_MOTOR_MOVE4 , ID_RX_MOTOR_MOVE4 , MODE_POS);
		MotorSetMode(ID_TX_MOTOR_MOVE6 , ID_RX_MOTOR_MOVE6 , MODE_POS);
		MotorSetMode(ID_TX_MOTOR_MOVE8 , ID_RX_MOTOR_MOVE8 , MODE_POS);
	
		MotorPositionRun(ID_TX_MOTOR_MOVE2 , ID_RX_MOTOR_MOVE2 , 50000 , 300*333);
		MotorPositionRun(ID_TX_MOTOR_MOVE4 , ID_RX_MOTOR_MOVE4 , 50000 , 300*333);
		MotorPositionRun(ID_TX_MOTOR_MOVE6 , ID_RX_MOTOR_MOVE6 , 50000 , 300*333);
		MotorPositionRun(ID_TX_MOTOR_MOVE8 , ID_RX_MOTOR_MOVE8 , 50000 , 300*333);

	  OSTimeDly(3000);

		MotorPositionRun(ID_TX_MOTOR_MOVE2 , ID_RX_MOTOR_MOVE2 , 0 , 300*333);
		MotorPositionRun(ID_TX_MOTOR_MOVE4 , ID_RX_MOTOR_MOVE4 , 0 , 300*333);
		MotorPositionRun(ID_TX_MOTOR_MOVE6 , ID_RX_MOTOR_MOVE6 , 0 , 300*333);
		MotorPositionRun(ID_TX_MOTOR_MOVE8 , ID_RX_MOTOR_MOVE8 , 0 , 300*333);
		
		OSTimeDly(3000);
		
		MotorStop(ID_TX_MOTOR_MOVE2 , ID_RX_MOTOR_MOVE2);
		MotorStop(ID_TX_MOTOR_MOVE4 , ID_RX_MOTOR_MOVE4);	
		MotorStop(ID_TX_MOTOR_MOVE6 , ID_RX_MOTOR_MOVE6);	
		MotorStop(ID_TX_MOTOR_MOVE8 , ID_RX_MOTOR_MOVE8);	
		
		MotorDisEnable(ID_TX_MOTOR_MOVE2 , ID_RX_MOTOR_MOVE2);
		MotorDisEnable(ID_TX_MOTOR_MOVE4 , ID_RX_MOTOR_MOVE4);	
		MotorDisEnable(ID_TX_MOTOR_MOVE6 , ID_RX_MOTOR_MOVE6);	
		MotorDisEnable(ID_TX_MOTOR_MOVE8 , ID_RX_MOTOR_MOVE8);		
}
