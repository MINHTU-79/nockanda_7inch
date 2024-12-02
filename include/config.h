#ifndef CONFIG_H
#define CONFIG_H

#include <ElegantOTA.h>

#include <Arduino.h>

#if defined(ESP8266)
#include <ESP8266WiFi.h>
#include <ESPAsyncTCP.h>
#elif defined(ESP32)
#include <WiFi.h>
#include <AsyncTCP.h>
#endif

#include <ESPAsyncWebServer.h>
#include <ElegantOTA.h>
#include <Preferences.h>

#define FIRMWARE_VERSION "1.0.0.0"

struct AppConfig
{
  String wifiSSID;
  String wifiPassword;
  String httpIP;
  uint16_t httpPort;
  String fxStationCode;
  String pathGetStation;
  String pathGetProduct;
  String pathGetRunningProductOnStation;
  String pathPostChangedProduct;
  bool isBroadcastWifi;
  bool Mode_WiFi_softAP;
  bool previouslyConnected;
};

class ConfigManager
{
public:
  ConfigManager();
  bool loadConfig();
  bool saveConfig();
  AppConfig config;
  const char *storageKey = "app_config";

private:
  Preferences preferences;
};

// Khởi tạo các đối tượng toàn cục biến này dùng cho viêc bên web mở port để config và các việc khác . không dùng cho việc bên web lv !
inline ConfigManager configMgr;

void initOtaModule();

#endif // CONFIG_H
