#include "mbed.h"
#include <cstdint>
#include <cstring>
#include "Zigbee.h"

AnalogIn LDR(PA_1);
float LDRout;

#define MIN     1071517153
#define MAX     1072561119
#define BUFFSIZE        64

BufferedSerial pcLDR(USBTX,USBRX, 115200);
Zigbee ZigbeeLDR(PA_2, PA_3); // Zigbee module configured with 115200 baud rate - Change in Zigbee.cpp if required.

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
        if(ZigbeeLDR.receiveMessage(rcvBuff))
        {
            if(serialMutex.trylock_for(chrono::milliseconds(500)))
            {
                len = snprintf(msgBuff, BUFFSIZE, "\r\n%s", rcvBuff);
                pcLDR.write(msgBuff, len);
                serialMutex.unlock();
            }
        }
        ThisThread::sleep_for(chrono::milliseconds(10));
    }
}

int main()
{
    pcLDR.set_format(
        /* bits */ 8,
        /* parity */ BufferedSerial::None,
        /* stop bit */ 1
    );
    readThread.start(reader);
    
    if(serialMutex.trylock_for(chrono::milliseconds(500)))
    {
        len = snprintf(buffer, BUFFSIZE, "\r\nHello, I have started :)\r\n");
        pcLDR.write(buffer, len);
        serialMutex.unlock();
    }

    while (true) {
        len = snprintf(msgBuff, BUFFSIZE, "counter %d", counter);
        ZigbeeLDR.sendMessage(msgBuff);
        counter++;
        ThisThread::sleep_for(chrono::seconds(10));
    }
}
/*
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
*/
//Test One
//LDR Value = 1072561119 - TORCH

//LDR Value = 1071517153 - BLOCKED