#include <Adafruit_PM25AQI.h>
#include <PubSubClient.h>
#include <WiFiClient.h>

class MqttClient {
  public:
    MqttClient();
    void use_server(const char *server, std::uint16_t port = 1883);
    void use_server(IPAddress server_ip, std::uint16_t port = 1883);

    void publish_pms(const PM25_AQI_Data &);
    void loop();

  private:
    void callback(char *, std::uint8_t *, unsigned int);

  private:
    WiFiClient esp_client;
    PubSubClient mqtt_client;
};