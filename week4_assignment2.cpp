// #include "main.h"

// DigitalOut led(L27);
// DigitalOut led2(L26);
// DigitalOut led3(L25);
// DigitalOut ledbuiltin(LED1);

// DJIMotor testMot(5, CANHandler::CANBUS_1, GM6020, "testbench_motor");

// #define IMPULSE_DT 100
// #define IMPULSE_STRENGTH 16383

// int main(){

//     DJIMotor::s_setCANHandlers(&canHandler1, &canHandler2, false, false);
//     DJIMotor::s_sendValues();
//     DJIMotor::s_getFeedback();

//     unsigned long timeStart;
//     unsigned long loopTimer = us_ticker_read();
//     int refLoop = 0;

//     testMot.setSpeedPID(1.5,0,200);

//     bool prevL = false;
//     bool switL = false;

//     int motorSpeed = 0;

//     while(true){
//         timeStart = us_ticker_read();

//         if ((timeStart - loopTimer) / 1000 > 25){
//             loopTimer = timeStart;
//             led = !led;
//             ledbuiltin = !ledbuiltin;

//             refLoop++;
//             if (refLoop >= 5){
//                 refereeThread(&referee);
//                 refLoop = 0;
//                 led2 = !led2;
//             }
//             prevL = switL;
//             remoteRead();
//             switL = (remote.leftSwitch() == Remote::SwitchState::UP);

//             // testMot.setPower(remote.leftX() * 3);
//             // if(!prevL && switL){
//             //     motorSpeed += 10;
//             // }

//             motorSpeed = remote.leftX() / 6;

//             testMot.setSpeed(motorSpeed);
//             int dir = 0;
//             if(motorSpeed > 0){
//                 dir = 1;
//             }else if(motorSpeed < 0){
//                 dir = -1;
//             }
//             testMot.pidSpeed.feedForward = dir * (874 + (73.7 * abs(motorSpeed)) + (0.0948 * motorSpeed * motorSpeed));
//             printff("%d\t%d\t%d\n", testMot>>POWEROUT, motorSpeed, testMot>>VELOCITY);

//             DJIMotor::s_sendValues();
//         }
//         DJIMotor::s_getFeedback();
//         ThisThread::sleep_for(1ms);
//     }
// }

//STARTER 
//THIS EXERCISE IS TO BE DONE AND THEN RUN IN REAL LIFE ON A ROBOT
//YOU WILL NOT BE ABLE TO COMPILE THIS UNLESS YOU SET UP THE BUILD ENVIRONMENTrobots/TestBench/main.cpp
//STARTER 
//THIS EXERCISE IS TO BE DONE AND THEN RUN IN REAL LIFE ON A ROBOT
//YOU WILL NOT BE ABLE TO COMPILE THIS UNLESS YOU SET UP THE BUILD ENVIRONMENT
#include "main.h"

DigitalOut led(L26);
DigitalOut led2(L27);
DigitalOut led3(L25);

I2C i2c(I2C_SDA, I2C_SCL);

//DEFINE MOTORS, ETC
float maxRPM = 9000.0;
float maxRemote = 660.0;
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
