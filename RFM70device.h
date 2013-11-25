/* RFM70device.h
 *
 *  Created on: May 27, 2013
 *      Author: nleoni
 */

/*
 * RFM70device.h
 *
 *  Created on: May 25, 2013
 *      Author: nleoni
 */
#include <stdio.h> // for size_t

#ifndef RFM70DEVICE_H_
#define RFM70DEVICE_H_

typedef char u8;

typedef unsigned int u32;

//define initialization for memory map addresses
//This is the initial state for the register map
#define NREGISTERS 30
#define REGISTERDATABYTESMAX 5
const u8 registerMapInit[NREGISTERS][3]={
    //{Address, Reset Value, Number of bytes}
    {0x00,0x08,1},      //CONFIG        configuration register:                 default value 0b00001000 = 0x08
    {0x01,0x3F,1},      //EN_AA         Enable Auto acknowledge:                default value 0b00111111 = 0x3F
    {0x02,0x03,1},      //EN_RXADDR     Enable RX addressess:                   default value 0b00000011 = 0x03
    {0x03,0x03,1},      //SETUP_AW      setup of ...                            default value 0b00000011 = 0x03
    {0x04,0x03,1},      //SETUP_RETR    Setup of automatic retransmission       default value 0b00000011 = 0x03
    {0x05,0x02,1},      //RF_CH         RF channel                              default value 0b00000010 = 0x02
    {0x06,0x3F,1},      //RF_setup      RF setup Register                       default value 0b00111111 = 0x3F
    {0x07,0x0E,1},      //STATUS        Status register                         default value 0b00001110 = 0x0E
    {0x08,0x00,1},      //OBSERVE_TX    Transmit observe register               default value 0b00000000 = 0x00
    {0x09,0x00,1},      //CD            Carrier Detect                          default value 0b00000000 = 0x00
    {0x0A,0xE7,5},      //RX_ADDR_P0    Receive address data pipe 0, byte init repeats for all five bytes available
    {0x0B,0xC2,5},      //RX_ADDR_P1    Receive address data pipe 1, byte init repeats for all five bytes available
    {0x0C,0xC3,1},      //RX_ADDR_P2    Receive address data pipe 2
    {0x0D,0xC4,1},      //RX_ADDR_P3    Receive address data pipe 3
    {0x0E,0xC5,1},      //RX_ADDR_P4    Receive address data pipe 4
    {0x0F,0xC6,1},      //RX_ADDR_P5    Receive address data pipe 5
    {0x10,0xE7,5},      //TX_ADDR       Transmit address, PTX device only, byte init repeats for all five bytes available
    {0x11,0x00,1},      //RX_PW_P0      Number of bytes in RX payload data pipe 0   default value 0b00000000 = 0x00
    {0x12,0x00,1},      //RX_PW_P1      Number of bytes in RX payload data pipe 1   default value 0b00000000 = 0x00
    {0x13,0x00,1},      //RX_PW_P2      Number of bytes in RX payload data pipe 2   default value 0b00000000 = 0x00
    {0x14,0x00,1},      //RX_PW_P3      Number of bytes in RX payload data pipe 3   default value 0b00000000 = 0x00
    {0x15,0x00,1},      //RX_PW_P4      Number of bytes in RX payload data pipe 4   default value 0b00000000 = 0x00
    {0x16,0x00,1},      //RX_PW_P5      Number of bytes in RX payload data pipe 5   default value 0b00000000 = 0x00
    {0x17,0x11,1},      //FIFO_STATUS   FIFO status register                        default value 0b00010001 = 0x11
    {0x00,0x08,1},      //CONFIG        (dummy, protected register 0x18)            default value 0b00001000 = 0x08
    {0x00,0x08,1},      //CONFIG        (dummy, protected register 0x19)            default value 0b00001000 = 0x08
    {0x00,0x08,1},      //CONFIG        (dummy, protected register 0x1A)            default value 0b00001000 = 0x08
    {0x00,0x08,1},      //CONFIG        (dummy, protected register 0x1B)            default value 0b00001000 = 0x08
    {0x1C,0x00,1},      //DYNPD         enable dynamic payload length               default value 0b00000000 = 0x00
    {0x1D,0x00,1}       //FEATURE       feature register                            default value 0b00000000 = 0x00
};

//define structure types to hold the memory addresses
typedef struct{
    u8 address;
    u8 bank;
    u8 readOnly;
    u8 bitlowrange;
    u8 bithighrange;
    //u8 *allowableValues;//this are the allowable values
    char *description;
} MemoryMnemonic;

//Define Mnemonics: Maybe I should define these within the class, so that the
//implementation of the class is not visible and also so that a mnemonic
//may not be defined externally to the class

//Memory Map Address 0x00
//{address,bank,readOnly,bitlowrange,bithighrange,description}
MemoryMnemonic const PRIM_RX = {0x00,0,0,0,0,"RX TX control"};//Maybe define this one within the class!!
MemoryMnemonic const PWR_UP = {0x00,0,0,1,1,"1: Power up 0:Power Down"};
MemoryMnemonic const CRCO = {0x00,0,0,2,2,"CRC enconding scheme 0 - 1 byte; 1 - 2 bytes"};
MemoryMnemonic const EN_CRC = {0x00,0,0,3,3,"Enable CRC forced high if any high bit in EN_AA"};
MemoryMnemonic const MASK_MAX_RT= {0x00,0,0,4,4,"Mask interrupt caused by MAX_RT\n1: Interrupt not reflected on the IRQ pin\n0: Reflect MAX_RT as active low interrupt on the IRQ pin"};
MemoryMnemonic const MASK_TX_DS = {0x00,0,0,5,5,"Mask interrupt caused by TX_DS\n1: Interrupt not reflected on the IRQ pin\n0: Reflect TX_DS as active low interrupt on the IRQ pin"};
MemoryMnemonic const MASK_RX_DR = {0x00,0,0,6,6,"Mask interrupt caused by RX_DR\n1: Interrupt not reflected on the IRQ pin\n0: Reflect RX_DR as active low interrupt on the IRQ pin"};

//Memory Map Address 0x01 EN_AA register
MemoryMnemonic const ENAA_P0 = {0x01,0,0,0,0,"Enable auto acknowledgement data pipe 0"};
MemoryMnemonic const ENAA_P1 = {0x01,0,0,1,1,"Enable auto acknowledgement data pipe 1"};
MemoryMnemonic const ENAA_P2 = {0x01,0,0,2,2,"Enable auto acknowledgement data pipe 2"};
MemoryMnemonic const ENAA_P3 = {0x01,0,0,3,3,"Enable auto acknowledgement data pipe 3"};
MemoryMnemonic const ENAA_P4 = {0x01,0,0,4,4,"Enable auto acknowledgement data pipe 4"};
MemoryMnemonic const ENAA_P5 = {0x01,0,0,5,5,"Enable auto acknowledgement data pipe 5"};

//Memory Map Address 0x02 EN_RXADDR register
MemoryMnemonic const ERX_P0 = {0x02,0,0,0,0,"Enable data pipe 0"};
MemoryMnemonic const ERX_P1 = {0x02,0,0,1,1,"Enable data pipe 1"};
MemoryMnemonic const ERX_P2 = {0x02,0,0,2,2,"Enable data pipe 2"};
MemoryMnemonic const ERX_P3 = {0x02,0,0,3,3,"Enable data pipe 3"};
MemoryMnemonic const ERX_P4 = {0x02,0,0,4,4,"Enable data pipe 4"};
MemoryMnemonic const ERX_P5 = {0x02,0,0,5,5,"Enable data pipe 5"};

//Memory Map Address 0x03 SETUP_AW register
MemoryMnemonic const AW = {0x03,0,0,0,1,"RX/TX Address field width\n 00 - Illegal\n 01 - 3 bytes\n 10 - 4 bytes\n 11 - 5 bytes\nLSB bytes are used if address width is below 5 bytes"};

//Memory Map Address 0x04 SETUP_RETR register
MemoryMnemonic const ARD = {0x04,0,0,4,7,"Auto Retransmission Delay 0000 – Wait 250 us\n0001 – Wait 500 us\n0010 – Wait 750 us\n1111 – Wait 4000 us"};
MemoryMnemonic const ARC = {0x04,0,0,0,3,"Auto Retransmission Count\n0000 –Re-Transmit disabled\n0001 – Up to 1 Re-Transmission on fail of AA\n1111 – Up to 15 Re-Transmission on fail of AA"};

//Memory Map Address 0x05 RF_CH register
MemoryMnemonic const RF_CH = {0x05,0,0,0,6,"Sets the frequency channel"};

//Memory Map Address 0x06 EN_RXADDR register
MemoryMnemonic const LNA_HCURR = {0x06,0,0,0,0,"Setup LNA gain 0:Low gain(20dB down) 1:High gain"};
MemoryMnemonic const RF_PWR = {0x06,0,0,1,2,"Set RF output power in TX mode RF_PWR[1:0] \n00 – -10 dBm 01 – -5 dBm 10 – 0 dBm 11 – 5 dBm"};
MemoryMnemonic const RF_DR  = {0x06,0,0,3,3,"Air Data Rate 0-1MBPS 1-2MBPS"};

//Memory Map Address 0x07 STATUS register
MemoryMnemonic const TX_FULL = {0x07,0,0,0,0,"TX FIFO full flag; 1:TX FIFO FULL"};
MemoryMnemonic const RX_P_NO = {0x07,0,1,1,3,"Data pipe number for the payload available for reading from RX_FIFO 000-101: Data Pipe Number 110: Not used 111: RX FIFO Empty"};
MemoryMnemonic const MAX_RT = {0x07,0,0,4,4,"Maximum number of TX retransmits interrupt\nWrite 1 to clear bit if asserted must be cleared to enable further comm"};
MemoryMnemonic const TX_DS = {0x07,0,0,5,5,"Data Sent TX FIFO interrupt\nAsserted when packet transmitted on TX.\nIf AUTO_ACK is activated, this bit is set high only when ACK is received.\nWrite 1 to clear bit."};
MemoryMnemonic const RX_DR = {0x07,0,0,6,6,"Data Ready RX FIFO interrupt\nAsserted when new data arrives RX FIFO Write 1 to clear bit"};
MemoryMnemonic const RBANK = {0x07,0,1,7,7,"Register bank selection states. Switch\nregister bank is done by SPI command\n ACTIVATE followed by 0x53\n0: Register bank 0\n1: Register bank 1"};

//Memory Map Address 0x08 OBSERVE_TX register
MemoryMnemonic const ARC_CNT   = {0x08,0,1,1,3,"Count retransmitted packets"};
MemoryMnemonic const PLOS_CNT  = {0x08,0,1,4,7,"Count lost packets"};

//Memory Map Address 0x09 CD register
MemoryMnemonic const CD = {0x09,0,1,0,0,"Carrier detect"};

//Memory Map Address 0x0A RX_ADDR_P0 register
MemoryMnemonic const RX_ADDR_P0 = {0x0A,0,0,0,39,"Receive address data pipe 0. 5 Bytes maximum length. n\(LSB byte is written first. Write the number of bytes defined by SETUP_AW)"};
//Memory Map Address 0x0B RX_ADDR_P1 register
MemoryMnemonic const RX_ADDR_P1 = {0x0B,0,0,0,39,"Receive address data pipe 1. 5 Bytes maximum length. n\(LSB byte is written first. Write the number of bytes defined by SETUP_AW)"};
//Memory Map Address 0x0B RX_ADDR_P2 register
MemoryMnemonic const RX_ADDR_P2 = {0x0C,0,0,0,7,"Receive address data pipe 2. Only LSB, MSB bytes is equal to RX_ADDR_P1[39:8]"};
//Memory Map Address 0x0D RX_ADDR_P3 register
MemoryMnemonic const RX_ADDR_P3 = {0x0D,0,0,0,7,"Receive address data pipe 3. Only LSB, MSB bytes is equal to RX_ADDR_P1[39:8]"};
//Memory Map Address 0x0E RX_ADDR_P4 register
MemoryMnemonic const RX_ADDR_P4 = {0x0E,0,0,0,7,"Receive address data pipe 4. Only LSB, MSB bytes is equal to RX_ADDR_P1[39:8]"};
//Memory Map Address 0x0F RX_ADDR_P5 register
MemoryMnemonic const RX_ADDR_P5 = {0x0F,0,0,0,7,"Receive address data pipe 5. Only LSB, MSB bytes is equal to RX_ADDR_P1[39:8]"};
//Memory Map Address 0x10 TX_ADDR register
MemoryMnemonic const TX_ADDR = {0x10,0,0,0,39,"Transmit address. Used for a PTX deviceonly.(LSB byte is written first)\nSet RX_ADDR_P0 equal to this address to handle automatic acknowledge if this is a PTX device"};

//Memory Map Address 0x11 RX_ADDR_P0 register
MemoryMnemonic const RX_PW_P0 = {0x11,0,0,0,5,"Number of bytes in RX payload in data pipe"};
//Memory Map Address 0x12 RX_ADDR_P0 register
MemoryMnemonic const RX_PW_P1 = {0x12,0,0,0,5,"Number of bytes in RX payload in data pipe"};
//Memory Map Address 0x13 RX_ADDR_P0 register
MemoryMnemonic const RX_PW_P2 = {0x13,0,0,0,5,"Number of bytes in RX payload in data pipe"};
//Memory Map Address 0x14 RX_ADDR_P0 register
MemoryMnemonic const RX_PW_P3 = {0x14,0,0,0,5,"Number of bytes in RX payload in data pipe"};
//Memory Map Address 0x15 RX_ADDR_P0 register
MemoryMnemonic const RX_PW_P4 = {0x15,0,0,0,5,"Number of bytes in RX payload in data pipe"};
//Memory Map Address 0x16 RX_ADDR_P0 register
MemoryMnemonic const RX_PW_P5 = {0x16,0,0,0,5,"Number of bytes in RX payload in data pipe"};

//Memory Map Address 0x17 FIFO_STATUS register
MemoryMnemonic const RX_EMPTY = {0x17,0,1,0,0,"RX FIFO empty flag, 1 if Empty"};
MemoryMnemonic const RX_FULL = {0x17,0,1,1,1,"RX FIFO full flag, 1 if full"};
MemoryMnemonic const TX_EMPTY = {0x17,0,1,4,4,"TX FIFO empty flag., 1 if Empty"};
//MemoryMnemonic const TX_FULL = {0x17,0,1,5,5,"TX FIFO full flag, 1 if full"}; //Already defined
MemoryMnemonic const TX_REUSE = {0x17,0,1,6,6,"Reuse last transmitted data packet if set high."};


enum RFMdeviceTypicalConfigs{PR_TXnoIRQ = 1,PRX = 2};//Typical configurations to initialize the RFM70device

class RFM70device {
public:
    RFM70device(u8 (*pSPI)(u8),void (*pCSN)(u8),void (*pCE)(u8),RFMdeviceTypicalConfigs myConfiguration,void (*printFunction)(char *)) {
        // TODO Auto-generated constructor stub
        this->mStatusRegister=registerMapInit[0][1];
        this->pSPIinterfaceWrite=pSPI;
        this->pSPIChipSelectInv=pCSN;
        this->pChipEnable=pCE;
        this->pSPIChipSelectInv(1);//set chip select high to deselect the slave SPI device.
        this->printToTerminal=printFunction;
        char* sometext="\nInside constructor: case no PR_TXnoIRQ\n>";
        char* sometext2="\nInside constructor: case no PRX\n>";
        char* sometext3="\nInside constructor: case no default\n>";

        //Perform typical initialization and verification here.
        this->RFMconfiguration=myConfiguration;
        switch(this->RFMconfiguration){
        case PR_TXnoIRQ: //PR_TXnoIRQ device
        //Define intialization
        //For PRXnoIRQ mode all IRQ are disabled, the device will simply transmit data
        //the EN_AA=1 and NO_ACQ=0 ensure receiving an ACQ package from PR_TX device

        //Set all required bits in Register 0 Bank 0, including those set by default
        this->setBits(&PRIM_RX,0);
        this->setBits(&PWR_UP,0);//Keep power down, until later
        this->setBits(&CRCO,0);
        this->setBits(&EN_CRC,1);
        this->setBits(&MASK_MAX_RT,1);
        this->setBits(&MASK_TX_DS,1);
        this->setBits(&MASK_RX_DR,1);

        //Set Enable Auto Acknowledgement
        this->setBits(&ENAA_P0,1);
        //Set the address field width to the minimum
        this->setBits(&AW,1);

        //For now keep RF channel and RF setup as they are.
        break;

        case PRX: //PRX device
#ifdef _NLEONITRACE
            this->RFtracePrint(sometext2);
            this->RFtracePrint(PRIM_RX.address);
#endif
        //Define intialization
        this->setBits(&PRIM_RX,1);
        this->setBits(&PWR_UP,0);//Keep power down, until later
        this->setBits(&CRCO,0);
        this->setBits(&EN_CRC,1);
        this->setBits(&MASK_MAX_RT,1);//disable interrupt from max nu mber of retransmission
        this->setBits(&MASK_TX_DS,1);//disable data sent interrupt
        this->setBits(&MASK_RX_DR,0);//allow Interrupt from data received

        //Set Enable Auto Acknowledgement
        this->setBits(&ENAA_P0,1);
        //Set the address field width to the minimum
        this->setBits(&AW,1);

        //Number of bytes in Receive payload data pipe
        //as we are not using dynamic payload we need to set this
        this->setBits(&RX_PW_P0,1);//set to 1 byte
        break;

        default:
        //should give an error here TODO
        this->RFtracePrint(sometext3);
        }
    }
    virtual ~RFM70device();

    //access to Class state
    u8 getStatusRegister();

    //Commands to RFM70 via SPI interface
    void readRegister(u8 registerAddress,u8 *pregisterData);//note that the data read from the Memory register may be up to 5 bytes
    //Functions to set or clear a specific bit as defined by a mnemonic or identifier
    u8 setBits(const MemoryMnemonic *pmnemonic,u8 bvalue);//Note that some mnemonics have more than 1 bit, e.g. b101, would set bits 0 and 2 leaving bit 1 unmodified
    u8 clearBits(const MemoryMnemonic *pmnemonic,u8 bvalue);//Note that some mnemonics have more than 1 bit, e.g. b101, would clear bits 0 and 2 leaving bit 1 unmodified

    void writeTXPayload(u8 data);//for now we are only transmitting 1 byte, valid for PR_TX only
    void readRXPayload(u8* data);//valid for all devices
    void writeAcknowledgePackage(u8 data); //For PRX device, this is the payload to be sent with auto acknowledge package
    void powerUp(void);
    void powerDown(void);
    void chipEnable(void);
    void chipDisable(void);
    void RFM70device::clearRXIRQ(void);

private:
    u8 mStatusRegister;
    /*const*/ u8 (*pSPIinterfaceWrite)(u8); //This is the pointer to the function handling the SPI comm
                                            //it will be initialized with the constructor.
    void (*pSPIChipSelectInv)(u8); //this is a pointer to a provided function handling the SPI chip select
                                    //signal inverted, should take 0 or 1 as input: 1: deselect;
                                    //                                              0L select;

    void (*printToTerminal)(char *pstring); //needed for trace generation!! function to print strings to terminal
    void (*pChipEnable)(u8);//pointer to chip enable wrapper function

    void RFtracePrint(char *pstring);
    void RFtracePrint(u8 value);
    void writeRegister(u8 registerAddress,const u8 *registerData);//we make this private as the only interface we want to provide
                                                            //is that to write thru the defined mnemonics
    u8 RFMdeviceStatus;
    RFMdeviceTypicalConfigs RFMconfiguration;
};
#endif /* RFM70DEVICE_H_ */
