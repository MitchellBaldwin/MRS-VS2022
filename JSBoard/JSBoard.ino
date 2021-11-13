/*
  Joystick remote controller board with Arduino Pro Mini (ATMega328P (5V, 16 MHz))

  Adafruit 4x4 keypad Trellis at I2C address 0X70

*/


#include <Wire.h>
#include <Adafruit_Trellis.h>
#include <Tasks.h>

int DrvJSX, DrvJSY, PTJSX, PTJSY;

#define DRVJSBTNPIN 7
#define PTJSBTNPIN 8

 /***************************************************
   The Adafruit Trellis parts of this example show reading buttons and 
   setting/clearing buttons in a loop
   "momentary" mode has the LED light up only when a button is pressed
   "latching" mode lets you turn the LED on/off when pressed

   Up to 8 matrices can be used but this example will show 4 or 1
  ****************************************************/

#define MOMENTARY 0
#define LATCHING 1
  // set the mode here
#define MODE LATCHING 

Adafruit_Trellis matrix0 = Adafruit_Trellis();

// uncomment the below to add 3 more matrices
/*
Adafruit_Trellis matrix1 = Adafruit_Trellis();
Adafruit_Trellis matrix2 = Adafruit_Trellis();
Adafruit_Trellis matrix3 = Adafruit_Trellis();
// you can add another 4, up to 8
*/

// Just one
Adafruit_TrellisSet trellis = Adafruit_TrellisSet(&matrix0);
// or use the below to select 4, up to 8 can be passed in
//Adafruit_TrellisSet trellis =  Adafruit_TrellisSet(&matrix0, &matrix1, &matrix2, &matrix3);

// set to however many you're working with here, up to 8
#define NUMTRELLIS 1

#define numKeys (NUMTRELLIS * 16)

// Connect Trellis Vin to 5V and Ground to ground.
// Connect the INT wire to pin #A2 (can change later!)
#define INTPIN A2
// Connect I2C SDA pin to your Arduino SDA line
// Connect I2C SCL pin to your Arduino SCL line
// All Trellises share the SDA, SCL and INT pin! 
// Even 8 tiles use only 3 wires max

void blink(void) {
  digitalWrite(LED_BUILTIN, !digitalRead(LED_BUILTIN));
}

void ReadTrellis(void)
{
  if (MODE == MOMENTARY) {
    // If a button was just pressed or released...
    if (trellis.readSwitches()) {
      // go through every button
      for (uint8_t i = 0; i < numKeys; i++) {
        // if it was pressed, turn it on
        if (trellis.justPressed(i)) {
          Serial.print("v"); Serial.println(i);
          trellis.setLED(i);
        }
        // if it was released, turn it off
        if (trellis.justReleased(i)) {
          Serial.print("^"); Serial.println(i);
          trellis.clrLED(i);
        }
      }
      // tell the trellis to set the LEDs we requested
      trellis.writeDisplay();
    }
  }

  if (MODE == LATCHING) {
    // If a button was just pressed or released...
    if (trellis.readSwitches()) {
      // go through every button
      for (uint8_t i = 0; i < numKeys; i++) {
        // if it was pressed...
        if (trellis.justPressed(i)) {
          Serial.print("v"); Serial.println(i);
          // Alternate the LED
          if (trellis.isLED(i))
            trellis.clrLED(i);
          else
            trellis.setLED(i);
        }
      }
      // tell the trellis to set the LEDs we requested
      trellis.writeDisplay();
    }
  }
}

void ReadJoysticks()
{
  DrvJSX = analogRead(A0);
  DrvJSY = analogRead(A1);
  PTJSX = analogRead(A6);
  PTJSY = analogRead(A7);

  if (!digitalRead(DRVJSBTNPIN))
  {
    trellis.setLED(0);
    trellis.writeDisplay();
  }
}

void setup() {
  // initialize digital LED_BUILTIN pin as an output
  pinMode(LED_BUILTIN, OUTPUT);
  // Configure task scheduler to blink onboard LED
  Tasks_Init();
  Tasks_Add((Task)blink, 500, 0);
  Tasks_Add((Task)ReadTrellis, 30, 0);
  Tasks_Add((Task)ReadJoysticks, 100, 0);
  Tasks_Start();

  Serial.begin(115200);
  while (!Serial);
  Serial.println("JS Board");
  Serial.println(sizeof(DrvJSX));

  // Initialize joystick buttons
  pinMode(DRVJSBTNPIN, INPUT);
  digitalWrite(DRVJSBTNPIN, HIGH);
  pinMode(PTJSBTNPIN, INPUT);
  digitalWrite(PTJSBTNPIN, HIGH);

  // Initialize Trellis INT pin; requires a pullup
  pinMode(INTPIN, INPUT);
  digitalWrite(INTPIN, HIGH);

  // begin() with the addresses of each panel in order
  // I find it easiest if the addresses are in order
  trellis.begin(0x70);  // only one
  // trellis.begin(0x70, 0x71, 0x72, 0x73);  // or four!

  // light up all the LEDs in order
  for (uint8_t i = 0; i < numKeys; i++) {
    trellis.setLED(i);
    trellis.writeDisplay();
    delay(50);
  }
  // then turn them off
  for (uint8_t i = 0; i < numKeys; i++) {
    trellis.clrLED(i);
    trellis.writeDisplay();
    delay(50);
  }

}                                                                    

// the loop function runs over and over again forever
void loop() {
  //digitalWrite(13, HIGH);   // turn the LED on (HIGH is the voltage level)
  //delay(1000);              // wait for a second
  //digitalWrite(13, LOW);    // turn the LED off by making the voltage LOW
  //delay(1000);              // wait for a second
}
