#include "buttons.h"
#include "ble.h"
#include <M5Stack.h>

static std::uint8_t brightness = 50;
static TaskHandle_t buttons_handle;

static void buttons_task(void *pvParameters) {
    ESP_LOGI(TAG, "Start buttons updatE task");
    while (true) {
        M5.BtnA.read();
        M5.BtnB.read();
        M5.BtnC.read();

        if (M5.BtnA.wasPressed()) {
            // Serial.println("Button A was pressed");
            if (brightness > 10) {
                brightness -= 10;
            } else if (brightness > 0) {
                brightness = 0;
                M5.Lcd.sleep();
            }
            M5.Lcd.setBrightness(brightness);
        }

        if (M5.BtnB.wasPressed()) {
            // Serial.println("Button B was pressed");
            if (brightness <= 90) {
                if (brightness == 0) {
                    M5.Lcd.wakeup();
                }
                brightness += 10;
            } else if (brightness < 100) {
                brightness = 100;
            }
            M5.Lcd.setBrightness(brightness);
        }

        if (M5.BtnC.wasPressed()) {
            // Serial.println("Button C was pressed");
            update_button();
        }
        vTaskDelay(pdMS_TO_TICKS(100));
    }
}

void setup_buttons() { xTaskCreate(buttons_task, "buttonsTask", 4096 * 2, NULL, 0, &buttons_handle); }
