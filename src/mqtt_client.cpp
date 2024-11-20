#include "mqtt_client.h"

MqttClient::MqttClient() : mqtt_client(esp_client) {}

void MqttClient::use_server(const char *server, std::uint16_t port) {
    mqtt_client.setServer(server, port);
    mqtt_client.setCallback([this](char *, uint8_t *, unsigned int) { this->callback() });
}

void MqttClient::callback(char *, uint8_t *, unsigned int) {}

void MqttClient::connect() {
    while (!mqtt_client.connected()) {
        // Create a random client ID.
        const char *clientId = "m5stack-pmmeter";
        // Attempt to connect.
        if (mqtt_client.connect(clientId)) {
            Serial.printf("\nSuccess\n");
            // Once connected, publish an announcement to the topic.
            mqtt_client.publish("M5Stack", "hello world");
        }
    }
}