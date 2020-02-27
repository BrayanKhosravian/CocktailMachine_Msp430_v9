#include <msp430.h>
#include "Components/Bluetooth.h"
#include "Components/Platform.h"
#include "Components/Scale.h"

#if MSP430G2553
#include <msp430g2553.h>
#endif


#if MSP430f2274
#include <msp430f2274.h>
#endif


void setup();
void reset();

// global
static volatile unsigned int _routine = 0;
volatile unsigned int _bottlePosition = 0;
volatile int _milliliter = 0;
volatile bool _isBusy = false;

// bluetooth - global
static volatile bool _btDataReceived = false;
static volatile int _btParsedData[12];
void ClearBluetoothState();

// example
// _btParsedData[5] = {4,500,6,250,2,233,0,0,0,0,0,0};
// _btParsedData[5] = {1,50,2,50,3,50,4,50,5,50,6,50};

int main(void) {
    setup();

    while(true);

    while(true){
		if(_btDataReceived){
			for (_routine = 0; _routine <= sizeof(_btParsedData)/sizeof(int); )
			{
				if(_btParsedData[_routine] == 0)
				{
				    reset();
				    break;
				}

				// IMMER getränk position
				else if(_routine % 2 == 0)
				{
				   _bottlePosition = _btParsedData[_routine];
				   _isBusy = true;
				   Platform::EngineControl(Platform::EngineStatus::Forward);
				   while(_isBusy);
				}
				// IMMER milliliter
				else if(_routine % 2 == 1)
				{
					_milliliter = _btParsedData[_routine];
					while(1){
						Scale::EngineControl(Scale::EngineStatus::Up);

                        double result = 0;
                        do
                        {
                            result = Scale::Measure(100);
                        }while(result < _milliliter);

                        // TODO: offset
                        Scale::EngineControl(Scale::EngineStatus::Down);
                        _isBusy = true;
                        while(_isBusy);

                        if(_routine >=  sizeof(_btParsedData)/sizeof(int))
                            reset();

                        break;

					}
				}
			}

		}
    }
}


void setup(){

    // global config
	WDTCTL = WDTPW + WDTHOLD;                 // Stop WDT
	if (CALBC1_1MHZ==0xFF)					// If calibration constant erased
	{
		while(1);                               // do not load, trap CPU!!
	}

	// TODO: p2.6 input oscillator, p2.7 oscillator out

	// Feature specific setup
	Bluetooth::Init();
	Platform::Init();
    Scale::Init();

    __enable_interrupt();
}

void reset(){
    ClearBluetoothState();

    _isBusy = true;
    Scale::EngineControl(Scale::EngineStatus::Down);
    while(_isBusy);

    if(!Platform::GetPortBottlePosition() == 0)
    {
        _isBusy = true;
        _bottlePosition = 0;
        Platform::EngineControl(Platform::EngineStatus::Backward);
        while(_isBusy);
    }


}

// Bluetooth

// Test Data
// FF FF 01 00 32 FF FF
static int _stopBitCounter;
static unsigned int _btInterruptCounter;
static volatile int _btRawData[23];

#pragma vector=USCIAB0RX_VECTOR
__interrupt void USCI0RX_ISR(void)
{
    while (!(IFG2&UCA0TXIFG));                // USCI_A0 TX buffer ready?
    int result = UCA0RXBUF;                   // TX -> RXed character

    _btRawData[_btInterruptCounter++] = result;

    if(result == 0xFF){
      _stopBitCounter++;

      if(_stopBitCounter == 4){				// finished => vildate + parse data
          // 1.) validate raw data
          if(!Bluetooth::IsRawDataValid(_btRawData)){			// <= breakpoint
              ClearBluetoothState();
              return;
          }
          // 2.) parse raw data
          Bluetooth::ParseData(_btParsedData, _btRawData);					// <= breakpoint
          _btDataReceived = true;
          return;
      }
    }

}

inline void ClearBluetoothState(){
    _btDataReceived = false;
    _stopBitCounter = 0;
    _btInterruptCounter = 0;

    unsigned int i;
    for (i = 0; i < sizeof(_btRawData)/sizeof(int); i++) {
        _btRawData[i] = 0;
    }
    for (i = 0; i < sizeof(_btParsedData)/sizeof(int); i++) {
        _btParsedData[i] = 0;
    }
}



// Engine

#pragma vector=PORT1_VECTOR
__interrupt  void Port_1(void)
{
	int result = Platform::GetPortBottlePosition();

	if(result == -1)
	{
		reset(); // Fahre zur grundposition
	}
	else if(result == _bottlePosition)
	{
		Platform::EngineControl(Platform::EngineStatus::Stop);
		_routine++;
		_isBusy = false;
	}
	else if(result > _bottlePosition) // engine drove too far
	{
		reset();
	}
	else if(result >= 6)
	    Platform::EngineControl(Platform::EngineStatus::Stop);	// Stop because position 6 is the last available position.

	P1IFG &= ~0x04;	// P1.2 IFG cleared

}


// Scale
#pragma vector=PORT2_VECTOR
__interrupt  void Port_2(void)
{
	int result = Scale::GetScaleDoserPosition();
	if(result == 0){
		_routine++;
		_isBusy = false;
	}
	P2IFG &= ~0x03; // P1.2 IFG cleared
}



