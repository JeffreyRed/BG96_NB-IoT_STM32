/*
 * bg96.c
 *
 *  Created on: 3 June 2018
 *      Author: Jeffrey Redondo
 */

/* Includes ------------------------------------------------------------------*/
#include "bg96.h"
#include "stm32l4xx_hal.h"
#include "UART.h"
#include <stdint.h>
#include <stdio.h>
#include <string.h>

extern uint8_t EVB_Buffer[200],EVB_indx;
char rssi2[4]="", mode2[7]="",rsrp2[4]="", snr2[3]="", rsrq2[3]="";

//int nameSize = 10; //this could come from an input or any other source
//  char *name = malloc(nameSize * sizeof(char));
//  //do something with name
//  free(name);

void BG96_Context_Activation(void);
void BG96_Send_Clean(void);
int BG96_Context_Check();
void BG96_PING(void);
int BG96_Check_Internet();
/**
  * @brief  enable and disable airplane mode
  * @param  None
  * @retval None
  */
void BG96_Airplane_Mode(){
//Airplain Mode
			HAL_UART_Transmit(&huart1,(uint8_t *)&BG96_AIRPLANE_MODE_ON,sizeof(BG96_AIRPLANE_MODE_ON),1500);
			HAL_Delay(5000);
			BG96_Send_Clean();
			HAL_UART_Transmit(&huart1,(uint8_t *)&BG96_AIRPLANE_MODE_OFF,sizeof(BG96_AIRPLANE_MODE_OFF),1500);
			HAL_Delay(5000);
			BG96_Send_Clean();
}
/**
  * @brief  Attached checking
  * @param  None
  * @retval 1 attached, 0 not attached.
  */
int BG96_Attached_check(){
	  int att=0;
		//cont++;
		HAL_UART_Transmit(&huart1,(uint8_t *)&BG96_AT_COPS_ASK,sizeof(BG96_AT_COPS_ASK),1500);
		HAL_Delay(1000);
		char *c = strstr(EVB_Buffer,"Telia"); //here is Carrier/operator name
		BG96_Send_Clean();
		if (c){
			att=1;
		}
return att;
}	
/**
  * @brief  Clear the buffer where bytes coming from UART are saved
  *  
  * @param  None
  * @retval None
  */
void BG96_clean_EVB_Buffer(){
	for(int j=0;j<EVB_indx;j++){
		EVB_Buffer[j]=0x00;
	}
	EVB_indx=0;	
	HAL_Delay(100);
}
/**
  * @brief  Send EVB_buffer to PC via UART 2. 
  *  
  * @param  None
  * @retval None
  */
void BG96_Send_UARTPC_EVB_Buffer(){
	HAL_UART_Transmit(&huart2,EVB_Buffer,EVB_indx,1000);						
}
/**
  * @brief  Combine both send and clean EVB_Buffer
  *  
  * @param  None
  * @retval None
  */
void BG96_Send_Clean(){
	BG96_Send_UARTPC_EVB_Buffer();
	BG96_clean_EVB_Buffer();						
}

/**
  * @brief  Initiate the BG96 module with Scan mode, APN, IOTOPMODE
  *  
  * @param  None
  * @retval None
  */
void BG96_Init(void){
	HAL_UART_Transmit(&huart1,(uint8_t *)&BG96_AT_PIN,sizeof(BG96_AT_PIN),1500);
	HAL_Delay(500);
	BG96_Send_Clean();
	HAL_UART_Transmit(&huart1,(uint8_t *)&BG96_AT_BAND_NB_20,sizeof(BG96_AT_BAND_NB_20),1500);
	HAL_Delay(1000);
	BG96_Send_Clean();
	HAL_UART_Transmit(&huart1,(uint8_t *)&BG96_AT_NWKSCANMODE,sizeof(BG96_AT_NWKSCANMODE),1500);
	//HAL_UART_Transmit(&huart1,(uint8_t *)&BG96_AT_NWKSCANMODE_AUTO,sizeof(BG96_AT_NWKSCANMODE_AUTO),1500);
	HAL_Delay(1000);
	BG96_Send_Clean();
	HAL_UART_Transmit(&huart1,(uint8_t *)&BG96_AT_NWKSCANSEQ,sizeof(BG96_AT_NWKSCANSEQ),1500);
	HAL_Delay(1000);
	BG96_Send_Clean();
	HAL_UART_Transmit(&huart1,(uint8_t *)&BG96_AT_IOT_OPMODE_NB1,sizeof(BG96_AT_IOT_OPMODE_NB1),1500);
	//HAL_UART_Transmit(&huart1,(uint8_t *)&BG96_AT_IOT_OPMODE_AUTO,sizeof(BG96_AT_IOT_OPMODE_AUTO),1500);
	HAL_Delay(1000);
	BG96_Send_Clean();
	HAL_UART_Transmit(&huart1,(uint8_t *)&BG96_AT_AP,sizeof(BG96_AT_AP),1500);
	HAL_Delay(1000);
	BG96_Send_Clean();
	HAL_UART_Transmit(&huart1,(uint8_t *)&BG96_AT_QIDEACT,sizeof(BG96_AT_QIDEACT),1000);
	HAL_Delay(1000);
	BG96_Send_Clean();
	HAL_UART_Transmit(&huart1,(uint8_t *)&BG96_AT_CREG_2,sizeof(BG96_AT_CREG_2),1000);
	HAL_Delay(1000);
	BG96_Send_Clean();
}
/**
  * @brief  Send AT command to get RF parameters such as rssi, snr, rsrp, rsqr, tech.
  *  
  * @param  None
  * @retval None
  */
void BG96_Signal(){
	volatile int snr3=0;
	volatile char *tech, *mode1;
	BG96_clean_EVB_Buffer(); // think about filling only a dedicated buffer for the signal.
	HAL_UART_Transmit(&huart1,(uint8_t *)&BG96_SIGNAL_STRENGTH,sizeof(BG96_SIGNAL_STRENGTH),1500);
	HAL_Delay(1000);
//	memset(rssi2,0,strlen(rssi2)); // or memset(buffer,0,sizeof(buffer));
//	rssi2[0]='-';
	//rssi2 = (int*) malloc(strlen(rssi2)* sizeof(int));
	tech = strtok(EVB_Buffer,"\r,\"");  // first call returns pointer
	mode1 = strtok(NULL,"\r,\"");
	strcpy(mode2,strtok(NULL,"\r,\"")); //mode1 = strtok(NULL,"\r,\"");
	strcpy(rssi2,strtok(NULL,"\r,\""));
	strcpy(rsrp2,strtok(NULL,"\r,\""));//rsrp = strtok(NULL,"\r,\"");
	strcpy(snr2,strtok(NULL,"\r,\""));//snr = strtok(NULL,"\r,\"");
	strcpy(rsrq2,strtok(NULL,"\r,\""));//rsrq = strtok(NULL,"\r,\"");
	snr3 =(((snr2[0]-'0')*100)+((snr2[1]-'0')*10)+(snr2[2]-'0'));
	snr3 =((snr3*30)/250);//conver to dB.
	sprintf (snr2,"%d", snr3);//convert back to string.
  BG96_Send_Clean();  
}
/**
  * @brief  Check if the BG96 module is attached to the Network
  *  
  * @param  None
  * @retval None
  */
void BG96_Attached(){
	char sw=0,ap=0;
	//int cont=0;
	while (sw==0){ //waiting device is attached and has a context.
//    cont++;
//		HAL_UART_Transmit(&huart1,(uint8_t *)&BG96_AT_COPS_ASK,sizeof(BG96_AT_COPS_ASK),1500);
//		HAL_Delay(1000);
//		char *c = strstr(EVB_Buffer,"Telia");
//		BG96_Send_Clean();
		if ((BG96_Attached_check())==1){
			if ((BG96_Context_Check())==1){
			HAL_UART_Transmit(&huart1,(uint8_t *)&BG96_AT_QNWINFO,sizeof(BG96_AT_QNWINFO),1500);
			HAL_Delay(1000);
			BG96_Send_Clean();
			sw=1;
			}
			else{
				HAL_UART_Transmit(&huart2,"Context activation in process.\r\n",sizeof("Context activation in process.\r\n"),1000);
				while (BG96_Context_Check()==0){ //infinte loop until context is up
					BG96_Context_Activation(); //
				}
				HAL_UART_Transmit(&huart2,"Context activated.\r\n",sizeof("Context activated.\r\n"),1000);
				BG96_PING();
			}
		}
		else{
			HAL_UART_Transmit(&huart2,"No Service\r\n",sizeof("No Service\r\n"),1000);
			HAL_UART_Transmit(&huart2,"Please, Check if the eNB is in Service or if you have coverage\r\n",sizeof("Please, Check if the eNB is in Service or if you have coverage\r\n"),1000);
		}
	}
}
/**
  * @brief  this function is to test the data connection to cloud 
  *  				it sends a random variable
  * @param  None
  * @retval None
  */
void BG96_Temp_Data(void){
	int var = rand()%30; //random variable
	char str[2]="00";
	sprintf (str,"%d", var);
	char temp[7]="";
	HAL_UART_Transmit(&huart1,(uint8_t *)&BG96_URL_CHARACTERS,sizeof(BG96_URL_CHARACTERS),1000);
	HAL_Delay(1000);
	BG96_Send_Clean();
	HAL_UART_Transmit(&huart1,(uint8_t *)&BG96_URL_TOKEN,sizeof(BG96_URL_TOKEN),1000);
	HAL_Delay(2000);
	BG96_Send_Clean();
	HAL_UART_Transmit(&huart1,(uint8_t *)&BG96_HTTP_POST,sizeof(BG96_HTTP_POST),1000);
	HAL_Delay(2000);
	BG96_Send_Clean();
	strcpy(temp,"TEMP=");
	strcat(temp, str);
	//strcat(temp,"\r\n");
	HAL_UART_Transmit(&huart1,temp,sizeof(temp),1000);
	HAL_UART_Transmit(&huart1,"\r\n",sizeof("\r\n"),1000);
	BG96_Send_Clean();
	//HAL_UART_Transmit(&huart1,BG96_TEMP,sizeof(BG96_TEMP),1000);
	HAL_Delay(500);
	HAL_UART_Transmit(&huart2,"Data sent\r\n",sizeof("Data sent\r\n"),1000);
	HAL_UART_Transmit(&huart2,temp,sizeof(temp),1000);
	HAL_UART_Transmit(&huart2,"\r\n",sizeof("\r\n"),1000);
}
/**
  * @brief  gathers and sends rssi parameter via HTTP POST Request
  *  
  * @param  None
  * @retval None
  */
void BG96_SEND_RSSI(){
	HAL_Delay(1000);
	char rssichar[9]="";
	volatile char post[22]="AT+QHTTPPOST=9,80,80\r\n";
	post[13]=((6+strlen(rssi2))+'0'); //conver to char and then change the total bytes in post
	HAL_UART_Transmit(&huart1,(uint8_t *)&BG96_URL_CHARACTERS,sizeof(BG96_URL_CHARACTERS),1000);
	HAL_Delay(1000);
	BG96_Send_Clean();
	HAL_UART_Transmit(&huart1,(uint8_t *)&BG96_URL_TOKEN,sizeof(BG96_URL_TOKEN),1000);
	HAL_Delay(2000);
	BG96_Send_Clean();
	HAL_UART_Transmit(&huart1,(uint8_t *)&post,sizeof(BG96_HTTP_POST_RSSI),1000);
	HAL_Delay(2000);
	strcpy(rssichar,"RSSI=-");
	strcat(rssichar, rssi2);
	HAL_UART_Transmit(&huart1,rssichar,strlen(rssichar),1000);
  BG96_Send_Clean();
	HAL_Delay(500);
	HAL_UART_Transmit(&huart2,"Data sent\r\n",sizeof("Data sent\r\n"),1000);
	HAL_UART_Transmit(&huart2,rssichar,sizeof(rssichar),1000);
	HAL_UART_Transmit(&huart2,"\r\n",sizeof("\r\n"),1000);
}
/**
  * @brief  it checks if the module is ON , if not, it send the signal trough PC8
  *  				high level for Avnet Shield
  * @param  None
  * @retval None
  */
void BG96_Power_On(){
	//Try with ATI command to identify if the module is on, if not, send UART2 saying sth is wrong.
	char sw=0;
	//int cont=0;
	while (sw==0){ 
		//cont++;
		HAL_UART_Transmit(&huart1,(uint8_t *)&BG96_ATI3,sizeof(BG96_ATI3),1500);
		HAL_Delay(1000);
		char *c = strstr(EVB_Buffer,"BG96");
		BG96_Send_Clean();
		if (c){
			sw=1;
			HAL_UART_Transmit(&huart2,"Module is ON\r\n",sizeof("Module is ON\r\n"),1000);
		}
		else{
			HAL_UART_Transmit(&huart2,"Module is OFF\r\n",sizeof("Module is OFF\r\n"),1000);
			HAL_UART_Transmit(&huart2,"Trying to turn on the Module. However check the BG96 Module.\r\n",sizeof("Trying to turn on the Module. However check the BG96 Module.\r\n"),1000);
			HAL_GPIO_WritePin(GPIOC, GPIO_PIN_8,GPIO_PIN_SET);
			HAL_Delay(150);
			HAL_GPIO_WritePin(GPIOC, GPIO_PIN_8,GPIO_PIN_RESET);
			HAL_Delay(10000);
		}
	}
}
/**
  * @brief  it checks if the module is ON , if not, it send the signal trough PC_8
  *  				low level for EVB Quectel Board.
  * @param  None
  * @retval None
  */
void BG96_Power_On_EVB(){
	//Try with ATI command to identify if the module is on, if not, send UART2 saying sth is wrong.
	char sw=0;
	//int cont=0;
	while (sw==0){ 
		//cont++;
		HAL_UART_Transmit(&huart1,(uint8_t *)&BG96_ATI3,sizeof(BG96_ATI3),1500);
		HAL_Delay(3000);
		char *c = strstr(EVB_Buffer,"BG96");
		BG96_Send_Clean();
		if (c){
			sw=1;
			HAL_UART_Transmit(&huart2,"Module is ON\r\n",sizeof("Module is ON\r\n"),1000);
		}
		else{
			HAL_UART_Transmit(&huart2,"Module is OFF\r\n",sizeof("Module is OFF\r\n"),1000);
			HAL_UART_Transmit(&huart2,"Trying to turn on the Module. However check the BG96 Module.\r\n",sizeof("Trying to turn on the Module. However check the BG96 Module.\r\n"),1000);
			HAL_GPIO_WritePin(GPIOC, GPIO_PIN_8,GPIO_PIN_RESET);
			HAL_Delay(100);
			HAL_GPIO_WritePin(GPIOC, GPIO_PIN_8,GPIO_PIN_SET);
			HAL_Delay(10000);
		}
	}
}
/**
  * @brief  It sends the signal trough PC_8
  *  				low level for EVB Quectel Board.
  * @param  None
  * @retval None
  */
void BG96_Power_Off_EVB(){
			HAL_GPIO_WritePin(GPIOC, GPIO_PIN_8,GPIO_PIN_RESET);
			HAL_Delay(150);
			HAL_GPIO_WritePin(GPIOC, GPIO_PIN_8,GPIO_PIN_SET);
			HAL_Delay(8000);
}
/**
  * @brief  It sends the signal trough PC_8
  *  				high level for Avnet shield.
  * @param  None
  * @retval None
  */
void BG96_Power_off(){
	    HAL_GPIO_WritePin(GPIOC, GPIO_PIN_8,GPIO_PIN_SET);
			HAL_Delay(150);
			HAL_GPIO_WritePin(GPIOC, GPIO_PIN_8,GPIO_PIN_RESET);
			HAL_Delay(8000);
}
/**
  * @brief  Initiate the BG96 Avnet Shield module with Scan mode, APN, IOTOPMODE.
  * @param  None
  * @retval None
  */
void BG96_Init_EVB(void){
	/* Workaround
	HAL_UART_Transmit(&huart1,(uint8_t *)&BG96_AT_PIN,sizeof(BG96_AT_PIN),1500);
	HAL_Delay(500);
	BG96_Send_Clean();
	HAL_UART_Transmit(&huart1,(uint8_t *)&BG96_AT_AP_INTERNET,sizeof(BG96_AT_AP_INTERNET),1500);
	HAL_Delay(1000);
	BG96_Send_Clean();
	BG96_Power_Off_EVB();
	BG96_Power_On_EVB();
	*/ //END of Workaround
	
	HAL_UART_Transmit(&huart1,(uint8_t *)&BG96_AT_PIN,sizeof(BG96_AT_PIN),1500);
	HAL_Delay(500);
	BG96_Send_Clean();
	HAL_UART_Transmit(&huart1,(uint8_t *)&BG96_AT_BAND_NB_20,sizeof(BG96_AT_BAND_NB_20),1500);
	HAL_Delay(1000);
	BG96_Send_Clean();
	HAL_UART_Transmit(&huart1,(uint8_t *)&BG96_AT_NWKSCANMODE,sizeof(BG96_AT_NWKSCANMODE),1500);
	//HAL_UART_Transmit(&huart1,(uint8_t *)&BG96_AT_NWKSCANMODE_AUTO,sizeof(BG96_AT_NWKSCANMODE_AUTO),1500);
	HAL_Delay(1000);
	BG96_Send_Clean();
	HAL_UART_Transmit(&huart1,(uint8_t *)&BG96_AT_NWKSCANSEQ,sizeof(BG96_AT_NWKSCANSEQ),1500);
	HAL_Delay(1000);
	BG96_Send_Clean();
	
	HAL_UART_Transmit(&huart1,(uint8_t *)&BG96_AT_IOT_OPMODE_NB1,sizeof(BG96_AT_IOT_OPMODE_NB1),1500);
	//HAL_UART_Transmit(&huart1,(uint8_t *)&BG96_AT_IOT_OPMODE_AUTO,sizeof(BG96_AT_IOT_OPMODE_AUTO),1500);
	HAL_Delay(1000);
	BG96_Send_Clean();
	HAL_UART_Transmit(&huart1,(uint8_t *)&BG96_AT_AP,sizeof(BG96_AT_AP),1500);
	HAL_Delay(3000);
	BG96_Send_Clean();
		
	HAL_UART_Transmit(&huart1,(uint8_t *)&BG96_AT_QIDEACT,sizeof(BG96_AT_QIDEACT),1000);
	HAL_Delay(1000);
	BG96_Send_Clean();
	HAL_UART_Transmit(&huart1,(uint8_t *)&BG96_AT_CREG_2,sizeof(BG96_AT_CREG_2),1000);
	HAL_Delay(1000);
	BG96_Send_Clean();	
}

/**
  * @brief  Activate context by 10 times if doesn't work it will do Airplane mode.
  * if the Airplane mode doesn't work then reboot.
  * @param  None
  * @retval None
  */

void BG96_Context_Activation(){
	char sw=0,ap=0;
	int cont=0;
	while ((sw==0)&&(cont<=10)){ //10 time trying to activate the context
		cont++;
		while (BG96_Attached_check()==0){}  //verifying or waitting to attached again after Airplane mode apply.
		//enable context
		HAL_Delay(3000);
		HAL_UART_Transmit(&huart1,(uint8_t *)&BG96_AT_QIACT_1,sizeof(BG96_AT_QIACT_1),1000);
		HAL_Delay(1000);
		char *c = strstr(EVB_Buffer,"OK");
		BG96_Send_Clean();
		if (c){
			HAL_UART_Transmit(&huart2,"Context is activated.\r\n",sizeof("Context is activated.\r\n"),1000);
			sw=1;
		}
		else{
			BG96_Airplane_Mode();
			HAL_UART_Transmit(&huart2,"Context is not activated.\r\n",sizeof("Context is not activated.\r\n"),1000);
			HAL_UART_Transmit(&huart2,"Airplane mode was applied to solve the issue, if this doesn't work until 9 tries the unit will reboot.\r\n",sizeof("Airplane mode was applied to solve the issue, if this doesn't work until 9 tries the unit will reboot..\r\n"),1000);
			if (cont==9){
			BG96_Power_Off_EVB();
			HAL_UART_Transmit(&huart2,"Rebooting.\r\n",sizeof("Rebooting.\r\n"),1000);
			BG96_Power_On_EVB();
			}
			
		}
	}
}

/**
  * @brief  Check if context is up
  * @param  None
  * @retval 1 if context is up, 0 if context is down
  */

int BG96_Context_Check(){
	char sw=0,ap=0;
	int cont=0;
		while ((BG96_Attached_check())==0){}  //verifying or waitting to attached again after Airplane mode apply.
		//enable context
		HAL_UART_Transmit(&huart1,(uint8_t *)&BG96_AT_QIACT_ASK,sizeof(BG96_AT_QIACT_ASK),1000);
		HAL_Delay(1000);
			char *c = strstr(EVB_Buffer,"+QIACT:");
		BG96_Send_Clean();
		if (c){
			cont=1;
	}
return cont;
}

/**
  * @brief  gathers and sends rsrp parameter via HTTP POST Request
  *  
  * @param  None
  * @retval None
  */
void BG96_SEND_RSRP(){
	HAL_Delay(1000);
	char rsrpchar[9]="";
	volatile char post[22]="AT+QHTTPPOST=9,80,80\r\n";
	post[13]=((5+strlen(rsrp2))+'0'); //conver to char and then change the total bytes in post
	HAL_UART_Transmit(&huart1,(uint8_t *)&BG96_URL_CHARACTERS,sizeof(BG96_URL_CHARACTERS),1000);
	HAL_Delay(1000);
	BG96_Send_Clean();
	HAL_UART_Transmit(&huart1,(uint8_t *)&BG96_URL_TOKEN,sizeof(BG96_URL_TOKEN),1000);
	HAL_Delay(2000);
	BG96_Send_Clean();
	HAL_UART_Transmit(&huart1,(uint8_t *)&post,sizeof(BG96_HTTP_POST_RSRP),1000);
	HAL_Delay(2000);
	strcpy(rsrpchar,"RSRP=");
	strcat(rsrpchar, rsrp2);
	HAL_UART_Transmit(&huart1,rsrpchar,strlen(rsrpchar),1000); // or sizeof
  BG96_Send_Clean();
	HAL_Delay(500);
	HAL_UART_Transmit(&huart2,"Data sent\r\n",sizeof("Data sent\r\n"),1000);
	HAL_UART_Transmit(&huart2,rsrpchar,sizeof(rsrpchar),1000);
	HAL_UART_Transmit(&huart2,"\r\n",sizeof("\r\n"),1000);
}
/**
  * @brief  gathers and sends rsrq parameter via HTTP POST Request
  *  
  * @param  None
  * @retval None
  */
void BG96_SEND_RSRQ(){
	HAL_Delay(1000);
	char rsrqchar[9]="";
	volatile char post[22]="AT+QHTTPPOST=9,80,80\r\n";
	post[13]=((5+strlen(rsrq2))+'0'); //conver to char and then change the total bytes in post
	HAL_UART_Transmit(&huart1,(uint8_t *)&BG96_URL_CHARACTERS,sizeof(BG96_URL_CHARACTERS),1000);
	HAL_Delay(1000);
	BG96_Send_Clean();
	HAL_UART_Transmit(&huart1,(uint8_t *)&BG96_URL_TOKEN,sizeof(BG96_URL_TOKEN),1000);
	HAL_Delay(2000);
	BG96_Send_Clean();
	HAL_UART_Transmit(&huart1,(uint8_t *)&post,sizeof(BG96_HTTP_POST_RSRQ),1000);
	HAL_Delay(2000);
	strcpy(rsrqchar,"RSRQ=");
	strcat(rsrqchar, rsrq2);
	HAL_UART_Transmit(&huart1,rsrqchar,sizeof(rsrqchar),1000);
  BG96_Send_Clean();
	HAL_Delay(500);
	HAL_UART_Transmit(&huart2,"Data sent\r\n",sizeof("Data sent\r\n"),1000);
	HAL_UART_Transmit(&huart2,rsrqchar,sizeof(rsrqchar),1000);
	HAL_UART_Transmit(&huart2,"\r\n",sizeof("\r\n"),1000);
}
/**
  * @brief  gathers and sends SNR parameter via HTTP POST Request
  *  
  * @param  None
  * @retval None
  */
void BG96_SEND_SNR(){
	HAL_Delay(1000);
	char snrchar[7]="";
	volatile char post[22]="AT+QHTTPPOST=9,80,80\r\n";
	post[13]=((4+strlen(snr2))+'0'); //conver to char and then change the total bytes and put it in post character 13 which 
	//correspond to byte length of the message. 
	HAL_UART_Transmit(&huart1,(uint8_t *)&BG96_URL_CHARACTERS,sizeof(BG96_URL_CHARACTERS),1000);
	HAL_Delay(1000);
	BG96_Send_Clean();
	HAL_UART_Transmit(&huart1,(uint8_t *)&BG96_URL_TOKEN,sizeof(BG96_URL_TOKEN),1000);
	HAL_Delay(2000);
	BG96_Send_Clean();
	HAL_UART_Transmit(&huart1,(uint8_t *)&BG96_HTTP_POST_SNR,sizeof(BG96_HTTP_POST_SNR),1000);
	HAL_Delay(2000);
	strcpy(snrchar,"SNR=");
	strcat(snrchar, snr2);
	HAL_UART_Transmit(&huart1,snrchar,sizeof(snrchar),1000);
  BG96_Send_Clean();
	HAL_Delay(500);
	HAL_UART_Transmit(&huart2,"Data sent\r\n",sizeof("Data sent\r\n"),1000);
	HAL_UART_Transmit(&huart2,snrchar,sizeof(snrchar),1000);
	HAL_UART_Transmit(&huart2,"\r\n",sizeof("\r\n"),1000);
}
/**
  * @brief  gathers and sends TECH parameter via HTTP POST Request
  *  
  * @param  None
  * @retval None
  */
void BG96_SEND_TECH(){
	HAL_Delay(1000);
	char techchar[12]="";
	HAL_UART_Transmit(&huart1,(uint8_t *)&BG96_URL_CHARACTERS,sizeof(BG96_URL_CHARACTERS),1000);
	HAL_Delay(1000);
	BG96_Send_Clean();
	HAL_UART_Transmit(&huart1,(uint8_t *)&BG96_URL_TOKEN,sizeof(BG96_URL_TOKEN),1000);
	HAL_Delay(2000);
	BG96_Send_Clean();
	HAL_UART_Transmit(&huart1,(uint8_t *)&BG96_HTTP_POST_TECH,sizeof(BG96_HTTP_POST_TECH),1000);
	HAL_Delay(2000);
	strcpy(techchar,"TECH=");
	strcat(techchar, mode2);
	HAL_UART_Transmit(&huart1,techchar,sizeof(techchar),1000);
  BG96_Send_Clean();
	HAL_Delay(500);
	HAL_UART_Transmit(&huart2,"Data sent\r\n",sizeof("Data sent\r\n"),1000);
	HAL_UART_Transmit(&huart2,techchar,sizeof(techchar),1000);
	HAL_UART_Transmit(&huart2,"\r\n",sizeof("\r\n"),1000);
}

/**
  * @brief  Send data to comulocity using UDP
  *  
  * @param  None
  * @retval None
  */
void BG96_Cumulocity(){	
	//Variable and function to create the data string that should be sent to cumulocity
	char data[153]="";
	char data2[30]=""; //second part of the message
	volatile int total_length=0;
	char length[2]=""; 
	volatile int length_rssi=0;
	strcat(data2, "\n\n202,");
	strcat(data2, BG96_Distance);
	strcat(data2, ",-");
	strcat(data2, rssi2);
	strcat(data2, ",");
	strcat(data2, snr2);
	strcat(data2, ",");
	strcat(data2, rsrp2);
	strcat(data2, ",");
	strcat(data2, BG96_ID);
	strcpy(data,BG96_DATA_STRING);
	total_length= (strlen(data2)-2);
	sprintf (length,"%d", total_length);//convert back to string
	strcat(data, length);
	strcat(data, data2);
	//length_rssi=strlen(data);
	//for debuging to check the data string
	
	HAL_UART_Transmit(&huart2,data,strlen(data),1000);
	HAL_Delay(200);
	
	//check connectivity
	while (BG96_Context_Check()==0){ //infinte loop until context is up
					BG96_Context_Activation(); //
				}

	//UART data transmition. open the socket
	HAL_UART_Transmit(&huart1,(uint8_t *)&BG96_AT_PREPARE_UDP_PCKT,sizeof(BG96_AT_PREPARE_UDP_PCKT),1000);
	HAL_Delay(2000);
	char *c = strstr(EVB_Buffer,"+QIOPEN: 0,0");
	BG96_Send_Clean();
		if (c){
			HAL_UART_Transmit(&huart2,"Socket opened.\r\n",sizeof("Socket opened.\r\n"),1000);
			HAL_Delay(200);
		}
		else {
			HAL_UART_Transmit(&huart2,"Check the health of the server and the internet service.\r\n",sizeof("Check the health of the server and the internet service.\r\n"),1000);
			HAL_Delay(200);
			if (BG96_Check_Internet()==1){
			HAL_UART_Transmit(&huart2,"The internet service is OK , check the health of the server.\r\n",sizeof("The internet service is OK , check the health of the server.\r\n"),1000);
			HAL_Delay(200);
			}else {
			HAL_UART_Transmit(&huart2,"The context will be disable and enable one more time to solve the issue.\r\n",sizeof("The context will be disable and enable one more time to solve the issue.\r\n"),1000);
			HAL_Delay(200);	
			HAL_UART_Transmit(&huart1,(uint8_t *)&BG96_AT_QIDEACT,sizeof(BG96_AT_QIDEACT),1000);
			HAL_Delay(200);
				//check connectivity
				while (BG96_Context_Check()==0){ //infinte loop until context is up
					BG96_Context_Activation(); //
				}
			}
		}
	
	//send data string 
	HAL_UART_Transmit(&huart1,(uint8_t *)&BG96_AT_SEND_UDP_PCKT,sizeof(BG96_AT_SEND_UDP_PCKT),1000);
	HAL_Delay(200);
	BG96_Send_Clean();
	HAL_UART_Transmit(&huart1,data,strlen(data),1000);
	HAL_Delay(200);
	BG96_Send_Clean();
	//finishes the data transmittion
	HAL_UART_Transmit(&huart1,(uint8_t *)&BG96_AT_CRTL_Z,sizeof(BG96_AT_CRTL_Z),1000);
	HAL_Delay(200);
	BG96_Send_Clean();
	HAL_UART_Transmit(&huart1,(uint8_t *)&BG96_AT_CLOSE,sizeof(BG96_AT_CLOSE),1000);
	HAL_Delay(200);
	BG96_Send_Clean();
	
}
int BG96_Check_Internet(){
	//check connectivity
	int cont=0;
	HAL_UART_Transmit(&huart1,(uint8_t *)&BG96_AT_PING,sizeof(BG96_AT_PING),1000);
	HAL_Delay(1000); // waiting 10 sec to received the ping responses.
	char *c = strstr(EVB_Buffer,"+QPING: 0,"); // ping received
	BG96_Send_Clean();
		if (c){
			HAL_UART_Transmit(&huart2,"Internet service is OK.\r\n",sizeof("Internet service is OK.\r\n"),1000);
			HAL_Delay(200);
			cont=1;
		}
return cont;
}
void BG96_PING(){
	//check connectivity
	HAL_UART_Transmit(&huart1,(uint8_t *)&BG96_AT_PING,sizeof(BG96_AT_PING),1000);
	HAL_Delay(10000); // waiting 10 sec to received the ping responses.
	BG96_Send_Clean();
}