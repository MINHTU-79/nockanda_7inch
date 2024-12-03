
#include "NetworkModule.h"
#include "NetworkConfig.h"
#include "config.h"
#include <Arduino.h>
#include <WiFi.h>
#include <Preferences.h>
#include "esp_task_wdt.h"

extern Preferences preferences;
extern ConfigManager configMgr;
extern void update_wifi_status(bool connected);

void NetworkModule::initConnect()
{
    Serial.print("Mac address: ");
    Serial.println(WiFi.macAddress());
    if (configMgr.config.isBroadcastWifi)
    {
        WiFi.mode(WIFI_AP_STA);
    }
    else
    {
        WiFi.mode(WIFI_STA);
    }

    ssid = configMgr.config.wifiSSID.c_str();
    password = configMgr.config.wifiPassword.c_str();

    int lastCheckTime;
    const char *ssidViot;
    const char *passwordViot = "Viot379!9";
    // Lấy địa chỉ MAC của ESP32
    uint8_t mac[6];
    esp_read_mac(mac, ESP_MAC_WIFI_STA); // Đọc địa chỉ MAC của ESP32

    char tempBuffer[30]; // Tạo một mảng tạm thời để lưu chuỗi

    // Sử dụng mảng tạm thời trong sprintf
    sprintf(tempBuffer, "C_EDGEViot%02X%02X", mac[4], mac[5]);

    // Gán con trỏ ssidViot tới mảng tạm thời
    ssidViot = tempBuffer;

    // Thông tin mạng tĩnh
    IPAddress local_ip(172, 16, 16, 81); // Địa chỉ IP tĩnh
    IPAddress gateway(172, 16, 16, 1);   // Gateway (thường là IP router)
    IPAddress subnet(255, 255, 254, 0);  // Subnet mask
    IPAddress primaryDNS(172, 16, 16, 20);    // DNS chính (ví dụ Google DNS)
    IPAddress secondaryDNS(172, 16, 16, 20);  // DNS phụ (tùy chọn)

    // // Cấu hình mạng tĩnh
    // if (!WiFi.config(local_ip, gateway, subnet, primaryDNS, secondaryDNS))
    // {
    //     Serial.println("Static IP configuration failed!");
    //     return;
    // }

    WiFi.begin(ssid, password);
    Serial.println("Start connection:");
    lastCheckTime = millis();

    // Wait for connection
    while (WiFi.status() != WL_CONNECTED)
    {
        Serial.println("Connecting First..");
        if (millis() - lastCheckTime > 20000)
        {
            if (configMgr.config.isBroadcastWifi)
            {
                // Kết nối internet đã bị thiếu hơn 10 giây, thay đổi trạng thái nguồn wifi
                WiFi.disconnect();
                WiFi.softAP(ssidViot, passwordViot);
                Serial.println("The internet connection has been missing for more than 10 seconds, change the wifi source status to automatic wifi source.");
                Serial.print("AP IP address: ");
                Serial.println(WiFi.softAPIP());
                configMgr.config.Mode_WiFi_softAP = true;
            }

            // giử cả lại dòng này . nếu bỏ tự phát wifi.
            configMgr.config.previouslyConnected = true;

            update_wifi_status(false);

            return;
        }
        delay(1000);
    }

    // If connection successful show IP address
    Serial.println("");
    Serial.print("Connected to ");
    Serial.println(ssid);
    Serial.print("IP address: ");
    Serial.println(WiFi.localIP());
    WiFi.mode(WIFI_STA);
    configMgr.config.Mode_WiFi_softAP = false;
    // Đặt biến cờ đã có kết nối trước đó.
    configMgr.config.previouslyConnected = true;
    update_wifi_status(true);
}

void NetworkModule::reConnect()
{
    while (1)
    {
        if (configMgr.config.previouslyConnected == true && WiFi.status() != WL_CONNECTED)
        {
            Serial.println("WiFi connection has been interrupted. Trying to reconnect...");

            // // WiFi.reconnect(); // Thay 'ssid' và 'password' bằng thông tin mạng WiFi của bạn
            // WiFi.disconnect(true, true);
            // WiFi.begin(ssid, password);
            Serial.println("SSSSS");
            WiFi.begin(ssid, password);
            Serial.println("Connecting..");
           update_wifi_status(false);

            unsigned long startAttemptTime = xTaskGetTickCount();

            while (WiFi.status() != WL_CONNECTED && xTaskGetTickCount() - startAttemptTime < 5000)
            {
                delay(500);
                // Serial.println("Connecting First..");
            }

            if (WiFi.status() == WL_CONNECTED)
            {
                Serial.println("WiFi connection successful!");
                if (configMgr.config.isBroadcastWifi)
                {
                    // Đặt lại để không còn tự phát wifi nữa.
                    if (WiFi.getMode() == WIFI_AP_STA)
                    {
                        WiFi.mode(WIFI_STA);
                        // Serial.println("WiFi setup Mode. ");
                        configMgr.config.Mode_WiFi_softAP = false;
                    }
                }

                // If connection successful show IP address
                Serial.println("");
                Serial.print("Connected to ");
                Serial.println(ssid);
                Serial.print("IP address: ");
                Serial.println(WiFi.localIP());
                configMgr.config.previouslyConnected == true;
                update_wifi_status(true);
            }
            else
            {
                WiFi.disconnect(true, true);
            }
        }

        vTaskDelay(5000 / portTICK_PERIOD_MS); // 1 giây 1 lần cho đở hiệu xuất.
        esp_task_wdt_reset();
    }
}
