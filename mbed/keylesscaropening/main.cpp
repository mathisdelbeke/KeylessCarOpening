#include "mbed.h"
#include <cstring>
#include <vector>

// Maximum number of element the application buffer can contain
#define MAXIMUM_BUFFER_SIZE 32

// Create a DigitalOutput object to toggle an LED whenever data is received.
static DigitalOut led(LED1);

// Create a BufferedSerial object with a default baud rate.
static UnbufferedSerial pc(USBTX, USBRX);
static UnbufferedSerial bluetooth(PA_9, PA_10);

//Create CAN class (rx, tx, speed)
CAN car(PA_11, PA_12, 125000);

//1 => open, 0 => closed
bool doorStatus = false;

//Keep track of end of transmission
bool transferComplete = false;
bool transferBusy = false;

//buffer pointer
int rxBuffPointer = 0;

//rx buffer
char rxBuff[MAXIMUM_BUFFER_SIZE] = {0};

//Close command struct
struct SCloseCommand {
    int id;
    char data[8];
};

//Close command struct
struct SOpenCommand {
    int id;
    char data[8];
};

//Filling the struct
struct SCloseCommand closeCommand[] = {
    {0x038, {0x05, 0x00, 0x00, 0x00, 0x00, 0x84, 0x00, 0x20}},
    {0x028, {0x00, 0x00, 0x01, 0x00, 0x09, 0x00, 0x00, 0x00}},
    {0x210, {0x20, 0x2C, 0x94, 0x00, 0x00, 0x00, 0x00, 0x90}}
};

//Filling the struct
struct SOpenCommand openCommand[] = {
    {0x210, {0x40, 0x2C, 0x94, 0x00, 0x00, 0x00, 0x00, 0x90}}
};


static void printChar(BufferedSerial, char* const );

//Lock the doors
static void lockDoors() {


    //Sending all can messages
    for(int i = 0; i < sizeof(closeCommand) / sizeof(closeCommand[0]); i++) {

        //Create the message for the CAN bus
        // int id, *char data
        CANMessage msg = CANMessage(closeCommand[i].id, closeCommand[i].data);

        //Status
        bool transactionSuccess = false;

        //Try sending the message
        do {
            transactionSuccess = car.write(msg);
            HAL_Delay(10);
        } while(!transactionSuccess);

    }

}

//Unlock the doors
static void unlockDoors() {

    //Sending all can messages
    for(int i = 0; i < sizeof(openCommand) / sizeof(openCommand[0]); i++) {

        //Create the message for the CAN bus
        // int id, *char data
        CANMessage msg = CANMessage(openCommand[i].id, openCommand[i].data);

        //Status
        bool transactionSuccess = false;

        //Try sending the message
        do {
            transactionSuccess = car.write(msg);
            HAL_Delay(10);
        } while(!transactionSuccess);

    }
}

//Print single char
static void printChar(UnbufferedSerial& target, char const *p) {
    target.write(p, 1);
}

//Println function
static void println(UnbufferedSerial& target, char const *p) {
    target.write(p, strlen(p));
    target.write("\n", 1);
}

//Print function
static void print(UnbufferedSerial& target, char const *p) {
    target.write(p, strlen(p));
}

//Clear the buffer
static void clearBuffer() {
    for(int i = 0; i < MAXIMUM_BUFFER_SIZE; i++) rxBuff[i] = 0;
}

//Reads the content of the rx buffer
static char* readBuffer() {

    //Make a new vector => variable size array
    static vector<char> v;

    //Fill the vector
    for(int i = 0; i < MAXIMUM_BUFFER_SIZE; i++) {
        if(rxBuff[i] != 0) v.push_back(rxBuff[i]);
        else break;
    }

    //Add termination
    v.push_back(0);
    return v.data();

}

//Get command based on contents of the buffer
static char getCommandFromBuffer(bool deleteBuffer = false) {

    char cmd = 0;
    
    //If "open" is found the rx-buffer
    if(strstr(rxBuff, "open")) cmd = 'o';

    //If "close" is found the rx-buffer
    if(strstr(rxBuff, "close")) cmd = 'c';

    //Clear the buffer
    if(deleteBuffer) clearBuffer();

    //Return the command
    return cmd;
}


//Serial interrupt
void on_rx_interrupt() {

    //Buffer of the received char
    char c;

    // Toggle the LED.
    led = !led;

    //Put rx data in the buffer
    bluetooth.read(&c, 1);

    //Print the received char
    printChar(pc, &c);

    //If we start with delimiter '%'
    if(c == '%') {

        //Usefull data is received => reset the pointer and start transaction
        rxBuffPointer = 0;
        transferBusy = true;

    }

    //If we are the end of the line
    if(c == '\n' && transferBusy) {

        //End of the transmit => stop transmission
        transferBusy = false;
        transferComplete = true;
    }

    if(transferBusy) {
        rxBuff[rxBuffPointer] = c;
        rxBuffPointer++;
    }


}


int main(void)
{
    //Serial settings
    pc.baud(115200);
    pc.format(8, BufferedSerial::None, 1);

    //Serial settings
    bluetooth.baud(9600);
    bluetooth.format(8, BufferedSerial::None, 1);

    //Attach the interrupt on every received char
    bluetooth.attach(&on_rx_interrupt, SerialBase::RxIrq);

    //Normal CAN mode
    car.mode(CAN::Normal);


    println(pc, "starting");

    while (1) {

        //Transfer is complete
        if(transferComplete) {

            //Print the buffer
            println(pc, readBuffer());

            //Get command in the buffer
            char cmd = getCommandFromBuffer();

            //Command is 'open'
            if(cmd == 'o') {

                //If the doors are still closed
                if(!doorStatus) {
                    println(pc, "unlocking doors");
                    unlockDoors();
                    doorStatus = true;
                }
            }

            //Command is 'closed'
            if(cmd == 'c') {

                //If the doors are still open
                if(doorStatus) {
                    println(pc, "locking doors");
                    lockDoors();
                    doorStatus = false;
                }
            }

            //Reset the flag
            transferComplete = false;

        }

        HAL_Delay(2000);
        println(pc, "running");
  }
}