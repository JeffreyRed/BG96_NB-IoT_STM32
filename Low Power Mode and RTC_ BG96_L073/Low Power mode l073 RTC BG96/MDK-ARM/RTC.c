/*
 * RTC.c
 *
 *  Created on: 11 July 2018
 *      Author: Jeffrey Redondo
 *  Description: I took the code given by STM32CubeMx and created a separate file RTC.c
 */
#include "stm32l0xx_hal.h"
#include "RTC.h"
#include "main.h"
#include "UART.h"

extern RTC_HandleTypeDef hrtc;

void HAL_RTC_AlarmAEventCallback(RTC_HandleTypeDef *hrtc)
{
  /* Turn LED1 on: Alarm generation */
  //HAL_GPIO_TogglePin(GPIOA, GPIO_PIN_5);
		//procedure to set alarm every 2 minutes;
	  RTC_TimeTypeDef sTime;
    HAL_RTC_GetTime(hrtc, &sTime, RTC_FORMAT_BIN);
	
    uint8_t next_minute_alarm = sTime.Minutes + Alarm_Minutes_Next;//5 minutes
    if (next_minute_alarm > 59) next_minute_alarm = 0;

    RTC_AlarmTypeDef sAlarm;
    sAlarm.AlarmTime.Hours = 0x00;
    sAlarm.AlarmTime.Minutes = RTC_ByteToBcd2(next_minute_alarm);
    sAlarm.AlarmTime.Seconds = 0x00;
    sAlarm.AlarmTime.TimeFormat = RTC_HOURFORMAT_24;
    sAlarm.AlarmTime.DayLightSaving = RTC_DAYLIGHTSAVING_NONE;
    sAlarm.AlarmTime.StoreOperation = RTC_STOREOPERATION_RESET;
    sAlarm.AlarmMask = RTC_ALARMMASK_DATEWEEKDAY_HOUR;
    sAlarm.AlarmDateWeekDaySel = RTC_ALARMDATEWEEKDAYSEL_WEEKDAY;
    sAlarm.AlarmDateWeekDay = RTC_WEEKDAY_MONDAY;
    sAlarm.Alarm = RTC_ALARM_A;
    HAL_RTC_SetAlarm_IT(hrtc, &sAlarm, FORMAT_BCD);
		SystemClock_Config(); //only with stop mode
}

void RTC_CalendarShow(uint8_t *showtime, uint8_t *showdate)
{
  RTC_DateTypeDef sdatestructureget;
  RTC_TimeTypeDef stimestructureget;
	RTC_AlarmTypeDef sAlarm;
  /* Get the RTC current Time */
  HAL_RTC_GetTime(&hrtc, &stimestructureget, RTC_FORMAT_BIN);
  /* Get the RTC current Date */
  HAL_RTC_GetDate(&hrtc, &sdatestructureget, RTC_FORMAT_BIN);
  /* Display time Format : hh:mm:ss */
  printf("%2d:%2d:%2d", stimestructureget.Hours, stimestructureget.Minutes, stimestructureget.Seconds);
  HAL_UART_Transmit(&huart2,"\r\nDate and Alarm.\r\n",sizeof("\r\nDate and Alarm.\r\n"),1000);
	//HAL_Delay(200);
	/* Display date Format : mm-dd-yy */
  printf("%2d-%2d-%2d", sdatestructureget.Month, sdatestructureget.Date, 2000 + sdatestructureget.Year);
  /* Get the RTC current Time */
  HAL_RTC_GetAlarm(&hrtc, &sAlarm,RTC_ALARM_A, RTC_FORMAT_BIN);
	/* Display time Format : hh:mm:ss */
  printf("%2d:%2d:%2d", sAlarm.AlarmTime.Hours, sAlarm.AlarmTime.Minutes, sAlarm.AlarmTime.Seconds);
  /* Display date Format : mm-dd-yy */
}
/* RTC init function */
void MX_RTC_Init(void)
{

  RTC_TimeTypeDef sTime;
  RTC_DateTypeDef sDate;
  RTC_AlarmTypeDef sAlarm;

    /**Initialize RTC Only 
    */
  hrtc.Instance = RTC;
  hrtc.Init.HourFormat = RTC_HOURFORMAT_24;
  hrtc.Init.AsynchPrediv = 127;
  hrtc.Init.SynchPrediv = 255;
  hrtc.Init.OutPut = RTC_OUTPUT_DISABLE;
  hrtc.Init.OutPutRemap = RTC_OUTPUT_REMAP_NONE;
  hrtc.Init.OutPutPolarity = RTC_OUTPUT_POLARITY_HIGH;
  hrtc.Init.OutPutType = RTC_OUTPUT_TYPE_OPENDRAIN;
  if (HAL_RTC_Init(&hrtc) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

    /**Initialize RTC and set the Time and Date 
    */
//  if(HAL_RTCEx_BKUPRead(&hrtc, RTC_BKP_DR0) != 0x32F2){
  sTime.Hours = 0x0;
  sTime.Minutes = 0x0;
  sTime.Seconds = 0x0;
	sTime.TimeFormat = RTC_HOURFORMAT_24;
  sTime.DayLightSaving = RTC_DAYLIGHTSAVING_NONE;
  sTime.StoreOperation = RTC_STOREOPERATION_RESET;
  if (HAL_RTC_SetTime(&hrtc, &sTime, RTC_FORMAT_BCD) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

  sDate.WeekDay = RTC_WEEKDAY_TUESDAY;
  sDate.Month = RTC_MONTH_JULY;
  sDate.Date = 0x10;
  sDate.Year = 0x18;

  if (HAL_RTC_SetDate(&hrtc, &sDate, RTC_FORMAT_BCD) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

 //   HAL_RTCEx_BKUPWrite(&hrtc,RTC_BKP_DR0,0x32F2);
 // }
    /**Enable the Alarm A 
    */
  sAlarm.AlarmTime.Hours = 0x00;
  sAlarm.AlarmTime.Minutes = Alarm_Minutes;
  sAlarm.AlarmTime.Seconds = 0x01;
  sAlarm.AlarmTime.SubSeconds = 0x00;
	sAlarm.AlarmTime.TimeFormat = RTC_HOURFORMAT_24;
  sAlarm.AlarmTime.DayLightSaving = RTC_DAYLIGHTSAVING_NONE;
  sAlarm.AlarmTime.StoreOperation = RTC_STOREOPERATION_RESET;
  sAlarm.AlarmMask = RTC_ALARMMASK_DATEWEEKDAY_HOUR;
  sAlarm.AlarmSubSecondMask = RTC_ALARMSUBSECONDMASK_NONE;
  sAlarm.AlarmDateWeekDaySel = RTC_ALARMDATEWEEKDAYSEL_WEEKDAY;
  sAlarm.AlarmDateWeekDay = RTC_WEEKDAY_MONDAY;
  sAlarm.Alarm = RTC_ALARM_A;
  if (HAL_RTC_SetAlarm_IT(&hrtc, &sAlarm, RTC_FORMAT_BCD) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

}
