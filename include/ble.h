#pragma once

#include <Adafruit_PM25AQI.h>

void setup_ble();
void update_ble(const PM25_AQI_Data &measurements);

