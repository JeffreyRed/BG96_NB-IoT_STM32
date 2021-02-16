/*
 * bg96.h
 *
 *  Created on: 13. märts 2018
 *      Author: MAiro Leier
 *Some modification were done related to definition by Jeffrey Redondo
 */
#include <stdint.h>
//#ifndef BG96_H_
//#define BG96_H_
#define RTC_ALARMMASK_ONLYMINUTES         ((uint32_t)0x80800080)
#define RTC_ALARMMASK_DATEWEEKDAY_HOUR    ((uint32_t)0x80800000) //only take into account Minutes and Second
#define RTC_ALARMMASK_DATEWEEKDAY_HOUR_SECOND    ((uint32_t)0x80800080) //only take into account Minutes and Second
#define Alarm_Minutes 0x10; //set the minutes you want the alarm to trigger (FORMAT FOR ALARM)
#define Alarm_Minutes_Next 10; //set the minutes you want the alarm to trigger INT FORMAT FOR NEXT ALARM
//------------MX_RTC_Init------------
// Initialize the RTC Calendar and Alarm(clck 32KHz),
// Input: none
// Output: none
void MX_RTC_Init(void);
//------------MX_RTC_Init------------
// Initialize the RTC Calendar and Alarm(clck 32KHz),
// Input: none
// Output: none
void RTC_CalendarShow(uint8_t *showtime, uint8_t *showdate);
