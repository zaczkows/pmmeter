#include "ble.h"

#include "BTHome.h"

#include <NimBLEDevice.h>

static NimBLECharacteristic *pm2p5_characteristic = nullptr;
static NimBLECharacteristic *pm1p0_characteristic = nullptr;
static NimBLECharacteristic *pm10p0_characteristic = nullptr;

static BTHome bt_home;
SemaphoreHandle_t BT_SEM;

void setup_ble() {
    Serial.println("NimBLEDevice::init");
    BT_SEM = xSemaphoreCreateMutex();
    bt_home.begin("m5stack air quality", false, nullptr, false);
}

void update_measurements(const PM25_AQI_Data &measurements, const SHT20Data &th_data) {
    Serial.println("BLE update measurement data");
    xSemaphoreTake(BT_SEM, portMAX_DELAY);
    bt_home.resetMeasurement();
    bt_home.addMeasurement(ID_PM10, (std::uint64_t)measurements.pm10_standard);
    bt_home.addMeasurement(ID_PM25, (std::uint64_t)measurements.pm25_standard);
    bt_home.addMeasurement(ID_TEMPERATURE, th_data.temperature);
    bt_home.addMeasurement(ID_HUMIDITY, th_data.humidity);
    bt_home.buildPacket();
    if (!bt_home.isAdvertising()) {
        bt_home.start();
    }
    xSemaphoreGive(BT_SEM);
}

void update_button() {
    Serial.println("BLE update button data");
    xSemaphoreTake(BT_SEM, portMAX_DELAY);
    bt_home.resetMeasurement();
    bt_home.addMeasurement_state(EVENT_BUTTON, EVENT_BUTTON_PRESS);
    bt_home.buildPacket();
    if (!bt_home.isAdvertising()) {
        bt_home.start();
    }
    xSemaphoreGive(BT_SEM);
}
