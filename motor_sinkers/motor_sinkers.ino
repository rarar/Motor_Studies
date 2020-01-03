#include <AccelStepper.h>
#include <SoftwareSerial.h>

SoftwareSerial BTserial(10, 11); // RX | TX

char c = ' ';
char lineBuffer[100];
char subBuffer[30];
int index = 0;
int index2 = 0;
int total = 0;
bool capture = false;
String send = "";

unsigned long previousMillis = 0;  //will store last time LED was blinked
unsigned long period = 15000;
const int MAX_PERIOD = 30000;

int motorSpeed = 2500;
int motorAccel = 200;
int movePos = 5000;

const int TOP_SPEED = 3000;
const int TOP_ACCEL = 200;
const int MAX_DIST = 20000;

// Define a stepper and the pins it will use
AccelStepper stepper(AccelStepper::DRIVER, 2, 3);

boolean motorOn = true;
int timeWithoutDelayCount = 0;

void delayAndRead()
{
  delay(50);
  while (BTserial.available())
  {
    c = BTserial.read();
  }
  delay(100);
}

void initHC05ToInq()
{
  BTserial.println("AT+CMODE=1");// Enable connect to any device
  delayAndRead();
  BTserial.println("AT+ROLE=1");// Set to master in order to enable scanning
  delayAndRead();
  BTserial.println("AT+INQM=1,50,5");//RSSI, Max 10 devices, ~30s
  delayAndRead();
  BTserial.println("AT+CLASS=0");// Disable COD filter
  delayAndRead();
  BTserial.println("AT+INIT");// Init.
  delayAndRead();
}

void initMessage()
{
  send = "Bluetooth1;{\"state\":{\"reported\":{\"devices\":[";
}


void setup() {
  pinMode(9, OUTPUT);
  digitalWrite(9, HIGH);
  Serial.begin(9600);           // set up Serial library at 9600 bps

  // HC-05 default serial speed for AT mode is 38400
  BTserial.begin(38400);

  // Wait for hardware to initialize
  delay(1000);

  // Set correct states for inq
  initHC05ToInq();

  // Start inq
  initMessage();
  BTserial.println("AT+INQ");

  stepper.setMaxSpeed(motorSpeed);
  stepper.setAcceleration(motorAccel);
  stepper.moveTo(movePos);
}

void readBuffer() {

  // Keep reading from HC-05 and send to Arduino Serial Monitor
  if (BTserial.available())
  {
    // Read character and append into buffer
    c = BTserial.read();
    lineBuffer[index] = c;
    index++;

    // When line ends
    if (c == '\n')
    {
      // Remove line end characters from buffer
      lineBuffer[index - 1] = 0; // \r\n

      // Reset buffer index for next line
      index = 0;

      if (lineBuffer[0] == 'O' && lineBuffer[1] == 'K')
      {
        // Finish message
        send += "]}}}";
        // DEBUG / TODO actually send this message
        if (total > 0)
        {
          Serial.print("Total = ");
          Serial.println(total);
          period = map(total, 0, 50, MAX_PERIOD, 0);
          Serial.print("Period is now ");
          Serial.println(period);
          motorSpeed = map(total, 0, 50, 2500, TOP_SPEED);
          motorAccel = motorSpeed * 0.08;
          movePos = map(total, 0, 50, 5000, MAX_DIST);
          stepper.setMaxSpeed(motorSpeed);
          stepper.setAcceleration(motorAccel);
          Serial.print("Motor Speed is now ");
          Serial.println(motorSpeed);
          Serial.print("Motor Acceleration is now ");
          Serial.println(motorAccel);
          Serial.print("Move Position is now ");
          Serial.println(movePos);
          Serial.println(send);

          Serial.print("stepper.distanceToGo = ");
          Serial.println(stepper.distanceToGo());
        }
        // Restart INQ
        BTserial.println("AT+INQ");
        total = 0;
        initMessage();
      }
      else
      {
        capture = false;
        index2 = 0;
        for (index = 0; index < 30; index++)
        {
          if (!capture)
          {
            if (lineBuffer[index] == ':')
            {
              capture = true;
            }
          }
          else
          {
            subBuffer[index2] = lineBuffer[index];
            if (lineBuffer[index] == ',')
            {
              subBuffer[index2] = 0;
              break;
            }
            index2++;
          }
        }
        index = 0;

        // Add this line buffer
        String str((char*)subBuffer);

        if (send.indexOf(str) <= 0)
        {
          // If not first then add comma
          if (total > 0)
          {
            send += ",";
          }

          send += "\"";
          send += str;
          send += "\"";
          // Keep count
          total++;
        }
      }
    }
  }
}

void loop() {
  unsigned long currentMillis = millis();

  if ((currentMillis - previousMillis) >= period) {

    motorOn = !motorOn;

    // reset the motor position
    if (motorOn && stepper.distanceToGo() == 0) {
      stepper.moveTo(movePos);
    }

    // Don't let the motor run too long without a pause
    if (!motorOn && stepper.isRunning()) {
      timeWithoutDelayCount++;
      if (timeWithoutDelayCount > 4) {
        motorOn = false;
      }
    }
    previousMillis = currentMillis;
  }

  // if the motor should be on, run the business
  if (motorOn) {
    if (stepper.distanceToGo() == 0) {
      stepper.moveTo(-stepper.currentPosition());
    }
    stepper.run();
  } else if (!motorOn) { // if the motor should be off…
    if (stepper.distanceToGo() == 0) { // and it's completed its turn
      timeWithoutDelayCount = 0;
      readBuffer(); // check devices
    } else {
      stepper.run(); // otherwise let it finish its thing!
    }
  }



}
