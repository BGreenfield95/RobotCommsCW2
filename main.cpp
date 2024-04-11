#include "mbed.h"
#include <cstdint>
#include <cstring>

AnalogIn LDR(PA_1);

#define MIN     1071517153
#define MAX     1072561119

int LDRout;

// main() runs in its own thread in the OS
int main()
{
    while (true) {
        //LDRout = (MAX - LDR.read()) * 100 / (MAX - MIN);
        printf("LDR Raw = %d\n", LDR.read());
        LDRout = LDR.read();
        printf("LDR = %d\n", LDRout);
        ThisThread::sleep_for(chrono::seconds(3));;

    }
}

//Test One
//LDR Value = 1072561119 - TORCH

//LDR Value = 1071517153 - BLOCKED