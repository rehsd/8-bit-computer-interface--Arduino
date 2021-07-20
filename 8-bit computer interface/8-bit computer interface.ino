/*
 Name:				8-bit-computer-interface.ino
 Created:			19 July 2021
 Author:			rehsd
 Version:			0.02
 Target Arduino:	Mega 2560
 
 *** Much of this code can be improved for better efficiency, adherence to coding standards,
 general elegance, etc. This version is the initial prototype.

 ***********************************************************
 *** This code is provided as-is. Use at your own risk! ****
 ***********************************************************
 
 Reference wiring diagram. Ensure Arduino ground is connected to ground on the 8-bit computer.
 
 To use the board setup (programming of RAM contents):
 -Set clock mode switch to manual
 -Set memory program mode switch to enable
 -Set RAM value dip switches to UP (on)
 -Set RAM address value dip switches to UP (up)

 To use the monitor:
 -Set clock mode switch to automatic
 -Set memory program mode switch to dsiable

 If using an additional Arduino Nano to capture the board clock, code for that Nano is at that bottom of this page.
*/

#include<Wire.h>

#define PIN_BUS1 23							//PIN1 (far left) of Bus
#define PIN_BUS2 25
#define PIN_BUS3 27
#define PIN_BUS4 29
#define PIN_BUS5 31
#define PIN_BUS6 33
#define PIN_BUS7 35
#define PIN_BUS8 37

#define PIN_CONTROL_HALT 52					//Halt clock
#define PIN_CONTROL_MEMORY_ADDRESS_IN 24	//Memory address register IN
#define PIN_CONTROL_RAM_IN 26				//RAM data IN
#define PIN_CONTROL_RAM_OUT 28				//RAM data OUT
#define PIN_CONTROL_INSTRUCTION_OUT 30		//Instruction register OUT
#define PIN_CONTROL_INSTRUCTION_IN 32		//Instruction register IN
#define PIN_CONTROL_A_REGISTER_IN 34		//A register IN
#define PIN_CONTROL_A_REGISTER_OUT 36		//A register out
#define PIN_CONTROL_SUM_OUT 38				//ALU OUT
#define PIN_CONTROL_SUBTRACT 40				//ALU Subtract
#define PIN_CONTROL_B_INSTRUCTION_IN 42		//B register IN
#define PIN_CONTROL_OUTPUT_IN 39			//Output register IN
#define PIN_CONTROL_COUNTER_ENABLE 41		//Program counter ENABLE
#define PIN_CONTROL_COUNTER_OUT 43			//Program counter OUT
#define PIN_CONTROL_JUMP 45					//Jump
#define PIN_CONTROL_FLAGS 22				//Flags

#define PIN_LED_OUT_128	A7	//9					//Value 128 for output LED segment display
#define PIN_LED_OUT_64	A6	//8					//Value 64 for output LED segment display
#define PIN_LED_OUT_32	A5	//7					//Value 32 for output LED segment display
#define PIN_LED_OUT_16	A4	//6					//Value 16 for output LED segment display
#define PIN_LED_OUT_8	A3	//5					//Value 8 for output LED segment display
#define PIN_LED_OUT_4	A2	//4					//Value 4 for output LED segment display
#define PIN_LED_OUT_2	A1	//3					//Value 2 for output LED segment display
#define PIN_LED_OUT_1	A0	//2					//Value 1 for output LED segment display

#define PIN_SET_RAM		A15					//Program RAM button
#define PIN_RAM_128		44					//Pins for setting bits for writing to RAM
#define PIN_RAM_64		46			
#define PIN_RAM_32		48
#define PIN_RAM_16		50
#define PIN_RAM_8		10
#define PIN_RAM_4		11
#define PIN_RAM_2		12
#define PIN_RAM_1		13
#define PIN_RAM_ADDR_8	47					//Pins for setting address of RAM to write to (dec 0-15; 16 locations)
#define PIN_RAM_ADDR_4	49
#define PIN_RAM_ADDR_2	51
#define PIN_RAM_ADDR_1	53

#define PIN_CLOCK		2					//Pin for clock from 555 IC
#define PIN_LOAD_DEFAULTS	3				//Pin for button interrupts to load default program

const String COMMAND_LDA	= "0001";
const String COMMAND_ADD	= "0010";
const String COMMAND_SUB	= "0011";
const String COMMAND_STA	= "0100";
const String COMMAND_LDI	= "0101";
const String COMMAND_JMP	= "0110";
const String COMMAND_JC		= "0111";
const String COMMAND_JZ		= "1000";
const String COMMAND_NOOP1	= "1001";
const String COMMAND_NOOP2	= "1010";
const String COMMAND_NOOP3	= "1011";
const String COMMAND_NOOP4	= "1100";
const String COMMAND_NOOP5	= "1101";
const String COMMAND_OUT	= "1110";
const String COMMAND_HLT	= "1111";

int valBus1 = 0;
int valBus2 = 0;
int valBus3 = 0;
int valBus4 = 0;
int valBus5 = 0;
int valBus6 = 0;
int valBus7 = 0;
int valBus8 = 0;

int valControlHalt = 0;
int valControlMemoryAddressIn = 0;
int valControlRamIn = 0;
int valControlRamOut = 0;
int valControlInstructionOut = 0;
int valControlInstructionIn = 0;
int valControlARegisterIn = 0;
int valControlARegisterOut = 0;
int valControlSumOut = 0;
int valControlSubtract = 0;
int valControlBRegisterIn = 0;
int valControlOutputIn = 0;
int valControlCounterEnable = 0;
int valControlCounterOut = 0;
int valControlJump = 0;
int valControlFlags = 0;

int valLEDout128 = 0;
int valLEDout64 = 0;
int valLEDout32 = 0;
int valLEDout16 = 0;
int valLEDout8 = 0;
int valLEDout4 = 0;
int valLEDout2 = 0;
int valLEDout1 = 0;
volatile char outputString[8]{'0','0','0','0','0','0','0','0'};

int valBus1_prev = 0;
int valBus2_prev = 0;
int valBus3_prev = 0;
int valBus4_prev = 0;
int valBus5_prev = 0;
int valBus6_prev = 0;
int valBus7_prev = 0;
int valBus8_prev = 0;

int valControlHalt_prev = 0;
int valControlMemoryAddressIn_prev = 0;
int valControlRamIn_prev = 0;
int valControlRamOut_prev = 0;
int valControlInstructionOut_prev = 0;
int valControlInstructionIn_prev = 0;
int valControlARegisterIn_prev = 0;
int valControlARegisterOut_prev = 0;
int valControlSumOut_prev = 0;
int valControlSubtract_prev = 0;
int valControlBRegisterIn_prev = 0;
int valControlOutputIn_prev = 0;
int valControlCounterEnable_prev = 0;
int valControlCounterOut_prev = 0;
int valControlJump_prev = 0;
int valControlFlags_prev = 0;

int valLEDout128_prev = 0;
int valLEDout64_prev = 0;
int valLEDout32_prev = 0;
int valLEDout16_prev = 0;
int valLEDout8_prev = 0;
int valLEDout4_prev = 0;
int valLEDout2_prev = 0;
int valLEDout1_prev = 0;

String setRAMCustomNewVals = "";

bool bMonitor = false;
volatile long int currentClockSpeed = 0;

void receiveEvent(int bytes)
{
	//Serial.print('\n' + bytes + '\n');
	//Serial.print(":");
	//Used to receive clock data on SCL/SDA

	//Serial.print("\tevent\t");
	byte a[2];
	int sumBoth;
	Wire.readBytes(a, 2);
	//b = Wire.read();

	sumBoth = a[0];
	sumBoth = (sumBoth << 8) | a[1];

	//Serial.print(sumBoth);

	currentClockSpeed = sumBoth;
	//Serial.println(currentClockSpeed);

}

void buttonLoadDefaults_ISR()
{
	static unsigned long last_interrupt_time = 0;
	unsigned long interrupt_time = millis();
	// If interrupts come faster than 200ms, assume it's a bounce and ignore
	if (interrupt_time - last_interrupt_time > 1000)
	{
		setRAM();
	}
	last_interrupt_time = interrupt_time;
	
}
void setup() {
	//Serial.println("\n\nLoading...\n");
	
	pinMode(PIN_BUS1, INPUT);
	pinMode(PIN_BUS2, INPUT);
	pinMode(PIN_BUS3, INPUT);
	pinMode(PIN_BUS4, INPUT);
	pinMode(PIN_BUS5, INPUT);
	pinMode(PIN_BUS6, INPUT);
	pinMode(PIN_BUS7, INPUT);
	pinMode(PIN_BUS8, INPUT);

	pinMode(PIN_CONTROL_HALT, INPUT);
	pinMode(PIN_CONTROL_MEMORY_ADDRESS_IN, INPUT);
	pinMode(PIN_CONTROL_RAM_IN, INPUT);
	pinMode(PIN_CONTROL_RAM_OUT, INPUT);
	pinMode(PIN_CONTROL_INSTRUCTION_OUT, INPUT);
	pinMode(PIN_CONTROL_INSTRUCTION_IN, INPUT);
	pinMode(PIN_CONTROL_A_REGISTER_IN, INPUT);
	pinMode(PIN_CONTROL_A_REGISTER_OUT, INPUT);
	pinMode(PIN_CONTROL_SUM_OUT, INPUT);
	pinMode(PIN_CONTROL_SUBTRACT, INPUT);
	pinMode(PIN_CONTROL_B_INSTRUCTION_IN, INPUT);
	pinMode(PIN_CONTROL_OUTPUT_IN, INPUT);
	pinMode(PIN_CONTROL_COUNTER_ENABLE, INPUT);
	pinMode(PIN_CONTROL_COUNTER_OUT, INPUT);
	pinMode(PIN_CONTROL_JUMP, INPUT);
	pinMode(PIN_CONTROL_FLAGS, INPUT);

	pinMode(PIN_LED_OUT_128, INPUT);
	pinMode(PIN_LED_OUT_64, INPUT);
	pinMode(PIN_LED_OUT_32, INPUT);
	pinMode(PIN_LED_OUT_16, INPUT);
	pinMode(PIN_LED_OUT_8, INPUT);
	pinMode(PIN_LED_OUT_4, INPUT);
	pinMode(PIN_LED_OUT_2, INPUT);
	pinMode(PIN_LED_OUT_1, INPUT);

	pinMode(PIN_RAM_128, OUTPUT);
	pinMode(PIN_RAM_64, OUTPUT);
	pinMode(PIN_RAM_32, OUTPUT);
	pinMode(PIN_RAM_16, OUTPUT);
	pinMode(PIN_RAM_8, OUTPUT);
	pinMode(PIN_RAM_4, OUTPUT);
	pinMode(PIN_RAM_2, OUTPUT);
	pinMode(PIN_RAM_1, OUTPUT);
	pinMode(PIN_RAM_ADDR_8, OUTPUT);
	pinMode(PIN_RAM_ADDR_4, OUTPUT);
	pinMode(PIN_RAM_ADDR_2, OUTPUT);
	pinMode(PIN_RAM_ADDR_1, OUTPUT);
	pinMode(PIN_SET_RAM, OUTPUT);			//to physical set RAM button on board

	pinMode(PIN_CLOCK, INPUT);
	pinMode(PIN_LOAD_DEFAULTS, INPUT_PULLUP);

	attachInterrupt(digitalPinToInterrupt(PIN_CLOCK), onClock, RISING);
	attachInterrupt(digitalPinToInterrupt(PIN_LOAD_DEFAULTS), buttonLoadDefaults_ISR, CHANGE);

	Serial.begin(115200);

	Wire.begin(9);
	Wire.onReceive(receiveEvent);
}




void loop() {
	String mySelection;
	Serial.write("Choose Action:\n(P) Program Defaults\n(C) Program Custom Values\n(M) Monitor Bus, Control, & Output\n>\n");
	while (Serial.available() == 0) {
		//sit and wait 
	}

	mySelection = Serial.readString();
	Serial.println("You selected '" + mySelection + "'");

	if (mySelection == "P" || mySelection == "p")
	{
		Serial.println("Populating memory with defaults...\n");
		setRAM();
		Serial.println("\nProgramming complete\n");
	}
	else if (mySelection == "C" || mySelection == "c")
	{
		Serial.println("Enter custom RAM contents string:");
		while (Serial.available() == 0) {
			//sit and wait 
		}
		setRAMCustomNewVals = Serial.readString();

		Serial.println("Populating memory with your custom values...\n");
		setRAMCustom();
		Serial.println("\nProgramming complete\n");
	}
	else if (mySelection == "M" || mySelection == "m")
	{
		Serial.println("Starting monitor...\n (Press 'x' and 'Enter' to exit monitor)\n");
		//bMonitor = true;

		while (Serial.available() == 0)
		{
			bMonitor = true;
		}
	}
	else if (mySelection == "X" || mySelection == "x")
	{
		bMonitor = false;
		Serial.println("Monitoring stopped");
	}
	else
	{
		Serial.println("Invalid choice");
	}
}


void onClock() {

	if (bMonitor)
	{
		valBus1 = digitalRead(PIN_BUS1);
		valBus2 = digitalRead(PIN_BUS2);
		valBus3 = digitalRead(PIN_BUS3);
		valBus4 = digitalRead(PIN_BUS4);
		valBus5 = digitalRead(PIN_BUS5);
		valBus6 = digitalRead(PIN_BUS6);
		valBus7 = digitalRead(PIN_BUS7);
		valBus8 = digitalRead(PIN_BUS8);

		valControlHalt = digitalRead(PIN_CONTROL_HALT);
		valControlMemoryAddressIn = digitalRead(PIN_CONTROL_MEMORY_ADDRESS_IN);
		valControlRamIn = digitalRead(PIN_CONTROL_RAM_IN);
		valControlRamOut = digitalRead(PIN_CONTROL_RAM_OUT);
		valControlInstructionOut = digitalRead(PIN_CONTROL_INSTRUCTION_OUT);
		valControlInstructionIn = digitalRead(PIN_CONTROL_INSTRUCTION_IN);
		valControlARegisterIn = digitalRead(PIN_CONTROL_A_REGISTER_IN);
		valControlARegisterOut = digitalRead(PIN_CONTROL_A_REGISTER_OUT);
		valControlSumOut = digitalRead(PIN_CONTROL_SUM_OUT);
		valControlSubtract = digitalRead(PIN_CONTROL_SUBTRACT);
		valControlBRegisterIn = digitalRead(PIN_CONTROL_B_INSTRUCTION_IN);
		valControlOutputIn = digitalRead(PIN_CONTROL_OUTPUT_IN);
		valControlCounterEnable = digitalRead(PIN_CONTROL_COUNTER_ENABLE);
		valControlCounterOut = digitalRead(PIN_CONTROL_COUNTER_OUT);
		valControlJump = digitalRead(PIN_CONTROL_JUMP);
		valControlFlags = digitalRead(PIN_CONTROL_FLAGS);

		valLEDout128 = digitalRead(PIN_LED_OUT_128);
		valLEDout64 = digitalRead(PIN_LED_OUT_64);
		valLEDout32 = digitalRead(PIN_LED_OUT_32);
		valLEDout16 = digitalRead(PIN_LED_OUT_16);
		valLEDout8 = digitalRead(PIN_LED_OUT_8);
		valLEDout4 = digitalRead(PIN_LED_OUT_4);
		valLEDout2 = digitalRead(PIN_LED_OUT_2);
		valLEDout1 = digitalRead(PIN_LED_OUT_1);

		getBinaryOutput();
		String outString;
		for (int i = 0; i < 8; i++) {
			outString += outputString[i];
		}


		char buffer[100];
		sprintf(buffer, "Bus:%d%d%d%d%d%d%d%d  Control:%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d  Out:%s [%d]  Clock:%d", valBus1, valBus2, valBus3, valBus4, valBus5, valBus6, valBus7, valBus8
			, valControlHalt, valControlMemoryAddressIn, valControlRamIn, valControlRamOut
			, valControlInstructionIn, valControlInstructionOut, valControlARegisterIn, valControlARegisterOut, valControlSumOut, valControlSubtract, valControlBRegisterIn
			, valControlOutputIn, valControlCounterEnable, valControlCounterOut, valControlJump, valControlFlags
			, outString.c_str()
			, getDecimalOutput()
			, (int)currentClockSpeed
		);

		Serial.println(buffer);

	}

}



void setRAMCustom()
{
	//16 bytes of data to be received as a single string
	//grab portion of string for each address
	String s0000 = setRAMCustomNewVals.substring(0, 8);
	String s0001 = setRAMCustomNewVals.substring(8, 16);
	String s0010 = setRAMCustomNewVals.substring(16, 24);
	String s0011 = setRAMCustomNewVals.substring(24, 32);
	String s0100 = setRAMCustomNewVals.substring(32, 40);
	String s0101 = setRAMCustomNewVals.substring(40, 48);
	String s0110 = setRAMCustomNewVals.substring(48, 56);
	String s0111 = setRAMCustomNewVals.substring(56, 64);
	String s1000 = setRAMCustomNewVals.substring(64, 72);
	String s1001 = setRAMCustomNewVals.substring(72, 80);
	String s1010 = setRAMCustomNewVals.substring(80, 88);
	String s1011 = setRAMCustomNewVals.substring(88, 96);
	String s1100 = setRAMCustomNewVals.substring(96, 104);
	String s1101 = setRAMCustomNewVals.substring(104, 112);
	String s1110 = setRAMCustomNewVals.substring(112, 120);
	String s1111 = setRAMCustomNewVals.substring(120, 128);

	
	setSingleRAM("0000", s0000);
	setSingleRAM("0001", s0001);
	setSingleRAM("0010", s0010);
	setSingleRAM("0011", s0011);
	setSingleRAM("0100", s0100);
	setSingleRAM("0101", s0101);
	setSingleRAM("0110", s0110);
	setSingleRAM("0111", s0111);
	setSingleRAM("1000", s1000);
	setSingleRAM("1001", s1001);
	setSingleRAM("1010", s1010);
	setSingleRAM("1011", s1011);
	setSingleRAM("1100", s1100);
	setSingleRAM("1101", s1101);
	setSingleRAM("1110", s1110);
	setSingleRAM("1111", s1111);
}

void setRAM() {
	//set values as you see appropriate
	//use to easily load a common set of values for testing
	//currently using Ben's example of counting up to 255 and back down, from the Conditional jump  instructions video
	setSingleRAM("0000", COMMAND_OUT + "0000");
	setSingleRAM("0001", COMMAND_ADD + "1111");
	setSingleRAM("0010", COMMAND_JC + "0100");
	setSingleRAM("0011", COMMAND_JMP + "0000");
	setSingleRAM("0100", COMMAND_SUB + "1111");
	setSingleRAM("0101", COMMAND_OUT + "0000");
	setSingleRAM("0110", COMMAND_JZ + "0000");
	setSingleRAM("0111", COMMAND_JMP + "0100");
	setSingleRAM("1000", COMMAND_NOOP1 + "0000");
	setSingleRAM("1001", COMMAND_NOOP1 + "0000");
	setSingleRAM("1010", COMMAND_NOOP1 + "0000");
	setSingleRAM("1011", COMMAND_NOOP1 + "0000");
	setSingleRAM("1100", COMMAND_NOOP1 + "0000");
	setSingleRAM("1101", COMMAND_NOOP1 + "0000");
	setSingleRAM("1110", COMMAND_NOOP1 + "0000");
	setSingleRAM("1111", "00000001");		//NoOp decimal 1			
}

void setSingleRAM(String RAMaddress, String RAMcontents) {

	//writes 8 bits to a RAM address
	Serial.print("setSingleRAM::");
	Serial.print(RAMaddress);
	Serial.print(":");
	Serial.print(RAMcontents);

	char bit8 = RAMaddress.charAt(0);
	char bit4 = RAMaddress.charAt(1);
	char bit2 = RAMaddress.charAt(2);
	char bit1 = RAMaddress.charAt(3);

	char val128 = RAMcontents[0];
	char val64 = RAMcontents[1];
	char val32 = RAMcontents[2];
	char val16 = RAMcontents[3];
	char val8 = RAMcontents[4];
	char val4 = RAMcontents[5];
	char val2 = RAMcontents[6];
	char val1 = RAMcontents[7];

	//set address
	if(bit8 == '0'){
		digitalWrite(PIN_RAM_ADDR_8, 0);
	}
	else
	{
		digitalWrite(PIN_RAM_ADDR_8, 255);
	}

	if (bit4 == '0') {
		digitalWrite(PIN_RAM_ADDR_4, 0);
	}
	else
	{
		digitalWrite(PIN_RAM_ADDR_4, 255);
	}

	if (bit2 == '0') {
		digitalWrite(PIN_RAM_ADDR_2, 0);
	}
	else
	{
		digitalWrite(PIN_RAM_ADDR_2, 255);
	}

	if (bit1 == '0') {
		digitalWrite(PIN_RAM_ADDR_1, 0);
	}
	else
	{
		digitalWrite(PIN_RAM_ADDR_1, 255);
	}

	//set contents
	if (val128 == '0') {
		digitalWrite(PIN_RAM_128, 0);
	}
	else
	{
		digitalWrite(PIN_RAM_128, 255);
	}

	if (val64 == '0') {
		digitalWrite(PIN_RAM_64, 0);
	}
	else
	{
		digitalWrite(PIN_RAM_64, 255);
	}

	if (val32 == '0') {
		digitalWrite(PIN_RAM_32, 0);
	}
	else
	{
		digitalWrite(PIN_RAM_32, 255);
	}

	if (val16 == '0') {
		digitalWrite(PIN_RAM_16, 0);
	}
	else
	{
		digitalWrite(PIN_RAM_16, 255);
	}

	if (val8 == '0') {
		digitalWrite(PIN_RAM_8, 0);
	}
	else
	{
		digitalWrite(PIN_RAM_8, 255);
	}

	if (val4 == '0') {
		digitalWrite(PIN_RAM_4, 0);
	}
	else
	{
		digitalWrite(PIN_RAM_4, 255);
	}

	if (val2 == '0') {
		digitalWrite(PIN_RAM_2, 0);
	}
	else
	{
		digitalWrite(PIN_RAM_2, 255);
	}

	if (val1 == '0') {
		digitalWrite(PIN_RAM_1, 0);
	}
	else
	{
		digitalWrite(PIN_RAM_1, 255);
	}

	//virtually 'push' the RAM write button on the board
	delay(5);
	analogWrite(PIN_SET_RAM, 0);
	delay(5);
	analogWrite(PIN_SET_RAM, 255);

	Serial.println("   ...complete!");
}

void getBinaryOutput()
{
	/*for (int i = 0; i < 8; i++) {
		outputString[i] = '0';
	}*/

	if (valLEDout128) { outputString[0] = '1'; }
	else { outputString[0] = '0'; }
	if (valLEDout64) { outputString[1] = '1'; }
	else { outputString[1] = '0'; }
	if (valLEDout32) { outputString[2] = '1'; }
	else { outputString[2] = '0'; }
	if (valLEDout16) { outputString[3] = '1'; }
	else { outputString[3] = '0'; }
	if (valLEDout8) { outputString[4] = '1'; }
	else { outputString[4] = '0'; }
	if (valLEDout4) { outputString[5] = '1'; }
	else { outputString[5] = '0'; }
	if (valLEDout2) { outputString[6] = '1'; }
	else { outputString[6] = '0'; }
	if (valLEDout1) { outputString[7] = '1'; }
	else { outputString[7] = '0'; }
}

int getDecimalOutput()
{
	int decimalOut = 0;
	if (valLEDout128) { decimalOut += 128; }
	if (valLEDout64) { decimalOut += 64; }
	if (valLEDout32) { decimalOut += 32; }
	if (valLEDout16) { decimalOut += 16; }
	if (valLEDout8) { decimalOut += 8; }
	if (valLEDout4) { decimalOut += 4; }
	if (valLEDout2) { decimalOut += 2; }
	if (valLEDout1) { decimalOut += 1; }
	return decimalOut;
}





/*
 //Sample code for separate Nano to capture clock and send to Mega


 
 //Name:		Clock_Speed.ino
 //Created:	7/19/2021 3:53:25 AM
 //Author:	rehsd
 //
 //Connect SCL and SDA between Mega and Nano.

#include<Wire.h>

unsigned int pulses = 0;
unsigned long lastMicros = 0;
volatile float currentClockSpeed = 0.0;


// the setup function runs once when you press reset or power the board
void setup() {
	pinMode(2, INPUT);	//clock
	attachInterrupt(digitalPinToInterrupt(2), onClock, RISING);
	Wire.begin();
}

// the loop function runs over and over again until power down or reset
void loop() {
	//Serial.println((int)currentClockSpeed);
	int cClock = (int)currentClockSpeed;
	Serial.print(cClock);
	Serial.print(".");
	byte myArray[2];
	myArray[0] = (cClock >> 8) & 0xFF;
	myArray[1] = cClock & 0xFF;

	Wire.beginTransmission(9);
	Wire.write(myArray, 2);
	Wire.endTransmission();
	delay(1000);
}

void onClock() {
	//Serial.println("clock...");
	pulses++;
	unsigned long currentMicros = micros();
	float timeLapsed = (currentMicros - lastMicros) / 1000000.0;
	//Serial.println(timeLapsed);
	if (timeLapsed > .1)
	{
		currentClockSpeed = pulses / timeLapsed;
		//Serial.println(currentClockSpeed);
		pulses = 0;
		lastMicros = currentMicros;
	}
}
*/