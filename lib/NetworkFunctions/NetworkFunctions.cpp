#include <NetworkFunctions.h>

boolean startNetwork(const char* ssid,const char* password,const char* device_name) {
    Serial.print("Connecting to ");
    Serial.println(ssid);

    WiFi.mode(WIFI_STA);

    WiFi.begin(ssid, password);

    for (uint8_t i = 0; i<50; i++) {
        if(WiFi.waitForConnectResult() == WL_CONNECTED) {
            break;
        }
        delay(100);
    }

    if(WiFi.waitForConnectResult() != WL_CONNECTED) {
        return false;
    }

    WiFi.setAutoReconnect(true);
    WiFi.persistent(true);

    if (MDNS.begin(device_name)) {
        Serial.print("* MDNS responder started. Hostname -> ");
        Serial.println(device_name);
    }

    return true;
}