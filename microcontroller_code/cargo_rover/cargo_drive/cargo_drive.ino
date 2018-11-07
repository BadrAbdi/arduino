//Import required files
#include <ros.h>
#include <std_msgs/Byte.h>
//#include <std_msgs/Empty.h>
#include <std_msgs/Bool.h>
#include <joysticks/drive.h>
#include <AltSoftSerial.h>
#include <AltSSPololuQik.h>

//AltSoftSerial uses Pin 8 for Rx and Pin 9 for Tx, and that is unchangeable because hardware interrupts. PWM Pin 10 cannot be used.
//Connect Arduino Rx to driver Tx , arduino Tx to driver Rx

const byte resetPin = 4;
bool timeout = false;
int rSpd;
int lSpd;

PololuQik2s12v10 drv(resetPin);

//Create a ROS node-handler to handle ROS stuff
ros::NodeHandle  nh;

std_msgs::Byte error_msg;

ros::Publisher error("error", &error_msg);

//Declare a reset function
void(* resetFunc) (void) = 0;

void setTimeOut(const std_msgs::Bool &timeout_data) {
  timeout = timeout_data.data;
}

void spinWheel(const joysticks::drive &drive) {
  drv.setM0Speed(drive.right);
  drv.setM1Speed(drive.left);
}
//
//void getErrorMsg(const std_msgs::Empty &e_msg) {
//  error_msg.data = drv.getErrors();
//  error.publish(&error_msg);
//}


//Setup subscribers

ros::Subscriber<joysticks::drive> sub_drive("drive", &spinWheel);
//ros::Subscriber<std_msgs::Empty> sub_error("getError", &getErrorMsg);
ros::Subscriber<std_msgs::Bool> sub_timeout("timeout", &setTimeOut);


void setup()
{
  //Initiate the node & subscribers
  nh.initNode();
  nh.subscribe(sub_drive);
  //  nh.subscribe(sub_error);
  nh.subscribe(sub_timeout);
  //nh.advertise(error);
  //Initialize serial comms with the driver
  drv.init();
  delay(10);

}


void loop()

{
  if (!timeout) {
    nh.spinOnce();
  } else {
    drv.setM0Speed(0);
    drv.setM1Speed(0);
    delay(1000);
    resetFunc();
  }

}
