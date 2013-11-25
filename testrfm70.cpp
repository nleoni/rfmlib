/*
    Test Program for RFM70device class
    Created by Napoleon Leoni, 5/25/13
    last compiled, 6/9/13
    This program uses the RFM70device class
    to use the mbed platform as a PRX (primary
    receive device). It runs on the IRQ generated
    by RX data in the RFM70 and outputs the data
    to the analog output on pin 18, simulating
    the receiving data function for a wireless
    oscilloscope probe

*/




#include "mbed.h"
#include "RFM70device.h"

SPI ser_port(p11,p12,p13); //mosi,miso,sclk
DigitalOut spiCSN(p14);
DigitalOut CE(p10);
InterruptIn rfmIRQ(p9);
void ISRrfmIRQ(void);
AnalogOut Aout(p18);
DigitalOut redled(p26); //define and name a digital output on pin 5


Serial pc(USBTX,USBRX); //Set serial communication with PC via UART
u8 SerialPortWriteWrapper(u8);
void spiCSNWrapper(u8);
void CEwrapper(u8 bitToWrite);
void printToTerminalWrapper(char *);

char switch_word;
char regdata,*pregdata;
char cCurrent,cCurrentMod;
int cBaudRate=38400;
//configure class
RFMdeviceTypicalConfigs myRFMconfig=PRX; //Sets this RFM to PRX meaning primary receive operation
RFM70device* pmyRFTX;

int main() {
pregdata=&regdata;
//Setup serial port parameters
pc.baud(cBaudRate);
pc.printf("RFM70 device simple test program, Baid Rate:%i\n",cBaudRate);
pc.printf(">");

    //Setup SPI port parameters
    ser_port.format(8,0);
    ser_port.frequency(8000000);

//construct rfm70 object
RFM70device myRFTX(SerialPortWriteWrapper,spiCSNWrapper,CEwrapper,myRFMconfig,printToTerminalWrapper);
pmyRFTX=&myRFTX;
myRFTX.readRegister(0x00,pregdata);
pc.printf("The status register is: %X\n",myRFTX.getStatusRegister());
pc.printf("\nThe data in the register is: %X\n",regdata);
pmyRFTX->powerUp();
//attach interrupt to IRQ pin
rfmIRQ.fall(&ISRrfmIRQ);
//Now enable the chip for continuous receive
pmyRFTX->chipEnable();

    while(1) {
     if(pc.readable()){
        cCurrent=pc.getc();//adds 1 to the ascii coed and returns it
        cCurrentMod=cCurrent+1;
        pc.printf("%c:echo+1=%c\n>",cCurrent,cCurrentMod);
        }
        wait(10);
        pmyRFTX->readRegister(0x07,&regdata);//read status register
        pc.printf("Status register%X\n>",regdata);
    }

}

u8 SerialPortWriteWrapper(u8 byteToWrite){
    return ser_port.write(byteToWrite);
}

void spiCSNWrapper(u8 bitToWrite){
    spiCSN.write(bitToWrite);
}

void CEwrapper(u8 bitToWrite){
    CE.write(bitToWrite);
}

void printToTerminalWrapper(char* pString){
    pc.printf(pString);
}

//INterrupt service routine for IRQ from RFM70 device, interrupt will occur when ata is received
void ISRrfmIRQ(void){
u8 data;
u8 status;
unsigned short data2;
redled.write(1);
//Function to handle interrupt from RFM device data received
//Next read data available
pmyRFTX->readRXPayload(&data);
redled.write(0);
data2=data;
data2=data2<<2;
Aout.write_u16(data2);
wait_us(20);
redled.write(1);
//First reset interrup on RFM device
pmyRFTX->clearRXIRQ();
//pmyRFTX->setBits(&RX_DR,1); //Write one to clear the bit
redled.write(0);
//pc.printf("RF data received %X\n>",data);

/*if(data>128){
    redled=1;
    } else {
    redled=0;
}*/
//Next print the status register for debug purposes
pmyRFTX->readRegister(0x07,&status);//read status register
//pc.printf("Status register%X\n>",data);
}
