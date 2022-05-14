#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <OneWire.h>
#include <DHT.h>
#define MAX_MSG_LEN (128)

#define DHTPIN D8
#define DHTTYPE DHT11   // DHT 11
DHT dht(DHTPIN, DHTTYPE);
float humi_read = 0, temp_read = 0;

int DS18S20_Pin = 2; //DS18S20 Signal pin on digital 2
//Temperature chip I/O
OneWire ds(DS18S20_Pin);  // on digital pin 3

const char* ssid = "YOUR_WIFI_SSID_HERE";
const char* password = "YOUR_WIFI_PSWD_HERE";
// const char* serverHostname = "rpi4b";
// MQTT Server IP Address fill in below
const IPAddress serverIPAddress(192,168,0,10); 
const char* topic = "temp/humi";  // MQTT Topic
WiFiClient espClient;
PubSubClient client(espClient);

void connectWifi(){
  delay(10);
  Serial.printf("\nConnecting to %s\n", ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED){
    delay(250);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("WiFi connected on IP Address ");
  Serial.println(WiFi.localIP());
}

void connectMQTT(){
  while(!client.connected()){
    String clientId = "ESP8266-";
    clientId += String(random(0xffff),HEX);
    Serial.printf("MQTT connecting as client %s...\n", clientId.c_str());
    //Attempt to connect
    if(client.connect(clientId.c_str())){
      Serial.println("MQTT connected");
      client.publish(topic, "hello from ESP8266");
      client.subscribe(topic);
      
    }else{
      Serial.printf("MQTT failed, state %s, retrying ...\n",client.state());
      delay(2500);
    }
  }
}

void callback(char *msgTopic, byte* msgPayload, unsigned int msgLength){
  static char message[MAX_MSG_LEN+1];
  if(msgLength > MAX_MSG_LEN){
    msgLength = MAX_MSG_LEN;
  }
  strncpy(message, (char*)msgPayload, msgLength);
  message[msgLength] = '\0';
  Serial.printf("topic %s, message received: %s\n",msgTopic, message);
}

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  
  dht.begin();
  
  connectWifi();
  client.setServer(serverIPAddress, 1883);
  client.setCallback(callback);
}

void loop() {
  // put your main code here, to run repeatedly:
  if (!client.connected()){
    connectMQTT();
  }

  //int ana = analogRead(A0); Read Env Temp & Humidity
  float h = dht.readHumidity();
  float t = dht.readTemperature();

  //GET Soil Temp using DS18B20
  float soilTemperature = getTemp();

  //GET Soil Moisture Percentage
  int soilMoisture = analogRead(A0);
  int soilMoisturePercent = map(soilMoisture,357,170,0,100);
  
  //String str1 = "t:";
  String str_t = String(t);
  String str2 = ",";
  String str_h = String(h);
  String str_soilTemp = String(soilTemperature);
  String str_soilMoisture = String(soilMoisturePercent);
  
  if (isnan(h) || isnan(t) || isnan(soilTemperature))
  {
    Serial.println("Failed to read from DHT/Soil Temp sensor!");
  }
  else
  {
    //str1.concat(str_t);
    //str2.concat(str_h);
    //str1.concat(str2); //return to str1
    str_t.concat(str2);
    str_t.concat(str_h);
    str_t.concat(str2);
    str_t.concat(str_soilTemp);
    str_t.concat(str2);
    str_t.concat(str_soilMoisture);
    
    Serial.print("Humidity: ");
    Serial.print(h);
    Serial.print(" %; Temperature: ");
    Serial.print(t);
    Serial.print(" *C, SoilTemp: ");
    Serial.print(soilTemperature);
    Serial.print(" *C, SoilMoisture(%): ");
    Serial.println(soilMoisturePercent);
    Serial.print("soilMoisture(anaValue): ");
    Serial.println(soilMoisture);
    
    Serial.println(str_t);
  }
  //Serial.println(ana);  //Read data from A0, Output 0-1023 according to the input voltage 0-1.0 V
  
  client.publish(topic, String(str_t).c_str());
  client.loop();
  delay(1000);
}

float getTemp(){
  //returns the temperature from one DS18S20 in DEG Celsius

  byte data[12];
  byte addr[8];

  if ( !ds.search(addr)) {
      //no more sensors on chain, reset search
      ds.reset_search();
      Serial.println("No Sensor!");
      return -1000;
  }

  if ( OneWire::crc8( addr, 7) != addr[7]) {
      Serial.println("CRC is not valid!");
      return -1000;
  }

  if ( addr[0] != 0x10 && addr[0] != 0x28) {
      Serial.print("Device is not recognized");
      return -1000;
  }

  ds.reset();
  ds.select(addr);
  ds.write(0x44,1); // start conversion, with parasite power on at the end

  byte present = ds.reset();
  ds.select(addr);    
  ds.write(0xBE); // Read Scratchpad

  
  for (int i = 0; i < 9; i++) { // we need 9 bytes
    data[i] = ds.read();
  }
  
  ds.reset_search();
  
  byte MSB = data[1];
  byte LSB = data[0];

  float tempRead = ((MSB << 8) | LSB); //using two's compliment
  float TemperatureSum = tempRead / 16;
  
  return TemperatureSum;
  
}
