
#include "config_page.hpp"

extern lv_obj_t *mainPage;
extern lv_obj_t *create_top_bar(lv_obj_t *parent, const char *title, lv_event_cb_t back_cb, void *back_cb_user_data, lv_event_cb_t refresh_cb, void *refresh_cb_user_data);


ConfigurationPage::ConfigurationPage(ConfigManager *configMgr)
    : config(configMgr), keyboard(nullptr)
{
    page = lv_obj_create(NULL);

    // Tạo thanh bar với tiêu đề "Configuration"
  create_top_bar(
      page,
      "Cấu hình", // Tiêu đề
      [](lv_event_t *e)
      { lv_disp_load_scr(mainPage); }, // Callback nút Back
      nullptr,                         // Không cần user_data cho Back
      nullptr,     // Callback nút Refresh
      nullptr);

    // lv_obj_add_flag(page, LV_OBJ_FLAG_SCROLLABLE);         // Bật tính năng cuộn cho trang bằng cờ 'LV_OBJ_FLAG_SCROLLABLE'
    // lv_obj_set_flex_flow(page, LV_FLEX_FLOW_COLUMN);       // Thiết lập giao diện linh hoạt của trang theo chiều dọc, cho phép các phần tử xếp chồng theo một cột
    // lv_obj_set_scroll_dir(page, LV_DIR_VER);               // Cấu hình hướng cuộn của trang để chỉ cho phép cuộn theo chiều dọc
    // lv_obj_set_scroll_snap_y(page, LV_SCROLL_SNAP_CENTER); // Bật tính năng snap khi cuộn, đảm bảo cuộn đến vị trí chính giữa theo chiều dọc trong các phần tử
    
    // lv_obj_set_size(page, LV_PCT(100), LV_PCT(100));       // Thiết lập kích thước của trang vừa khít khung hiển thị, chiếm 100% cả chiều rộng và chiều cao
    // lv_obj_set_style_pad_row(page, 10, 0);                 // Thêm 10px khoảng cách giữa mỗi hàng phần tử bên trong trang, tăng khoảng cách trực quan giữa chúng
    // // lv_obj_clear_flag(page, LV_OBJ_FLAG_SCROLL_MOMENTUM);  // Tắt cuộn quán tính cho trang chính

}

void ConfigurationPage::init()
{

    // lv_obj_t *container1 = lv_obj_create(page);            // Or use an appropriate parent
    // lv_obj_set_style_pad_all(container1, 0, LV_PART_MAIN); //
    // lv_obj_set_size(container1, LV_PCT(100), LV_PCT(100));

    lv_obj_t *content_container = lv_obj_create(page);
    lv_obj_set_size(content_container, LV_PCT(100), lv_obj_get_height(page) - 50);
    lv_obj_set_style_pad_all(content_container, 0, LV_PART_MAIN);
    lv_obj_clear_flag(content_container, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_clear_flag(content_container, LV_OBJ_FLAG_SCROLL_MOMENTUM);  // Tắt cuộn quán tính cho trang chính
    lv_obj_align(content_container, LV_ALIGN_TOP_MID, 0, 50); // Đặt container1 dưới thanh bar, bỏ 50px


    // Create a parent container for flex layout
    lv_obj_t *container = lv_obj_create(content_container);
    lv_obj_set_flex_flow(container, LV_FLEX_FLOW_COLUMN); // Set the layout to column for the container
    // lv_obj_set_flex_align(container, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER); // Center alignment
    lv_obj_set_flex_align(container, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);

    lv_obj_set_size(container, LV_PCT(100), LV_PCT(100)); // Set container size to full screen

    // Set container scrollable
    lv_obj_set_scrollbar_mode(container, LV_SCROLLBAR_MODE_ON);
    lv_obj_set_scroll_dir(container, LV_DIR_VER); // Vertical scrolling

    // lv_obj_clear_flag(container, LV_OBJ_FLAG_SCROLL_MOMENTUM); // Tắt cuộn quán tính cho container

    // // Add Back Button with Icon inside container, but positioned using absolute positioning
    // lv_obj_t *btnBack = lv_btn_create(container1);
    // lv_obj_set_size(btnBack, 50, 50); // Smaller size for icon button

    // // Using absolute position inside the container
    // lv_obj_set_style_pad_all(btnBack, 0, 0);
    // lv_obj_set_pos(btnBack, 10, 10);              // Place at top-left within the container
    // lv_obj_set_align(btnBack, LV_ALIGN_TOP_LEFT); // Align to top left of the container

    // lv_obj_add_event_cb(btnBack, [](lv_event_t *e)
    //                     {
    //                         lv_disp_load_scr(mainPage); // Navigate back to the main page
    //                     },
    //                     LV_EVENT_CLICKED, NULL);

    // // Add icon to the button (using LV_SYMBOL_LEFT)
    // lv_obj_t *btnBackIcon = lv_label_create(btnBack);
    // lv_label_set_text(btnBackIcon, LV_SYMBOL_LEFT); // Left arrow icon
    // lv_obj_center(btnBackIcon);

    // Create Text Areas with Labels

    // Add a label at the top to display the IP address of the ESP32
    lv_obj_t *labelIP = lv_label_create(container);
    lv_label_set_text(labelIP, "IP Address: Not Connected");

    // Fetch the actual IP address after Wi-Fi connection
    IPAddress ip = WiFi.localIP(); // Assuming you're using the WiFi library
    String ipStr = ip.toString();
    lv_label_set_text_fmt(labelIP, "IP Address: %s", ipStr.c_str());

    // WiFi SSID
    lv_obj_t *labelSSID = lv_label_create(container);
    lv_label_set_text(labelSSID, "WiFi SSID:");
    taSSID = lv_textarea_create(container);
    lv_obj_set_width(taSSID, LV_PCT(98));
    lv_textarea_set_one_line(taSSID, true);
    lv_textarea_set_text(taSSID, config->config.wifiSSID.c_str());
    lv_obj_add_event_cb(taSSID, ta_event_cb, LV_EVENT_CLICKED, this);
    lv_obj_add_event_cb(taSSID, ta_event_cb, LV_EVENT_DEFOCUSED, this);

    // WiFi Password
    lv_obj_t *labelPassword = lv_label_create(container);
    lv_label_set_text(labelPassword, "WiFi Password:");
    taPassword = lv_textarea_create(container);
    lv_obj_set_width(taPassword, LV_PCT(98));
    lv_textarea_set_one_line(taPassword, true);
    lv_textarea_set_text(taPassword, config->config.wifiPassword.c_str());
    lv_textarea_set_password_mode(taPassword, true);
    lv_obj_add_event_cb(taPassword, ta_event_cb, LV_EVENT_CLICKED, this);
    lv_obj_add_event_cb(taPassword, ta_event_cb, LV_EVENT_DEFOCUSED, this);

    // Repeat the above pattern for other text areas...

    // HTTP IP
    lv_obj_t *labelHttpIP = lv_label_create(container);
    lv_label_set_text(labelHttpIP, "HTTP IP:");
    taHttpIP = lv_textarea_create(container);
    lv_obj_set_width(taHttpIP, LV_PCT(98));
    lv_textarea_set_one_line(taHttpIP, true);
    lv_textarea_set_text(taHttpIP, config->config.httpIP.c_str());
    lv_obj_add_event_cb(taHttpIP, ta_event_cb, LV_EVENT_CLICKED, this);
    lv_obj_add_event_cb(taHttpIP, ta_event_cb, LV_EVENT_DEFOCUSED, this);

    // HTTP Port
    lv_obj_t *labelHttpPort = lv_label_create(container);
    lv_label_set_text(labelHttpPort, "HTTP Port:");
    taHttpPort = lv_textarea_create(container);
    lv_obj_set_width(taHttpPort, LV_PCT(98));
    lv_textarea_set_one_line(taHttpPort, true);
    lv_textarea_set_text(taHttpPort, String(config->config.httpPort).c_str());
    lv_obj_add_event_cb(taHttpPort, ta_event_cb, LV_EVENT_CLICKED, this);
    lv_obj_add_event_cb(taHttpPort, ta_event_cb, LV_EVENT_DEFOCUSED, this);

    // FX Station Code
    lv_obj_t *labelFXStation = lv_label_create(container);
    lv_label_set_text(labelFXStation, "FX Station Code:");
    taFXStationCode = lv_textarea_create(container);
    lv_obj_set_width(taFXStationCode, LV_PCT(98));
    lv_textarea_set_one_line(taFXStationCode, true);
    lv_textarea_set_text(taFXStationCode, config->config.fxStationCode.c_str());
    lv_obj_add_event_cb(taFXStationCode, ta_event_cb, LV_EVENT_CLICKED, this);
    lv_obj_add_event_cb(taFXStationCode, ta_event_cb, LV_EVENT_DEFOCUSED, this);

    // Path Get Station
    lv_obj_t *labelPathGetStation = lv_label_create(container);
    lv_label_set_text(labelPathGetStation, "Path Get Station:");
    taPathGetStation = lv_textarea_create(container);
    lv_obj_set_width(taPathGetStation, LV_PCT(98));
    lv_textarea_set_one_line(taPathGetStation, true);
    lv_textarea_set_text(taPathGetStation, config->config.pathGetStation.c_str());
    lv_obj_add_event_cb(taPathGetStation, ta_event_cb, LV_EVENT_CLICKED, this);
    lv_obj_add_event_cb(taPathGetStation, ta_event_cb, LV_EVENT_DEFOCUSED, this);

    // Path Get Product
    lv_obj_t *labelPathGetProduct = lv_label_create(container);
    lv_label_set_text(labelPathGetProduct, "Path Get Product:");
    taPathGetProduct = lv_textarea_create(container);
    lv_obj_set_width(taPathGetProduct, LV_PCT(98));
    lv_textarea_set_one_line(taPathGetProduct, true);
    lv_textarea_set_text(taPathGetProduct, config->config.pathGetProduct.c_str());
    lv_obj_add_event_cb(taPathGetProduct, ta_event_cb, LV_EVENT_CLICKED, this);
    lv_obj_add_event_cb(taPathGetProduct, ta_event_cb, LV_EVENT_DEFOCUSED, this);

    // Path Get Running Product on Station
    lv_obj_t *labelPathGetRunning = lv_label_create(container);
    lv_label_set_text(labelPathGetRunning, "Path Get Running Product:");
    taPathGetRunningProduct = lv_textarea_create(container);
    lv_obj_set_width(taPathGetRunningProduct, LV_PCT(98));
    lv_textarea_set_one_line(taPathGetRunningProduct, true);
    lv_textarea_set_text(taPathGetRunningProduct, config->config.pathGetRunningProductOnStation.c_str());
    lv_obj_add_event_cb(taPathGetRunningProduct, ta_event_cb, LV_EVENT_CLICKED, this);
    lv_obj_add_event_cb(taPathGetRunningProduct, ta_event_cb, LV_EVENT_DEFOCUSED, this);

    // Path Post Changed Product
    lv_obj_t *labelPathPostChanged = lv_label_create(container);
    lv_label_set_text(labelPathPostChanged, "Path Post Changed Product:");
    taPathPostChangedProduct = lv_textarea_create(container);
    lv_obj_set_width(taPathPostChangedProduct, LV_PCT(98));
    lv_textarea_set_one_line(taPathPostChangedProduct, true);
    lv_textarea_set_text(taPathPostChangedProduct, config->config.pathPostChangedProduct.c_str());
    lv_obj_add_event_cb(taPathPostChangedProduct, ta_event_cb, LV_EVENT_CLICKED, this);
    lv_obj_add_event_cb(taPathPostChangedProduct, ta_event_cb, LV_EVENT_DEFOCUSED, this);

    // Save Button at the bottom of the container
    btnSave = lv_btn_create(container);
    lv_obj_set_size(btnSave, 150, 50);
    lv_obj_set_align(btnSave, LV_ALIGN_CENTER); // Align to center in layout
    lv_obj_add_event_cb(btnSave, eventHandler, LV_EVENT_CLICKED, this);

    lv_obj_t *btnLabel = lv_label_create(btnSave);
    lv_label_set_text(btnLabel, "Save");
    lv_obj_center(btnLabel);

    // Create Keyboard
    keyboard = lv_keyboard_create(content_container);
    lv_obj_set_size(keyboard, LV_PCT(100), LV_PCT(50));
    lv_obj_align(keyboard, LV_ALIGN_BOTTOM_MID, 0, 0);
    lv_keyboard_set_mode(keyboard, LV_KEYBOARD_MODE_TEXT_UPPER);
    // lv_obj_add_flag(keyboard, LV_OBJ_FLAG_HIDDEN); // Hide initially
    lv_obj_add_flag(keyboard, LV_OBJ_FLAG_HIDDEN | LV_OBJ_FLAG_FLOATING); // Ẩn ban đầu và làm floating

    // Thêm padding dưới cùng để tránh bị che bởi bàn phím
    lv_obj_set_style_pad_bottom(container, 100, 0); // 100 pixels hoặc chiều cao của bàn phím
}

lv_obj_t *ConfigurationPage::getPage()
{
    return page;
}

// Callback cho các trường nhập liệu
void ConfigurationPage::ta_event_cb(lv_event_t *e)
{
    ConfigurationPage *page = (ConfigurationPage *)lv_event_get_user_data(e);
    lv_obj_t *target = lv_event_get_target(e);
    lv_obj_t *container = lv_obj_get_parent(target); // Get container to scroll

    if (lv_event_get_code(e) == LV_EVENT_CLICKED)
    {
        // Đặt bàn phím hướng đến textarea được focus
        lv_keyboard_set_textarea(page->keyboard, target);
        lv_obj_clear_flag(page->keyboard, LV_OBJ_FLAG_HIDDEN);

        // Tính toán xem textarea có bị che bởi bàn phím không
        lv_coord_t ta_y = lv_obj_get_y(target);                   // Y position of textarea
        lv_coord_t ta_height = lv_obj_get_height(target);         // Height of textarea
        lv_coord_t kb_y = lv_obj_get_y(page->keyboard);           // Y position of keyboard
        lv_coord_t kb_height = lv_obj_get_height(page->keyboard); // Height of keyboard

        // Nếu textarea bị che, điều chỉnh chiều cao của container
        if (ta_y + ta_height > kb_y)
        {
            lv_coord_t new_height = kb_y - lv_obj_get_y(container);
            lv_obj_set_height(container, new_height);
            lv_obj_scroll_to_view(target, LV_ANIM_ON); // Cuộn textarea vào tầm nhìn
        }
    }
    else if (lv_event_get_code(e) == LV_EVENT_DEFOCUSED)
    {
        // Ẩn bàn phím khi textarea mất focus
        lv_obj_add_flag(page->keyboard, LV_OBJ_FLAG_HIDDEN);

        // Khôi phục chiều cao ban đầu của container
        lv_obj_set_height(container, LV_PCT(100));
    }
}
void ConfigurationPage::eventHandler(lv_event_t *e)
{
    ConfigurationPage *page = (ConfigurationPage *)lv_event_get_user_data(e);
    if (e->code == LV_EVENT_CLICKED)
    {
        if (e->target == page->btnSave)
        {
            page->onSave();
        }
    }
}

void ConfigurationPage::onSave()
{
    // Retrieve text from input fields and save to config
    config->config.wifiSSID = String(lv_textarea_get_text(taSSID));
    config->config.wifiPassword = String(lv_textarea_get_text(taPassword));
    config->config.httpIP = String(lv_textarea_get_text(taHttpIP));
    config->config.httpPort = String(lv_textarea_get_text(taHttpPort)).toInt();
    config->config.fxStationCode = String(lv_textarea_get_text(taFXStationCode));
    config->config.pathGetStation = String(lv_textarea_get_text(taPathGetStation));
    config->config.pathGetProduct = String(lv_textarea_get_text(taPathGetProduct));
    config->config.pathGetRunningProductOnStation = String(lv_textarea_get_text(taPathGetRunningProduct));
    config->config.pathPostChangedProduct = String(lv_textarea_get_text(taPathPostChangedProduct));

    config->saveConfig();
    Serial.println("Configuration Saved");

    // Display confirmation message
    lv_obj_t *msg = lv_msgbox_create(NULL, "Success", "Configuration saved successfully.", NULL, true);
    lv_obj_center(msg);
    lv_obj_add_event_cb(msg, [](lv_event_t *e)
                        {
        lv_obj_t* obj = lv_event_get_target(e);
        lv_obj_del(obj); }, LV_EVENT_CLICKED, NULL);
}
