#include "Scale.h"

void Scale::Init(){
	AD_DIR|=ADSK;
	AD_DIR&=~ADDT;

	P4DIR |= BIT2 + BIT3; // P4.0 and P4.1 output => used for platform (engine)

	// endposition
	P2DIR &= 0x03; //set P2.0-P2.1 in Input direction
	P2IE  |= 0x03; // P2.0-P2.1 Interrupt enable
	P2IFG |= 0x03; //declares P2.0-P2.1 as Interrupt Flags
	P2IES |= 0x00; //P2.0-P2.1 low to high Edge

	delaynms(1);
	delaynms(1);
}

void Scale::EngineControl(Scale::EngineStatus status){
	if(status == Scale::EngineStatus::Stop){
		P4OUT &= ~BIT2;
		P4OUT &= ~BIT3;
	}
	else if(status == Scale::EngineStatus::Up){
		P4OUT &= ~BIT3;
		P4OUT |= BIT2;
	}
	else if(status == Scale::EngineStatus::Down){
		P4OUT &= ~BIT2;
		P4OUT |= BIT3;
	}

}

int Scale::ReadCount(){
	unsigned long Count;
	unsigned int ADval;
	unsigned char i;
	ADSK_0;
	Count=0;
	while(ADDO);
	for(i=0;i<24;i++){
		ADSK_1;
		Count=Count<<1;
		ADSK_0;
		if(ADDO)Count++;
	}
	ADSK_1;
	Count=Count^0x800000;
	ADSK_0;
	ADval=(unsigned int)(Count>>8);
	return ADval;
}

double Scale::Measure(int cycles){

    //AD_val=ReadCount();
    //weight = (WEIGHT_BASE - AD_val) / divisor;

    // To calculate the divisor you have to weight 1000 gramm and then you have to get the erase value of the HX711
    // divisor = [value of hx711 (0gramm) - value of hx711 (1000gramm)] / 1000


    static int i = 0;
    static const double divisor = 5.959;
    int mittel;
    float weight;
    int AD_val;
    int endwert;

    for (i = 0; i <= cycles; ++i)
    {
        AD_val=ReadCount();
        weight = (WEIGHT_BASE - AD_val) / divisor;
        mittel = mittel + weight;
    }

    endwert = weight/cycles;
    return endwert;
}


int Scale::GetScaleDoserPosition(){
    int port = (int)P2IN;

    if (port == BIT0)
      return 0;
    else if(port == BIT1)
      return 1;

    else return -1;
}

