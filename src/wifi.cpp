#include "wifi.h"

#include <ArduinoJson.h>
#include <M5Stack.h>
#include <Preferences.h>
#include <WiFi.h>

static char endOfHeaders[] = "\r\n\r\n";

static Preferences preferences; // wifi config store
static String wifi_ssid;        // Store the name of the wireless network
static String wifi_password;    // Store the password of the wireless network

static String ha_token; // Store token used to get information from HA
static TaskHandle_t state_handle;
static bool pump_working = false;

static void state_task(void *pvParameters) {
    ESP_LOGI(TAG, "Start state update task");

    while (!WiFi.isConnected()) {
        Serial.printf("Waiting for WiFi connection...");
        delay(1000);
    }

    // char auth_buffer[256] = {0};
    // snprintf(auth_buffer, 256, "Authorization: Bearer ");
    IPAddress ha_server(192, 168, 1, 8);
    WiFiClient client;
    while (true) {
        if (client.connect(ha_server, 8123)) {
            Serial.println("connecting...");
            client.println("GET /api/states/switch.pumpsocket HTTP/1.1");
            client.println("Host: 192.168.1.8:8123");
            client.println("User-Agent: ArduinoWiFi/1.1");
            client.println("Accept: */*");
            client.println("Content-Type: application/json");
            client.printf("Authorization: Bearer %s\r\n", ha_token.c_str());
            client.println("Connection: close");
            client.println();
            client.flush();

            // Check HTTP status
            char status[32] = {0};
            client.readBytesUntil('\r', status, sizeof(status));
            // It should be "HTTP/1.0 200 OK" or "HTTP/1.1 200 OK"
            if (strcmp(status + 9, "200 OK") != 0) {
                Serial.printf("Unexpected response: %s\n", status);
            } else {
                // Skip HTTP headers
                if (!client.find(endOfHeaders)) {
                    Serial.println("Invalid response");
                } else {
                    JsonDocument jdoc;
                    const DeserializationError error = deserializeJson(jdoc, client);
                    if (!error) {
                        const String state = jdoc["state"];
                        if (state == "on") {
                            pump_working = true;
                        } else {
                            pump_working = false;
                        }
                        Serial.printf("-->>> Sensor state: %s\n", state);
                    } else {
                        Serial.printf("deserializeJson error: %s\n", error.c_str());
                    }
                }
            }
            client.stop();
        }
        vTaskDelay(pdMS_TO_TICKS(5000));
    }
}

void restoreConfig() {
    preferences.begin("net-config");
    /* Check whether there is wifi configuration information storage, if there is
     * 1 return, if no return 0 */
    const char *WIFI_SSID_PREFERENCE = "WIFI_SSID";
    const char *WIFI_PASSWD_PREFERENCE = "WIFI_PASSWD";
    const char *HA_TOKEN = "HA_TOKEN";
    wifi_ssid = preferences.getString(WIFI_SSID_PREFERENCE);
    wifi_password = preferences.getString(WIFI_PASSWD_PREFERENCE);
    ha_token = preferences.getString(HA_TOKEN);

    if (wifi_ssid.length() > 0 && wifi_password.length() > 0) {
        Serial.printf("Loaded settings from preferences: ssid: %s, passwd: %s\n", wifi_ssid.c_str(),
                      wifi_password.c_str());
    } else {
        // TODO: fill if necessary on initial startup
        wifi_ssid = "";
        wifi_password = "";
        preferences.putString(WIFI_SSID_PREFERENCE, wifi_ssid);
        preferences.putString(WIFI_PASSWD_PREFERENCE, wifi_password);
    }

    if (ha_token.length() > 0) {
        Serial.printf("Loaded settings from preferences: ha_token: %s\n", ha_token.c_str());
    } else {
        // TODO: fill if necessary on initial startup
        ha_token = "";
        preferences.putString(HA_TOKEN, ha_token);
    }
}

void setup_wifi() {
    restoreConfig();
    WiFi.mode(WIFI_MODE_STA); // Set Wi-Fi mode to WIFI_MODE_STA.
    WiFi.setAutoReconnect(true);
    WiFi.begin(wifi_ssid.c_str(), wifi_password.c_str());

    xTaskCreate(state_task, "stateTask", 4096 * 4, NULL, 0, &state_handle);
}

#define X_LOCAL 40
#define Y_LOCAL 30
#define X_OFFSET 160
#define Y_OFFSET 16

void show_net_status() {
    const std::uint8_t FRONT = 2;

    M5.Lcd.setTextColor(TFT_RED, TFT_BLACK);
    M5.Lcd.setCursor(X_LOCAL + X_OFFSET / 4, Y_LOCAL + Y_OFFSET * 9, FRONT);
    M5.Lcd.print("Network information");
    M5.Lcd.setTextColor(TFT_WHITE, TFT_BLACK);
    M5.Lcd.setCursor(X_LOCAL, Y_LOCAL + Y_OFFSET * 10, FRONT);
    M5.Lcd.print("Wifi ssid: ");
    M5.Lcd.print(wifi_ssid.c_str());
    M5.Lcd.setCursor(X_LOCAL, Y_LOCAL + Y_OFFSET * 11, FRONT);
    M5.Lcd.print("Pump state: ");
    if (pump_working) {
        M5.Lcd.setTextColor(TFT_GREEN, TFT_BLACK);
        M5.Lcd.print("on ");
    } else {
        M5.Lcd.setTextColor(TFT_RED, TFT_BLACK);
        M5.Lcd.print("off");
    }
}
