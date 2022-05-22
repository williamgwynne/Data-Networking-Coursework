#include "main.h"

#include "Time_Delays.h"
#include "Clk_Config.h"
#include "LCD_Display.h"

#include <stdio.h>
#include <string.h>
#include <stdbool.h>

#include "Small_7.h"
#include "Arial_9.h"
#include "Arial_12.h"
#include "Arial_24.h"

#define ACCELADR 0x98
#define EEPROMADR 0xA0

//Function prototypes defined as needed:
//...

//Global variables defined below. For example:
char outputString[18]; //Buffer to store text in for LCD
//others if needed:
//...

/*
   This program reads the tilt register from the accelerometer when the joystick
   is pushed down, then displays it on the LCD.
*/

uint32_t joystick_centre ( void ) {
	// returns 1 if the joystick is pressed in the centre , or 0 otherwise
	return ( LL_GPIO_IsInputPinSet ( GPIOB , LL_GPIO_PIN_5 ));
}

uint32_t joystick_left ( void ) {
	// returns 1 if the joystick is moved left , or 0 otherwise
	return ( LL_GPIO_IsInputPinSet ( GPIOC , LL_GPIO_PIN_1 ));
}

uint32_t joystick_right ( void ) {
	// returns 1 if the joystick is moved right right , or 0 otherwise
	return ( LL_GPIO_IsInputPinSet ( GPIOC , LL_GPIO_PIN_0 ));
}

uint32_t joystick_up ( void ) {
	// returns 1 if the joystick is moved up , or 0 otherwise
	return ( LL_GPIO_IsInputPinSet ( GPIOA , LL_GPIO_PIN_4 ));
}

uint32_t joystick_down ( void ) {
	// returns 1 if the joystick is moved down , or 0 otherwise
	return ( LL_GPIO_IsInputPinSet ( GPIOB , LL_GPIO_PIN_0 ));//I removed the ; so you see this - please verify that up,down,left,right work by means of testing with an output then begin to assign appropritate functions
}

void displayData(char line1[18], uint8_t data) {
	//Displays function and relevant data
	char data_bin[8] = {0};
	for ( int j = 0; j < 8; j ++) {
		data_bin [j] = ( data & (0x80 >> j)) > 0;
	}			
	sprintf(outputString ,"%x%x%x%x%x%x%x%x", data_bin [0] , data_bin [1] ,data_bin [2] , data_bin [3] , data_bin [4] ,\
	data_bin [5] , data_bin [6] , data_bin [7]);
	
	Clear_Screen();
	put_string(0,0,line1);
	put_string(0,15,outputString);
}

int main(void){
	//Init
        SystemClock_Config();/* Configure the system clock to 84.0 MHz */
	SysTick_Config_MCE2(us);	
	
	//Configure LCD
	Configure_LCD_Pins();
	Configure_SPI1();
	Activate_SPI1();
	Clear_Screen();
	Initialise_LCD_Controller();
	set_font((unsigned char*) Arial_12);
	
	//Configure GPIO and Joystick
	LL_AHB1_GRP1_EnableClock ( LL_AHB1_GRP1_PERIPH_GPIOA ) ; // enable peripheral clock port A
	LL_AHB1_GRP1_EnableClock ( LL_AHB1_GRP1_PERIPH_GPIOB ) ; // enable peripheral clock port B
	LL_AHB1_GRP1_EnableClock ( LL_AHB1_GRP1_PERIPH_GPIOC ) ; // enable peripheral clock port C
	//joystick up:
	LL_GPIO_SetPinMode ( GPIOA , LL_GPIO_PIN_4 , LL_GPIO_MODE_INPUT ); // set A4 as Input
	LL_GPIO_SetPinPull ( GPIOA , LL_GPIO_PIN_4 , LL_GPIO_PULL_NO ); // set A4 as NO pull
	//joystick down:
	LL_GPIO_SetPinMode ( GPIOB , LL_GPIO_PIN_0 , LL_GPIO_MODE_INPUT ); // set B0 as Input
	LL_GPIO_SetPinPull ( GPIOB , LL_GPIO_PIN_0 , LL_GPIO_PULL_NO ); // set B0 as NO pull
	//joystick left:
	LL_GPIO_SetPinMode ( GPIOC , LL_GPIO_PIN_1 , LL_GPIO_MODE_INPUT ); // set C1 as Input
	LL_GPIO_SetPinPull ( GPIOC , LL_GPIO_PIN_1 , LL_GPIO_PULL_NO ); // set C1 as NO pull
	//joystick right:
	LL_GPIO_SetPinMode ( GPIOC , LL_GPIO_PIN_0 , LL_GPIO_MODE_INPUT ); // set C0 as Input
	LL_GPIO_SetPinPull ( GPIOC , LL_GPIO_PIN_0 , LL_GPIO_PULL_NO ); // set C0 as NO pull
	//joystick centre:
	LL_GPIO_SetPinMode ( GPIOB , LL_GPIO_PIN_5 , LL_GPIO_MODE_INPUT ); // set B5 as Input
	LL_GPIO_SetPinPull ( GPIOB , LL_GPIO_PIN_5 , LL_GPIO_PULL_NO ); // set B5 as NO pull


	
	//Configure I2C
	// configure SCL as Alternate function , Open Drain , Pull Up:
	LL_GPIO_SetPinMode ( GPIOB , LL_GPIO_PIN_8 , LL_GPIO_MODE_ALTERNATE );
	LL_GPIO_SetAFPin_8_15 ( GPIOB , LL_GPIO_PIN_8 , LL_GPIO_AF_4 );
	LL_GPIO_SetPinOutputType ( GPIOB , LL_GPIO_PIN_8 , LL_GPIO_OUTPUT_OPENDRAIN );
	LL_GPIO_SetPinPull ( GPIOB , LL_GPIO_PIN_8 , LL_GPIO_PULL_UP );
	// configure SDA as: Alternate , Open Drain , Pull Up:
	LL_GPIO_SetPinMode ( GPIOB , LL_GPIO_PIN_9 , LL_GPIO_MODE_ALTERNATE );
	LL_GPIO_SetAFPin_8_15 ( GPIOB , LL_GPIO_PIN_9 , LL_GPIO_AF_4 );
	LL_GPIO_SetPinOutputType ( GPIOB , LL_GPIO_PIN_9 , LL_GPIO_OUTPUT_OPENDRAIN );
	LL_GPIO_SetPinPull ( GPIOB , LL_GPIO_PIN_9 , LL_GPIO_PULL_UP );
	// Enable the I2C1 Peripheral :
	LL_APB1_GRP1_EnableClock ( LL_APB1_GRP1_PERIPH_I2C1 );
	LL_I2C_Disable ( I2C1 ); // disable I2C1 prior to configuration
	LL_I2C_SetMode ( I2C1 , LL_I2C_MODE_I2C ) ;
	LL_I2C_ConfigSpeed ( I2C1 , 84000000 , 100000 , LL_I2C_DUTYCYCLE_2 ); //set speed to 100 kHz
	LL_I2C_Enable ( I2C1 ); // re - enable I2C1


	//Configure Accelerometer
	// Set the accelerometer to active mode
	LL_I2C_GenerateStartCondition ( I2C1 ); // START
	while (!LL_I2C_IsActiveFlag_SB ( I2C1 ) );
	LL_I2C_TransmitData8 ( I2C1 , ACCELADR ); // ADDRESS + WRITE
	while (!LL_I2C_IsActiveFlag_ADDR ( I2C1 ));
	LL_I2C_ClearFlag_ADDR ( I2C1 );
	LL_I2C_TransmitData8 ( I2C1 , 0x07 ) ; // Set pointer register to mode register
	while (!LL_I2C_IsActiveFlag_TXE ( I2C1 ));
	LL_I2C_TransmitData8 ( I2C1 , 0x01 ) ; // Set set to active mode
	while (!LL_I2C_IsActiveFlag_TXE ( I2C1 )) ;
	LL_I2C_GenerateStopCondition ( I2C1 ); // STOP
	
	uint8_t sample = 0; //int to store tilt register value
	
	enum field {packet_dest, packet_src, packet_length, packet_payload, packet_checksum, packet_checksum_cmp};
	enum field state = packet_dest;
	bool showField = false; //flag to indicate whether current field needs sowing as I'm too lazy to create a function and pass the entire packet
	
	uint8_t dest[4] = {0xcc};
	uint8_t src[4] = {0xdd};
	uint16_t length = 54;
	uint8_t payload[54] = {0};
	uint16_t checksum;
	
	uint8_t read_data[66] = {0}; //int read from EEPROM
	
	//Main Loop
	while (1){
		if(joystick_centre()){
			//Joystick ’centre’ button press: read the tilt register value from the sensor\
			over I2C and display it on the LCD screen. Store the value in the payload\
			field of the packet in memory. Further ’centre’ button presses result in\
			the tilt value being read again and stored in the packet, overwriting the\
			previous value. Display a simple message to indicate the operation has\
			completed.
			
			//Read tilt register from accelerometer
			LL_I2C_GenerateStartCondition ( I2C1 ); // START
			while (! LL_I2C_IsActiveFlag_SB ( I2C1 ));
			
			LL_I2C_TransmitData8 ( I2C1 , ACCELADR ); // ADDRESS + WRITE
			while (! LL_I2C_IsActiveFlag_ADDR ( I2C1 ));
			LL_I2C_ClearFlag_ADDR ( I2C1 );
			
			LL_I2C_TransmitData8 ( I2C1 , 0x03 ) ; // Set pointer register to tilt register
			while (! LL_I2C_IsActiveFlag_TXE ( I2C1 )) ;
			
			LL_I2C_GenerateStartCondition ( I2C1 ); //RE - START
			
			while (! LL_I2C_IsActiveFlag_SB ( I2C1 ));
			
			LL_I2C_TransmitData8 ( I2C1 , ACCELADR +1) ; // ADDRESS + READ
			while (! LL_I2C_IsActiveFlag_ADDR ( I2C1 ) );
			LL_I2C_ClearFlag_ADDR ( I2C1 );
			
			LL_I2C_AcknowledgeNextData ( I2C1 , LL_I2C_NACK ); // ACK INCOMING DATA
			while (! LL_I2C_IsActiveFlag_RXNE ( I2C1 ) );
			sample = LL_I2C_ReceiveData8 ( I2C1 ); // DATA BYTE
			
			LL_I2C_GenerateStopCondition ( I2C1 ); // STOP
			
			payload[0] = sample;//store tilt register value into first byte of payload field
			
			displayData("Data Sampled", sample);
		}
		else if (joystick_up()) {
			//Joystick up button press: display the next field of the packet. For example,\
			if the payload is currently displayed, then a single ’up’ button press will\
			result in the length field being displayed instead. A further ’up’ press\
			results in the src field being displayed, etc. Once the first field of the\
			packet is displayed, further ’up’ presses have no effect, and the first field\
			is continuously displayed.
			/*if (state == packet_checksum)
				state = packet_dest; //wraps back to the front of the enum
			else*/
			if (state != packet_dest)
				state--;
			showField = true;
			
			while(joystick_up()); //waits for joystick to be released so it doesn't rapidly cycle through
		}
		else if (joystick_down()) {
			//Joystick ’down’ button press: display the next field of the packet. For\
			example, if the dest is currently displayed, then a single ’down’ button\
			press will result in the src field being displayed instead. A further ’down’\
			press results in the length field being displayed, and so on. Once the last\
			field of the packet is displayed, further ’down’ presses have no effect, and\
			the last field is continuously displayed.
			if (state == packet_checksum || state == packet_checksum_cmp){
				int sum = 0;
				for (int i=0; i<32; i++) //calculating checksum
					sum += ((read_data[2*i] << 8) | read_data[2*i+1]);
				while (sum>65535) //i.e. when sum>16 bits, loop used to eliminate all carry bits
					sum = (sum & 0xFFFF) + ((sum ^ 0xFFFF) >> 16); //adding "carry" bits to bit 0
				checksum = sum ^ 0xFFFF; //invert all bits, chevron denote XOR
				state = packet_checksum_cmp;
			}
			else
				state++;
			showField = true;
			while(joystick_down()); //waits for joystick to be released so it doesn't rapidly cycle through
		}
		else if (joystick_left()) {
			//Joystick ’left’ button press: read the stored packet from the EEPROM\
			(use 0xA1 when reading) and display it on the LCD. Display a simple message to indicate the read operation has completed. Further ’left’ button\
			presses result in a repeat of the operation
			
			//read data from EEPROM
			LL_I2C_GenerateStartCondition ( I2C1 ); // START
			while (! LL_I2C_IsActiveFlag_SB ( I2C1 ));
			
			LL_I2C_TransmitData8 ( I2C1 , EEPROMADR ); // ADDRESS + WRITE
			while (! LL_I2C_IsActiveFlag_ADDR ( I2C1 ));
			LL_I2C_ClearFlag_ADDR ( I2C1 );
			
			LL_I2C_TransmitData8 ( I2C1 , 0x0000 ) ; // Transmit internal address high byte
			while (! LL_I2C_IsActiveFlag_TXE ( I2C1 )) ;
			
			LL_I2C_TransmitData8 ( I2C1 , 0x0000 ) ; // Transmit internal address low byte
			while (! LL_I2C_IsActiveFlag_TXE ( I2C1 )) ;
			
			LL_I2C_GenerateStartCondition ( I2C1 ); //RE - START
			while (! LL_I2C_IsActiveFlag_SB ( I2C1 ));
			
			LL_I2C_TransmitData8 ( I2C1 , EEPROMADR+1) ; // ADDRESS + READ
			while (! LL_I2C_IsActiveFlag_ADDR ( I2C1 ) );
			LL_I2C_ClearFlag_ADDR ( I2C1 );
			
			for (int i = 0; i<66; i++) {
				LL_I2C_AcknowledgeNextData ( I2C1 , LL_I2C_ACK ); // ACK INCOMING DATA
				while (! LL_I2C_IsActiveFlag_RXNE ( I2C1 ) );
				read_data[i] = LL_I2C_ReceiveData8 ( I2C1 ); // DATA BYTE
			}
			
			//receive final byte
			LL_I2C_AcknowledgeNextData ( I2C1 , LL_I2C_NACK ); // NACK INCOMING DATA for final byte
			while (! LL_I2C_IsActiveFlag_RXNE ( I2C1 ) );
			//read_data[59] = LL_I2C_ReceiveData8 ( I2C1 ); // DATA BYTE
			
			LL_I2C_GenerateStopCondition ( I2C1 ); // STOP
			
			state = packet_dest;
			showField = true;
			
		}
		else if (joystick_right()) {
			//Joystick ’right’ button press: write the entire 60-byte packet to the external EEPROM at I2C device address 0xA0, and starting at internal\
			address 0x0000. Display a simple message to indicate the write operation\
			has completed. Further ’right’ button presses result in a repeat of the\
			operation
			
			
			//preparing packet for transmission
			for (int i = 0; i<4; i++) {
				dest[i] = 0xcc;
				src[i] = 0xdd;
			}
			length = 54;
			payload[0] = sample; //stores sampled value into packet payload
			for (int i=1; i<54; i++)
				payload[i]=0x00;
			
			uint8_t packetToSend[66];
			for (int i=0; i<4; i++)
				packetToSend[i] = dest[i];
			for (int i=0; i<4; i++)
				packetToSend[i+4]=src[i];
			packetToSend[8] = (uint8_t)((length >> 8) & 0xff); //high-byte of size
			packetToSend[9] = (uint8_t)(length & 0xff); //low-byte of size
			for (int i=0; i<54; i++)
				packetToSend[i+10] = payload[i];
			
			int sum = 0;
			for (int i=0; i<32; i++) //calculating checksum
				sum += ((packetToSend[2*i] << 8) | packetToSend[2*i+1]);
			while (sum>65535) //i.e. when sum>16 bits, loop used to eliminate all carry bits
				sum = (sum & 0xFFFF) + ((sum ^ 0xFFFF) >> 16); //adding "carry" bits to bit 0
			checksum = sum ^ 0xFFFF; //invert all bits, chevron denote XOR
			packetToSend[64] = (uint8_t)((checksum >> 8) & 0xff); //high-byte of checksum
			packetToSend[65] = (uint8_t)(checksum & 0xff); //low-byte of checksum
			
			//send data to EEPROM
			do  { //wait for page 1 write to be complete
				LL_I2C_ClearFlag_AF(I2C1); //clear flag
				LL_I2C_GenerateStartCondition ( I2C1 ); // START
				while (! LL_I2C_IsActiveFlag_SB ( I2C1 ));
				
				LL_I2C_TransmitData8 ( I2C1 , EEPROMADR ); // ADDRESS + WRITE
			
				LL_mDelay(100); //short delay;
			} while (LL_I2C_IsActiveFlag_AF(I2C1)); //if set then restart the loop
			
			while (! LL_I2C_IsActiveFlag_ADDR ( I2C1 ));
			LL_I2C_ClearFlag_ADDR ( I2C1 );
			
			LL_I2C_TransmitData8 ( I2C1 , 0x0000 ) ; // Transmit internal address high byte
			while (! LL_I2C_IsActiveFlag_TXE ( I2C1 )) ;
			
			LL_I2C_TransmitData8 ( I2C1 , 0x0000 ) ; // Transmit internal address low byte
			while (! LL_I2C_IsActiveFlag_TXE ( I2C1 )) ;
			
			for (int i=0; i<32; i++) {
				LL_I2C_TransmitData8 ( I2C1 , packetToSend[i] ) ; // Send first 32 bits of packet
				while (! LL_I2C_IsActiveFlag_TXE ( I2C1 )) ;
			}
			
			LL_I2C_GenerateStopCondition ( I2C1 ); // STOP
		
			
			do  { //wait for page 1 write to be complete
				LL_I2C_ClearFlag_AF(I2C1); //clear flag
				
				LL_I2C_GenerateStartCondition ( I2C1 ); // START
				while (! LL_I2C_IsActiveFlag_SB ( I2C1 ));
				
				LL_I2C_TransmitData8 ( I2C1 , EEPROMADR ); // ADDRESS + WRITE
				
				LL_mDelay(100); //short delay;
			} while (LL_I2C_IsActiveFlag_AF(I2C1)); //if set then restart the loop
			
			while (! LL_I2C_IsActiveFlag_ADDR ( I2C1 ));
			LL_I2C_ClearFlag_ADDR ( I2C1 ); //clear address flag (fails if inside of loop)
			
			
			LL_I2C_TransmitData8 ( I2C1 , 0x0020 ) ; // Transmit internal address high byte (address 32)
			while (! LL_I2C_IsActiveFlag_TXE ( I2C1 )) ;
			
			LL_I2C_TransmitData8 ( I2C1 , 0x0020 ) ; // Transmit internal address low byte (address 32)
			while (! LL_I2C_IsActiveFlag_TXE ( I2C1 )) ;
			
			for (int i=32; i<64; i++) {
				LL_I2C_TransmitData8 ( I2C1 , packetToSend[i] ) ; // Send last 28 bits of packet
				while (! LL_I2C_IsActiveFlag_TXE ( I2C1 )) ;
			}
			
			LL_I2C_GenerateStopCondition ( I2C1 ); // STOP
			
			do  { //wait for page 1 write to be complete
				LL_I2C_ClearFlag_AF(I2C1); //clear flag
				
				LL_I2C_GenerateStartCondition ( I2C1 ); // START
				while (! LL_I2C_IsActiveFlag_SB ( I2C1 ));
				
				LL_I2C_TransmitData8 ( I2C1 , EEPROMADR ); // ADDRESS + WRITE
				
				LL_mDelay(100); //short delay;
			} while (LL_I2C_IsActiveFlag_AF(I2C1)); //if set then restart the loop
			
			while (! LL_I2C_IsActiveFlag_ADDR ( I2C1 ));
			LL_I2C_ClearFlag_ADDR ( I2C1 ); //clear address flag (fails if inside of loop)
			
			
			LL_I2C_TransmitData8 ( I2C1 , 0x0040 ) ; // Transmit internal address high byte (address 64)
			while (! LL_I2C_IsActiveFlag_TXE ( I2C1 )) ;
			
			LL_I2C_TransmitData8 ( I2C1 , 0x0040 ) ; // Transmit internal address low byte (address 64)
			while (! LL_I2C_IsActiveFlag_TXE ( I2C1 )) ;
			
			for (int i=64; i<66; i++) {
				LL_I2C_TransmitData8 ( I2C1 , packetToSend[i] ) ; // Send last 28 bits of packet
				while (! LL_I2C_IsActiveFlag_TXE ( I2C1 )) ;
			}
			
			LL_I2C_GenerateStopCondition ( I2C1 ); // STOP
			
			displayData("Data Written", sample);
			
		}
		
		
  if (showField) {
		Clear_Screen();
		switch(state) {			
			case packet_dest:
				put_string(0,0, "Destination");
				sprintf(outputString, "%x%x%x%x", read_data[0], read_data[1], read_data[2], read_data[3]);
				put_string(0,15,outputString);
				break;
			
			case packet_src:
				put_string(0,0,"Source");
				sprintf(outputString, "%x%x%x%x", read_data[4], read_data[5], read_data[6], read_data[7]);
				put_string(0,15,outputString);
				break;
			
			case packet_length:
				put_string(0,0, "Length");
				sprintf(outputString, "%x", ((read_data[8] << 8) | read_data[9]));
				put_string(0,15,outputString);
				break;
			
			case packet_payload:
				displayData("Payload", read_data[10]);
				break;
			
			case packet_checksum:
				put_string(0,0, "Checksum");
				sprintf(outputString, "%x%x", read_data[64], read_data[65]);
				put_string(0,15,outputString);
				break;
			
			case packet_checksum_cmp:
				if (checksum == ((read_data[64] << 8) | read_data[65]))
					put_string(0,0,"Checksum OK");
				else
					put_string(0,0,"Checksum Failed");
				sprintf(outputString, "%x%x // %X", read_data[64], read_data[65], checksum);
				put_string(0,15, outputString);
			break;
		}
	showField = false; //resets flag to 0
	}
	}
}
