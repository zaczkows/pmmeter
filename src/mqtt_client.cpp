#include "mqtt_client.h"

const char *clientId = "m5stack-pmmeter";

MqttClient::MqttClient() : mqtt_client(esp_client) {}

void MqttClient::use_server(const char *server, std::uint16_t port) {
    mqtt_client.setServer(server, port);
    mqtt_client.setCallback(
        [this](char *topic, uint8_t *payload, unsigned int length) { this->callback(topic, payload, length); });
}

void MqttClient::use_server(IPAddress server_ip, std::uint16_t port) {
    mqtt_client.setServer(server_ip, port);
    mqtt_client.setCallback(
        [this](char *topic, uint8_t *payload, unsigned int length) { this->callback(topic, payload, length); });
}

void MqttClient::callback(char *, uint8_t *, unsigned int) {}

void MqttClient::loop() {
    if (!mqtt_client.connected()) {
        if (mqtt_client.connect(clientId, "mosquitto", "mosquitto")) {
            Serial.println("Successful connection to mqtt server\n");
        }
    }

    if (mqtt_client.connected()) {
        mqtt_client.loop();
    }
}

void MqttClient::publish_pms(const PM25_AQI_Data &measurements) {
    if (mqtt_client.connected()) {
        String data("{\"pm10_std\":");
        data += measurements.pm10_standard;
        data += ",\"pm25_std\":";
        data += measurements.pm25_standard;
        data += ",\"pm100_std\":";
        data += measurements.pm100_standard;
        data += "}";
        // Serial.printf("Sending: %s\n", data.c_str());
        mqtt_client.publish("m5stack/pmmeter", data.c_str(), true);
    }
}
