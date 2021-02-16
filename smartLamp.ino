/*
 Version 0.3 - March 06 2018
 Grensom Version
*/ 
/*https://sinric.com/
 * 
 */
#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>
#include <WebSocketsClient.h> //  https://github.com/kakopappa/sinric/wiki/How-to-add-dependency-libraries
#include <ArduinoJson.h> // https://github.com/kakopappa/sinric/wiki/How-to-add-dependency-libraries
#include <StreamString.h>

ESP8266WiFiMulti WiFiMulti;
WebSocketsClient webSocket;
WiFiClient client;

///////////WiFi Setup/////////////

#define MyApiKey "0ca0c3f7-7c81-4670-852a-e3d3c85ed093" // TODO: Change to your sinric API Key. Your API Key is displayed on sinric.com dashboard
#define MySSID "jeniffer" // TODO: Change to your Wifi network SSID
#define MyWifiPassword "49735418" // TODO: Change to your Wifi network password

/////////////////////////////////

#define HEARTBEAT_INTERVAL 300000 // 5 Minutes 

uint64_t heartbeatTimestamp = 0;
bool isConnected = false;

void setPowerStateOnServer(String deviceId, String value);
void setTargetTemperatureOnServer(String deviceId, String value, String scale);

// deviceId is the ID assgined to your smart-home-device in sinric.com dashboard. Copy it from dashboard and paste it here

void webSocketEvent(WStype_t type, uint8_t * payload, size_t length) {
  switch(type) {
    case WStype_DISCONNECTED:
      isConnected = false;    
      Serial.printf("[WSc] Webservice disconnected from sinric.com!\n");
      break;
    case WStype_CONNECTED: {
      isConnected = true;
      Serial.printf("[WSc] Service connected to sinric.com at url: %s\n", payload);
      Serial.printf("Waiting for commands from sinric.com ...\n");        
      }
      break;
    case WStype_TEXT: {
        Serial.printf("[WSc] get text: %s\n", payload);
        // Example payloads

        // For Switch or Light device types
        // {"deviceId": xxxx, "action": "setPowerState", value: "ON"} // https://developer.amazon.com/docs/device-apis/alexa-powercontroller.html

        // For Light device type
        // Look at the light example in github
          
        DynamicJsonBuffer jsonBuffer;
        JsonObject& json = jsonBuffer.parseObject((char*)payload); 
        String deviceId = json ["deviceId"];     
        String action = json ["action"];
        
        if(action == "setPowerState") { // Switch or Light
            String value = json ["value"];
            if(value == "ON") {
                if(deviceId == "5e9159f63fe9eb7fe2516d9f"){
                  turnOnLamp(deviceId);
                }
            } else { // OFF
              if(deviceId == "5e9159f63fe9eb7fe2516d9f"){
                turnOffLamp(deviceId);
              }
            }
        }

        if(action == "SetColor") { // Switch or Light
            String hue = json ["value"]["hue"];
                if(deviceId == "5e9159f63fe9eb7fe2516d9f"){
                  setColorOnLamp(hue);
            }
        }

        else if (action == "SetTargetTemperature") {
            String deviceId = json ["deviceId"];     
            String action = json ["action"];
            String value = json ["value"];
        }

         else if (action == "SetColorTemperature") {
            String value = json ["value"];
                if(deviceId == "5e9159f63fe9eb7fe2516d9f"){
                  setColorOnLamp(value);
            }
        }
        
        else if (action == "test") {
            Serial.println("[WSc] received test command from sinric.com");
        }
      }
      break;
    case WStype_BIN:
      Serial.printf("[WSc] get binary length: %u\n", length);
      break;
  }
}

void setup() {
  Serial.begin(115200);
  
  pinMode(D0, OUTPUT);
  digitalWrite(D0, HIGH);

  
  pinMode(D1, OUTPUT);
  digitalWrite(D1, HIGH);
  
  pinMode(D2, OUTPUT);
  digitalWrite(D2, HIGH);
  
  pinMode(D3, OUTPUT);
  digitalWrite(D3, HIGH);

  pinMode(D5, OUTPUT);
  digitalWrite(D5, HIGH);
  
  pinMode(D6, OUTPUT);
  digitalWrite(D6, HIGH);
  
  pinMode(D7, OUTPUT);
  digitalWrite(D7, HIGH);
  
  WiFiMulti.addAP(MySSID, MyWifiPassword);
  Serial.println();
  Serial.print("Connecting to Wifi: ");
  Serial.println(MySSID);

  // Waiting for Wifi connect
  while(WiFiMulti.run() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  if(WiFiMulti.run() == WL_CONNECTED) {
    Serial.println("");
    Serial.print("WiFi connected. ");
    Serial.print("IP address: ");
    Serial.println(WiFi.localIP());
  }

  // server address, port and URL
  webSocket.begin("iot.sinric.com", 80, "/");

  // event handler
  webSocket.onEvent(webSocketEvent);
  webSocket.setAuthorization("apikey", MyApiKey);
  
  // try again every 5000ms if connection has failed
  webSocket.setReconnectInterval(5000);   // If you see 'class WebSocketsClient' has no member named 'setReconnectInterval' error update arduinoWebSockets
}

void loop() {
  webSocket.loop();
  
  if(isConnected) {
      uint64_t now = millis();
      
      // Send heartbeat in order to avoid disconnections during ISP resetting IPs over night. Thanks @MacSass
      if((now - heartbeatTimestamp) > HEARTBEAT_INTERVAL) {
          heartbeatTimestamp = now;
          webSocket.sendTXT("H");          
      }

      setPowerStateOnServer("5e9159f63fe9eb7fe2516d9f", "OFF");
  }   
}


void turnOnLamp(String deviceId) {
    digitalWrite(D0, LOW);

    digitalWrite(D1, HIGH);
    digitalWrite(D2, HIGH);
    digitalWrite(D3, HIGH);
    digitalWrite(D5, HIGH);
    digitalWrite(D6, HIGH);
    digitalWrite(D7, HIGH);
}

void turnOffLamp(String deviceId) {
    digitalWrite(D0, HIGH);
    
    digitalWrite(D1, HIGH);
    digitalWrite(D2, HIGH);
    digitalWrite(D3, HIGH);
    digitalWrite(D5, HIGH);
    digitalWrite(D6, HIGH);
    digitalWrite(D7, HIGH);
}

void setColorOnLamp(String hue) {
  if (hue == "0") // rojo
  { 
    digitalWrite(D2, LOW);
    digitalWrite(D5, LOW);

    digitalWrite(D0, HIGH);
    digitalWrite(D1, HIGH);
    digitalWrite(D6, HIGH);
    digitalWrite(D3, HIGH);
    digitalWrite(D7, HIGH);
  }
    else if (hue == "120") // verde
  { 
    digitalWrite(D1, LOW);
    digitalWrite(D6, LOW);

    digitalWrite(D0, HIGH);
    digitalWrite(D2, HIGH);
    digitalWrite(D5, HIGH);
    digitalWrite(D3, HIGH);
    digitalWrite(D7, HIGH);
  }

    else if (hue == "240") // azul
  { 
    digitalWrite(D3, LOW);
    digitalWrite(D7, LOW);

    digitalWrite(D0, HIGH);
    digitalWrite(D2, HIGH);
    digitalWrite(D5, HIGH);
    digitalWrite(D1, HIGH);
    digitalWrite(D6, HIGH);
  }

   else if (hue == "4000") // blanco
  { 
    digitalWrite(D3, LOW);
    digitalWrite(D7, LOW);

    digitalWrite(D2, LOW);
    digitalWrite(D5, LOW);
    digitalWrite(D1, LOW);
    digitalWrite(D6, LOW);
  }
  
    Serial.print("HUE color lamp:");
    Serial.println(hue);  
     
}



//** non-mandatory functions **//

// If you are going to use a push button to on/off the switch manually, use this function to update the status on the server
// so it will reflect on Alexa app.
// eg: setPowerStateOnServer("deviceid", "ON")

// Call ONLY If status changed. DO NOT CALL THIS IN loop() and overload the server. 

void setPowerStateOnServer(String deviceId, String value) {
  DynamicJsonBuffer jsonBuffer;
  JsonObject& root = jsonBuffer.createObject();
  root["deviceId"] = deviceId;
  root["action"] = "setPowerState";
  root["value"] = value;
  StreamString databuf;
  root.printTo(databuf);
  
  webSocket.sendTXT(databuf);
}

//eg: setPowerStateOnServer("deviceid", "CELSIUS", "25.0")

// Call ONLY If status changed. DO NOT CALL THIS IN loop() and overload the server. 

void setTargetTemperatureOnServer(String deviceId, String value, String scale) {
  DynamicJsonBuffer jsonBuffer;
  JsonObject& root = jsonBuffer.createObject();
  root["action"] = "SetTargetTemperature";
  root["deviceId"] = deviceId;
  
  JsonObject& valueObj = root.createNestedObject("value");
  JsonObject& targetSetpoint = valueObj.createNestedObject("targetSetpoint");
  targetSetpoint["value"] = value;
  targetSetpoint["scale"] = scale;
   
  StreamString databuf;
  root.printTo(databuf);
  
  webSocket.sendTXT(databuf);
}
