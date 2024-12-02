#ifndef DOWNTIME_PAGE_HPP
#define DOWNTIME_PAGE_HPP

#include <Arduino.h>
#include "production.hpp"
#include <lvgl.h>
#include <vector>
#include <string>
#include "config.h"
#include "Help.hpp"
#include "InitStruct.hpp"



// struct DowntimeItem {
//     std::string code;
//     std::string position;
//     std::string time;
//     std::string duration;
// };

class DowntimePage
{
public:
    DowntimePage(ConfigManager *configMgr);
    void init();
    lv_obj_t *getPage();
    void setDowntimeDetails(const std::vector<DowntimeItem> &details);

private:
    ConfigManager *config;
    lv_obj_t *page;
    lv_obj_t *content_container;
    lv_obj_t *container;

    lv_obj_t *labelMet;

    std::vector<DowntimeItem> downtimeDetails;

    void updateUI();
    static void eventHandler(lv_event_t *e);
};

#endif // DOWNTIME_PAGE_HPP
