#include "mbed.h"
#include <chrono>
#include <cstdint>
#include <cstring>
#include "Zigbee.h"

BufferedSerial pc(USBTX, USBRX, 115200);
Zigbee xbee(PA_9, PA_10);

AnalogIn LDR(PA_1);
int LDRin;
int LDRout;

//#define MIN     1071677704
//#define MAX     1072535513
#define BUFFSIZE        64

int MAX = 57700;
int MIN = 9000;

Thread readThread;
Mutex serialMutex;

char buffer[BUFFSIZE]   = {0};
char msgBuff[BUFFSIZE]  = {0};
char rcvBuff[BUFFSIZE]  = {0};
int len                 = 0;

void reader()
{
    while(1)
    {
        if(xbee.receiveMessage(rcvBuff))
        {
            if(serialMutex.trylock_for(chrono::milliseconds(500)))
            {
                len = snprintf(msgBuff, BUFFSIZE, "\r\n%s", rcvBuff);
                pc.write(msgBuff, len);
                serialMutex.unlock();
            }
        }
        ThisThread::sleep_for(chrono::milliseconds(1));
    }
}

// main() runs in its own thread in the OS
int main()
{
    pc.set_format(
        /* bits */ 8,
        /* parity */ BufferedSerial::None,
        /* stop bit */ 1
    );
    readThread.start(reader);
    
    if(serialMutex.trylock_for(chrono::milliseconds(500)))
    {
        len = snprintf(buffer, BUFFSIZE, "\r\nHello, I have started :)\r\n");
        pc.write(buffer, len);
        serialMutex.unlock();
    }

    while (true) {

        len = snprintf(msgBuff, BUFFSIZE, "%d\r\n", ((LDR.read_u16() - MIN) / (MAX - MIN)));
        pc.write(msgBuff, len);
        xbee.sendMessage(msgBuff);
        ThisThread::sleep_for(chrono::seconds(1));
    }
}