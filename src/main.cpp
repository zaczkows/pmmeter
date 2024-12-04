#include "Free_Fonts.h"
#include "ble.h"
#include <Adafruit_PM25AQI.h>
#include <DFRobot_SHT20.h>
#include <M5Stack.h>
#include <Wire.h>

DFRobot_SHT20 sht20;
Adafruit_PM25AQI aqi;

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

static std::uint8_t brightness = 100;
static bool display_on = true;
static unsigned long last_time = 0;

void setup() {
    // Serial is initialized by M5.begin
    M5.begin(true, false, true, true);
    M5.Lcd.clear();
    M5.Lcd.setBrightness(brightness);
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

    // Setup BLE
    setup_ble();

    last_time = millis();
}

void loop() {
    PM25_AQI_Data data;
    M5.update();

    const unsigned long now = millis();
    const unsigned long time_diff = now - last_time;
    last_time = now;
    Serial.printf("Now = %lu, diff = %lu\n", now, time_diff);

    Serial.printf("Buttons pressed: A: %d, B: %d, C: %d\n", (int)M5.BtnA.wasPressed(), (int)M5.BtnB.wasPressed(),
                  (int)M5.BtnC.wasPressed());

    if (M5.BtnA.wasPressed()) {
        Serial.println("Button A was pressed");
        if (brightness >= 10) {
            brightness -= 10;
        } else {
            brightness = 0;
        }
        M5.Lcd.setBrightness(brightness);
    }

    if (M5.BtnB.wasPressed()) {
        Serial.println("Button B was pressed");
        if (brightness <= 90) {
            brightness += 10;
        } else {
            brightness = 100;
        }
        M5.Lcd.setBrightness(brightness);
    }

    if (M5.BtnB.wasPressed()) {
        Serial.println("Button C was pressed");
        display_on = !display_on;
        if (display_on) {
            M5.Lcd.wakeup();
        } else {
            M5.Lcd.sleep();
        }
    }

    if (aqi.read(&data)) {
        LCD_Display_Val(&data);
        TempHumRead();
        update_ble(data);
    } else {
        Serial.println("Could not read from AQI");
    }

    // delay(1000);
}
