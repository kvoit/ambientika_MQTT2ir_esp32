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
    if(mode>7) {
        Serial.println("Not reporting boost mode or reset");
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

bool MqttAmbientika::homeassistantDiscover(const char *name, const char *unique_id) {
    DynamicJsonDocument doc(1024);
    size_t n;

    char discovery_msg[1024]{};  
    char device_buffer[64]{};
    char name_buffer[64]{};
    char unique_id_buffer[64]{};
    
    char discovery_topic[128]{};
    char command_topic[128]{};
    char state_topic[128]{};

    mqtt_controller.handle();
    delay(100);
    mqtt_controller.handle();
    delay(100);

    snprintf(device_buffer, sizeof(device_buffer), "luefter_%s", name);

    // mode select
    doc.clear();

    snprintf(name_buffer, sizeof(name_buffer), "Lueftermodus (%s)", name);
    snprintf(unique_id_buffer, sizeof(name_buffer), "lueftermodus_%s", unique_id);
    snprintf(discovery_topic, sizeof(discovery_topic), "homeassistant/select/ambientika/mode_%s/config", name);

    strncpy(state_topic, topic,(127<baselength)?127:baselength);
    state_topic[(127<baselength)?127:(baselength)] = '\0';
    strncat(state_topic,"/ambientika/mode",127);
    strncpy(command_topic, topic,(127<baselength)?127:baselength);
    command_topic[(127<baselength)?127:(baselength)] = '\0';
    strncat(command_topic,"/ambientika/mode/set",127);

    uint8_t i =0;
    doc["name"] = name_buffer;
    doc["unique_id"] = unique_id_buffer;
    doc["state_topic"] = state_topic;
    doc["command_topic"] = command_topic;
    doc["optimistic"] = false;
    doc["retain"] = true;
    doc["options"][i++] = "Manual";
    doc["options"][i++] = "In";
    doc["options"][i++] = "Out";
    doc["options"][i++] = "MaSl";
    doc["options"][i++] = "SlMa";
    doc["options"][i++] = "Auto";
    doc["options"][i++] = "Watch";
    doc["options"][i++] = "Night";
    doc["options"][i++] = "Boost";
    doc["device"]["name"] = name;
    doc["device"]["identifiers"][0] = unique_id;

    n = serializeJson(doc, discovery_msg);   
    mqtt_controller.handle();
    mqtt_controller.sendMessage(discovery_topic, discovery_msg, true);
    delay(50);
    mqtt_controller.handle();

    // mode level
    doc.clear();

    snprintf(name_buffer, sizeof(name_buffer), "Luefterlevel (%s)", name);
    snprintf(unique_id_buffer, sizeof(name_buffer), "luefterlevel_%s", unique_id);
    snprintf(discovery_topic, sizeof(discovery_topic), "homeassistant/select/ambientika/level_%s_%s/config", name, unique_id);

    strncpy(state_topic, topic,(127<baselength)?127:baselength);
    state_topic[(127<baselength)?127:(baselength)] = '\0';
    strncat(state_topic,"/ambientika/level",127);
    strncpy(command_topic, topic,(127<baselength)?127:baselength);
    command_topic[(127<baselength)?127:(baselength)] = '\0';
    strncat(command_topic,"/ambientika/level/set",127);

    i =0;
    doc["name"] = name_buffer;
    doc["unique_id"] = unique_id_buffer;
    doc["state_topic"] = state_topic;
    doc["command_topic"] = command_topic;
    doc["optimistic"] = false;
    doc["retain"] = true;
    doc["options"][i++] = "1";
    doc["options"][i++] = "2";
    doc["options"][i++] = "3";
    doc["device"]["name"] = name;
    doc["device"]["identifiers"][0] = unique_id;

    n = serializeJson(doc, discovery_msg);   
    mqtt_controller.handle();
    mqtt_controller.sendMessage(discovery_topic, discovery_msg, true);
    delay(50);
    mqtt_controller.handle();

    
    return true;
}