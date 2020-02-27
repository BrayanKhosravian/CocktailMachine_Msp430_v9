#include <MSP430F2274.h>

#ifndef COMPONENTS_BLUETOOTH_H_
#define COMPONENTS_BLUETOOTH_H_

class Bluetooth
{

public:
    // Methods
    static void Init();
    static void Enable();
    static void Disable();

    static void ParseData(volatile int* parsedData, volatile int* rawData);
    static bool IsRawDataValid(volatile int* rawData);

};

#endif /* COMPONENTS_BLUETOOTH_H_ */
