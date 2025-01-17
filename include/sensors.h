#pragma once

#include <Adafruit_PM25AQI.h>

struct SHT20Data {
    float temperature{0.0f};
    float humidity{0.0f};
};

void init_sensors();
bool update_pm_data(PM25_AQI_Data &data);
bool update_sht20_data(SHT20Data &th_data);
