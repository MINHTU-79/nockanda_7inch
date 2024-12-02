#ifndef PRODUCTION_HPP
#define PRODUCTION_HPP

#include <Arduino.h>
#include <lvgl.h>
#include <HTTPClient.h>
#include "config.h"
#include <ArduinoJson.h>
#include <vector>

struct Station
{
  String StationCode;
  String StationName;
};

struct Product
{
  String ProductCode;
  String ProductName;
};

class ProductionPage
{
public:
  ProductionPage(ConfigManager *configMgr);
  void init();
  lv_obj_t *getPage();
  // void refreshPage();

private:
  ConfigManager *config;
  lv_obj_t *page;

  lv_obj_t *ddStation;
  lv_obj_t *ddProduct;
  lv_obj_t *btnChangeProduct;

  // Các label để hiển thị thông tin Job
  lv_obj_t *labelProductCode;
  lv_obj_t *labelPlanQty;
  lv_obj_t *labelJobStatus;
  lv_obj_t *labelStationCode;

  std::vector<Station> stations;
  std::vector<Product> products;

  std::vector<int> filteredStationIndices;
  std::vector<int> filteredProductIndices;

  lv_obj_t *keyboard; // Keyboard object

  void fetchStations();
  void fetchProducts();
  static void eventHandler(lv_event_t *e);
  void onChangeProduct();
  static void ta_event_cb(lv_event_t *e);
  void filterDropdown(lv_obj_t *dropdown, const std::vector<Station> &dataList, const char *filter);
  void filterDropdown(lv_obj_t *dropdown, const std::vector<Product> &dataList, const char *filter);

  // Hàm để fetch và cập nhật thông tin Job
  void fetchRunningJob(const String &stationCode);
  void updateJobInfo(const JsonObject &job);
  static void refreshPage(lv_event_t *e);
};

#endif // PRODUCTION_HPP
