#include <Arduino.h>
#include <RemoteDebug.h>
#include <MqttController.hpp>
#include <NetworkFunctions.h>
#include <OTAFunctions.h>
#include <RemoteDebugFunctions.h>
#include <INTERVAL.h>
#include <MqttAmbientika.hpp>

#include <BaseConfig.h>

RemoteDebug Debug;

WiFiClient espClient;

PubSubClient pubsubclient(mqtt_server, 1883, espClient);
MqttController mqtt_controller(pubsubclient,device_name,mqtt_user,mqtt_pw);
void mqtt_callback_func(const char* topic, const byte* payload, unsigned int length) { Serial.println("Callback"); mqtt_controller.callback(topic, payload,length); }

MqttAmbientika mqttir(mqtt_controller, MQTT_TOPIC_BASE "#", 16);

void setup(void) {
  pinMode(16, OUTPUT);
  pinMode(2, OUTPUT);
  Serial.begin(115200);	
  if(!startNetwork(ssid1,password1,device_name))
    startNetwork(ssid2,password2,device_name);

E  startOTA(device_name, ota_password);
  beginRemoteDebug(device_name);
  
  // Wait for debug connections
  while(millis()<10000) {
    ArduinoOTA.handle();
    Debug.handle();
    INTERVAL(1000,millis()) {
      debugI("Initial wait %lu, %s",millis(),WiFi.macAddress().c_str());
      Serial.print(".");
    }
  }
  Serial.println();

  pubsubclient.setServer(mqtt_server, 1883);
  pubsubclient.setCallback(mqtt_callback_func);

  mqttir.begin();

  const char* mac = WiFi.macAddress().c_str();
  const char device_id[] = {*(mac+0), *(mac+1), *(mac+3), *(mac+4), *(mac+6), *(mac+7), *(mac+9), *(mac+10), *(mac+12), *(mac+13), *(mac+15), *(mac+16), '\0'};
  mqttir.homeassistantDiscover("luefter_bedroom", &device_id[6]);
}

void loop() {
  if(WiFi.status() == WL_CONNECTED) {
    ArduinoOTA.handle();
    Debug.handle();

    mqtt_controller.handle();
  }
  INTERVAL(1000,millis()) {
    if(WiFi.status() != WL_CONNECTED) {
      WiFi.disconnect();
      WiFi.reconnect();

      if(WiFi.status() == WL_CONNECTED) {
        delay(60000);
        ESP.restart();
      }
    }
    Serial.print("+");
  }

  mqttir.handle();
}