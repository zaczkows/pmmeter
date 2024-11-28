#include "wifi.h"

#include <Preferences.h>
#include <WiFi.h>

Preferences preferences; // wifi config store
String wifi_ssid;        // Store the name of the wireless network
String wifi_password;    // Store the password of the wireless network


void printWiFiInformation() {
    // M5.Lcd.setTextColor(TFT_BLUE, TFT_BLACK);
    // M5.Lcd.setCursor(X_LOCAL, Y_LOCAL + Y_OFFSET * 10, FRONT);
    // M5.Lcd.print("                     ");
    // M5.Lcd.setCursor(X_LOCAL, Y_LOCAL + Y_OFFSET * 10, FRONT);
    // M5.Lcd.print("WiFi SSID: ");
    // M5.Lcd.print(WiFi.SSID());
    // M5.Lcd.print(",  IP: ");
    // M5.Lcd.print(WiFi.localIP());
}

void restoreConfig() {
    /* Check whether there is wifi configuration information storage, if there is
     * 1 return, if no return 0 */
    const char *WIFI_SSID_PREFERENCE = "WIFI_SSID";
    const char *WIFI_PASSWD_PREFERENCE = "WIFI_PASSWD";
    wifi_ssid = preferences.getString(WIFI_SSID_PREFERENCE);
    wifi_password = preferences.getString(WIFI_PASSWD_PREFERENCE);
    // M5.Lcd.setCursor(X_LOCAL, Y_LOCAL + Y_OFFSET * 9, FRONT);

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
}

void setupWiFi() {
    preferences.begin("wifi-config");
    restoreConfig();
    WiFi.mode(WIFI_MODE_STA); // Set Wi-Fi mode to WIFI_MODE_STA.
    WiFi.begin(wifi_ssid.c_str(), wifi_password.c_str());
    WiFi.setAutoReconnect(true);
}

bool is_wifi_connected() { return WiFi.isConnected(); }
