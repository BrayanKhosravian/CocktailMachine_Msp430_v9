#include "Platform.h"


void Platform::Init(){
    // TODO: check if port interrupt for bottle positon is configuret correctly
    P1DIR &= 0x00; // set Port 1 in Input direction
    P1IE  |= BIT0 + BIT1 + BIT2 + BIT3 + BIT4 + BIT5 + BIT6 + BIT7; // P1.1-P1.7 Interrupt enable // => 0x7F
    P1IFG |= 0x7F; // declares Port 1 Input (except P1.0) as Interrupt Flags
    P1IES |= 0x00; // P1.1-P1.6 low to high Edge

}


// TODO: Add delay for transitors
// mosfet delay = 1.16 us * 3 = 3.6 uS
// bipolar fet delay = 28nS * 3 = 90 ns
void Platform::EngineControl(Platform::EngineStatus engineDirection){
    if(engineDirection == Platform::EngineStatus::Stop){
        P4OUT &= ~engineForward;
        P4OUT &= ~engineBackward;
    }
    else if(engineDirection == Platform::EngineStatus::Backward){
        P4OUT &= ~engineForward;
        P4OUT |= engineBackward;
    }
    else if(engineDirection == Platform::EngineStatus::Forward){
        P4OUT &= ~engineBackward;
        P4OUT |= engineForward;
    }
}


int Platform::GetPortBottlePosition(){

    // 0000 0001 => grundposition => 0x01
    // 0000 0010 => pos 1 => 0x02
    // 0000 0100 => pos 2 => 0x04
    // 0000 1000 => pos 3 => 0x08
    // 0001 0000 => pos 4 => 0x10
    // 0010 0000 => pos 5 => 0x20
    // 0100 0000 => pos 6 => 0x40

    // _bottlePosition => 0, 1, 2 ,3, 4, 5, 6

    int port = P1IN;

    if(port == 0x01)
        return 0;
    else if(port == 0x02)
        return 1;
    else if(port == 0x04)
        return 2;
    else if(port == 0x08)
        return 3;
    else if(port == 0x10)
        return 4;
    else if(port == 0x08)
        return 3;
    else if(port == 0x10)
        return 4;
    else if(port == 0x20)
        return 5;
    else if(port == 0x40)
        return 6;
    else if(port == 0x00)
        return -2;  // nothing

    else return -1;  // error => fahre zu grund position

}


