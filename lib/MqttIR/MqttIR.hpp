#pragma once

#include <Arduino.h>
#include <PubSubClient.h>
#include <MqttController.hpp>
#include <queue>
#include <vector>

extern "C" {
#include "driver/rmt.h"
}

class MqttIR : public MqttListener
{
public:
    MqttIR(MqttController &mqtt, const char *mqtt_topic, const uint8_t pin_led)
        : MqttListener(mqtt,mqtt_topic), pin_led(pin_led)
    {
        last_write = millis();
    }; 

    void begin();
    void handle() { write_next(); };

    virtual bool presentMessage(const char *topic,const char *payload);

    bool send_hex(const char* hex);   

    void homeassistantDiscover(const char *name, const char *unique_id);

    void hex2rmt(const char *hexstr, rmt_item32_t *items);
    uint8_t hexchar2int(const char hexchar);

protected:
    const uint8_t pin_led = 0;
    bool write_hex(const char *hex);

    bool write_next(void);

    std::queue<std::vector<rmt_item32_t>> commands;

    uint32_t frequency = 38000;
    uint8_t duty_cycle = 50;
    uint32_t pre[2] = { 9100, 4662 };
    uint32_t post[2] = { 587, 657 };
    uint32_t zero[2] = { 587, 657 };
    uint32_t one[2] = { 587, 1772 };
    size_t length = 49;
    uint32_t gap = 800;    
    uint32_t last_write = 0;    

    enum protocol { RAW };
    protocol prot = RAW;

    rmt_config_t config;
};