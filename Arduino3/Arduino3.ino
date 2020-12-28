// nrf24_client
//room id 3
#include <SPI.h>
#include <RH_NRF24.h>

// Singleton instance of the radio driver
RH_NRF24 nrf24;
// RH_NRF24 nrf24(8, 7); // use this to be electrically compatible with Mirf
// RH_NRF24 nrf24(8, 10);// For Leonardo, need explicit SS pin
// RH_NRF24 nrf24(8, 7); // For RFM73 on Anarduino Mini

int room_id = 3;
int lightSensorPin = A0;
//const char[] room_id = "room_id";
//const char[] light_sensor = "light_sensor";

void setup() 
{
  Serial.begin(9600);
  while (!Serial) 
    ; // wait for serial port to connect. Needed for Leonardo only
  if (!nrf24.init())
    Serial.println("init failed");
  // Defaults after init are 2.402 GHz (channel 2), 2Mbps, 0dBm
  if (!nrf24.setChannel(1))
    Serial.println("setChannel failed");
  if (!nrf24.setRF(RH_NRF24::DataRate2Mbps, RH_NRF24::TransmitPower0dBm))
    Serial.println("setRF failed");    
}


void loop()
{
  Serial.println("\n");
  Serial.println("Sending to nrf24_server");
  // Send a message to nrf24_server
  uint8_t data1[2];
  
  //int led=1;
  //int motor = 0;
  //int light_sensor = analogRead(lightSensorPin);
  int light_sensor = 532;
  data1[0] = room_id;
  data1[1] = light_sensor;
  nrf24.send(data1, sizeof(data1));
  //nrf24.send(data2, sizeof(data2));
  
  nrf24.waitPacketSent();
  // Now wait for a reply
  uint8_t buf[RH_NRF24_MAX_MESSAGE_LEN];
  uint8_t len = sizeof(buf);
  //Serial.println(RH_NRF24_MAX_MESSAGE_LEN);

  if (nrf24.waitAvailableTimeout(500))
  { 
    // Should be a reply message for us now   
    if(nrf24.recv(buf, &len))
    {
     
      Serial.print("got reply: ");
      if(buf[0] == 3){
        Serial.println();
        Serial.print(buf[0]);
        Serial.print("; ");
        Serial.print(buf[1]);
        Serial.print("; ");
        Serial.print(buf[2]);
        Serial.print("; ");
      }
//      Serial.print("\nTemperature: ");
//      Serial.print(buf[0]);
//      Serial.print("; Humidity: ");
//      Serial.print(buf[1]);
//      Serial.print("; Gas: ");
//      Serial.print(buf[2]);
//      Serial.print(";");
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
