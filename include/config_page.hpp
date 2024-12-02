#ifndef CONFIG_PAGE_HPP
#define CONFIG_PAGE_HPP

#include <Arduino.h>
#include <lvgl.h>
#include "config.h"

class ConfigurationPage
{
public:
    ConfigurationPage(ConfigManager *configMgr);
    void init();
    lv_obj_t *getPage();

private:
    ConfigManager *config;
    lv_obj_t *page;

    // Input fields
    lv_obj_t *taSSID;
    lv_obj_t *taPassword;
    lv_obj_t *taHttpIP;
    lv_obj_t *taHttpPort;
    lv_obj_t *taFXStationCode;
    lv_obj_t *taPathGetStation;
    lv_obj_t *taPathGetProduct;
    lv_obj_t *taPathGetRunningProduct;
    lv_obj_t *taPathPostChangedProduct;

    lv_obj_t *btnSave;

    lv_obj_t *keyboard; // Keyboard object

    static void eventHandler(lv_event_t *e);
    void onSave();
    static void ta_event_cb(lv_event_t *e);
};

#endif // CONFIG_PAGE_HPP
