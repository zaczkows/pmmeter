#include "sensors.h"

#include <DFRobot_SHT20.h>

DFRobot_SHT20 sht20;
Adafruit_PM25AQI aqi;

void init_sensors() {
    Serial.println("Initializing sensors");

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
}

bool update_pm_data(PM25_AQI_Data &data) { return aqi.read(&data); }

bool update_sht20_data(SHT20Data &th_data) {
    th_data.humidity = sht20.readHumidity();       // Read Humidity
    th_data.temperature = sht20.readTemperature(); // Read Temperature
    return true;
}
