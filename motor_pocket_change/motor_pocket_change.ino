/*
  This is a test sketch for the Adafruit assembled Motor Shield for Arduino v2
  It won't work with v1.x motor shields! Only for the v2's with built in PWM
  control

  For use with the Adafruit Motor Shield v2
  ---->  http://www.adafruit.com/products/1438
*/

#include <Wire.h>
#include <Adafruit_MotorShield.h>

#include <SoftwareSerial.h>

SoftwareSerial BTserial(10, 11); // RX | TX

// Create the motor shield object with the default I2C address
Adafruit_MotorShield AFMS = Adafruit_MotorShield();
// Or, create it with a different I2C address (say for stacking)
// Adafruit_MotorShield AFMS = Adafruit_MotorShield(0x61);

// Select which 'port' M1, M2, M3 or M4. In this case, M1
Adafruit_DCMotor *myMotor = AFMS.getMotor(1);
// You can also make another motor on port M2
Adafruit_DCMotor *myOtherMotor = AFMS.getMotor(2);

char c = ' ';
char lineBuffer[100];
char subBuffer[30];
int index = 0;
int index2 = 0;
int total = 0;
bool capture = false;
String send = "";

unsigned long previousMillis = 0;  //will store last time LED was blinked
unsigned long period = 2000;
unsigned long motorSpeeds[] = {50, 50};

boolean phaseOneOn = true;
boolean phaseTwoOn = false;
boolean phaseThreeOn = false;

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
  pinMode(8, OUTPUT);
  digitalWrite(8, HIGH);
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

  AFMS.begin();  // create with the default frequency 1.6KHz
  
  myMotor->setSpeed(motorSpeeds[0]);
  myOtherMotor->setSpeed(motorSpeeds[1]);
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
          period = map(total, 0, 50, 45000, 2000);
          Serial.print("Period is now ");
          Serial.println(period);
          motorSpeeds[0] = map(total, 0, 50, 65, 100);
          motorSpeeds[1] = random(motorSpeeds[0] - 10, motorSpeeds[1] + 10);
          Serial.print("Motor Speed 1 is now ");
          Serial.println(motorSpeeds[0]);
          Serial.print("Motor Speed 2 is now ");
          Serial.println(motorSpeeds[1]);
          Serial.println(send);
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
  readBuffer();
  if (currentMillis - previousMillis >= period) {
    if (phaseOneOn) {
      Serial.println("Phase One!");
      myMotor->run(FORWARD);
      myOtherMotor->run(BACKWARD);
      phaseOneOn = false;
      phaseTwoOn = true;
    } else if (phaseTwoOn) {
      Serial.println("Phase Two!");
      myMotor->run(RELEASE);
      myOtherMotor->run(RELEASE);
      phaseTwoOn = false;
      phaseThreeOn = true;
    } else if (phaseThreeOn) {
      Serial.println("Phase Three!");
      myMotor->run(BACKWARD);  // run in reverse
      myOtherMotor->run(FORWARD);
      phaseThreeOn = false;
      phaseOneOn = true;
    }
    myMotor->setSpeed(motorSpeeds[0]);
    myOtherMotor->setSpeed(motorSpeeds[1]);
    previousMillis = currentMillis;   
  }

  
}
