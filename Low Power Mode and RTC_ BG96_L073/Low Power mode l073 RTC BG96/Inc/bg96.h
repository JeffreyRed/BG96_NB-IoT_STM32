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
#define BG96_AT_NWKSCANMODE "AT+QCFG=\"nwscanmode\",3,1\r\n"	// Turn on LTE and GSM off
#define BG96_AT_NWKSCANMODE_AUTO "AT+QCFG=\"nwscanmode\",0,1\r\n"	// 
#define BG96_AT_NWKSCANSEQ "AT+QCFG=\"nwscanseq\",030201,1\r\n"
#define BG96_AT_IOT_OPMODE_NB1 "AT+QCFG=\"iotopmode\",1,1\r\n"		// Activate NB-IoT
#define BG96_AT_IOT_OPMODE_CATM1 "AT+QCFG=\"iotopmode\",0,1\r\n"	// Activate LTE Cat-M1
#define BG96_AT_IOT_OPMODE_AUTO "AT+QCFG=\"iotopmode\",2,1\r\n"	// 
#define BG96_AT_CREG_2 "AT+CREG=2\r\n"		// Check network info
#define BG96_AT_CREG_ASK "AT+CREG?\r\n"
#define BG96_AT_QNWINFO "AT+QNWINFO\r\n"
#define BG96_AT_CSQ "AT+CSQ\r\n"
#define BG96_AT_AP "AT+QICSGP=1,1,\"NBIOT.TELIA.EE\",\"\",\"\",0\r\n"
#define BG96_AT_QIACT_1 "AT+QIACT=1\r\n"
#define BG96_AT_QIACT_ASK "AT+QIACT?\r\n"
#define BG96_AT_QIDEACT "AT+QIDEACT=1\r\n" //drop context;
#define BG96_AT_SERVICE_MODE "AT+QCFG=\"servicedomain\",1,1\r\n"
#define BLE_BUAD "AT+BAUD\r\n"
#define BG96_AT_COPS_ASK "AT+COPS?\r\n"
#define BG96_AT "AT\r\n"
#define BG96_ATI3 "ATI\r\n"
#define BG96_URL_CHARACTERS "AT+QHTTPURL=88,80\r\n"
#define BG96_URL_TOKEN "http://things.ubidots.com/api/v1.6/devices/bg96?token=A1E-g30TOMSwB7M7s827HqtAgsUWNwmhhp\r\n"
#define BG96_HTTP_POST "AT+QHTTPPOST=7,80,80\r\n"
#define BG96_HTTP_POST_RSSI "AT+QHTTPPOST=8,80,80\r\n"
#define BG96_HTTP_POST_RSRP "AT+QHTTPPOST=8,80,80\r\n"
#define BG96_HTTP_POST_RSRQ "AT+QHTTPPOST=8,80,80\r\n"
#define BG96_HTTP_POST_SNR "AT+QHTTPPOST=6,80,80\r\n"
#define BG96_HTTP_POST_TECH "AT+QHTTPPOST=12,80,80\r\n"
#define BG96_TEMP "AT+QTEMP\r\n"
#define BG96_HTTP_RESPONSE_CODE "AT+QHTTPREAD=80\r\n"
#define BG96_AIRPLANE_MODE_ON "AT+CFUN=4\r\n"
#define BG96_AIRPLANE_MODE_OFF "AT+CFUN=1\r\n"
#define BG96_POWER_DOWN "AT+QPOWD=1\r\n"  //Normal power down 1, Immediately power down 0
#define BG96_SIGNAL_STRENGTH "AT+QCSQ\r\n"
#define BG96_AT_AP_INTERNET "AT+QICSGP=1,1,\"internet.emt.ee\",\"\",\"\",0\r\n"
#define BG96_AT_PIN "AT+CPIN=0000\r\n"
#define BG96_AT_BAND_NB_20 "AT+QCFG=\"band\",f,8000,a0e189f\r\n"  //GSM,NB,CATM -> all,band20,all
#define BG96_AT_BAND_NB_ALL "AT+QCFG=\"band\",f,400a0e189f,a0e189f\r\n"  //GSM,NB,CATM ->all,all, all
#define BG96_AT_PREPARE_UDP_PCKT "AT+QIOPEN=1,0,\"UDP\",\"node.iot.ttu.ee\",6805,0,1\r\n"
#define BG96_AT_SEND_UDP_PCKT "AT+QISEND=0\r\n"
#define BG96_DATA_STRING "POST /s/ HTTP/1.0\nAuthorization: Basic amVyZWRvc2FybWllbnRvOkVzdG9uaWEyMDE4\nX-Id: 200699\nContent-Type: text/plain\nContent-Length: "// 18\n\n201,-10,-75,157030"; 18= Lenght of the packets
#define BG96_AT_CRTL_Z "\x1A"
#define BG96_AT_CLOSE "AT+QICLOSE=0\r\n"
#define BG96_ID "157035"//(157033) 18\n\n201,-10,-75,157030"; 18= Lenght of the packets, 201=ID in middleware, -10 data, -75, data, 57030 ID.
#define BG96_Distance "148"//meters
#define BG96_AT_PING "AT+QPING=1,\"node.iot.ttu.ee\"\r\n"

extern uint8_t EVB_Buffer[200];

//------------BG96_Init------------
// Initialize BG96 with operation mode, context, APN 
// Input: none
// Output: none
void BG96_Init(void);
//------------BG96_Attached------------
// Check with COPS? if the UE is attached to the Network
// Input: none
// Output: noneBG96_Signal
void BG96_Attached(void);
//------------BG96_Signal------------
// Check Signal Strength after the UE is attached to the Network
// Input: none
// Output: none
void BG96_Signal(void);
//------------BG96_TEMP------------
// Send TEMP by HTTP POST 
// Input: none
// Output: none
void BG96_Temp_Data(void);
//------------BG96_SEND_RSSI------------
// Send RSSI by HTTP POST 
// Input: none
// Output: none
void BG96_SEND_RSSI(void);
//------------BG96_SEND_RSRP------------
// Send RSRQ by HTTP POST 
// Input: none
// Output: none
void BG96_SEND_RSRP(void);
//------------BG96_SEND_RSRQ------------
// Send RSRQ by HTTP POST 
// Input: none
// Output: none
void BG96_SEND_RSRQ(void);
//------------BG96_SEND_SNR------------
// Send SNR by HTTP POST 
// Input: none
// Output: none
void BG96_SEND_SNR(void);
//------------BG96_SEND_TECH------------
// Send TECH by HTTP POST 
// Input: none
// Output: none
void BG96_SEND_TECH(void);
//------------BG96_Power_On------------
// Power on the BG96 Module Avnet Shield
// Input: none
// Output: none
void BG96_Power_On(void);
//------------BG96_Power_On_EVB------------
// Power on the BG96 Module on EVB Board
// Input: none
// Output: none
void BG96_Power_On_EVB(void);
//------------BG96_Init_EVB------------
// Init subrouting which includes workaround for APN over EVB board
// Input: none
// Output: none
void BG96_Init_EVB(void);
//------------BG96_Context_Activation------------
// Activate context
// Input: none
// Output: none
void BG96_Context_Activation(void);
//------------BG96_Cumulocity------------
// Send dat a to Cumulocity
// Input: none
// Output: none
void BG96_Cumulocity(void);
//#endif /* BG96_H_ */


