#include "mbed.h"
#include <cstdint>
#include <cstring>
#include "Zigbee.h"

AnalogIn LDR(PA_1);
int LDRout;

//#define MIN     1071677704
//#define MAX     1072535513
#define BUFFSIZE        64

BufferedSerial pcLDR(USBTX, USBRX, 115200);
Zigbee ZigbeeLDR(PA_9, PA_10);

char buffer[BUFFSIZE]   = {0};
char msgBuff[BUFFSIZE]  = {0};
char rcvBuff[BUFFSIZE]  = {0};
int counter             = 0;
int len                 = 0;

int MAX = 59502;
int MIN = 8540;

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
        LDRout = LDR.read();
//        len = snprintf(msgBuff, BUFFSIZE, "LDR: %d\n", LDR.read_u16());
        len = snprintf(msgBuff, BUFFSIZE, "%d", (LDR.read_u16() - MIN) / (MAX - MIN));
//      printf("LDR: %d \n", (LDR.read_u16() - MIN) / (MAX - MIN));
        printf("MSGBuff: %s\n", msgBuff);
//        printf("LDR: %d \n", LDR.read_u16());

        //printf("Name: ZigbeeLDR, LDR: %d\n", LDR.read_u16());
        //printf("LDR: %d \n", LDR.read());
        ZigbeeLDR.sendMessage(msgBuff);
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