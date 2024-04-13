// Write code in here to map Servo behaviours dependant on LDR readings... 
// Servo at minimum position at full brightness and maximum position at the lowest brightness.

#include "mbed.h"
#include <cstdint>
#include <cstring>
#include "Zigbee.h"

#define MID     1500
#define MIN     1000
#define MAX     2000
#define STEP    50

#define BUFFSIZE    64

BufferedSerial pcServo(USBTX, USBRX, 115200);
Zigbee ZigbeeServo(PA_9, PA_10); 

PwmOut servo(PB_3);

char buffer[BUFFSIZE]   = {0};
char msgBuff[BUFFSIZE]  = {0};
char rcvBuff[BUFFSIZE]  = {0};
int counter             = 0;
int len                 = 0;

Thread readThread;
Mutex serialMutex;

void reader()
{
    while(1)
    {
//        printf("0");
        if(ZigbeeServo.receiveMessage(rcvBuff))
        {
            printf("1");
            if(serialMutex.trylock_for(chrono::milliseconds(500)))
            {
                printf("Received");
                len = snprintf(msgBuff, BUFFSIZE, "\r\n%s", rcvBuff);
                pcServo.write(msgBuff, len);
                serialMutex.unlock();

                if(strcmp(rcvBuff, "0") == 0){
                    servo.pulsewidth_us(MIN);
                }

                if(strcmp(rcvBuff, "1") == 0){
                    servo.pulsewidth_us(MAX);
                }
            }
        ThisThread::sleep_for(chrono::milliseconds(10));
        }
    }
}

int main()
{
    servo.period_ms(20);
    servo.pulsewidth_us(MID); //NB in microseconds
    
    pcServo.set_format(
        /* bits */ 8,
        /* parity */ BufferedSerial::None,
        /* stop bit */ 1
    );
    readThread.start(reader);
    
    if(serialMutex.trylock_for(chrono::milliseconds(500)))
    {
        printf("Started");
        len = snprintf(buffer, BUFFSIZE, "\r\nHello, I have started :)\r\n");
        pcServo.write(buffer, len);
        serialMutex.unlock();
    }
}

/*
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
*/