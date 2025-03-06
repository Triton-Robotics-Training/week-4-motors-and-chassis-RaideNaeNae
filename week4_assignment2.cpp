#include "main.h"

DigitalOut led(L26);
DigitalOut led2(L27);
DigitalOut led3(L25);

I2C i2c(I2C_SDA, I2C_SCL);

//DEFINE MOTORS, ETC
int maxRPM = 9000;
    int maxRemote = 660;
    double scaleFactor = 9000/660;
    DJIMotor tl(1, CANHandler::CANBUS_1, M3508, "top left motor");
    DJIMotor tr(2, CANHandler::CANBUS_1, M3508, "top right motor");
    DJIMotor bl(3, CANHandler::CANBUS_1, M3508, "bottom left motor");
    DJIMotor br(4, CANHandler::CANBUS_1, M3508, "bottom right motor");

int main(){

    //assigning can handler objects to motor class.
    DJIMotor::s_setCANHandlers(&canHandler1,&canHandler2, false, false); 

    //getting initial feedback.
    DJIMotor::s_getFeedback();

    unsigned long loopTimer_u = us_ticker_read();
    unsigned long timeEnd_u;
    unsigned long timeStart_u;

    int refLoop = 0;

    //DEFINE PIDs AND OTHER CONSTANTS

    while(true){ //main loop
        timeStart_u = us_ticker_read();
        
        //inner loop runs every 25ms
        if((timeStart_u - loopTimer_u) / 1000 > 25) { 
            loopTimer_u = timeStart_u;
            led = !led; //led blink tells us how fast the inner loop is running

            if (refLoop >= 5) { //ref code runs 5 of every inner loop, 
                refLoop = 0;
                refereeThread(&referee);
            }
            refLoop ++;

            remoteRead(); //reading data from remote

            //MAIN CODE 
            //MOST CODE DOESNT NEED TO RUN FASTER THAN EVERY 25ms
            int x = remote.leftX();
            int y = remote.leftY();
            int r = remote.rightX();

            if(x<50&&x>-50) {x=0;}
            if(y<50&&y>-50) {y=0;}
            if(r<50&&r>-50) {r=0;}

            //printff("%d %d\n", x, y);

            int tlSpeed = (y+x+r) * scaleFactor;
            int trSpeed = (-y+x+r) * scaleFactor;
            int blSpeed = (y-x+r) * scaleFactor;
            int brSpeed = (-y-x+r) * scaleFactor;

            tl.setSpeed(tlSpeed);
            tr.setSpeed(trSpeed);
            bl.setSpeed(blSpeed);
            br.setSpeed(brSpeed);

            timeEnd_u = us_ticker_read();
            DJIMotor::s_sendValues();
        }

        //FEEDBACK CODE DOES NEED TO RUN FASTER THAN 1MS
        //OTHER QUICK AND URGENT TASKS GO HERE

        DJIMotor::s_getFeedback();
        ThisThread::sleep_for(1ms);
    }
}
