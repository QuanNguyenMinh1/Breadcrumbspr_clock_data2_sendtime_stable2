#ifndef RTC_H
#define RTC_H
#include "stdint.h"

typedef struct
{
	uint8_t second;
	uint8_t min;
	uint8_t hour;
	uint8_t day;
	uint8_t date;
	uint8_t month;
	uint8_t year;
} DateTime_t;



void RTC_WriteTime(DateTime_t *dt);

void RTC_ReadTime(DateTime_t *dt);


#endif
