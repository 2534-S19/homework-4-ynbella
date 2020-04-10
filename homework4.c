#include <ti/devices/msp432p4xx/driverlib/driverlib.h>
#include "homework4.h"

int main(void) {
	char rChar; // Stores most recently entered character

	// Response to display when correct character sequence detected
	char *response = "\n\n\r2534 is the best course in the curriculum!\r\n\n";

	// Declare the variables that main uses to interact with your state machine.
	bool finished = false;

	// Stops the Watchdog timer.
	initBoard();

	// Declare a UART config struct as defined in uart.h.
	//       To begin, configure the UART for 9600 baud, 8-bit payload (LSB first), no parity, 1 stop bit.
	eUSCI_UART_ConfigV1 config =
			{
					EUSCI_A_UART_CLOCKSOURCE_SMCLK,                // selectClockSource
					19,                                            // clockPrescalar
					8,                                             // firstModReg
					0x55,                                          // secondModReg
					EUSCI_A_UART_NO_PARITY,                        //  parity
					EUSCI_A_UART_LSB_FIRST,                        // msborLsbFirst
					EUSCI_A_UART_ONE_STOP_BIT,                     // numberofStopBits
					EUSCI_A_UART_MODE,                             // uartMode
					EUSCI_A_UART_OVERSAMPLING_BAUDRATE_GENERATION, // overSampling
					EUSCI_A_UART_8_BIT_LEN                         // dataLength
			};

	// Make sure Tx AND Rx pins of EUSCI_A0 work for UART and not as regular GPIO pins.
	GPIO_setAsPeripheralModuleFunctionInputPin(GPIO_PORT_P1, GPIO_PIN2, GPIO_PRIMARY_MODULE_FUNCTION);
	GPIO_setAsPeripheralModuleFunctionOutputPin(GPIO_PORT_P1, GPIO_PIN3, GPIO_PRIMARY_MODULE_FUNCTION);

	// Initialize EUSCI_A0
	UART_initModule(EUSCI_A0_BASE, &config);

	// Enable EUSCI_A0
	UART_enableModule(EUSCI_A0_BASE);

	while(1) {
		// Check the receive interrupt flag to see if a received character is available.
		//       Return 0xFF if no character is available.
		rChar = UART_getInterruptStatus(EUSCI_A0_BASE, EUSCI_A_UART_RECEIVE_INTERRUPT_FLAG) ==
				EUSCI_A_UART_RECEIVE_INTERRUPT_FLAG ? UART_receiveData(EUSCI_A0_BASE) : 0xFF;

		// If an actual character was received, echo the character to the terminal AND use it to update the FSM.
		//       Check the transmit interrupt flag prior to transmitting the character.
		if(rChar != 0xFF) {
			UART_transmitData(EUSCI_A0_BASE, rChar);
			finished = charFSM(rChar);
		}

		// If the FSM indicates a successful string entry, transmit the response string.
		//       Check the transmit interrupt flag prior to transmitting each character and moving on to the next one.
		//       Make sure to reset the success variable after transmission.
		if(finished) {
			int i = 0;

			// Attempt to send all characters of response string while transmit interrupt flag is not active
			while(response[i] != '\0') {
				if(UART_getInterruptStatus(EUSCI_A0_BASE, EUSCI_A_UART_TRANSMIT_INTERRUPT_FLAG) !=
				   EUSCI_A_UART_TRANSMIT_INTERRUPT_FLAG) {
					UART_transmitData(EUSCI_A0_BASE, response[i]);
					i++;
				}
			}
			finished = false; // Reset the value of finished when found
		}
	}
}

// Initializes board
void initBoard() {
	WDT_A_hold(WDT_A_BASE);
}

// FSM for detecting character sequence.
bool charFSM(char rChar) {
	// Instantiate and declare an enumeration for storing the finite-state
	// machine's current state
	static enum State currentState = FIRST;

	bool finished = false; // Variable for storing return value of function

	// Switch based on current state
	switch(currentState) {
		case FIRST:
			currentState = rChar == '2' ? SECOND : FIRST;
			break;
		case SECOND:
			currentState = rChar == '5' ? THIRD : rChar == '2' ? SECOND : FIRST;
			break;
		case THIRD:
			currentState = rChar == '3' ? FOURTH : rChar == '2' ? SECOND : FIRST;
			break;
		case FOURTH:
            currentState = rChar == '2' ? SECOND : FIRST;
			finished = rChar == '4' ? true : false;
			break;
		default:
			currentState = FIRST;
			break;
	}
	return finished;
}
