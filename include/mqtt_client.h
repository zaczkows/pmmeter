#include <PubSubClient.h>
#include <WiFiClient.h>

class MqttClient {
  public:
    MqttClient();
    void use_server(const char *server, std::uint16_t port = 1883);
    void connect();

    void loop();

  private:
    void callback(char *, std::uint8_t *, unsigned int);

  private:
    WiFiClient esp_client;
    PubSubClient mqtt_client;
    String mqtt_server_address;
};