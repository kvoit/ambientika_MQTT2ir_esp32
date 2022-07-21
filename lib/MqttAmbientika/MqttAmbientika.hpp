#pragma once

#include <Arduino.h>
#include <PubSubClient.h>
#include <MqttController.hpp>
#include <MqttIR.hpp>

enum AmbientikaMode : uint8_t { 
    Manual,
    In,
    Out,
    MaSl,
    SlMa,
    Auto,
    Watch,
    Night,
    Boost,
    AmbientikaModeLAST_ENTRY
};
static const char* AmbientikaMode_s[AmbientikaMode::AmbientikaModeLAST_ENTRY] = {
    "Manual",
    "In",
    "Out",
    "MaSl",
    "SlMa",
    "Auto",
    "Watch",
    "Night",
    "Boost",
};
static const char* AmbientikaMode_v[AmbientikaMode::AmbientikaModeLAST_ENTRY] = {
    "966954AB",
    "9669B44B",
    "966934CB",
    "9669748B",
    "9669F40B",
    "966914EB",
    "9669946B",
    "9669F20D",
    "966912ED",
};

enum AmbientikaHumLevel : uint8_t { 
    H1,
    H2,
    H3,
    AmbientikaHumLevelLAST_ENTRY
};
static const char* AmbientikaHumLevel_s[AmbientikaHumLevel::AmbientikaHumLevelLAST_ENTRY] = {
    "H1",
    "H2",
    "H3",
};
static const char* AmbientikaHumLevel_v[AmbientikaHumLevel::AmbientikaHumLevelLAST_ENTRY] = {
    "966952AD",
    "9669D22D",
    "966932CD",
};

enum AmbientikaVentLevel : uint8_t { 
    V1,
    V2,
    V3,
    AmbientikaVentLevelLAST_ENTRY
};
static const char* AmbientikaVentLevel_s[AmbientikaHumLevel::AmbientikaHumLevelLAST_ENTRY] = {
    "V1",
    "V2",
    "V3",
};
static const char* AmbientikaVentLevel_v[AmbientikaHumLevel::AmbientikaHumLevelLAST_ENTRY] = {
    "966936C9",
    "9669B649",
    "96697689",
};

class MqttAmbientika : public MqttIR
{
public:
    MqttAmbientika(MqttController &mqtt, const char *mqtt_topic, const uint8_t pin_led)
        : MqttIR(mqtt,mqtt_topic,pin_led)
    {}; 

    bool setMode(const char*);
    bool setLevel(const char*);
    bool sendState();

    virtual bool presentMessage(const char *topic,const char *payload);

    void homeassistantDiscover(const char *name, const char *unique_id) {};


protected:
    AmbientikaMode mode = AmbientikaMode::Auto;
    AmbientikaHumLevel hum_level = AmbientikaHumLevel::H3;
    AmbientikaVentLevel vent_level = AmbientikaVentLevel::V3;
};