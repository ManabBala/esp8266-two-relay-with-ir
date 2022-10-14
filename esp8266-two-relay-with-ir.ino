/**********************************************************************************
 *  Preferences--> Aditional boards Manager URLs : 
 *  https://dl.espressif.com/dl/package_esp32_index.json, http://arduino.esp8266.com/stable/package_esp8266com_index.json
 *  
 *  Download Board ESP8266 NodeMCU : https://github.com/esp8266/Arduino
 *
 *  Download the libraries 
 *  Blynk Library (1.1.0):  https://github.com/blynkkk/blynk-library
 *  IRremote Library (3.6.1): https://github.com/Arduino-IRremote/Arduino-IRremote
 *  DHT Library (1.4.3): https://github.com/adafruit/DHT-sensor-library
 *  Preferences Library (2.0.0): https://github.com/vshymanskyy/Preferences
 **********************************************************************************/

/* Fill-in your Template ID (only if using Blynk.Cloud) */
#define BLYNK_TEMPLATE_ID "TMPLeWx8qppS"
#define BLYNK_DEVICE_NAME "Tapu Demo Board"
#define BLYNK_AUTH_TOKEN "OG9QSUC1ffhLiSKEwayS4TuX8PBV7rYe"

// Your WiFi credentials.
// Set password to "" for open networks.
char ssid[] = "Tapu(2.4G)";
char pass[] = "nanditaMa@2807";

//Update the HEX code of IR Remote buttons 0x<HEX CODE>
#define IR_Button_1   0x80BF49B6
#define IR_Button_2   0x80BFC936
#define IR_All_Off    0x80BF3BC4

//#define BLYNK_PRINT Serial
#include <ESP8266WiFi.h>        
#include <BlynkSimpleEsp8266.h>
#include <IRremote.h>
#include <Preferences.h>
Preferences pref;


#define IR_RECV_PIN 2 // D4 (IR receiver pin)


IRrecv irrecv(IR_RECV_PIN);
decode_results results;
  
// define the GPIO connected with Relays and switches
#define RelayPin1 5 //D1  
#define RelayPin2 4  //D2

#define SwitchPin1 12 //D6
#define SwitchPin2 13 //D7

#define wifiLed   16   //D0

//Change the virtual pins according the rooms
#define VPIN_BUTTON_1    V1 
#define VPIN_BUTTON_2    V2

#define VPIN_BUTTON_C    V5

// Relay State
bool toggleState_1 = LOW; //Define integer to remember the toggle state for relay 1
bool toggleState_2 = LOW; //Define integer to remember the toggle state for relay 2

// Switch State
bool SwitchState_1 = LOW;
bool SwitchState_2 = LOW;

int wifiFlag = 0;

char auth[] = BLYNK_AUTH_TOKEN;

BlynkTimer timer;

// When App button is pushed - switch the state

BLYNK_WRITE(VPIN_BUTTON_1) {
  toggleState_1 = param.asInt();
  digitalWrite(RelayPin1, !toggleState_1);
  pref.putBool("Relay1", toggleState_1);
}

BLYNK_WRITE(VPIN_BUTTON_2) {
  toggleState_2 = param.asInt();
  digitalWrite(RelayPin2, !toggleState_2);
  pref.putBool("Relay2", toggleState_2);
}

BLYNK_WRITE(VPIN_BUTTON_C) {
  all_SwitchOff();
}

void all_SwitchOff(){
  toggleState_1 = 0; digitalWrite(RelayPin1, HIGH); pref.putBool("Relay1", toggleState_1); Blynk.virtualWrite(VPIN_BUTTON_1, toggleState_1); delay(100);
  toggleState_2 = 0; digitalWrite(RelayPin2, HIGH); pref.putBool("Relay2", toggleState_2); Blynk.virtualWrite(VPIN_BUTTON_2, toggleState_2); delay(100);
}

void checkBlynkStatus() { // called every 2 seconds by SimpleTimer

  bool isconnected = Blynk.connected();
  if (isconnected == false) {
    wifiFlag = 1;
    Serial.println("Blynk Not Connected");
    digitalWrite(wifiLed, HIGH);
  }
  if (isconnected == true) {
    wifiFlag = 0;
    digitalWrite(wifiLed, LOW);
    //Serial.println("Blynk Connected");
  }
}

BLYNK_CONNECTED() {
  // update the latest state to the server
  Blynk.virtualWrite(VPIN_BUTTON_1, toggleState_1);
  Blynk.virtualWrite(VPIN_BUTTON_2, toggleState_2);
}

void ir_remote(){
  if (irrecv.decode(&results)) {
      switch(results.value){
          case IR_Button_1:  
            digitalWrite(RelayPin1, toggleState_1);
            toggleState_1 = !toggleState_1;
            pref.putBool("Relay1", toggleState_1);
            Blynk.virtualWrite(VPIN_BUTTON_1, toggleState_1);
            delay(100);            
            break;
          case IR_Button_2:  
            digitalWrite(RelayPin2, toggleState_2);
            toggleState_2 = !toggleState_2;
            pref.putBool("Relay2", toggleState_2);
            Blynk.virtualWrite(VPIN_BUTTON_2, toggleState_2);
            delay(100);            
            break;
          case IR_All_Off:
            all_SwitchOff();  
            break;
          default : break;         
        }   
        Serial.println(results.value, HEX);    
        irrecv.resume();   
  } 
}

void getRelayState()
{
  //Serial.println("reading data from NVS");
  toggleState_1 = pref.getBool("Relay1", 0);
  digitalWrite(RelayPin1, !toggleState_1); 
  Blynk.virtualWrite(VPIN_BUTTON_1, toggleState_1);
  delay(200);
  toggleState_2 = pref.getBool("Relay2", 0);
  digitalWrite(RelayPin2, !toggleState_2); 
  Blynk.virtualWrite(VPIN_BUTTON_2, toggleState_2);
  delay(200);
}

void manual_control()
{
  if (digitalRead(SwitchPin1) == LOW && SwitchState_1 == LOW) {
    digitalWrite(RelayPin1, LOW);
    toggleState_1 = HIGH;
    SwitchState_1 = HIGH;
    pref.putBool("Relay1", toggleState_1);
    Blynk.virtualWrite(VPIN_BUTTON_1, toggleState_1);
    Serial.println("Switch-1 on");
  }
  if (digitalRead(SwitchPin1) == HIGH && SwitchState_1 == HIGH) {
    digitalWrite(RelayPin1, HIGH);
    toggleState_1 = LOW;
    SwitchState_1 = LOW;
    pref.putBool("Relay1", toggleState_1);
    Blynk.virtualWrite(VPIN_BUTTON_1, toggleState_1);
    Serial.println("Switch-1 off");
  }
  if (digitalRead(SwitchPin2) == LOW && SwitchState_2 == LOW) {
    digitalWrite(RelayPin2, LOW);
    toggleState_2 = HIGH;
    SwitchState_2 = HIGH;
    pref.putBool("Relay2", toggleState_2);
    Blynk.virtualWrite(VPIN_BUTTON_2, toggleState_2);
    Serial.println("Switch-2 on");
  }
  if (digitalRead(SwitchPin2) == HIGH && SwitchState_2 == HIGH) {
    digitalWrite(RelayPin2, HIGH);
    toggleState_2 = LOW;
    SwitchState_2 = LOW;
    pref.putBool("Relay2", toggleState_2);
    Blynk.virtualWrite(VPIN_BUTTON_2, toggleState_2);
    Serial.println("Switch-2 off");
  }
}  

void setup()
{
  Serial.begin(9600);
  //Open namespace in read-write mode
  pref.begin("Relay_State", false);
  
  pinMode(RelayPin1, OUTPUT);
  pinMode(RelayPin2, OUTPUT);

  pinMode(wifiLed, OUTPUT);

  pinMode(SwitchPin1, INPUT_PULLUP);
  pinMode(SwitchPin2, INPUT_PULLUP);

  //During Starting all Relays should TURN OFF
  digitalWrite(RelayPin1, !toggleState_1);
  digitalWrite(RelayPin2, !toggleState_2);

  irrecv.enableIRIn(); // Enabling IR sensor

  digitalWrite(wifiLed, HIGH);

  //Blynk.begin(auth, ssid, pass);
  WiFi.begin(ssid, pass);
  timer.setInterval(2000L, checkBlynkStatus); // check if Blynk server is connected every 2 seconds
  Blynk.config(auth);
  delay(1000);
  
  getRelayState(); // Get the last state of Relays
  
  Blynk.virtualWrite(VPIN_BUTTON_1, toggleState_1);
  Blynk.virtualWrite(VPIN_BUTTON_2, toggleState_2);
}

void loop()
{ 
  manual_control();
  ir_remote(); //IR remote Control
  Blynk.run();
  timer.run();
}
