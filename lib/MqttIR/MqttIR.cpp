#include <MqttIR.hpp>

#include<RemoteDebug.h>
extern RemoteDebug Debug;

void MqttIR::begin()
{
    pinMode(pin_led, OUTPUT);

    config.channel = RMT_CHANNEL_0;
    config.gpio_num = (gpio_num_t)pin_led;
    config.mem_block_num = 2;
    config.clk_div = 80;
    config.tx_config.loop_en = false;
    config.tx_config.carrier_duty_percent = duty_cycle;
    config.tx_config.carrier_freq_hz = frequency;
    config.tx_config.carrier_level = (rmt_carrier_level_t)1;
    config.tx_config.carrier_en = true;
    config.tx_config.idle_level = (rmt_idle_level_t)0;
    config.tx_config.idle_output_en = true;
    config.rmt_mode = RMT_MODE_TX;
    rmt_driver_install(config.channel, 0, 0);
}

bool MqttIR::presentMessage(const char *topic, const char *payload)
{
    if(!strncmp(topic,this->getMQTTTopic(),baselength)) {
        if(!strcmp(&topic[baselength],"/protocol")) {
            if ( !strcmp(payload, "RAW") )
            {
               prot = RAW;
               return true;
            }
        }
        else if(!strcmp(&topic[baselength],"/frequency")) {
            frequency = atoi(payload);
            return true;
        }
        else if(!strcmp(&topic[baselength],"/code")) {
            return send_hex(payload);
        }
    }
    return false;
}

void MqttIR::homeassistantDiscover(const char *name, const char *unique_id)
{
}

void MqttIR::hex2rmt(const char *hexstr, rmt_item32_t *items) {
    uint8_t charint;
    uint8_t item_i = 0;
    for(uint8_t i = 0; i<strlen(hexstr); i++) {
        charint = hexchar2int(hexstr[i]);
        for(uint8_t j = 0; j<4; j++) {
            if(1<<(3-j) & charint) {
                Serial.print("1");
                items[item_i].duration0 = one[0];
                items[item_i].level0 = 1;
                items[item_i].duration1 = one[1];
                items[item_i].level1 = 0;
            } else {
                Serial.print("0");
                items[item_i].duration0 = zero[0]+1;
                items[item_i].level0 = 1;
                items[item_i].duration1 = zero[1];
                items[item_i].level1 = 0;
            }
            item_i++;
        }
    }
    Serial.println();
}

uint8_t MqttIR::hexchar2int(const char hexchar) {
    if(hexchar=='0') {
        return 0;
    } else if(hexchar=='1') {
        return 1;
    } else if(hexchar=='2') {
        return 2;
    } else if(hexchar=='3') {
        return 3;
    } else if(hexchar=='4') {
        return 4;
    } else if(hexchar=='5') {
        return 5;
    } else if(hexchar=='6') {
        return 6;
    } else if(hexchar=='7') {
        return 7;
    } else if(hexchar=='8') {
        return 8;
    } else if(hexchar=='9') {
        return 9;
    } else if(hexchar=='A') {
        return 10;
    } else if(hexchar=='B') {
        return 11;
    } else if(hexchar=='C') {
        return 12;
    } else if(hexchar=='D') {
        return 13;
    } else if(hexchar=='E') {
        return 14;
    } else if(hexchar=='F') {
        return 15;
    }
    return 0;
}

bool MqttIR::write_next()
{
    if(commands.empty())
        return true;

    if(millis()-last_write < gap)
        return true;

    auto& items_vector = commands.front();
    rmt_item32_t *items = &items_vector[0];

    esp_err_t err = rmt_write_items(config.channel, items, items_vector.size(), false);
    if(err == ESP_OK) {
        Serial.printf("Send successfully!\n");
        commands.pop();
        last_write = millis();
        return true;
    } else {
        Serial.printf("Failed to send items (%d)\n", err);
        return false;
    }
}

bool MqttIR::send_hex(const char* hex) {
    debugI("Sending %s", hex);
    config.tx_config.carrier_duty_percent = duty_cycle;
    config.tx_config.carrier_freq_hz = frequency;
    rmt_config(&config);

    const uint8_t items_n = strlen(hex)*4+2;
    rmt_item32_t items[items_n];
    items[0].duration0 = pre[0];
    items[0].level0 = 1;
    items[0].duration1 = pre[1];
    items[0].level1 = 0;
    items[items_n-1].duration0 = post[0];
    items[items_n-1].level0 = 1;
    items[items_n-1].duration1 = post[1];
    items[items_n-1].level1 = 0;
    hex2rmt(hex, &items[1]);

    std::vector<rmt_item32_t> items_vector(items, items + items_n);
    commands.push(items_vector);
    return true;
}