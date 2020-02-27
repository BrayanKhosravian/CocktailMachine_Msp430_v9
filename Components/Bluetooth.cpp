#include <Components/Bluetooth.h>

void Bluetooth::Init(){
    DCOCTL = 0;                               // Select lowest DCOx and MODx settings
    BCSCTL1 = CALBC1_1MHZ;                    // Set DCO
    DCOCTL = CALDCO_1MHZ;
    P3SEL |= 0x30;                             // P3.4,5 = USCI_A0 TXD/RXD
    UCA0CTL1 |= UCSSEL_2;                     // SMCLK
    UCA0BR0 = 104;                            // 1MHz 9600
    UCA0BR1 = 0;                              // 1MHz 9600
    UCA0MCTL = UCBRS0;                        // Modulation UCBRSx = 1
    UCA0CTL1 &= ~UCSWRST;                     // **Initialize USCI state machine**
    IE2 |= UCA0RXIE;                          // Enable USCI_A0 RX interrupt
}

void Bluetooth::Enable(){
    IE2 |= UCA0RXIE;
}

void Bluetooth::Disable(){
    IE2 &= ~UCA0RXIE;
}


bool Bluetooth::IsRawDataValid(volatile int* rawData){
    unsigned int stopBytesCounter = 0;
    unsigned int lastStopByteIndex = 0;

    // 1.) verify for start bytes
    if(rawData[0] != 0xFF && rawData[1] != 0xFF)
        return false;

    if(rawData[22] != 0x00) // last byte is only a dummy and it has to have alawys a value of "0x00"
        return false;

    unsigned int i;
    for (i = 0; i <= sizeof(rawData)/sizeof(int); i++) {
        if(rawData[i] == 0xFF) stopBytesCounter++;
        if(stopBytesCounter == 4){
            lastStopByteIndex = i;
            break;
        }
    }

    // total count of 4 start/stop bytes are needed
    if(stopBytesCounter != 4)
        return false;

    // all other bytes after last stop byte have to be 0x00
    for (i = lastStopByteIndex + 1; i < sizeof(rawData)/sizeof(int); i++){
        if(rawData[i] != 0x00) return false;
    }

    // no more checks => everything is valid and raw data can be parsed
    return true;
}

void Bluetooth::ParseData(volatile int* parsedData, volatile int* rawData){

    int result = 0;
    unsigned int p = 1;
    unsigned int i;

    // skip start bits
    for (i = 2; i <= sizeof(rawData)/sizeof(int); i+=3) {
        // parse first block
        if(i == 2) {
            parsedData[0] = rawData[2];
            result = rawData[3] + rawData[4];
            parsedData[1] = result;
            if(rawData[5] == 0xFF && rawData[6] == 0xFF) return;  // stop bytes => return
            p++;
        }
        else{   // parse other blocks

            //  pos = _btRawData[i];    // bottle index
            //  _btParsedData[pos] = _btRawData[i+1] + _btRawData[i+2];

            parsedData[p] = rawData[i];
            result = rawData[i+1] + rawData[i+2];
            parsedData[++p] = result;
            if(i > 17 || rawData[p+1] == 0xFF && rawData[p+2] == 0xFF) return; // end reached or stop bytes => return
            p++;
        }
    }

}
