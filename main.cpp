#include "mbed.h"
#include <cstdint>
#include <cstring>

PwmOut servo(PB_3);

// main() runs in its own thread in the OS
int main()
{
    servo.period(0.020);          // servo requires a 20ms period
    while (1) {
        for(float offset=0.0; offset<0.001; offset+=0.0001) {
            servo.pulsewidth(0.001 + offset); // servo position determined by a pulsewidth between 1-2ms
            ThisThread::sleep_for(chrono::seconds(1));;
        }
}