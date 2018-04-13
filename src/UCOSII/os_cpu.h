/*
*********************************************************************************************************
*                                               uC/OS-II
*                                         The Real-Time Kernel
*
*
*                                (c) Copyright 2006, Micrium, Weston, FL
*                                          All Rights Reserved
*
*                                           ARM Cortex-M3 Port
*
* File      : OS_CPU.H
* Version   : V2.86
* By        : Jean J. Labrosse
*
* For       : ARMv7M Cortex-M3
* Mode      : Thumb2
* Toolchain : RealView Development Suite
*             RealView Microcontroller Development Kit (MDK)
*             ARM Developer Suite (ADS)
*             Keil uVision
*********************************************************************************************************
*/

#ifndef  OS_CPU_H
#define  OS_CPU_H


#ifdef   OS_CPU_GLOBALS
#define  OS_CPU_EXT
#else
#define  OS_CPU_EXT  extern
#endif

/*
*********************************************************************************************************
*                                              DATA TYPES
*                                         (Compiler Specific)
*********************************************************************************************************
*/
typedef unsigned char  BOOLEAN;
typedef unsigned char  INT8U;
typedef signed char    INT8S;
typedef unsigned short INT16U;
typedef signed short   INT16S;
typedef unsigned int   INT32U;
typedef signed int     INT32S;
typedef float          FP32;
typedef double         FP64;
typedef unsigned int   OS_STK;
typedef unsigned int   OS_CPU_SR;

/*
*********************************************************************************************************
*                                              Cortex-M1
*                                      Critical Section Management
*
* Method #1:  Disable/Enable interrupts using simple instructions.  After critical section, interrupts
*             will be enabled even if they were disabled before entering the critical section.
*             NOT IMPLEMENTED
*
* Method #2:  Disable/Enable interrupts by preserving the state of interrupts.  In other words, if
*             interrupts were disabled before entering the critical section, they will be disabled when
*             leaving the critical section.
*             NOT IMPLEMENTED
*
* Method #3:  Disable/Enable interrupts by preserving the state of interrupts.  Generally speaking you
*             would store the state of the interrupt disable flag in the local variable 'cpu_sr' and then
*             disable interrupts.  'cpu_sr' is allocated in all of uC/OS-II's functions that need to
*             disable interrupts.  You would restore the interrupt disable state by copying back 'cpu_sr'
*             into the CPU's status register.
*********************************************************************************************************
*/

#define  OS_CRITICAL_METHOD   3

#if OS_CRITICAL_METHOD == 3
#define  OS_ENTER_CRITICAL()  {cpu_sr = OS_CPU_SR_Save();}
#define  OS_EXIT_CRITICAL()   {OS_CPU_SR_Restore(cpu_sr);}
#endif

/*
*********************************************************************************************************
*                                        Cortex-M3 Miscellaneous
*********************************************************************************************************
*/

#define  OS_STK_GROWTH        1
#define  OS_TASK_SW()         OSCtxSw()

/*
*********************************************************************************************************
*                                              PROTOTYPES
*********************************************************************************************************
*/

#if OS_CRITICAL_METHOD == 3
OS_CPU_SR  OS_CPU_SR_Save( void );
void       OS_CPU_SR_Restore( OS_CPU_SR cpu_sr );

void    CPU_IntDis( void );
void    CPU_IntEn( void );

#endif

void       OSCtxSw( void );
void       OSIntCtxSw( void );
void       OSStartHighRdy( void );
void       OS_CPU_PendSVHandler( void );
void       OS_CPU_SysTickInit(void);
#endif
