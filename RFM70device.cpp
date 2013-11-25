/*
 * RFM70device.cpp
 *
 *  Created on: May 25, 2013
 *      Author: nleoni
 *
 *      This Class provides handling of a HOPE RF RFM70device
 *      In order to make it platform independent the constructor is provided
 *      with pointers to two functions required to handle the SPI communication
 *      pSPIChipSelectInv(chip select not bit) -> when provided 0 as argument
 *      should set low the chip select output enabling SPI, when set to 1 should
 *      set high the CS bit thus disbaling SPI comm.
 *      u8(received data) pSPIinterfaceWrite(u8 data) -> should send one byte of data
 *      through the SPI interface, received data should be returned by the function.
 *
 *      SPI communications setup
 *      CLK...8 MHZ maximum
 *      SPI MODE: 0, data valid on rising edge of CLK at zero phase.
 */

#include "RFM70device.h"
//#include "Arduino.h"

RFM70device::~RFM70device() {
    // TODO Auto-generated destructor stub
}

u8 RFM70device::getStatusRegister(){
    return this->mStatusRegister;
}

void RFM70device::readRegister(u8 registerAddress,u8 *pregisterData){
    u8 registerValue=0;
    u8 command=0x1F;        //From the Data sheet the 000A AAAA Read command and status registers. AAAAA =
                           //5 bit Register Map Address, For reading a register the data sent is ignored
                           //still two SPI writes are needed the status register is returned on the first one and
                           //the returned DATA is 1 to 5 bytes, LSB byte first. Set all address bits high: 0b00011111=0x1F
    command=command&registerAddress;

    this->pSPIChipSelectInv(0);
    this->mStatusRegister=RFM70device::pSPIinterfaceWrite(command);
    u8 nbytes=registerMapInit[registerAddress][2];//TODO Because the accessible memory map is not all contiguous, need to modify
                                                   //this, maybe provide a memory Class to access registers and addresses.
    for(u8 i=0;i<nbytes;i++){
        *pregisterData=RFM70device::pSPIinterfaceWrite(command); //check number of bytes at the register address and iterate
        pregisterData++;
    }                                                                               //reading of the SPI (via writing a command), note that from the data
                                                                                    //sheet CS should be brought low only once at the beginning of the sequence
                                                                                    //and then pulled up at the end. So we need our class to have control of that as well
    this->pSPIChipSelectInv(1);
    return;
}

u8 RFM70device::setBits(const MemoryMnemonic* pmnemonic,u8 value){
    //This function will be implemented only for single byte locations
    //a separate function will address the specific cases like the RX addresses wihch have 5 bytes
    //Memory mnemonic structure: {address,bank,readOnly,bitlowrange,bithighrange,description}
    //this function should read the proper register, then set the proper bit and OR them with the existing register value,
    //then rewrite back to the device.
    //this->RFtracePrint("Inside setbits\n>");
    u8 registerData[REGISTERDATABYTESMAX]={0,0,0,0,0},bitmask=0,newRegisterValue;
    //ensure that memory location can be written to
    if(pmnemonic->readOnly){
        //Trace functions
            this->RFtracePrint("Error:this is a read only memory location");
            return 0;
    }

    this->readRegister(pmnemonic->address,registerData);//Note that data is read from LSB to MSB, MSB bit first in each byte
    if(pmnemonic->bithighrange>7){//if more than one byte in location do nothing...
        this->RFtracePrint("\nAttempted use of RFM70device::setBits with memory location with more than 1 byte\n>");
        return 0;
    }
    for(u8 i=pmnemonic->bitlowrange;i<=pmnemonic->bithighrange;i++){
        bitmask=bitmask|(1<<i);
    }
    newRegisterValue=((value<<(pmnemonic->bitlowrange))&bitmask)|registerData[0];//Value to be written to register
    //Trace functions
    this->RFtracePrint("Setbit called for\nAddress:");
    this->RFtracePrint(pmnemonic->address);
    this->RFtracePrint("Current Register Value:");
    this->RFtracePrint(registerData[0]);
    this->RFtracePrint("New register value to be written:");
    this->RFtracePrint(newRegisterValue);
    //end of trace section

    this->writeRegister(pmnemonic->address,&newRegisterValue);


    this->readRegister(pmnemonic->address,registerData);    //read to confirm successful write

    //trace functions
    this->RFtracePrint("Current Register Value after write:");
    this->RFtracePrint(registerData[0]);
    //end of trace section

    return 1;//WHat is this return value for?? to confirm operation?
}
void RFM70device::clearRXIRQ(void){
    u8 registerData;
    this->readRegister(0x07,&registerData);//read status register
    //mask registerData with proper IRQ clear bit
    registerData=registerData&0x40;
    this->writeRegister(0x07,&registerData);
}

u8 RFM70device::clearBits(const MemoryMnemonic* pmnemonic,u8 value){
    u8 i,n;
    i=1;
    return i;

}

void RFM70device::RFtracePrint(char *pstring){
    this->printToTerminal(pstring);
}

void RFM70device::RFtracePrint(u8 value){//TODO add RFtracePrint for arrays.... u8* pvalue,nbytes
    char numberString[6],*carriageReturn="\n>";
    sprintf(numberString,"%x",value);
    this->printToTerminal(numberString);
    this->RFtracePrint(carriageReturn);
}

void RFM70device::writeRegister(u8 registerAddress,const u8 *pregisterData){
    u8 command=0x20;        //From the Data sheet the 001A AAAA Read command and status registers. AAAAA =
                           //5 bit Register Map Address, For reading a register the data sent is ignored
                           //still two SPI writes are needed the status register is returned on the first SPI
                           //cycle which carries the command, the second SPI cycles sends the data.
    command=command|registerAddress;

    this->pSPIChipSelectInv(0);
    this->mStatusRegister=RFM70device::pSPIinterfaceWrite(command);
    //this->RFtracePrint("Command is:");
    //this->RFtracePrint(command);
    //this->RFtracePrint("\n>");
    u8 nbytes=registerMapInit[registerAddress][2];//TODO Because the accessible memory map is not all contiguous, need to modify
                                                   //this, maybe provide a memory Class to access registers and addresses.
    //note that pregisterData is a pointer to an array of bytes of data with the LSB byte a position pregisterData[0]
    //and the MSB byte at pregisterData[nbytes]
    for(u8 i=0;i<nbytes;i++){
        RFM70device::pSPIinterfaceWrite(*pregisterData); //check number of bytes at the register address and iterate
        pregisterData++;
    }                                                                               //reading of the SPI (via writing a command), note that from the data
                                                                                    //sheet CS should be brought low only once at the beginning of the sequence
                                                                                    //and then pulled up at the end. So we need our class to have control of that as well
    this->pSPIChipSelectInv(1);
    return;


}

void RFM70device::writeTXPayload(u8 data){//for now we are only transmitting 1 byte, valid for PR_TX only
    u8 command=0xA0;

    //note if this is a PRX device this is not a proper command so ignore it
    if(this->RFMconfiguration==PRX) return;

    this->pSPIChipSelectInv(0);
    this->mStatusRegister=RFM70device::pSPIinterfaceWrite(command);
    RFM70device::pSPIinterfaceWrite(data);
    this->pSPIChipSelectInv(1);
    return;

}
void RFM70device::readRXPayload(u8 *data){//valid for all devices
    u8 command=0x61;


    this->pSPIChipSelectInv(0);
    this->mStatusRegister=RFM70device::pSPIinterfaceWrite(command);
    *data=RFM70device::pSPIinterfaceWrite(command);
    this->pSPIChipSelectInv(1);
    return;

}
void RFM70device::writeAcknowledgePackage(u8 data){
    //For PRX device, this is the payload to be sent with auto acknowledge package

}
void RFM70device::powerUp(void){
    this->setBits(&PWR_UP,1);
}
void RFM70device::powerDown(void){
    this->setBits(&PWR_UP,0);
}
void RFM70device::chipEnable(void){
    this->pChipEnable(1);
}
void RFM70device::chipDisable(void){
    this->pChipEnable(0);
}
