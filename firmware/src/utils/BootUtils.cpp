#include "BootUtils.h"
#include <Arduino.h>
#include <esp_ota_ops.h>
#include <esp_partition.h>

namespace CrossPoint {

void returnToCrossPoint() {
    Serial.println("Returning to CrossPoint...");

    const esp_partition_t *partition = esp_partition_find_first(ESP_PARTITION_TYPE_APP, ESP_PARTITION_SUBTYPE_APP_OTA_0, NULL);
    
    if (partition == NULL) {
        Serial.println("Could not find CrossPoint partition (ota_0)!");
        return;
    }

    const esp_partition_t *running = esp_ota_get_running_partition();
    if (running == partition) {
         Serial.println("Already running on ota_0. Trying ota_1...");
         partition = esp_partition_find_first(ESP_PARTITION_TYPE_APP, ESP_PARTITION_SUBTYPE_APP_OTA_1, NULL);
         if (partition == NULL) {
             Serial.println("Could not find ota_1 either.");
             return;
         }
    }

    Serial.printf("Switching boot partition to %s at 0x%x\n", partition->label, partition->address);

    esp_err_t err = esp_ota_set_boot_partition(partition);
    if (err != ESP_OK) {
        Serial.printf("Failed to set boot partition: %s\n", esp_err_to_name(err));
        return;
    }

    Serial.println("Restarting...");
    delay(100);
    esp_restart();
}

} // namespace CrossPoint
