#include "ble.h"

#include "BTHome.h"
#include <NimBLEDevice.h>
#include <NimBLEUUID.h>

static NimBLEServer *pServer = nullptr;

/**  None of these are required as they will be handled by the library with defaults. **
 **                       Remove as you see fit for your needs                        */
class ServerCallbacks : public NimBLEServerCallbacks {
    void onConnect(NimBLEServer *pServer) {
        Serial.println("Client connected");
        Serial.println("Multi-connect support: start advertising");
        NimBLEDevice::startAdvertising();
    };
    /** Alternative onConnect() method to extract details of the connection.
     *  See: src/ble_gap.h for the details of the ble_gap_conn_desc struct.
     */
    void onConnect(NimBLEServer *pServer, ble_gap_conn_desc *desc) {
        Serial.print("Client address: ");
        Serial.println(NimBLEAddress(desc->peer_ota_addr).toString().c_str());
        /** We can use the connection handle here to ask for different connection parameters.
         *  Args: connection handle, min connection interval, max connection interval
         *  latency, supervision timeout.
         *  Units; Min/Max Intervals: 1.25 millisecond increments.
         *  Latency: number of intervals allowed to skip.
         *  Timeout: 10 millisecond increments, try for 5x interval time for best results.
         */
        pServer->updateConnParams(desc->conn_handle, 24, 48, 0, 60);
    };
    void onDisconnect(NimBLEServer *pServer) {
        Serial.println("Client disconnected - start advertising");
        NimBLEDevice::startAdvertising();
    };
    void onMTUChange(uint16_t MTU, ble_gap_conn_desc *desc) {
        Serial.printf("MTU updated: %u for connection ID: %u\n", MTU, desc->conn_handle);
    };

    /********************* Security handled here **********************
    ****** Note: these are the same return values as defaults ********/
    uint32_t onPassKeyRequest() {
        Serial.println("Server Passkey Request");
        /** This should return a random 6 digit number for security
         *  or make your own static passkey as done here.
         */
        return 123456;
    };

    bool onConfirmPIN(uint32_t pass_key) {
        Serial.print("The passkey YES/NO number: ");
        Serial.println(pass_key);
        /** Return false if passkeys don't match. */
        return true;
    };

    void onAuthenticationComplete(ble_gap_conn_desc *desc) {
        /** Check that encryption was successful, if not we disconnect the client */
        if (!desc->sec_state.encrypted) {
            NimBLEDevice::getServer()->disconnect(desc->conn_handle);
            Serial.println("Encrypt connection failed - disconnecting client");
            return;
        }
        Serial.println("Starting BLE work!");
    };
};

/** Handler class for characteristic actions */
class CharacteristicCallbacks : public NimBLECharacteristicCallbacks {
    void onRead(NimBLECharacteristic *pCharacteristic) {
        Serial.print(pCharacteristic->getUUID().toString().c_str());
        Serial.print(": onRead(), value: ");
        Serial.println(pCharacteristic->getValue().c_str());
    };

    void onWrite(NimBLECharacteristic *pCharacteristic) {
        Serial.print(pCharacteristic->getUUID().toString().c_str());
        Serial.print(": onWrite(), value: ");
        Serial.println(pCharacteristic->getValue().c_str());
    };
    /** Called before notification or indication is sent,
     *  the value can be changed here before sending if desired.
     */
    void onNotify(NimBLECharacteristic *pCharacteristic) { Serial.println("Sending notification to clients"); };

    /** The status returned in status is defined in NimBLECharacteristic.h.
     *  The value returned in code is the NimBLE host return code.
     */
    void onStatus(NimBLECharacteristic *pCharacteristic, Status status, int code) {
        String str = ("Notification/Indication status code: ");
        str += status;
        str += ", return code: ";
        str += code;
        str += ", ";
        str += NimBLEUtils::returnCodeToString(code);
        Serial.println(str);
    };

    void onSubscribe(NimBLECharacteristic *pCharacteristic, ble_gap_conn_desc *desc, uint16_t subValue) {
        String str = "Client ID: ";
        str += desc->conn_handle;
        str += " Address: ";
        str += std::string(NimBLEAddress(desc->peer_ota_addr)).c_str();
        if (subValue == 0) {
            str += " Unsubscribed to ";
        } else if (subValue == 1) {
            str += " Subscribed to notfications for ";
        } else if (subValue == 2) {
            str += " Subscribed to indications for ";
        } else if (subValue == 3) {
            str += " Subscribed to notifications and indications for ";
        }
        str += std::string(pCharacteristic->getUUID()).c_str();

        Serial.println(str);
    };
};

/** Handler class for descriptor actions */
class DescriptorCallbacks : public NimBLEDescriptorCallbacks {
    void onWrite(NimBLEDescriptor *pDescriptor) {
        std::string dscVal = pDescriptor->getValue();
        Serial.print("Descriptor witten value:");
        Serial.println(dscVal.c_str());
    };

    void onRead(NimBLEDescriptor *pDescriptor) {
        Serial.print(pDescriptor->getUUID().toString().c_str());
        Serial.println(" Descriptor read");
    };
};

/** Define callback instances globally to use for multiple Charateristics \ Descriptors */
static DescriptorCallbacks dscCallbacks;
static CharacteristicCallbacks chrCallbacks;

static NimBLECharacteristic *pm2p5_characteristic = nullptr;
static NimBLECharacteristic *pm1p0_characteristic = nullptr;
static NimBLECharacteristic *pm10p0_characteristic = nullptr;

static BTHome bt_home;

void setup_ble() {
    Serial.println("NimBLEDevice::init");

    bt_home.begin("m5stack air quality", false, nullptr, false);
#if 0
    /** sets device name */
    NimBLEDevice::init("m5stack air quality");

    /** Optional: set the transmit power, default is 3db */
#ifdef ESP_PLATFORM
    NimBLEDevice::setPower(ESP_PWR_LVL_P9); /** +9db */
#else
    NimBLEDevice::setPower(9); /** +9db */
#endif

    NimBLEDevice::setSecurityIOCap(BLE_HS_IO_NO_INPUT_OUTPUT);
    NimBLEDevice::setSecurityAuth(false, false, true);

    Serial.println("NimBLEDevice::createServer");
    pServer = NimBLEDevice::createServer();
    if (!pServer) {
        Serial.println("Failed to create pServer");
    }
    pServer->setCallbacks(new ServerCallbacks());

    Serial.println("air_quality_service = pServer->createService");
    const uint16_t air_quality_svc_number = 0x181A; //(((0x015 << 6) | 0x02) << 16) | 0x542;
    NimBLEService *air_quality_service = pServer->createService(NimBLEUUID(air_quality_svc_number));
    if (!air_quality_service) {
        Serial.println("Failed to create air_quality_service");
    }
    pm1p0_characteristic =
        air_quality_service->createCharacteristic(NimBLEUUID((uint16_t)0x2BD5), NIMBLE_PROPERTY::READ | NIMBLE_PROPERTY::NOTIFY);
    if (!pm1p0_characteristic) {
        Serial.println("Failed to create pm1p0_characteristic");
    }
    pm1p0_characteristic->setValue(0);
    pm1p0_characteristic->setCallbacks(&chrCallbacks);
    pm2p5_characteristic =
        air_quality_service->createCharacteristic(NimBLEUUID((uint16_t)0x2BD6), NIMBLE_PROPERTY::READ| NIMBLE_PROPERTY::NOTIFY);
    if (!pm2p5_characteristic) {
        Serial.println("Failed to create pm2p5_characteristic");
    }
    pm2p5_characteristic->setValue(0);
    pm2p5_characteristic->setCallbacks(&chrCallbacks);
    pm10p0_characteristic =
        air_quality_service->createCharacteristic(NimBLEUUID((uint16_t)0x2BD7), NIMBLE_PROPERTY::READ| NIMBLE_PROPERTY::NOTIFY);
    if (!pm10p0_characteristic) {
        Serial.println("Failed to create pm10p0_characteristic");
    }
    pm10p0_characteristic->setValue(0);
    pm10p0_characteristic->setCallbacks(&chrCallbacks);

    air_quality_service->start();

    Serial.println("air_quality_service = *pAdvertising = NimBLEDevice::getAdvertising");
    NimBLEAdvertising *pAdvertising = NimBLEDevice::getAdvertising();
    if (!pAdvertising) {
        Serial.println("Failed to create pAdvertising");
    }
    // /** Add the services to the advertisment data **/
    pAdvertising->addServiceUUID(air_quality_service->getUUID());
    // /** If your device is battery powered you may consider setting scan response
    //  *  to false as it will extend battery life at the expense of less data sent.
    //  */
    pAdvertising->setScanResponse(true);
    pAdvertising->start();

    Serial.println("Advertising Started");
#endif
}

void update_ble(const PM25_AQI_Data &measurements) {
#if 0
    if (pm1p0_characteristic) {
        pm1p0_characteristic->setValue(measurements.pm10_standard);
        pm1p0_characteristic->notify(true);
    }
    if (pm2p5_characteristic) {
        pm2p5_characteristic->setValue(measurements.pm25_standard);
        pm2p5_characteristic->notify(true);
    }
    if (pm10p0_characteristic) {
        pm10p0_characteristic->setValue(measurements.pm100_standard);
        pm10p0_characteristic->notify(true);
    }
#endif

    bt_home.resetMeasurement();
    bt_home.addMeasurement(ID_PM10, (std::uint64_t)measurements.pm10_standard);
    bt_home.addMeasurement(ID_PM25, (std::uint64_t)measurements.pm25_standard);
    bt_home.buildPacket();
    if (!bt_home.isAdvertising()) {
        bt_home.start();
    }
}

void arduino_default_ble() {

    // #include <BLEDevice.h>
    // #include <BLEUtils.h>
    // #include <BLEServer.h>

    // See the following for generating UUIDs:
    // https://www.uuidgenerator.net/

    // define SERVICE_UUID "4fafc201-1fb5-459e-8fcc-c5c9c331914b"
    // #define CHARACTERISTIC_UUID "beb5483e-36e1-4688-b7f5-ea07361b26a8"

#if 0
 BLEDevice::init("Long name works now");
  BLEServer *pServer = BLEDevice::createServer();
  BLEService *pService = pServer->createService(SERVICE_UUID);
  BLECharacteristic *pCharacteristic = pService->createCharacteristic(
                                         CHARACTERISTIC_UUID,
                                         BLECharacteristic::PROPERTY_READ |
                                         BLECharacteristic::PROPERTY_WRITE
                                       );

  pCharacteristic->setValue("Hello World says Neil");
  pService->start();
  // BLEAdvertising *pAdvertising = pServer->getAdvertising();  // this still is working for backward compatibility
  BLEAdvertising *pAdvertising = BLEDevice::getAdvertising();
  pAdvertising->addServiceUUID(SERVICE_UUID);
  pAdvertising->setScanResponse(true);
  pAdvertising->setMinPreferred(0x06);  // functions that help with iPhone connections issue
  pAdvertising->setMinPreferred(0x12);
  BLEDevice::startAdvertising();
#endif
}
