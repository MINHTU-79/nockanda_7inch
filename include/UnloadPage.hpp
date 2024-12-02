#ifndef Unload_HPP
#define Unload_HPP

#include <Arduino.h>
#include <lvgl.h>
#include <HTTPClient.h>
#include "config.h"
#include <ArduinoJson.h>
#include <vector>
#include "Help.hpp"


struct UnloadRequest
{
  double totalMeterRan;
  int rollCount;
};

class UnloadPage
{
public:
  UnloadPage(ConfigManager *configMgr);
  void init();
  lv_obj_t *getPage();
  // void refreshPage();

  void fetchPage();


private:
  ConfigManager *config;
  float meterRan;
  lv_obj_t *page;

  lv_obj_t *btnUnload;

  // Các label để hiển thị thông tin Job
  lv_obj_t *labelSoMeter;
  lv_obj_t *rollCount;
  lv_obj_t *adjustmentMetRun;
  lv_obj_t *keyboardD; // Keyboard object

  static void eventHandlerR(lv_event_t *e);
  void onChangeProduct();
  void fetchMeterRan();
  static void ta_event_cbB(lv_event_t *e);

  // Hàm để fetch và cập nhật thông tin Job
  void updateJobInfo(const JsonObject &job);
};

#endif // Unload_HPP
