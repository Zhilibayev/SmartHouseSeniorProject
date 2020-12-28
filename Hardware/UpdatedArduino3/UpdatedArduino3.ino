// nrf24_client
//room id 3
#include <SPI.h>
#include <RH_NRF24.h>
#include <Servo.h>
Servo myservo;
int servoPin = 7;

// Singleton instance of the radio driver
RH_NRF24 nrf24;
// RH_NRF24 nrf24(8, 7); // use this to be electrically compatible with Mirf
// RH_NRF24 nrf24(8, 10);// For Leonardo, need explicit SS pin
// RH_NRF24 nrf24(8, 7); // For RFM73 on Anarduino Mini

int ledPin = 5;
int pirPin = A2;
int lightSensorPin = A0;
int humidityPin = 6;
int gasPin = A1;
int room_id = 2;

int old_light_status = 0;
int old_servo_status = 0;
int pos = 0;

//const char[] room_id = "room_id";
//const char[] light_sensor = "light_sensor";

void executeCommand(int servo_status, int light_status){
  int pos;
  
  if (light_status == 0 && old_light_status != light_status){
    digitalWrite(ledPin, LOW);  
  }
  if (light_status == 1 && old_light_status != light_status){
    digitalWrite(ledPin, HIGH);
  }

  if (servo_status == 1 && old_servo_status != servo_status){
    for (int pos = 0; pos <= 180; pos += 1){
      myservo.write(pos);
      delay(15);
    }
  }else if (servo_status == 0 && old_servo_status != servo_status){
    for (int pos = 180; pos >= 0; pos -= 1){
      myservo.write(pos);
      delay(15);
    }
  }

  old_light_status = light_status;
  old_servo_status = servo_status;
}



void setup() 
{
  Serial.begin(115200);
  pinMode(ledPin, OUTPUT);
  while (!Serial) 
    ; // wait for serial port to connect. Needed for Leonardo only
  if (!nrf24.init())
    Serial.println("init failed");
  // Defaults after init are 2.402 GHz (channel 2), 2Mbps, 0dBm
  if (!nrf24.setChannel(1))
    Serial.println("setChannel failed");
  if (!nrf24.setRF(RH_NRF24::DataRate2Mbps, RH_NRF24::TransmitPower0dBm))
    Serial.println("setRF failed");    
  myservo.attach(servoPin);
  
}


void loop()
{
  Serial.println("\n");
  Serial.println("Sending to nrf24_server");
  // Send a message to nrf24_server
  uint8_t data1[3];

  int pir_sensor = analogRead(pirPin)/4;
  int light_sensor = analogRead(lightSensorPin)/4;
  int humidity_sensor = digitalRead(humidityPin);
  int gas_sensor = analogRead(gasPin);
  
  data1[0] = room_id;
  data1[1] = pir_sensor;
  data1[2] = light_sensor;
  data1[3] = humidity_sensor;
  data1[4] = gas_sensor;
  
  nrf24.send(data1, sizeof(data1));
  nrf24.waitPacketSent();
  // Now wait for a reply
  uint8_t buf[RH_NRF24_MAX_MESSAGE_LEN];
  uint8_t len = sizeof(buf);

  if (nrf24.waitAvailableTimeout(1000))
  { 
    // Should be a reply message for us now   
    if(nrf24.recv(buf, &len))
    {
      if(buf[0] == room_id){
        executeCommand(buf[1], buf[2]);
//        if(buf[1] == 1){
//          digitalWrite(ledPin, HIGH);  
//        }else if(buf[1] == 0){
//          digitalWrite(ledPin, LOW);
//        }
      }
    }
    else
    {
      Serial.println("recv failed");
    }
  }
  else
  {
    Serial.println("No reply, is nrf24_server running?");
  }
  delay(400);
}
