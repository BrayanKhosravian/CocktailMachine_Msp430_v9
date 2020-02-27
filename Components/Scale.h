#include <MSP430F2274.h>

#ifndef SCALE_H_
#define SCALE_H_

#define CPU_F 1100000
#define delaynms(x) __delay_cycles((long)((double)CPU_F*x/1000.0))
#define delaynus(x) __delay_cycles((long)((double)CPU_F*x/1000000.0))

#define ADDT BIT2
#define ADSK BIT3
#define AD_DIR P2DIR
#define AD_OUT P2OUT
#define AD_IN P2IN
#define ADSK_1 AD_OUT|=ADSK
#define ADSK_0 AD_OUT&=~ADSK
#define ADDO (AD_IN&ADDT)

#define WEIGHT_BASE 34844

class Scale {

public:
    enum class EngineStatus{Stop, Up, Down};
	static void Init();
	static double Measure(int cycles);
	static void EngineControl(EngineStatus status);

	static int GetScaleDoserPosition();

private:
	static unsigned char Data_str[16];

	static int ReadCount();

};

#endif /* SCALE_H_ */
