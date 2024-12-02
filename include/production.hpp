#ifndef PRODUCTION_HPP
#define PRODUCTION_HPP

#include <Arduino.h>
#include <lvgl.h>
#include <HTTPClient.h>
#include "config.h"
#include <ArduinoJson.h>
#include <vector>
#include "InitStruct.hpp"
#include "UnloadPage.hpp"


class ProductionPage
{
public:
  ProductionPage(ConfigManager *configMgr);
  void init();
  lv_obj_t *getPage();
  // void refreshPage();
  //RunningJob runningJob;
  void fetchProducts();

private:
  ConfigManager *config;
  lv_obj_t *page;
  lv_obj_t *content_container;


  lv_obj_t *ddStation;
  lv_obj_t *ddProduct;
  lv_obj_t *btnChangeProduct;


  // Các label để hiển thị thông tin Job
  lv_obj_t *labelNhanVien;
  lv_obj_t *labelMaHang;
  lv_obj_t *labelDaChay;
  lv_obj_t *labelCounter;

  std::vector<Station> stations;
  std::vector<Product> products;

  std::vector<int> filteredStationIndices;
  std::vector<int> filteredProductIndices;

  lv_obj_t *keyboard; // Keyboard object

  static void eventHandler(lv_event_t *e);
  void onChangeProduct();
  static void ta_event_cb(lv_event_t *e);


  void filterDropdown(lv_obj_t *dropdown, const std::vector<Station> &dataList, const char *filter);
  void filterDropdown(lv_obj_t *dropdown, const std::vector<Product> &dataList, const char *filter);

  // Hàm để fetch và cập nhật thông tin Job
  void updateJobInfo(const JsonObject &job);
  static void refreshPage(lv_event_t *e);
};

#endif // PRODUCTION_HPP
