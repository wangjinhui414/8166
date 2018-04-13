/*****************************************************************************
					本文件存放巡线控制和相关的函数
*****************************************************************************/
#include "Patrol_control.h"
#include "hal_process_COLLECT.h"
#include "hal_process_CONTROL.h"
#include <math.h>

float l_dis;
PwData PidData ;
int    Led_flag=0;		//0光电巡线  1 视觉巡线



char AD_IO[56];
float pid_value_f[10];
float pid_value_b[10];


PID fPID; // PID Control Structure 
PID bPID; // PID Control Structure

#define  Pf 2666
#define  Pb 2666
#define  Pr 2666
#define  Pl 2666


void ClearFpidBpid(void)
{
	fPID.LastError=0;
	fPID.PrevError=0;
	fPID.SumError=0;
	bPID.LastError=0;
	bPID.PrevError=0;
	bPID.SumError=0;	
}


float PIDCalc( PID *pp, float CurrPoint ) 
{ 
	float  Error; 
	float PP,PI,PD;
	bPID.Proportion=fPID.Proportion=PidData.pid;

	Error = CurrPoint - pp->SetPoint ;           // 偏差 
 

	PP = pp->Proportion * (Error - pp->LastError);		// 比例项 
	PI = pp->Integral * Error;	// 积分项
	PD = pp->Derivative * (Error - 2*(pp->LastError) + pp->PrevError);		// 微分项 */

	pp->PrevError = pp->LastError; 
	pp->LastError = Error; 

	pp->SumError = pp->SumError+(PP + PI  + PD);
	printf("ERROR:%f\n",pp->SumError);
	return (pp->SumError);


}

/****************************************************************************
函数功能：红外巡线偏离角度计算算法
参数：激光灯数据
****************************************************************************/

long getdis(char Q1, char Q2, char Q3, char Q4, char Q5, char Q6, char Q7, char Q8,char Q9, char Q10, char Q11, char Q12, char Q13, char Q14)//红外巡线偏离角度计算算法
{
		static u8 test_data_flag=0;
		float dis = 0;
		l_dis = 0.58;
		/*
		○●●●●●●●●●●●●●
		○○●●●●●●●●●●●●
		○○○●●●●●●●●●●●
		○○○○●●●●●●●●●●
		○○○○○●●●●●●●●●
		○○○○○○●●●●●●●●
		○○○○○○○●●●●●●●需要偏转角度大点，因为第七个到第八个灯之间距离是1cm
		○○○○○○○○●●●●●●
		○○○○○○○○○●●●●●
		●○○○○○○○○●●●●●
		●○○○○○○○○○●●●●
		●●○○○○○○○○●●●●
		●●○○○○○○○○○●●●

		●●●○○○○○○○○●●●

		●●●○○○○○○○○○●●
		●●●●○○○○○○○○●●
		●●●●○○○○○○○○○●
		●●●●●○○○○○○○○●
		●●●●●○○○○○○○○○
		●●●●●●○○○○○○○○
		●●●●●●●○○○○○○○需要偏转角度大点，因为第七个到第八个灯之间距离是1cm
		●●●●●●●●○○○○○○
		●●●●●●●●●○○○○○
		●●●●●●●●●●○○○○
		●●●●●●●●●●●○○○
		●●●●●●●●●●●●○○
		●●●●●●●●●●●●●○
		*/
		if(Q1&&!Q2)  return dis = (-13) * l_dis;                          
		else if(Q1&&Q2&&(!Q3)) return  dis= (-12) ;						   
		else if(Q1&&Q3&&(!Q4)) return  dis= (-11) ;						   
		else if(Q1&&Q4&&(!Q5)) return  dis= (-10) * l_dis;			    
		else if(Q1&&Q5&&(!Q6)) return  dis= (-9) * l_dis;		
		else if(Q1&&Q6&&(!Q7)) return  dis= (-8) * l_dis;
		else if(Q1&&Q7&&(!Q8)) return  dis= (-7) * l_dis;	//需要偏转角度大点，因为第七个到第八个灯之间距离是1cm
		else if(Q1&&Q8&&(!Q9)) return  dis= (-6) * l_dis;	
		else if(Q1&&Q9&&(!Q10)) return  dis= (-5) * l_dis;	
		else if(!(Q1)&&Q2&&Q9&&(!Q10)) return  dis= (-4) * l_dis;
		else if(!(Q1)&&Q2&&Q10&&(!Q11)) return  dis= (-3) * l_dis;	
		else if(!(Q2)&&Q3&&Q10&&(!Q11)) return  dis= (-2) * l_dis;
		else if(!(Q2)&&Q3&&Q11&&(!Q12)) return  dis= (-1) * l_dis;
		else if(!(Q3)&&Q4&&Q11&&(!Q12)) return  dis= 0;					  
		else if(!(Q3)&&Q4&&Q12&&(!Q13)) return  dis= (1) * l_dis;	
		else if(!(Q4)&&Q5&&Q12&&(!Q13)) return  dis= (2) * l_dis;
		else if(!(Q4)&&Q5&&Q13&&(!Q14)) return  dis= (3) * l_dis;
		else if(!(Q5)&&Q6&&Q13&&(!Q14)) return  dis= (4) * l_dis;	
		else if(!(Q5)&&Q6&&Q14) return  dis= (5) * l_dis;				  
		else if(!(Q6)&&Q7&&Q14) return  dis= (6) * l_dis;	       
		else if(!(Q7)&&Q8&&Q14) return  dis= (7) * l_dis;//需要偏转角度大点，因为第七个到第八个灯之间距离是1cm	
		else if(!(Q8)&&Q9&&Q14) return  dis= (8) * l_dis;	
		else if(!(Q9)&&Q10&&Q14) return  dis= (9) * l_dis;	
		else if(!(Q10)&&Q11&&Q14) return  dis= (10) * l_dis;	
		else if(!(Q11)&&Q12&&Q14) return  dis= (11) * l_dis;
		else if(!(Q12)&&Q13&&Q14) return  dis= (12) * l_dis;
		else if(!(Q13)&&Q14) return  dis= (13) * l_dis;				 	 
		else 
		{
			test_data_flag=~test_data_flag;
			dis = (float)0.01;
			return (float)dis;
		}
}


char *un_data_frame(unsigned char data)
{
		static char xx[4];
		memset(xx,0,4);	
		switch(data)
		{
		case 0x30:  xx[0]=0;  xx[1]=0;  xx[2]=0;  xx[3]=0; break;
		case 0x31:  xx[0]=0;  xx[1]=0;  xx[2]=0;  xx[3]=1;  break;
		case 0x32:  xx[0]=0;  xx[1]=0;  xx[2]=1;  xx[3]=0 ; break;
		case 0x33:  xx[0]=0;  xx[1]=0;  xx[2]=1;  xx[3]=1 ; break;
		case 0x34:  xx[0]=0;  xx[1]=1;  xx[2]=0;  xx[3]=0 ; break;
		case 0x35:  xx[0]=0;  xx[1]=1;  xx[2]=0;  xx[3]=1 ; break;
		case 0x36:  xx[0]=0;  xx[1]=1;  xx[2]=1;  xx[3]=0 ; break;
		case 0x37:  xx[0]=0;  xx[1]=1;  xx[2]=1;  xx[3]=1 ; break;
		case 0x38:  xx[0]=1;  xx[1]=0;  xx[2]=0;  xx[3]=0 ; break;
		case 0x39:  xx[0]=1;  xx[1]=0;  xx[2]=0;  xx[3]=1 ; break;
		case 0x41: xx[0]=1;  xx[1]=0;  xx[2]=1;  xx[3]=0 ; break;
		case 0x42: xx[0]=1;  xx[1]=0;  xx[2]=1;  xx[3]=1 ; break;
		case 0x43: xx[0]=1;  xx[1]=1;  xx[2]=0;  xx[3]=0 ; break;
		case 0x44: xx[0]=1;  xx[1]=1;  xx[2]=0;  xx[3]=1 ; break;
		case 0x45: xx[0]=1;  xx[1]=1;  xx[2]=1;  xx[3]=0 ; break;
		case 0x46: xx[0]=1;  xx[1]=1;  xx[2]=1;  xx[3]=1 ; break;
		default: xx[0]=0;  xx[1]=0;  xx[2]=0;  xx[3]=0;break;
		}
		return (xx);
}
/*****************************************************************************************
函数功能：激光巡线灯数据处理
*****************************************************************************************/
void OnReadIR(void)
{
	   char *array1;
	   memset(AD_IO,0,56);   
	
	   array1=un_data_frame(g_receive11[3]);
	   AD_IO[0]=array1[3];
	   AD_IO[1]=array1[2];
	   AD_IO[2]=array1[1];
	   AD_IO[3]=array1[0];
	   array1=un_data_frame(g_receive11[2]);
	   AD_IO[4]=array1[3];
	   AD_IO[5]=array1[2];
	   AD_IO[6]=array1[1];
	   AD_IO[7]=array1[0];
	   array1=un_data_frame(g_receive11[1]);
	   AD_IO[8]=array1[3];
	   AD_IO[9]=array1[2];
	   AD_IO[10]=array1[1];
	   AD_IO[11]=array1[0];
	   array1=un_data_frame(g_receive11[0]);
	   AD_IO[12]=array1[3];
	   AD_IO[13]=array1[2];
	   array1=un_data_frame(g_receive11[4]);
	   AD_IO[14]=array1[2];
	   AD_IO[15]=array1[3];
	   array1=un_data_frame(g_receive11[5]);
	   AD_IO[16]=array1[0];
	   AD_IO[17]=array1[1];
	   AD_IO[18]=array1[2];
	   AD_IO[19]=array1[3];
	   array1=un_data_frame(g_receive11[6]);
	   AD_IO[20]=array1[0];
	   AD_IO[21]=array1[1];
	   AD_IO[22]=array1[2];
	   AD_IO[23]=array1[3];
	   array1=un_data_frame(g_receive11[7]);
	   AD_IO[24]=array1[0];
	   AD_IO[25]=array1[1];
	   AD_IO[26]=array1[2];
	   AD_IO[27]=array1[3];
	   array1=un_data_frame(g_receive11[8]);
	   AD_IO[42]=array1[2];
	   AD_IO[43]=array1[3];
	   array1=un_data_frame(g_receive11[9]);
	   AD_IO[44]=array1[0];
	   AD_IO[45]=array1[1];
	   AD_IO[46]=array1[2];
	   AD_IO[47]=array1[3];
	   array1=un_data_frame(g_receive11[10]);
	   AD_IO[48]=array1[0];
	   AD_IO[49]=array1[1];
	   AD_IO[50]=array1[2];
	   AD_IO[51]=array1[3];
	   array1=un_data_frame(g_receive11[11]);
	   AD_IO[52]=array1[0];
	   AD_IO[53]=array1[1];
	   AD_IO[54]=array1[2];
	   AD_IO[55]=array1[3];
	   array1=un_data_frame(g_receive11[15]);
	   AD_IO[28]=array1[3];
	   AD_IO[29]=array1[2];
	   AD_IO[30]=array1[1];
	   AD_IO[31]=array1[0];
	   array1=un_data_frame(g_receive11[14]);
	   AD_IO[32]=array1[3];
	   AD_IO[33]=array1[2];
	   AD_IO[34]=array1[1];
	   AD_IO[35]=array1[0];
	   array1=un_data_frame(g_receive11[13]);
	   AD_IO[36]=array1[3];
	   AD_IO[37]=array1[2];
	   AD_IO[38]=array1[1];
	   AD_IO[39]=array1[0];
	   array1=un_data_frame(g_receive11[12]);
	   AD_IO[40]=array1[3];
	   AD_IO[41]=array1[2];	
}


/*****************************************************************************************
函数功能：前限位
*****************************************************************************************/
bool isFrontBlimited(void)
{
   bool bResult;
   if (AD_IO[28]||AD_IO[29]||AD_IO[30]||AD_IO[31]||AD_IO[32]||AD_IO[33]||AD_IO[34]||AD_IO[35]||AD_IO[36]||AD_IO[37]||AD_IO[38]||AD_IO[39]||AD_IO[40]||AD_IO[41])
   {
	   bResult = TRUE;
   }
   else
   {
	   bResult = FALSE;
   }
   return bResult;
}
/*****************************************************************************************
函数功能：后限位,没有用到
*****************************************************************************************/
bool isBackFlimited(void)//
{
	bool bResult;
	if (AD_IO[30]||AD_IO[31])
	{
		bResult = TRUE;
	}
	else
	{
		bResult = FALSE;
	}
	return bResult;
}
/*****************************************************************************************
函数功能：左限位
*****************************************************************************************/
bool isLeftRlimited(void)  
{
		bool bResult;
	if (AD_IO[0]==1||AD_IO[1]==1||AD_IO[2]==1||AD_IO[3]==1||AD_IO[10]==1||AD_IO[11]==1||AD_IO[12]==1||AD_IO[13]==1
	  ||AD_IO[14]==1||AD_IO[15]==1||AD_IO[16]==1||AD_IO[17]==1||AD_IO[24]==1||AD_IO[25]==1||AD_IO[26]==1||AD_IO[27]==1	
		)
	{
		bResult = TRUE;
	}
	else
	{
		bResult = FALSE;
	}
	return bResult;
}
/*****************************************************************************************
函数功能：右限位,没有用到
*****************************************************************************************/
bool isRightLlimited(void)
{
	bool bResult;
	if (AD_IO[3]==1||AD_IO[4]==1||AD_IO[10]==1||AD_IO[11]==1)
	{
		bResult = TRUE;
	}
	else
	{
		bResult = FALSE;
	}
	return bResult;
}

/****************************************************************************
函数功能：获取后面十次平均值
****************************************************************************/
float PidFlashAvrB(void)
{
    float temp_count_avr;
	int te_ii;
	temp_count_avr =0;
	for(te_ii=0;te_ii<3;te_ii++)
	{
		temp_count_avr = temp_count_avr +pid_value_b[te_ii];
	}

	temp_count_avr = temp_count_avr/3;
	return temp_count_avr;

}
/****************************************************************************
函数功能：获取前面十次平均值
****************************************************************************/
float PidFlashAvrF(void)
{
    float temp_count_avr;
	int te_ii;
temp_count_avr =0;
	for(te_ii=0;te_ii<3;te_ii++)
	{
		temp_count_avr = temp_count_avr +pid_value_f[te_ii];
	}

	temp_count_avr = temp_count_avr/3;
	return temp_count_avr;

}

int m_LoseLineNub;
float tempdisF=0,tempdisB=0,tempdisL=0,tempdisR=0;
float rIn;
float disF,disB;
void Tracking(TrackingDir dir)//直行前进后退，根据红外传感器，实时调整轮子角度
{
	
	float temp_fl,temp_bl;
	
	short tem_sh;
	fPID.Proportion=0.5;
	bPID.Proportion=0.5;
	fPID.Integral=0.05;
	bPID.Integral=0.05;
	fPID.Derivative=2;
	bPID.Derivative=2;
// 	fPID.Derivative=0;
// 	bPID.Derivative=0;
	fPID.SetPoint=0;
	bPID.SetPoint=0;
	
  
  OnReadIR();
	if(Led_flag==0)
	{
		

		//普通按比例循线方式 开始
		disF = getdis(AD_IO[0],AD_IO[1],AD_IO[2],AD_IO[3],AD_IO[4],AD_IO[5],AD_IO[6],AD_IO[7],AD_IO[8],AD_IO[9],AD_IO[10],AD_IO[11],AD_IO[12],AD_IO[13]);
		disB = getdis(AD_IO[14],AD_IO[15],AD_IO[16],AD_IO[17],AD_IO[18],AD_IO[19],AD_IO[20],AD_IO[21],AD_IO[22],AD_IO[23],AD_IO[24],AD_IO[25],AD_IO[26],AD_IO[27]);

		rIn = disF;

		disF = PIDCalc ( &fPID,rIn ); // Perform PID Interation 
		 
		rIn = 0;

		rIn = disB; 
		disB = PIDCalc ( &bPID,rIn ); // Perform PID Interation    	
	}
	else
	{
		temp_bl = PidFlashAvrB();
		temp_fl = PidFlashAvrF();

		fPID.SetPoint = 0;
		disF = PIDCalc ( &fPID,temp_fl ); // Perform PID Interation 

		bPID.SetPoint = 0;
		disB = PIDCalc ( &bPID,temp_bl ); // Perform PID Interation 

		tem_sh = (short)floor(disF*10+0.5);
		disF = (float)(tem_sh/10.0);     
		tem_sh = (short)floor(disB*10+0.5);
		disB = (float)(tem_sh/10.0);   

		if(disF>60)
		{	
			disF= 60  ;   
		}
		if(disF<-60)
		{	
			disF= -60  ;   
		}
		if(disB>60)
		{	
			disB= 60  ;   
		}
		if(disB<-60)
		{	
			disB= -60  ;   
		}
	}



	if( disF || disB ) 
	{
			if (tempdisB != disB || tempdisF != disF)
			{
					if ((disB > -60 && disB < 60) && (disF > -60 && disF < 60))
					{
							if( Forward == dir )
							{

								ForwardWheel(-disF*Pf);
								BackwardWheel(-disB*Pb);
							}
							else if(Backward == dir )
							{
								ForwardWheel(disF*Pf);
								BackwardWheel(disB*Pb);
							}
					}
					else
					{
							ForwardWheel(0);
							BackwardWheel(0);
					}
					tempdisF = disF;
					tempdisB = disB;
			}
	}
	else
	{// 不能正确获得位置
			ForwardWheel(0);
			BackwardWheel(0);
	}
	if(Led_flag==0)
	{
			if (((!AD_IO[0])&&(!AD_IO[1])&&(!AD_IO[2])&&(!AD_IO[3])&&(!AD_IO[4])&&(!AD_IO[5])&&(!AD_IO[6])&&(!AD_IO[7]) && (!AD_IO[8])&&(!AD_IO[9])&&(!AD_IO[10])&&(!AD_IO[11])&&(!AD_IO[12])&&(!AD_IO[13])) || 
							 ((!AD_IO[14]) &&(!AD_IO[15]) &&(!AD_IO[16]) &&(!AD_IO[17]) &&(!AD_IO[18]) &&(!AD_IO[19]) &&(!AD_IO[20]) &&(!AD_IO[21]) &&(!AD_IO[22]) &&(!AD_IO[23]) &&(!AD_IO[24]) &&(!AD_IO[25]) &&(!AD_IO[26]) &&(!AD_IO[27])))
			{
						m_LoseLineNub ++;
						if (m_LoseLineNub >= 15)//稳定状态;
						{
							MotorStopAll();
							m_LoseLineNub = 0;
						}
			}
	}
	else
	{	
			if (fabs(temp_fl) < 0.001
				|| fabs(temp_bl) < 0.001)//判断返回值是否为0，若为0则返回0，不做视频处理
			{
						m_LoseLineNub ++;
						if (m_LoseLineNub >= 40)//稳定状态;
						{
							MotorStopAll();				 
							m_LoseLineNub = 0;
						}
			}
	
	}
}

/*****************************************************************************************
函数功能：90度左右行走，根据红外传感器，实时调整轮子角度
dir：左面轮子或右面轮子
*****************************************************************************************/
void TrackingLR(TrackingDir dir)
{
	float  disL,disR;
  OnReadIR();	



	// 循线算法
 	disR = getdis(AD_IO[28],AD_IO[29],AD_IO[30],AD_IO[31],AD_IO[32],AD_IO[33],AD_IO[34],AD_IO[35],AD_IO[36],AD_IO[37],AD_IO[38],AD_IO[39],AD_IO[40],AD_IO[41]); 
	disL = getdis(AD_IO[42],AD_IO[43],AD_IO[44],AD_IO[45],AD_IO[46],AD_IO[47],AD_IO[48],AD_IO[49],AD_IO[50],AD_IO[51],AD_IO[52],AD_IO[53],AD_IO[54],AD_IO[55]);
		

		
	if( disR || disL) 
	{
 
        if (tempdisL != disL || tempdisR != disR)
        {	
			if ((disL > -60 && disL < 60) && (disR > -60 && disR < 60))
			{
				if( Right == dir )//+
				{
//					RightWheel(-disR*Pr);
//					LeftWheel (-disL*Pl);
					RightWheel(-disR*Pr);
				}
				else if(Left == dir )//-
				{
//					RightWheel(disR*Pr);
//					LeftWheel (disL*Pl);
					LeftWheel (disL*Pl);
				}
			}
			else
			{
				RightWheel(0);
				LeftWheel (0);
			}
			tempdisR = disR;
			tempdisL = disL;
		}
	}
	else
	{// 不能正确获得位置
		RightWheel(0);
		LeftWheel (0);
	}



	if (((!AD_IO[29]) && (!AD_IO[29])&&(!AD_IO[30])&&(!AD_IO[31])&&(!AD_IO[32])&&(!AD_IO[33])&&(!AD_IO[34])&&(!AD_IO[35])&&(!AD_IO[36]) && (!AD_IO[37])&&(!AD_IO[38])&&(!AD_IO[39])&&(!AD_IO[40])&&(!AD_IO[41]))|| 
		((!AD_IO[43]) && (!AD_IO[43])&&(!AD_IO[44])&&(!AD_IO[45])&&(!AD_IO[46])&&(!AD_IO[47])&&(!AD_IO[48])&&(!AD_IO[49])&&(!AD_IO[50]) && (!AD_IO[51])&&(!AD_IO[52])&&(!AD_IO[53])&&(!AD_IO[54])&&(!AD_IO[55]))) 
	{
		 m_LoseLineNub ++;
		if (m_LoseLineNub >= 15)//稳定状态;
		{
			MotorStopAll();
 
//			Log(_T("Off-line!"));
			m_LoseLineNub = 0;
		}

	}

} 

/*
*************************************************************************
避障函数 
 wpos1  判断的第一个声波传感器位置
 wpos2  判断的第二个声波传感器位置
 lpos1   判断的第一个光波传感器位置
 lpos2   判断的第二个光波传感器位置
返回值 0 无障碍物
       1 声波障碍
       2 光波障碍
       3 声光都障碍
*************************************************************************
*/
u8 Barrier(u16 wpos1 , u16 wpos2 ,u8 lpos1 , u8 lpos2)
{
	u8 ret = 0;
	u8 jval = 1<<lpos1 | 1<<lpos2;
	
	if(g_data_acousto_optic._wave[wpos1] > 1510 && g_data_acousto_optic._wave[wpos2] > 1510)
	{
		ret|=0x01;
	}
	if(g_data_acousto_optic._led & jval)
	{
		ret|=0x02;
	}
	
	return ret;
}
