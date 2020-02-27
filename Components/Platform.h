#include <MSP430F2274.h>

#ifndef TESTCLASS_H_
#define TESTCLASS_H_

class Platform {

public:
    static void Init(); // initialize the platform and engine logic

    enum class EngineStatus{Stop, Forward, Backward};
	static void EngineControl(Platform::EngineStatus engineDirection);
	static int GetPortBottlePosition();

private:
	#define position0 BIT0 // grundpos
	#define position1 BIT1 // Port1
	#define position2 BIT2
	#define position3 BIT3
	#define position4 BIT4
	#define position5 BIT5
	#define position6 BIT6

	#define engineForward BIT0 //engine goes away from position0 // PORT4
	#define engineBackward BIT1 //engine goes to position0		 // PORT 4

};


#endif
