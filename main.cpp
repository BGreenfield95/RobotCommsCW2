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
Zigbee ZigbeeServo(PA_11, PA_12); 

PwmOut servo(PA_10);

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
            printf("Received message\n");
            if(serialMutex.trylock_for(chrono::milliseconds(500)))
            {
                //printf("Received");
                len = snprintf(msgBuff, BUFFSIZE, "\r\n%s", rcvBuff);
                pcServo.write(msgBuff, len);
                serialMutex.unlock();

                if(strcmp(rcvBuff, "0") == 0)
                {
                    servo.pulsewidth(MIN);
                    printf("Min");
                }

                if(strcmp(rcvBuff, "1") == 0)
                {
                    servo.pulsewidth(MAX);
                    printf("Max");
                }
            }
        ThisThread::sleep_for(chrono::milliseconds(1));
        }
    }
}

int main()
{
    for(float offset=0.0; offset<0.001; offset+=0.0001) {
    servo.pulsewidth(0.001 + offset); // servo position determined by a pulsewidth between 1-2ms
    chrono::milliseconds(50);
    }
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
    ThisThread::sleep_for(chrono::seconds(1));

}