//This program implement a simple shell to configure a waveform, which is then sent via spi to an mbed board for analog output
//It also generates a lightweight RFM driver for the arduino.
//Created by Napoleon Leoni
//Last Compiled on 6/9/2013
//NOte that this is no really a .cpp source code file
//but rather an Arduino sketch.

   #include <TimerOne.h>
   #include <SPI.h>

   #define MAXCOMMANDLENGTH 24
   #define MAXSAMPLES 200 //this means about 200 bytes not much out of the 2kb available
   #define SPICLOCK 8 //This is the SPI clock in Megahertz
   #define DEFAULTFREQ 100//default waveform frequency in Hz
   #define DEFAULTSAMPL 10000//default sampling rate samples/sec
   #define TIMERINTPERIOD 20//default timer interrupt period in usseconds

   #define CONFIG_ADDRESS 0x00 //COnfig Registers
   #define PRIM_RX 0x01
   #define PWR_UP  0x02
   #define EN_CRC 0x08
   #define MASK_MAX_RT 0x10
   #define MASK_TX_DS 0x20
   #define MASK_RX_DR 0x40

   //CONFIG=PRIM_RX|MASK_MAX_RT|MASK_TX_DS|MASK_RX_DR|PWR_UP|EN_CRC;

   #define EN_AA_ADDRESS 0x01 //COnfig Registers
   #define ENAA_P0 0x01  //bit 0 in CONFIG

   #define SETUP_AW_ADDRESS  0x03
   #define AW 0x03

   #define SETUP_RETR_ADDRESS 0x04
   #define ARD 0xF0  //automatic retransmission setupl bits
   #define ARC 0xF  //retransmit bits

   #define STATUS_ADDRESS 0x07
   #define TX_FULL 0
   #define MAX_RT 0x10
   #define TX_DS 0x20
   #define RX_DR 0x40
   #define STATUSMASK 0xF0
   unsigned char STATUS,CONFIG,EN_AA,SETUP_AW,FIFO;
   unsigned char data1,data2=0;//Initially tried using type char and did not work, interesting :-)
                      //but also

   int machineState=0x0000,globalCounter=0;
   int samplingrate=DEFAULTSAMPL;
   unsigned char waveform[MAXSAMPLES];
   int nsamples;//declared as global so we may use it inside the interrupt
   const int slaveSelectPin = 7;//Note that timer interrupt seems to
   const int chipEnablePin = 8;//Chip enable pin for the RFM70 device

   void spiCSN(unsigned char CSbit);
   void chipEnableWrapper(unsigned char CEbit);
   void readRegister(unsigned char registerAddress,unsigned char *pregisterData);
   void writeRegister(unsigned char registerAddress,char data);
   void writeTXPayload(unsigned char data);
   void flushTX(void);
   void flushRX(void);
   void start(int samplingrate);
   void stop(void);

void setup() {
  pinMode(slaveSelectPin, OUTPUT);
  pinMode(chipEnablePin, OUTPUT);
  // initialize serial communication:
  Serial.begin(38400);
   // initialize the LED pins:
  Serial.print(">");
  Serial.print(">");
  Serial.print("Hope RF RFM70 device Test Program, v 1.0\n");
  Serial.print("written by Napoleon Leoni, Spring 2013.\n");

  //Timer interrupt initialize
  Timer1.initialize(1000000);
  SPI.setBitOrder(MSBFIRST);//This is the defalt config for the mbed
  SPI.setDataMode(SPI_MODE0);//SPI mode 0 rising edge clk pol 0 phase 0
  SPI.setClockDivider(SPI_CLOCK_DIV2);//Clock divider, Arduino is working
                                       //with 16 MHx cloxk, so this gives 1 MHz SPI clock
                                       //note that logic levels are 0-5 volts, also note
                                       //slight difference data is kept normally high, while
                                       //on mbed is kept normally low.
  SPI.begin();
  spiCSN(1);//set to high

  //Setup this one as a PR_TX with No interrupts
  //For this we need to set the
  unsigned char dataholder;
  //First setup CONFIG register

  readRegister(CONFIG_ADDRESS,&CONFIG);
  CONFIG=CONFIG|MASK_MAX_RT|MASK_TX_DS|MASK_RX_DR|EN_CRC;
  writeRegister(CONFIG_ADDRESS,CONFIG);

  writeRegister(SETUP_AW_ADDRESS,AW);
  delay(20);
  readRegister(CONFIG_ADDRESS,&CONFIG);
   CONFIG=CONFIG|PWR_UP;
  writeRegister(CONFIG_ADDRESS,CONFIG);

  //Setup retransmission
  writeRegister(SETUP_RETR_ADDRESS,0);

  //clear possible interrupts
  readRegister(STATUS_ADDRESS,&STATUS);
  STATUS=STATUS|MAX_RT|TX_DS|RX_DR;
  writeRegister(STATUS_ADDRESS,STATUS);

  generateSine(1,5);
  delay(300);
  start(5);
  delay(3000);
  stop();
}

void loop() {
  // check the serial input
  //note that the processcommand function is blocking!!
  processCommand();//this function parses a command
  //delay(1000);
  //Serial.print("Sending data\n>");
  //writeTXPayload(0x20);
  //readRegister(STATUS_ADDRESS,&STATUS);
  //sendDataRFM70();

}

void processCommand(){
  char str[MAXCOMMANDLENGTH]="",temp[MAXCOMMANDLENGTH];
  char* strCommands[]={"placeholder","placeholder","placeholder","placeholder","placeholder"};
  char **myCommand = strCommands;
  char delim[]="-";
  char *pch,*pStr;
  pStr=str;
  int i=0,j=0;
  if (Serial.available() > 0){
     do{
       if(Serial.available() >0 ){
	str[i]=Serial.read();
        Serial.print(str[i]);
	i=i+1;
         }
     } while(str[i-1]!='\n');
  } else {
    return;
  }

  Serial.print(">");

  str[i-1]='\0';
  spaceEater(pStr);
  pch = strtok (str,delim);
  strCommands[j]=pch;
          while (pch != NULL && j<4)
        	{
#ifdef DEBUG
		    Serial.print(strCommands[j]);
                    Serial.print("\n>");
#endif
        	pch = strtok (NULL, delim);
                  j++;
                  strCommands[j]=pch;
        	}
                parseCommand(strCommands,j);

}

void spaceEater(char *fatString){
	char slimString[MAXCOMMANDLENGTH];
	char *slimp;
	int i=0;
	slimp=slimString;
	while(*(fatString+i*sizeof(char))!='\0'){
		if(*(fatString+i*sizeof(char))!=' '){
			*slimp=*(fatString+i*sizeof(char));
			slimp++;
		}
		i++;
	}
	*slimp='\0';
#ifdef DEBUG
	Serial.print(slimString);
#endif
	slimp=slimString;
	while(*slimp!='\0'){
			*fatString=*slimp;
			slimp++;
			fatString++;
	}
	*fatString='\0';
	return;
}

void parseCommand(char **myCommand,int nArguments){
//This is here the action is here generate different types of waves
//walk across the command parts using the argument number
int i=1;
  int frequency=DEFAULTFREQ;
   long interruptPeriod;
  unsigned char address=0,data=0; //default addresss to read from

  if(strcmp(myCommand[0],"sine")==0){
  while(i<nArguments){
   char *pComm=myCommand[i];
   if(*pComm=='f'){ frequency=atoi(pComm+1); }
   if(*pComm=='s'){ samplingrate=atoi(pComm+1); }
   i++;
  }
  //Generate waveform table here
  Serial.println("Sine Waveform selected:");
  Serial.print(">Frequency=");
  Serial.println(frequency);
  Serial.print(">Sampling Rate=");
  Serial.println(samplingrate);
  Serial.println(">");
  generateSine(frequency,samplingrate);
//  } else if(){//process square wave here

  } else if(strcmp(myCommand[0],"start")==0){//process the start here
    start(samplingrate);
  } else if(strcmp(myCommand[0],"stop")==0){//process the stop command here
    stop();
  } else if(strcmp(myCommand[0],"read")==0){//process the stop command here

   while(i<nArguments){
   char *pComm=myCommand[i];
   if(*pComm=='a'){ address=atoi(pComm+1); }
   i++;
  }
  readRegister(address,&data);
  Serial.print("Reading From register:");
  Serial.println(address,HEX);
  Serial.print(">Data read:");
  Serial.print(data,HEX);
  Serial.print("\n>");

 } else if(strcmp(myCommand[0],"write")==0){//process the stop command here

   while(i<nArguments){
   char *pComm=myCommand[i];
   if(*pComm=='a'){ address=atoi(pComm+1); }
   if(*pComm=='d'){ data=atoi(pComm+1); }
   i++;
  }
  writeRegister(address,data);
  Serial.print("Write to register:");
  Serial.println(address,HEX);
  Serial.print(">Data:");
  Serial.print(data,HEX);
  Serial.print("\n>");

 } else if(strcmp(myCommand[0],"writeTX")==0){//process the stop command here

   while(i<nArguments){
   char *pComm=myCommand[i];
   if(*pComm=='d'){ data=atoi(pComm+1); }
   i++;
  }
  writeTXPayload(data);
  Serial.print("Write to TX FIFO:");
  Serial.println(data,HEX);
  Serial.print("\n>");

 } else if(strcmp(myCommand[0],"flush")==0){//process the stop command here


  flushTX();
  flushRX();
  Serial.print("flush TX and RX FIFO:");
  Serial.print("\n>");

 } else if(strcmp(myCommand[0],"send")==0){//process the stop command here

   sendDataRFM70();
  Serial.println("sending data...");

  } else {//unrecognized command processing here
    Serial.print("Unrecognized command entered");
  }
}

void timer1ISR(){
    // Toggle LED
    //digitalWrite( 13, digitalRead( 13 ) ^ 1 );
    data2=waveform[globalCounter];
    writeTXPayload(data2);
    //delayMicroseconds(50);
    sendDataRFM70();
    //Serial.println(data2);
    globalCounter++;
    if(globalCounter>(nsamples-1)) globalCounter=0;

}

void generateSine(int frequency,int samplingrate){
  nsamples= (int)floor((float)samplingrate/(float)frequency);
  if (nsamples>MAXSAMPLES){
    Serial.print("Nsamples exceeded maximum of:");
    Serial.println(MAXSAMPLES);
    Serial.print("\n>");
    int frequency=DEFAULTFREQ; //reestablish defaults
    int samplingrate=DEFAULTSAMPL;
    return;
  }
  int i=0;
  for(i=0;i<nsamples;i++){
      waveform[i]=floor(127*(sin((float)i/(float)nsamples*2.0*3.1416)+1.0));
      Serial.print(waveform[i]);
      Serial.print(",");
  }
  Serial.print("\n>");
  globalCounter=0;
}

void spiCSN(unsigned char CSbit){//Function to set CS high and low for SPI interface
  if(CSbit==0){
    digitalWrite(slaveSelectPin, LOW);
  } else if(CSbit==1){
    digitalWrite(slaveSelectPin, HIGH);
  }
}

void chipEnable(unsigned char CEbit){//Function to set CS high and low for SPI interface
  if(CEbit==0){
    digitalWrite(chipEnablePin, LOW);
  } else if(CEbit==1){
    digitalWrite(chipEnablePin, HIGH);
  }
}

void readRegister(unsigned char registerAddress,unsigned char *pregisterData){
	unsigned char registerValue=0;
	unsigned char command=0x1F; 		//From the Data sheet the 000A AAAA Read command and status registers. AAAAA =
						   //5 bit Register Map Address, For reading a register the data sent is ignored
						   //still two SPI writes are needed the status register is returned on the first one and
						   //the returned DATA is 1 to 5 bytes, LSB byte first. Set all address bits high: 0b00011111=0x1F
	command=command&registerAddress;

	spiCSN(0);
	SPI.transfer(command);
	*pregisterData=SPI.transfer(command); //check number of bytes at the register address and iterate
																				//sheet CS should be brought low only once at the beginning of the sequence
																					//and then pulled up at the end. So we need our class to have control of that as well
	spiCSN(1);
	return;
}

void writeRegister(unsigned char registerAddress,char data){
   unsigned char command=0x20;        //From the Data sheet the 001A AAAA Read command and status registers. AAAAA =
                           //5 bit Register Map Address, For reading a register the data sent is ignored
                           //still two SPI writes are needed the status register is returned on the first SPI
                           //cycle which carries the command, the second SPI cycles sends the data.
    command=command|registerAddress;

    spiCSN(0);
    SPI.transfer(command);
    SPI.transfer(data); //check number of bytes at the register address and iterate
                                                                                  //sheet CS should be brought low only once at the beginning of the sequence
                                                                                    //and then pulled up at the end. So we need our class to have control of that as well
    spiCSN(1);
    return;
}

void writeTXPayload(unsigned char data){//for now we are only transmitting 1 byte, valid for PR_TX only
    unsigned char command=0xA0;

    spiCSN(0);
    SPI.transfer(command);
    SPI.transfer(data);
    spiCSN(1);
    return;

}

void readRXFIFO(unsigned char *data){//for now we are only transmitting 1 byte, valid for PR_TX only
    unsigned char command=0x61;

    spiCSN(0);
    SPI.transfer(command);
    *data=SPI.transfer(command);
    spiCSN(1);
    return;

}

void sendDataRFM70(){
  unsigned char data;
  chipEnable(1);
  delayMicroseconds(20);//as per data sheet at least a 10 us pulse is needed
  //delay(1);
  chipEnable(0);
 //clear the status register
 delayMicroseconds(100);
 //Read FIFO to clearit
 readRXFIFO(&data);

 readRegister(STATUS_ADDRESS,&STATUS);

   while((STATUS&STATUSMASK)!=0){
     writeRegister(STATUS_ADDRESS,STATUS);
     readRegister(STATUS_ADDRESS,&STATUS);
   }
}

void flushTX(void){//for now we are only transmitting 1 byte, valid for PR_TX only
    unsigned char command=0xE1;

    spiCSN(0);
    SPI.transfer(command);
    spiCSN(1);
    return;

}

void flushRX(void){//for now we are only transmitting 1 byte, valid for PR_TX only
    unsigned char command=0xE2;

    spiCSN(0);
    SPI.transfer(command);
    spiCSN(1);
    return;

}

void start(int samplingrate){
     long interruptPeriod;
     Serial.print("In the start function, called interrupt with period:");
     interruptPeriod=(long)(1.0/((float)samplingrate)*1000000.0); //interrupt period in us
     Serial.println(interruptPeriod);
     Serial.print("\n>");
     delay(500);
     Timer1.attachInterrupt(timer1ISR, interruptPeriod);

}

void stop(void){
    //detach interrupt.
    Timer1.detachInterrupt();
    writeTXPayload(0);
    sendDataRFM70();
    flushTX();
}
