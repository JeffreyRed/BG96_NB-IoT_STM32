
''' Includes
------------------------------------------------------------------
'''
import serial
from time import sleep
import log
import re
''' Private variables
------------------------------------------------------------------
'''
# BG96 commands
BG96_AT = "at"
BG96_AT_NWKSCANMODE = "AT+QCFG=\"nwscanmode\",3,1\r\n" # Turn on LTE and GSM off 3,1
BG96_AT_IOT_OPMODE_NB1 = "AT+QCFG=\"iotopmode\",1,1\r\n" # Acitvat NB-IoT
BG96_AT_IOT_OPMODE_CATM1 = "AT+QCFG=\"iotopmode\",0,1\r\n" # Check network info
BG96_AT_CREG_2 = "AT+CREG=2\r\n"
BG96_AT_CREG_ASK = "AT+CREG?\r\n"
BG96_AT_QNWINFO = "AT+QNWINFO\r\n"
BG96_AT_CSQ = "AT+CSQ\r\n"
BG96_AT_AP = "AT+QICSGP=1,1,\"NBIOT.TELIA.EE\",\"\",\"\",0\r\n"
BG96_AT_QIACT_1 = "AT+QIACT=1\r\n"
BG96_AT_QIACT_ASK = "AT+QIACT?\r\n"
BG96_AT_QIACT_DEACTIVATE = "AT+QIDEACT=1\r\n"
BG96_AT_PREPARE_UDP_PCKT = "AT+QIOPEN=1,0,\"UDP\",\"90.191.202.188\",6666,0,1\r\n"
BG96_AT_ENABLE_UDP_SERVICE = "AT+QIOPEN=1,2,\"UDP SERVICE\",\"127.0.0.1\",0,6666,0\r\n"
BG96_AT_GET_UDP_SERVICE_STATE = "AT+QISTATE=0,1\r\n"
BG96_AT_GET_RESPONSE = "AT+QIRD=2\r\n"
BG96_AT_SEND_UDP_PCKT = "AT+QISEND=0\r\n";
BG96_AT_SEND_UDP_SERVICE_PCKT = "AT+QISEND=2\r\n";
BG96_AT_CRTL_Z = "\x1A"
BG96_AT_SERVICE_MODE = "AT+QCFG=\"servicedomain\",1,1"
BG96_AT_CLOSE_UDP = "AT+QICLOSE=0\r\n"
BG96_AT_POWER_DOWN = "AT+QPOWD=1\r\n"
BG96_AT_SET_NB_IOT_BAND_20 = "AT+QCFG=\"band\",f,80,80000,1\r\n" # NOTE: we HEX values are represented without Ox
BG96_AT_ASK_IMSI = "AT+CIMI\r\n"
BG96_AT_GET_CURRENT_BAND = "AT+QCFG=\"band\"\r\n"
BG96_AT_RESPONE_BUF_EMPTY = "+QIRD: 0"
BG96_AT_SERVICE_OPEN = "+QIOPEN"


# BG96 responses
BG96_RESPONSE_PACKET_SENT = "SEND OK"
BG96_RESPONSE_ERROR = "ERROR"
BG96_RESPONSE_NO_CARRIER = "CARRIER"
BG96_RESPONSE_QIACT_OK = "+QIACT"

# Serial port parameters
SERIAL_BAUDRATE = 115200
SERIAL_PARITY = serial.PARITY_NONE
SERIAL_STOP_BITS = serial.STOPBITS_ONE
SERIAL_BYTE_SIZE = serial.EIGHTBITS
SERIAL_FLOW_CONTROL = False
SERIAL_TIMEOUT = 140 # read timeout in seconds

# Raspberry configuration
RUNNING_ON_RASPBERRY = True
REBOOT_PIN = 18

# Control parameters
DELAY_BETWEEN_UART_COMMANDS = 2 # 2 seconds
PDP_ACTIVATION_TIME = 15 # 10 seconds
BG96_BOOTING_TIME = 10 # 10 seconds
BG96_RESET_PIN_PUSH_TIME = 3 # 3 seconds, how long we are going to hold down "button"

ALLOWED_FAILED_INITIALIZATIONS = 10 # 10 times
ALLOWED_CARRIER_SEARCHING_RETRIES = 10 # 10 times
ALLOWED_PACKET_SENDING_FAILURES = 3 # 3 times

# BG96 modes
MODE_NB_IOT = 1
MODE_CAT_M1 = 2

'''
 * @brief  Object for sending UDP packets using UART and BG96
'''
class BG96:

    '''
     * @brief  Constructor of BG96 object
     * @param  Strin port - COM port (COM1, COM2 etc..)
     * @param  Int mode - 1 (NB-IOT), 2 (CATM1)
     * @retval none
    '''
    def __init__(self, port, mode = MODE_NB_IOT):

        if mode is not MODE_NB_IOT and mode is not MODE_CAT_M1:
            print("Wrong mode: use mode 1 for NB-IOT and mode 2 for CATM1")
            return

        # initalize serial port for BG96
        self.__ser = serial.Serial()
        self.__ser.port = port
        self.__ser.baudrate = SERIAL_BAUDRATE
        self.__ser.parity = SERIAL_PARITY
        self.__ser.stopbuts = SERIAL_STOP_BITS
        self.__ser.bytesize = SERIAL_BYTE_SIZE
        self.__ser.xonxoff = SERIAL_FLOW_CONTROL
        self.__ser.timeout = SERIAL_TIMEOUT

        self.listener_enabled = False # UDP packet reciver status
        self.signal_strength = 0 # dBm
        self.ip_address = '127.0.0.1'

        self.__mode = mode

        try:

            # open serial connection
            self.__ser.open()
            self.__serial_connected = True

            # check if managed to make connection
            if not self.__ser.is_open:
                log.logger.error("Unable to open serial on specified port")
                self.__serial_connected = False
                return

        except (serial.SerialException, serial.SerialTimeoutException) as e:
            log.logger.error("Serial error")
            pass

    '''
     * @brief  Returns signal strength in dBm
     * @param  none
     * @retval float signal_strength - signal strenght
    '''
    def getSignalStrength(self):

        signal_quality = re.findall(r"[-+]?\d*\,\d+|\d+", self.__writeUART(BG96_AT_CSQ))
        if isinstance(signal_quality, list) and len(signal_quality) > 0: # check if regex found something
            signal_quality = signal_quality[0].replace(",", ".", 1)
            if signal_quality.replace('.','',1).isdigit():
                self.signal_strength = float(signal_quality)
        # conversion to dBm
        signal_in_dbm = -113 + round(self.signal_strength * 2)
        log.logger.info("Signal quality: " + str(signal_in_dbm) + " dBm")
        return signal_in_dbm

    '''
     * @brief  Returns IP address of BG96
     * @param  none
     * @retval string ip - UDP server IP address
    '''
    def getIpAddress(self):

        tmp_buf = re.findall(r'SERVICE\",\"(\d{1,3}\.\d{1,3}\.\d{1,3}\.\d{1,3})', self.__writeUART(BG96_AT_GET_UDP_SERVICE_STATE))
        if isinstance(tmp_buf, list) and len(tmp_buf) > 0:
            self.ip_address = tmp_buf[0]
            print(self.ip_address)

        return self.ip_address

    '''
     * @brief  Reads packets that are sent to UDP server
     * @param  none
     * @retval list packets - list of packets as strings
    '''
    def readRecivedPackets(self):

        packets = [] # store packets here
        while True:

            packet = self.__writeUART(BG96_AT_GET_RESPONSE)

            if packet.find(BG96_AT_RESPONE_BUF_EMPTY) is -1 and bool(packet.strip()) and packet.find(BG96_RESPONSE_ERROR) is -1: # we have response and respone is not empty and response is not error
                # parse the real contenet between other stuff
                matches = re.search(r',\d{1,}(.*?)OK', packet, re.M | re.I | re.S)
                if matches:
                    print(str(matches.group(1))) # remove junk from string
                    packets.append(str(matches.group(1)).strip("\r\n"))

            else:
                print("Cannot parse packet")
                break

        print(packets)
        return packets

    '''
     * @brief  Initialize BG96 for UDP packet sending
     * @param  none
     * @retval none
    '''
    def setup(self):

        times_failed = 0 # how many initialization retries have we made

        while times_failed < ALLOWED_FAILED_INITIALIZATIONS: # try initializing until it succeeds, but we cant do it forever

            log.logger.info("Initializing BG96")
            self.__ser.reset_input_buffer()
            self.__ser.reset_output_buffer()
            self.__reboot()
            self.__writeUART(BG96_AT)

            times_no_carrier = 0 # count how many times we do not manage to get carrier

            while True: # carrier set up can fail several times

                carrier_status = self.__writeUART(BG96_AT_NWKSCANMODE)
                if times_no_carrier > ALLOWED_CARRIER_SEARCHING_RETRIES: # failed 3 times, start from the beginning
                    times_failed += 1
                    break

                if carrier_status.find(BG96_RESPONSE_NO_CARRIER) is not -1 or carrier_status.find(BG96_RESPONSE_ERROR) is not -1:
                    times_no_carrier += 1
                    log.logger.warning("No carrier")
                else:
                    break

            self.__writeUART(BG96_AT_SET_NB_IOT_BAND_20)
            # select mode
            self.__writeUART(BG96_AT_IOT_OPMODE_NB1) if self.__mode is MODE_NB_IOT else self.__writeUART(BG96_AT_IOT_OPMODE_CATM1)
            self.__writeUART(BG96_AT_ASK_IMSI)
            self.__writeUART(BG96_AT_CREG_2)
            self.__writeUART(BG96_AT_CREG_ASK)
            self.__writeUART(BG96_AT_QNWINFO)
            self.getSignalStrength()
            self.__writeUART(BG96_AT_AP)
            self.__writeUART(BG96_AT_GET_CURRENT_BAND)

            if self.__writeUART(BG96_AT_QIACT_1).find(BG96_RESPONSE_ERROR) is not -1: # start whole initialization process from beginning
                times_failed += 1
                continue # failed, need to re-init

            sleep(PDP_ACTIVATION_TIME) # it takes some time to activate PDP

            if self.__writeUART(BG96_AT_QIACT_ASK).find(BG96_RESPONSE_QIACT_OK) > 0:
                if self.__writeUART(BG96_AT_ENABLE_UDP_SERVICE).find(BG96_AT_SERVICE_OPEN) is -1:
                    times_failed += 1
                else:
                    return # BG96 initialized!
            else: # unable to get PDP, retry from beginning
                log.logger.error("Failed to activate PDP")
                times_failed += 1
                self.__writeUART(BG96_AT_QIACT_DEACTIVATE)


    '''
     * @brief  Reboot BG96 chip using GPIO
     * @param  none
     * @retval none
    '''
    def __reboot(self):

        if RUNNING_ON_RASPBERRY:
            print("INFO: Rebooting BG96")
            log.logger.info("Rebooting BG96")
            import RPi.GPIO as GPIO
            GPIO.setmode(GPIO.BCM)
            GPIO.setwarnings(False)
            GPIO.setup(REBOOT_PIN, GPIO.OUT) #reboot
            self.__writeUART(BG96_AT_POWER_DOWN)
            sleep(BG96_BOOTING_TIME)
            GPIO.output(REBOOT_PIN, GPIO.LOW)
            sleep(BG96_RESET_PIN_PUSH_TIME)
            GPIO.output(REBOOT_PIN, GPIO.HIGH)
            sleep(BG96_RESET_PIN_PUSH_TIME)
            GPIO.output(REBOOT_PIN, GPIO.LOW)
            sleep(BG96_BOOTING_TIME)

    '''
     * @brief  Sends UDP packet
     * @param  payload - content of TCP packet
     * @retval bool - true (sending succeede), false (sending failed)
    '''
    def sendUdpPacket(self, payload, destination, port):

        send_retries = 0 # count of fail sendings
        while send_retries < ALLOWED_PACKET_SENDING_FAILURES:
            self.__writeUART("AT+QISEND=2," + str(len(payload)) + ",\"" + destination + "\"," + str(port) + "\r\n")
            if self.__writeUART(payload).find(BG96_RESPONSE_PACKET_SENT) is -1:
                send_retries += 1
                log.logger.error("Failed to send packet")
            else:
                return True

        sleep(2)
        self.getSignalStrength()
        return False
    #    self.__writeUART("AT+QIRD=2\r\n")


    '''
     * @brief  Writes string to serial
     * @param  msg - string that You want to pass
     * @retval buf - response from UART
    '''
    def __writeUART(self, msg):

        return_buf = ""; # seve response from UART here
        try:
            if self.__serial_connected and self.__ser.writable():
                print("Writing:\n\"" + msg + "\"")
                self.__ser.write(msg.encode("UTF-8")) # conversion to UTF8
                self.__ser.flush()
                sleep(DELAY_BETWEEN_UART_COMMANDS)
                while self.__ser.inWaiting() > 0:
                    try:
                        return_buf += self.__ser.read(1).decode("UTF-8") # read response for timeout duration
                    except UnicodeDecodeError:
                        pass
                print(return_buf)

            else:
                print("ERROR: Failed to write UART")
        except (serial.SerialException, serial.SerialTimeoutException) as e:
            print("ERROR: Serial error")
            log.logger.error("Writing to serial failed and caused exception")
            pass

        return return_buf

    '''
     * @brief  Closes serial port connection
     * @param  none
     * @retval none
    '''
    def end(self):

        try:
            self.__ser.close()
        except (serial.SerialException, serial.SerialTimeoutException) as e:
            pass
