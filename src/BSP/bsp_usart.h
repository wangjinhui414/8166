#ifndef _BSP_USART_H_
#define _BSP_USART_H_

#include "bsp.h"

#define MAX_BUFF_SIZE   100

typedef struct TYPEDEF_USART_BUFF
{
	u8 _buf[MAX_BUFF_SIZE];
	u8 cnt;
}TYPEDEF_USART_BUFF;

void ClearUart5Buff(void);

#endif
