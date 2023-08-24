#include <SPI.h>
#include <Ethernet.h>
#include <AccelStepper.h>

#define MAX_SPEED 1000 // steps per minute
#define MAX_ACCEL 1000
#define PITCH 0.25 // mm per revolution
#define RANGE 8 // mm. Allowed travel range.
#define RESOLUTION 200 // steps per revlution
#define MICROSTEPS 1 // microsteps per step

//W5500
#define W5500_RESET 9
#define W5500_SS 10
const byte mac[] = {0xDE, 0xAD, 0xB0, 0xEF, 0xFE, 0xED};
const IPAddress ip(192, 168, 10, 70);
EthernetUDP Udp;
#define PACKET_SIZE 10
#define PACKET_HALF_SIZE 5
const unsigned short cmdMask  = 0xE0;

#define localPort 8888      // local port to listen on
char inBuffer[PACKET_SIZE]; //buffer to hold incoming packet,
char outBuffer[PACKET_SIZE];
int packetSize = 0;

class Axis: public AccelStepper{
  public:
  const uint8_t limit1Pin;
  const uint8_t limit2Pin;
  const uint8_t number;
  bool isCalibrated = false;
  bool move_speed = false;

  Axis(uint8_t stepPin, uint8_t dirPin, uint8_t limit1Pin, uint8_t limit2Pin, uint8_t number, long position=0):
      AccelStepper(AccelStepper::DRIVER, stepPin, dirPin), limit1Pin(limit1Pin), limit2Pin(limit2Pin), number(number){
    
    this->setMaxSpeed(MAX_SPEED);
    this->setAcceleration(MAX_ACCEL);
    this->setMinPulseWidth(1000);
    pinMode(this->limit1Pin, INPUT);
    pinMode(this->limit2Pin, INPUT);

    this->setCurrentPosition(position);
  }

  void parse(){
    uint8_t cmd = 0;
    memcpy(&cmd, &inBuffer[this->number * PACKET_HALF_SIZE], sizeof(uint8_t));
    switch(cmd){
      case 0b00000000: //state

        break;
      case 0b00100000: //moveTo
        
        break;
      case 0b01000000:
        {
          float arg = 0;
          memcpy(&arg, &inBuffer[this->number * PACKET_HALF_SIZE + 1], sizeof(float));

          this->move_speed = true;
          this->setSpeed(arg);
        }
        break;
      case 0b01100000:
        //unused
        break;
      case 0b10000000:
        this->setSpeed(0.0);
        this->stop();
        //stop gracefully
        break;
      case 0b10100000:
        this->setSpeed(0.0); //debug
        this->stop();
        this->moveTo(this->currentPosition());
        break;
      default:
        // not implemented
        break;
    }
  }

  void reportStatus(){
    int32_t pos = this->currentPosition();
    memcpy(&outBuffer[this->number * sizeof(int32_t)], &pos, sizeof(int32_t));
    outBuffer[2 * sizeof(int32_t) + this->number] = 0;
    bitWrite(outBuffer[2 * sizeof(int32_t) + this->number], 0, this->isCalibrated);
    if(!this->isRunning()){
      this->move_speed = false;
    }
    bitWrite(outBuffer[2 * sizeof(int32_t) + this->number], 1, !this->isRunning());
    bitWrite(outBuffer[2 * sizeof(int32_t) + this->number], 2, digitalRead(this->limit1Pin));
    bitWrite(outBuffer[2 * sizeof(int32_t) + this->number], 3, digitalRead(this->limit2Pin));
    bitWrite(outBuffer[2 * sizeof(int32_t) + this->number], 4, this->_direction);
    
  }
};

// The X Stepper pins
#define STEPPER1_DIR_PIN 1
#define STEPPER1_STEP_PIN 2
#define STEPPER1_LIMIT_PIN1 4
#define STEPPER1_LIMIT_PIN2 6
Axis axis1(STEPPER1_STEP_PIN, STEPPER1_DIR_PIN, STEPPER1_LIMIT_PIN1, STEPPER1_LIMIT_PIN2, 0);

// The Y stepper pins
#define STEPPER2_DIR_PIN 0
#define STEPPER2_STEP_PIN 3
#define STEPPER2_LIMIT_PIN1 5
#define STEPPER2_LIMIT_PIN2 7
Axis axis2(STEPPER2_STEP_PIN, STEPPER2_DIR_PIN, STEPPER2_LIMIT_PIN1, STEPPER2_LIMIT_PIN2, 1);


void setup() {
  pinMode(W5500_RESET, OUTPUT);

  digitalWrite(W5500_RESET, LOW);
  delay(1000);
  digitalWrite(W5500_RESET, HIGH);
  delay(1000);

  Ethernet.init(W5500_SS);
  Ethernet.begin(mac, ip);
  if (Ethernet.hardwareStatus() == EthernetNoHardware) {
    while (true) {
      delay(1000);
    }
  }
  if (Ethernet.linkStatus() == LinkOFF) {
    //Serial.println("Ethernet cable is not connected.");
  }
  Udp.begin(localPort);
}

void loop() {
  //reconnect ethernet if link went missing

  if(axis1.isRunning()){
    if(axis1.move_speed){
      axis1.runSpeed();
    }else{
      axis1.run();
    }
  }
    
  if(axis2.isRunning()){
    if(axis2.move_speed){
      axis2.runSpeed();
    }else{
      axis2.run();
    }
  }

  packetSize = Udp.parsePacket();
  if(Udp.available()){
    // read the packet into packetBuffer
    Udp.read(inBuffer, PACKET_SIZE);
    axis1.parse();
    axis2.parse();

    while(Udp.available()){
      Udp.read(inBuffer, PACKET_SIZE);
    }

    axis1.reportStatus();
    axis2.reportStatus();

    Udp.beginPacket(Udp.remoteIP(), Udp.remotePort());
    Udp.write(outBuffer, PACKET_SIZE);
    Udp.endPacket();
 }
}