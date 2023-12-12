/********************************************
 *	Project: 	IMP project					*
 * 	Author:		Adam Ližièiar (xlizic00)	*
 ********************************************/

#include "main.h"


/*
 * 	Initialize the MCU - basic clock settings, turning the watchdog off
 */
void MCUInit(void) {

    MCG_C4       |= ( MCG_C4_DMX32_MASK | MCG_C4_DRST_DRS(0x01) );
    SIM_CLKDIV1  |= SIM_CLKDIV1_OUTDIV1(0x00);
    WDOG_STCTRLH &= ~WDOG_STCTRLH_WDOGEN_MASK;

}


/*
 * 	Initializes NVIC settings for PORTA
 */
void initNVICForPORTA() {

    NVIC_ClearPendingIRQ(PORTA_IRQn);
    NVIC_EnableIRQ(PORTA_IRQn);

}


/*
 * 	Initializes the ports and configures pin settings
 */
void PinsInit(void) {

    // Turn on all port clocks
    SIM->SCGC5 = SIM_SCGC5_PORTA_MASK | SIM_SCGC5_PORTD_MASK;

    // Configure PORTA pin 25
    PORTA->PCR[25] = PORT_PCR_ISF(0x01) | PORT_PCR_IRQC(0x0A) | PORT_PCR_MUX(0x01) | PORT_PCR_PE(0x01) | PORT_PCR_PS(0x01);

    // Initialize NVIC for PORTA
    initNVICForPORTA();

    // Configure remaining PORTA pins
    const int portA_pins[] = {6, 7, 8, 9, 10, 11, 24};
    for (int i = 0; i < sizeof(portA_pins)/sizeof(portA_pins[0]); i++) {
        PORTA->PCR[portA_pins[i]] = PORT_PCR_MUX(0x01);
    }

    // Configure PORTD pins
    const int portD_pins[] = {8, 9, 12, 13, 14, 15};
    for (int i = 0; i < sizeof(portD_pins)/sizeof(portD_pins[0]); i++) {
        PORTD->PCR[portD_pins[i]] = PORT_PCR_MUX(0x01);
    }

    // Change corresponding PTA/PTD port pins as outputs
    PTA->PDDR = GPIO_PDDR_PDD(0x1FC0 | PIN_SEND);
    PTD->PDDR = GPIO_PDDR_PDD(0xF300);

    // Clear PIN_SEND
    PTA->PDOR &= ~PIN_SEND;

}


/*
 * 	Activate a specific number
 */
void activateDigit(int index) {

    // Clear all digit control bits
    PTD->PDOR |= (NUM_FIRST | NUM_SECOND | NUM_THIRD | NUM_FOURTH);

    // Activate only the required digit
    switch(index) {
        case 0: PTD->PDOR &= ~NUM_FIRST; break;
        case 1: PTD->PDOR &= ~NUM_FOURTH; break;
        case 2: PTD->PDOR &= ~NUM_THIRD; break;
        case 3: PTD->PDOR &= ~NUM_SECOND; break;
        default: break;
    }

}


/*
 * 	Clears all segments of the display
 */
void writeNoneOnDisplay() {

    PTA->PDOR &= ~(LINE_LEFT_DOWN | LINE_LEFT_UP | LINE_DOT | LINE_RIGHT_UP | LINE_BOTTOM | LINE_UP);
    PTD->PDOR &= ~(LINE_RIGHT_DOWN | LINE_CENTER);

}


/*
 * 	Write number on display
 */
void writeDigitOnDisplay(int number) {

	writeNoneOnDisplay();

	switch(number) {
	    case 0:
	        PTA->PDOR |= LINE_UP | LINE_RIGHT_UP | LINE_LEFT_DOWN | LINE_LEFT_UP | LINE_BOTTOM;
	        PTD->PDOR |= LINE_RIGHT_DOWN;
	        break;
	    case 1:
	        PTA->PDOR |= LINE_RIGHT_UP;
	        PTD->PDOR |= LINE_RIGHT_DOWN;
	        break;
	    case 2:
	        PTA->PDOR |= LINE_UP | LINE_RIGHT_UP | LINE_BOTTOM | LINE_LEFT_DOWN;
	        PTD->PDOR |= LINE_CENTER;
	        break;
	    case 3:
	        PTA->PDOR |= LINE_UP | LINE_RIGHT_UP | LINE_BOTTOM;
	        PTD->PDOR |= LINE_CENTER | LINE_RIGHT_DOWN;
	        break;
	    case 4:
	        PTA->PDOR |= LINE_LEFT_UP | LINE_RIGHT_UP;
	        PTD->PDOR |= LINE_CENTER | LINE_RIGHT_DOWN;
	        break;
	    case 5:
	        PTA->PDOR |= LINE_UP | LINE_LEFT_UP | LINE_BOTTOM;
	        PTD->PDOR |= LINE_CENTER | LINE_RIGHT_DOWN;
	        break;
	    case 6:
	        PTA->PDOR |= LINE_UP | LINE_LEFT_UP | LINE_LEFT_DOWN | LINE_BOTTOM;
	        PTD->PDOR |= LINE_CENTER | LINE_RIGHT_DOWN;
	        break;
	    case 7:
	        PTA->PDOR |= LINE_UP | LINE_RIGHT_UP;
	        PTD->PDOR |= LINE_RIGHT_DOWN;
	        break;
	    case 8:
	        PTA->PDOR |= LINE_UP | LINE_RIGHT_UP | LINE_LEFT_UP | LINE_LEFT_DOWN | LINE_BOTTOM;
	        PTD->PDOR |= LINE_CENTER | LINE_RIGHT_DOWN;
	        break;
	    case 9:
	        PTA->PDOR |= LINE_UP | LINE_RIGHT_UP | LINE_LEFT_UP | LINE_BOTTOM;
	        PTD->PDOR |= LINE_CENTER | LINE_RIGHT_DOWN;
	        break;
	    default:
	        break;
	}

}


/*
 * 	Converts a floating-point number to a string without decimal part
 */
void convertStringToFloat(char* s, float f) {

	int intPart = (int)f; // Extract integer part
	sprintf(s, "%d", intPart); // Format as integer string

}


/*
 * 	Checks if a value is within the displayable range
 */
int isValueDisplayable(float val) {

    return val >= 0. && val <= 9999.;

}


/*
 * 	Determines the position to start displaying the number for right alignment
 */
int calculateDisplayStart(int strLength) {

    return 4 - strLength; // 4 is the display width

}


/*
 * 	Checks if a character is a digit
 */
int isDigit(char ch) {

    return ch >= '0' && ch <= '9';

}


/*
 * 	Write single digit
 */
void displayDigit(char digit, int position) {

    if (isDigit(digit)) {
    	activateDigit(position);
        writeDigitOnDisplay(digit - '0'); // Convert char to integer and display
    }
    else {
        writeNoneOnDisplay(); // Display nothing for non-digit characters
    }

}


/*
 * 	Write numbers
 */
void writeOnDisplay(float val) {

    if (!isValueDisplayable(val)) {
        writeNoneOnDisplay(); // Handle unprintable value
        return;
    }

    char valStr[10] = {}; // String to hold the integer value
    convertStringToFloat(valStr, val); // Convert float to string representation

    int strLength = strlen(valStr);
    int startPosOfDisplay = calculateDisplayStart(strLength);

    if (numOfDigitOnDisplay < startPosOfDisplay) {
        writeNoneOnDisplay(); // Display nothing if index is before the start of the number
    }
    else {
        int positionOfChar = numOfDigitOnDisplay - startPosOfDisplay;
        if (positionOfChar < strLength)
            displayDigit(valStr[positionOfChar], numOfDigitOnDisplay);
        else
            writeNoneOnDisplay(); // Display nothing if beyond the number length
    }

}


/*
 * 	Handler for PORTA_IRQ
 */
void PORTA_IRQHandler(void) {

    if ((PORTA->ISFR & PIN_RECEIVE) && !(GPIOA_PDIR & PIN_RECEIVE)) {
        sensorMeasuresDistance = 0; // Disable waiting for echo
        PORTA->ISFR = ~0;           // Reset interrLINE_UPt flag
    }

}


/*
 * 	Handler for LPTMR0_IRQ
 */
void LPTMR0_IRQHandler(void) {

    LPTMR0_CMR = 0x200 / 100;
    LPTMR0_CSR |= LPTMR_CSR_TCF_MASK;

    if (numOfDigitOnDisplay < 0)
        numOfDigitOnDisplay = 3; // Reset display index

    writeOnDisplay(distance); // Print on the display
    numOfDigitOnDisplay--; // Move to the next position

}


/*
 * 	Enable the Low Power Timer clock and disable the timer for setLINE_UP
 */
void SetLINE_UPLPTMR0() {

    SIM_SCGC5 |= SIM_SCGC5_LPTIMER_MASK; // Enable LPTMR clock
    LPTMR0_CSR &= ~LPTMR_CSR_TEN_MASK;   // Disable LPTMR

}


/*
 * 	Configure the prescaler and clock source for the timer
 */
void ConfigureTimer(int count) {

    LPTMR0_PSR = (LPTMR_PSR_PRESCALE(0) // Prescaler: divide by 2
                 | LPTMR_PSR_PBYP_MASK  // Prescaler bypassed
                 | LPTMR_PSR_PCS(1));   // Clock source selection
    LPTMR0_CMR = count;                 // Set compare value

}


/*
 * 	Enable the timer and its interrLINE_UPt, and start the timer
 */
void EnableAndStartTimer() {

    LPTMR0_CSR = (LPTMR_CSR_TCF_MASK    // Clear pending interrLINE_UPt
                 | LPTMR_CSR_TIE_MASK); // Enable interrLINE_UPt
    NVIC_EnableIRQ(LPTMR0_IRQn);        // Enable IRQ for the timer
    LPTMR0_CSR |= LPTMR_CSR_TEN_MASK;   // Enable and start timer

}


/*
 * Initialize the LPTMR0 timer with a given count value
 */
void LPTMR0Init(int count) {

    SetLINE_UPLPTMR0();          // SetLINE_UP the timer
    ConfigureTimer(count);  // Configure the timer with the count
    EnableAndStartTimer();  // Enable and start the timer

}


/*
 * 	Initialize
 */
void Init(int count) {

	MCUInit();
	PinsInit();
	LPTMR0Init(count);

}


/*
 *	Delay
 */
void delay(long long time) {

  long long i;
  for(i = 0; i < time; i++) {}

}


/*
 * 	Main function
 */
int main(void) {

	Init(0x200);

    while (1) {

		// Reset variables
		sensorMeasuresDistance = 1;
		senzorDistance = 0;

    	// Send signal
    	PTA->PDOR |= PIN_SEND;
    	delay(50);
    	PTA->PDOR &= ~PIN_SEND;

		// Receive signal
		while (sensorMeasuresDistance)
			senzorDistance++;

		// Transform sensor response to centimeters
		distance = (senzorDistance * 0.36) / 58;


		delay(500000);

    }

    return 0;

}
