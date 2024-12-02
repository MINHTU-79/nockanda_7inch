#ifndef DOWNTIME_ERROR_PAGE_HPP
#define DOWNTIME_ERROR_PAGE_HPP

#include <Arduino.h>
#include <lvgl.h>
#include <vector>
#include <string>
#include "config.h"
#include "InitStruct.hpp"


class DowntimeErrorPage {
public:
    DowntimeErrorPage(ConfigManager *configMgr);
    void init();
    void fetchDowntimeError();

    // lv_obj_t *getPage(std::string code, std::string other);
    //ver 2 khác tham số
    lv_obj_t *getPage(const DowntimeItem &itemData);
    DowntimeItem currentItemData; // Thuộc tính lưu bản sao

private:
    ConfigManager *config;
    lv_obj_t *page;
    lv_obj_t *content_container;
    lv_obj_t *container;
    std::vector<DowntimeErrorItem> downtimeDetails;

    void updateUI();
    void showError(const String &message);

    void showResult(const String &title, const String &message);
    void handleOption2();
    void handleOption1(String rollPrefix);
    void showPopup();

    static void eventHandler(lv_event_t *e);

    DowntimeErrorItem *getSelectedItem()
    {
        for (auto &item : downtimeDetails)
        {
            if (item.selected) // Nếu trường selected = true
            {
                return &item; // Trả về con trỏ tới item này
            }
        }
        return nullptr; // Nếu không tìm thấy, trả về nullptr
    }
};

#endif // DOWNTIME_ERROR_PAGE_HPP
