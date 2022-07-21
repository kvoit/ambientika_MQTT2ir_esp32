#include<MqttAmbientika.hpp>

#include<RemoteDebug.h>
extern RemoteDebug Debug;

bool MqttAmbientika::presentMessage(const char *topic, const char *payload)
{
    if(!strncmp(topic,this->getMQTTTopic(),baselength)) {
        if(!strcmp(&topic[baselength],"/ambientika/mode/set")) {
            Serial.println("Found mode");
            setMode(payload);
            return true;
        }
        else if(!strcmp(&topic[baselength],"/ambientika/level/set")) {
            setLevel(payload);
            return true;
        }
    } 
    return MqttIR::presentMessage(topic, payload);
}

bool MqttAmbientika::setMode(const char *payload) {
    uint8_t newmode = AmbientikaMode::AmbientikaModeLAST_ENTRY;
    for(uint8_t i = 0; i<AmbientikaMode::AmbientikaModeLAST_ENTRY; i++) {
        if(!strcmp(AmbientikaMode_s[i],payload)) {
            newmode = i;
        }
    }
    
    if(newmode == AmbientikaMode::AmbientikaModeLAST_ENTRY) {
        Serial.println("Could not find mode");
        return false;
    }

    mode = static_cast<AmbientikaMode>(newmode);
    return sendState();
}

bool MqttAmbientika::setLevel(const char* payload) {
    uint8_t newlevel = atoi(payload)-1;
    if(mode<5) {
        if(newlevel >= AmbientikaVentLevel::AmbientikaVentLevelLAST_ENTRY) {
            newlevel = AmbientikaVentLevel::AmbientikaVentLevelLAST_ENTRY-1;
        }
        vent_level = static_cast<AmbientikaVentLevel>(newlevel);

        return sendState();
    } else if(mode<7) {
        if(newlevel >= AmbientikaHumLevel::AmbientikaHumLevelLAST_ENTRY) {
            newlevel = AmbientikaHumLevel::AmbientikaHumLevelLAST_ENTRY-1;
        }
        hum_level = static_cast<AmbientikaHumLevel>(newlevel);
        
        return sendState();
    }
    return false;
}

bool MqttAmbientika::sendState() {
    char buffer[128];
    char buffer2[2];
    send_hex(AmbientikaMode_v[mode]);

    bool mode_sent = true;
    if(mode==8) {
        Serial.println("Not reporting boost mode");
        return true;
    } else {
        Serial.println("Sending new stat to MQTT");
        strncpy(buffer,topic,(127<baselength)?127:baselength);
        buffer[(127<baselength)?127:(baselength)] = '\0';
        strncat(buffer,"/ambientika/mode",127);
        mode_sent = mqtt_controller.sendMessage(buffer, AmbientikaMode_s[mode]);
    }

    if(mode<5) {
        send_hex(AmbientikaVentLevel_v[vent_level]);

    } else if(mode<7) {
        send_hex(AmbientikaHumLevel_v[hum_level]);
    } else {
        return mode_sent;
    }

    strncpy(buffer,topic,(127<baselength)?127:baselength);
    buffer[(127<baselength)?127:(baselength)] = '\0';
    strncat(buffer,"/ambientika/level",127);
    return mqtt_controller.sendMessage(buffer, itoa(vent_level+1, buffer2, 10));
}