// Write code in here to map Servo behaviours dependant on LDR readings... 
// Servo at minimum position at full brightness and maximum position at the lowest brightness.

#include "mbed.h"
#include <cstdint>
#include <cstring>

#define MID     1500
#define MIN     1000
#define MAX     2000
#define STEP    50

PwmOut servo(PB_3);

// main() runs in its own thread in the OS
int main() {
    
    servo.period_ms(20);
    servo.pulsewidth_us(MID); //NB in microseconds

    while(true) {
        for (int i=MIN;i<=MAX;i+=STEP){
            servo.pulsewidth_us(i);
            ThisThread::sleep_for(chrono::seconds(1));;
        }
        for (int i=MAX;i>=MIN;i-=STEP){
            servo.pulsewidth_us(i);
            ThisThread::sleep_for(chrono::seconds(1));;
        }
    }
}