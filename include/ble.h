#pragma once

#include "sensors.h"

void setup_ble();
void update_measurements(const PM25_AQI_Data &measurements, const SHT20Data &th_data);
void update_button();
