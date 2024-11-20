#include "Adafruit_PM25AQI.h"
#include "DFRobot_SHT20.h"
#include "Free_Fonts.h"
#include "WiFi.h"
#include <M5Stack.h>
#include <Preferences.h>
#include <Wire.h>

DFRobot_SHT20 sht20;
Adafruit_PM25AQI aqi;
Preferences preferences; // wifi config store
String wifi_ssid;        // Store the name of the wireless network
String wifi_password;    // Store the password of the wireless network

#define TFT_GREY 0x7BEF

#define X_LOCAL 40
#define Y_LOCAL 30

#define X_OFFSET 160
#define Y_OFFSET 16

const std::uint8_t FRONT = 2;

// Print the header for a display screen
void header(const char *string, uint16_t color) {
    M5.Lcd.fillScreen(color);
    M5.Lcd.setTextSize(1);
    M5.Lcd.setTextColor(TFT_MAGENTA, TFT_BLUE);
    M5.Lcd.fillRect(0, 0, 320, 30, TFT_BLUE);
    M5.Lcd.setTextDatum(TC_DATUM);
    M5.Lcd.drawString(string, 160, 3, 4);
}

void LCD_Display_Val(PM25_AQI_Data *data) {
    //     M5.Lcd.setTextSize(FRONT);
    M5.Lcd.setTextColor(TFT_RED, TFT_BLACK);
    M5.Lcd.setCursor(X_LOCAL, Y_LOCAL, FRONT);
    M5.Lcd.print("S P M");

    M5.Lcd.setTextColor(TFT_WHITE, TFT_BLACK);

    M5.Lcd.setCursor(X_LOCAL, Y_LOCAL + Y_OFFSET, FRONT);
    M5.Lcd.print("                     ");
    M5.Lcd.setCursor(X_LOCAL, Y_LOCAL + Y_OFFSET, FRONT);
    M5.Lcd.print("PM1.0 : ");
    M5.Lcd.print(data->pm10_standard);

    M5.Lcd.setCursor(X_LOCAL, Y_LOCAL + Y_OFFSET * 2, FRONT);
    M5.Lcd.print("                     ");
    M5.Lcd.setCursor(X_LOCAL, Y_LOCAL + Y_OFFSET * 2, FRONT);
    M5.Lcd.print("PM2.5 : ");
    M5.Lcd.print(data->pm25_standard);

    M5.Lcd.setCursor(X_LOCAL, Y_LOCAL + Y_OFFSET * 3, FRONT);
    M5.Lcd.print("                     ");
    M5.Lcd.setCursor(X_LOCAL, Y_LOCAL + Y_OFFSET * 3, FRONT);
    M5.Lcd.print("PM10  : ");
    M5.Lcd.print(data->pm100_standard);

    M5.Lcd.setTextColor(TFT_RED, TFT_BLACK);
    M5.Lcd.setCursor(X_LOCAL + X_OFFSET, Y_LOCAL, FRONT);
    M5.Lcd.print("A T M E");

    M5.Lcd.setCursor(X_LOCAL + X_OFFSET, Y_LOCAL + Y_OFFSET, FRONT);
    M5.Lcd.print("                     ");
    M5.Lcd.setTextColor(TFT_WHITE, TFT_BLACK);
    M5.Lcd.setCursor(X_LOCAL + X_OFFSET, Y_LOCAL + Y_OFFSET, FRONT);
    M5.Lcd.print("PM1.0 : ");
    M5.Lcd.print(data->pm10_env);

    M5.Lcd.setCursor(X_LOCAL + X_OFFSET, Y_LOCAL + Y_OFFSET * 2, FRONT);
    M5.Lcd.print("                     ");
    M5.Lcd.setCursor(X_LOCAL + X_OFFSET, Y_LOCAL + Y_OFFSET * 2, FRONT);
    M5.Lcd.print("PM2.5 : ");
    M5.Lcd.print(data->pm25_env);

    M5.Lcd.setCursor(X_LOCAL + X_OFFSET, Y_LOCAL + Y_OFFSET * 3, FRONT);
    M5.Lcd.print("                     ");
    M5.Lcd.setCursor(X_LOCAL + X_OFFSET, Y_LOCAL + Y_OFFSET * 3, FRONT);
    M5.Lcd.print("PM10  : ");
    M5.Lcd.print(data->pm100_env);

    M5.Lcd.setTextColor(TFT_RED, TFT_BLACK);
    M5.Lcd.setCursor(X_LOCAL + X_OFFSET / 4, Y_LOCAL + Y_OFFSET * 4, FRONT);
    M5.Lcd.print("Number of particles");

    M5.Lcd.setCursor(X_LOCAL, Y_LOCAL + Y_OFFSET * 5, FRONT);
    M5.Lcd.print("                     ");
    M5.Lcd.setTextColor(TFT_WHITE, TFT_BLACK);
    M5.Lcd.setCursor(X_LOCAL, Y_LOCAL + Y_OFFSET * 5, FRONT);
    M5.Lcd.print("0.3um : ");
    M5.Lcd.print(data->particles_03um);

    M5.Lcd.setCursor(X_LOCAL, Y_LOCAL + Y_OFFSET * 6, FRONT);
    M5.Lcd.print("                     ");
    M5.Lcd.setCursor(X_LOCAL, Y_LOCAL + Y_OFFSET * 6, FRONT);
    M5.Lcd.print("0.5um : ");
    M5.Lcd.print(data->particles_05um);

    M5.Lcd.setCursor(X_LOCAL, Y_LOCAL + Y_OFFSET * 7, FRONT);
    M5.Lcd.print("1.0um : ");
    M5.Lcd.print(data->particles_10um);

    M5.Lcd.setCursor(X_LOCAL + X_OFFSET, Y_LOCAL + Y_OFFSET * 5, FRONT);
    M5.Lcd.print("                     ");
    M5.Lcd.setCursor(X_LOCAL + X_OFFSET, Y_LOCAL + Y_OFFSET * 5, FRONT);
    M5.Lcd.print("2.5um : ");
    M5.Lcd.print(data->particles_25um);

    M5.Lcd.setCursor(X_LOCAL + X_OFFSET, Y_LOCAL + Y_OFFSET * 6, FRONT);
    M5.Lcd.print("                     ");
    M5.Lcd.setCursor(X_LOCAL + X_OFFSET, Y_LOCAL + Y_OFFSET * 6, FRONT);
    M5.Lcd.print("5.0um : ");
    M5.Lcd.print(data->particles_50um);

    M5.Lcd.setCursor(X_LOCAL + X_OFFSET, Y_LOCAL + Y_OFFSET * 7, FRONT);
    M5.Lcd.print("                     ");
    M5.Lcd.setCursor(X_LOCAL + X_OFFSET, Y_LOCAL + Y_OFFSET * 7, FRONT);
    M5.Lcd.print("10um  : ");
    M5.Lcd.print(data->particles_100um);
}

void TempHumRead(void) {
    float humd = sht20.readHumidity();    // Read Humidity
    float temp = sht20.readTemperature(); // Read Temperature

    M5.Lcd.setTextColor(TFT_GREEN, TFT_BLACK);
    M5.Lcd.setCursor(X_LOCAL, Y_LOCAL + Y_OFFSET * 8, FRONT);
    M5.Lcd.print("                     ");
    M5.Lcd.setCursor(X_LOCAL, Y_LOCAL + Y_OFFSET * 8, FRONT);
    M5.Lcd.print("T M P : ");
    M5.Lcd.print(temp);

    M5.Lcd.setCursor(X_LOCAL + X_OFFSET, Y_LOCAL + Y_OFFSET * 8, FRONT);
    M5.Lcd.print("                     ");
    M5.Lcd.setCursor(X_LOCAL + X_OFFSET, Y_LOCAL + Y_OFFSET * 8, FRONT);
    M5.Lcd.print("H U M : ");
    M5.Lcd.print(humd);
}

void printWiFiInformation() {
    M5.Lcd.setTextColor(TFT_BLUE, TFT_BLACK);
    M5.Lcd.setCursor(X_LOCAL, Y_LOCAL + Y_OFFSET * 10, FRONT);
    M5.Lcd.print("                     ");
    M5.Lcd.setCursor(X_LOCAL, Y_LOCAL + Y_OFFSET * 10, FRONT);
    M5.Lcd.print("WiFi SSID: ");
    M5.Lcd.print(WiFi.SSID());
    M5.Lcd.print(",  IP: ");
    M5.Lcd.print(WiFi.localIP());
}

void restoreConfig() {
    /* Check whether there is wifi configuration information storage, if there is
     * 1 return, if no return 0 */
    const char *WIFI_SSID_PREFERENCE = "WIFI_SSID";
    const char *WIFI_PASSWD_PREFERENCE = "WIFI_PASSWD";
    wifi_ssid = preferences.getString(WIFI_SSID_PREFERENCE);
    wifi_password = preferences.getString(WIFI_PASSWD_PREFERENCE);
    M5.Lcd.setCursor(X_LOCAL, Y_LOCAL + Y_OFFSET * 9, FRONT);

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
    WiFi.mode(WIFI_MODE_STA); // Set Wi-Fi mode to WIFI_MODE_STA.
    WiFi.begin(wifi_ssid.c_str(), wifi_password.c_str());
    WiFi.setAutoReconnect(true);
}

void setup() {
    // Serial is initialized by M5.begin
    M5.begin(true, false, true, true);
    M5.Lcd.clear();
    header("Initializing...", TFT_BLACK);

    // Wait three seconds for sensor to boot up!
    delay(3000);

    Serial2.begin(9600, SERIAL_8N1, 16, 17);
    pinMode(13, OUTPUT);
    digitalWrite(13, 1);

    header("PM 2.5", TFT_BLACK);

    // connect to the sensor over hardware serial
    if (!aqi.begin_UART(&Serial2)) {
        Serial.println("Could not find PM 2.5 sensor!");
        while (1) {
            delay(10);
        }
    }

    Serial.println("PM25 found!");

    // Init SHT20 Sensor
    sht20.initSHT20();
    delay(100);
    sht20.checkSHT20();

    // Init WiFi
    preferences.begin("wifi-config");
    restoreConfig();
    setupWiFi();
}

void loop() {
    PM25_AQI_Data data;

    if (!aqi.read(&data)) {
        Serial.println("Could not read from AQI");
        delay(500); // try again in a bit!
        return;
    }
    Serial.println("AQI reading success");

    LCD_Display_Val(&data);
    TempHumRead();
    printWiFiInformation();

    /*Serial.println();
    Serial.println(F("---------------------------------------"));
    Serial.println(F("Concentration Units (standard)"));
    Serial.println(F("---------------------------------------"));
    Serial.print(F("PM 1.0: "));
    Serial.print(data.pm10_standard);
    Serial.print(F("\t\tPM 2.5: "));
    Serial.print(data.pm25_standard);
    Serial.print(F("\t\tPM 10: "));
    Serial.println(data.pm100_standard);
    Serial.println(F("Concentration Units (environmental)"));
    Serial.println(F("---------------------------------------"));
    Serial.print(F("PM 1.0: "));
    Serial.print(data.pm10_env);
    Serial.print(F("\t\tPM 2.5: "));
    Serial.print(data.pm25_env);
    Serial.print(F("\t\tPM 10: "));
    Serial.println(data.pm100_env);
    Serial.println(F("---------------------------------------"));
    Serial.print(F("Particles > 0.3um / 0.1L air:"));
    Serial.println(data.particles_03um);
    Serial.print(F("Particles > 0.5um / 0.1L air:"));
    Serial.println(data.particles_05um);
    Serial.print(F("Particles > 1.0um / 0.1L air:"));
    Serial.println(data.particles_10um);
    Serial.print(F("Particles > 2.5um / 0.1L air:"));
    Serial.println(data.particles_25um);
    Serial.print(F("Particles > 5.0um / 0.1L air:"));
    Serial.println(data.particles_50um);
    Serial.print(F("Particles > 10 um / 0.1L air:"));
    Serial.println(data.particles_100um);
    Serial.println(F("---------------------------------------"));*/

    delay(1000);
}
