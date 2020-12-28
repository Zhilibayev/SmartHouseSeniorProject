// nrf24_server
// room_id 1
#include <SPI.h>
#include <RH_NRF24.h>
#include <ArduinoJson.h>
#include <dht.h>
//#include <Servo.h>
#define dht_apin A4

dht DHT;

//#include <Servo.h>

//Servo myservo;
RH_NRF24 nrf24;

int PIRPin = A2;
int LightPin = A0;
long startTime = millis();

int old_light_status = 0;
int old_servo_status = 0;
int ServoPin1 = 7;
int LEDPin = 5;
int pos = 0;

int GasPin = A1;
int HumidityPin = 6;
//int PIRPin = 2;
//int LightPin = A0;

//int HumidityValue = 0;
//int LightValue = 0;
//int PIRValue = 0;
//int GasValue = 0;


int servoPin = 7; 

void executeCommand(String command){
  int pos;
  
  if (command.charAt(0) - 48 != 1) {
    return;
  }
  int servo_status = command.charAt(2) - 48;
  int light_status = command.charAt(4) - 48;
  
  if (light_status == 0 && old_light_status != light_status){
    digitalWrite(LEDPin, LOW);  
  }
  if (light_status == 1 && old_light_status != light_status){
    digitalWrite(LEDPin, HIGH);
  }

//  if (servo_status == 1 && old_servo_status != servo_status){
//    for (int pos = 0; pos <= 180; pos += 1){
//      myservo.write(pos);
//      delay(15);
//    }
//  }else if (servo_status == 0 && old_servo_status != servo_status){
//    for (int pos = 180; pos >= 0; pos -= 1){
//      myservo.write(pos);
//      delay(15);
//    }
//  }

  old_light_status = light_status;
  old_servo_status = servo_status;
}

void setup() 
{
  
  Serial.begin(115200);
  while (!Serial); // wait for serial port to connect. Needed for Leonardo only
  if (!nrf24.init())
    Serial.println("init failed");// Defaults after init are 2.402 GHz (channel 2), 2Mbps, 0dBm
  if (!nrf24.setChannel(1))
    Serial.println("setChannel failed");
  if (!nrf24.setRF(RH_NRF24::DataRate2Mbps, RH_NRF24::TransmitPower0dBm))
    Serial.println("setRF failed");
    
  //myservo.attach(servoPin);    
}

void sendJson(int PIRValue, int LightValue, int HumidityValue, int GasValue, int id){
  String temp = "";
  
  DynamicJsonBuffer jsonBuffer;
  JsonObject& root = jsonBuffer.createObject();
  JsonObject& room = root.createNestedObject("sensors");
  
  room["pir"] = String(PIRValue);
  room["light"] = String(LightValue);
  room["humidity"] = String(HumidityValue);
  room["gas"] = String(GasValue);

  
  if (id == 1){
    root["room_name"] = "kitchen"; 
  }else if (id == 2){
    root["room_name"] = "bedroom"; 
  }else if (id == 3){
    root["room_name"] = "common"; 
  }
  
  root["home_id"] = String("3");
  root.printTo(temp);
  Serial.println(temp);
  
}

int counter = 0;
void loop()
{
  
  if(nrf24.available())
  {
    Serial.println("Hello My friends!!!!!"); 
    //Serial.println("Hello");
    //Should be a message for us now   
    //max length is 27 chars
    uint8_t buf[RH_NRF24_MAX_MESSAGE_LEN];
    uint8_t len = sizeof(buf);
    if (millis() - startTime >= 1000) {
        DHT.read11(dht_apin);
        int humidity_sensor = DHT.humidity;
        sendJson(analogRead(PIRPin)/4, analogRead(LightPin)/4, humidity_sensor, analogRead(GasPin)/4,  1);
        startTime = millis();
    }
    if(nrf24.recv(buf, &len))
    {
      //Serial.println(buf[0]);
      sendJson(buf[1], buf[2], buf[3], buf[4], buf[0]);
      if (Serial.available()){
        String inData = Serial.readStringUntil('*');
        executeCommand(inData.substring(0,6));
        
        delay(25);
        uint8_t data[3];
        
        data[0] = inData[6] - 48;
        data[1] = inData[8] - 48;
        data[2] = inData[10] - 48;
        nrf24.send(data, sizeof(data));
        nrf24.waitPacketSent();
        
        delay(25);
        uint8_t data2[3];
        
        data2[0] = inData[12] - 48;
        data2[1] = inData[14] - 48;
        data2[2] = inData[16] - 48;
        nrf24.send(data2, sizeof(data2));
        nrf24.waitPacketSent();
      }
    }
    else
    {
      Serial.println("recv failed");
    }
  }else{
      Serial.println("not available"); 
  }
}
