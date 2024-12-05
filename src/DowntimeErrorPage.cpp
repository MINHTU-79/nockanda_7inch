#include "DowntimePage.hpp"
#include "DowntimeErrorPage.hpp"
#include "production.hpp"
#include "Data.hpp"


extern DowntimePage *downtimePage;
extern ProductionPage *productionPage;
extern lv_obj_t *create_top_bar(lv_obj_t *parent, const char *title, lv_event_cb_t back_cb, void *back_cb_user_data, lv_event_cb_t refresh_cb, void *refresh_cb_user_data);

DowntimeErrorPage::DowntimeErrorPage(ConfigManager *configMgr)
    : config(configMgr)
{
    // Khởi tạo trang
    page = lv_obj_create(NULL);

    // Tạo thanh bar với tiêu đề "Configuration"
    create_top_bar(
        page,
        "Nhập lỗi dừng máy", // Tiêu đề
        [](lv_event_t *e)
        { lv_disp_load_scr(downtimePage->getPage()); }, // Callback nút Back
        nullptr,                                        // Không cần user_data cho Back
        nullptr,                                        // Callback nút Refresh
        nullptr);

    // lv_obj_set_size(page, LV_HOR_RES, LV_VER_RES);
    // container = lv_obj_create(page);
    // lv_obj_set_size(container, LV_PCT(100), LV_PCT(100));
    // lv_obj_set_layout(container, LV_LAYOUT_FLEX);
    // lv_obj_set_flex_flow(container, LV_FLEX_FLOW_COLUMN);
    // lv_obj_set_scroll_snap_y(container, LV_SCROLL_SNAP_CENTER);

  
}

// lv_obj_t *DowntimeErrorPage::getPage(std::string code, std::string other)
// {

//     fetchDowntimeError();

//     return page;
// }

//ver 2 khác tham số
lv_obj_t* DowntimeErrorPage::getPage(const DowntimeItem& itemData) {
    // Lưu bản sao của itemData
    currentItemData = itemData;

    // Gọi hàm xử lý tiếp
    //fetchDowntimeError();

    // Trả về đối tượng trang (giả định đã tồn tại trong class)
    return page;
}

// void DowntimeErrorPage::showPopup()
// {
//     // Tạo popup
//     lv_obj_t *popup = lv_obj_create(page);
//     lv_obj_set_size(popup, LV_PCT(90), LV_PCT(60));
//     lv_obj_center(popup);

//     // Tạo nút đầu tiên
//     lv_obj_t *btnOption1 = lv_btn_create(popup);
//     lv_obj_set_size(btnOption1, LV_PCT(80), 40);
//     lv_obj_align(btnOption1, LV_ALIGN_TOP_MID, 0, 20);
//     lv_obj_t *label1 = lv_label_create(btnOption1);
//     lv_label_set_text(label1, "Option 1");
//     lv_obj_center(label1);
//     lv_obj_add_event_cb(btnOption1, [](lv_event_t *e) {
//         DowntimeErrorPage *page = (DowntimeErrorPage *)lv_event_get_user_data(e);
//         page->handleOption1();
//     }, LV_EVENT_CLICKED, this);

//     // Tạo nút thứ hai
//     lv_obj_t *btnOption2 = lv_btn_create(popup);
//     lv_obj_set_size(btnOption2, LV_PCT(80), 40);
//     lv_obj_align(btnOption2, LV_ALIGN_TOP_MID, 0, 80);
//     lv_obj_t *label2 = lv_label_create(btnOption2);
//     lv_label_set_text(label2, "Option 2");
//     lv_obj_center(label2);
//     lv_obj_add_event_cb(btnOption2, [](lv_event_t *e) {
//         DowntimeErrorPage *page = (DowntimeErrorPage *)lv_event_get_user_data(e);
//         page->handleOption2();
//     }, LV_EVENT_CLICKED, this);
// }

void DowntimeErrorPage::showPopup()
{
    // Tạo popup
    lv_obj_t *popup = lv_obj_create(page);
    lv_obj_set_size(popup, LV_PCT(90), LV_PCT(60));
    lv_obj_center(popup);

    if (roll.cavity == 1 || roll.cavity == 2 || roll.cavity == 3)
    {
        // Tạo nút đầu tiên
        lv_obj_t *btnOption1 = lv_btn_create(popup);
        lv_obj_set_size(btnOption1, LV_PCT(80), 40);
        lv_obj_align(btnOption1, LV_ALIGN_TOP_MID, 0, 50);
        lv_obj_t *label1 = lv_label_create(btnOption1);
        lv_label_set_text(label1, "Cuộn A");
        lv_obj_set_style_text_font(label1, &arial, 0);
        lv_obj_center(label1);
        lv_obj_add_event_cb(btnOption1, [](lv_event_t *e)
                            {
        DowntimeErrorPage *page = (DowntimeErrorPage *)lv_event_get_user_data(e);
        page->handleOption1("A"); }, LV_EVENT_CLICKED, this);
    }

    if (roll.cavity == 2 || roll.cavity == 3)
    {
        // Tạo nút thứ hai
        lv_obj_t *btnOption2 = lv_btn_create(popup);
        lv_obj_set_size(btnOption2, LV_PCT(80), 40);
        lv_obj_align(btnOption2, LV_ALIGN_TOP_MID, 0, 110);
        lv_obj_t *label2 = lv_label_create(btnOption2);
        lv_label_set_text(label2, "Cuộn B");
        lv_obj_set_style_text_font(label2, &arial, 0);
        lv_obj_center(label2);
        lv_obj_add_event_cb(btnOption2, [](lv_event_t *e)
                            {
        DowntimeErrorPage *page = (DowntimeErrorPage *)lv_event_get_user_data(e);
        page->handleOption1("B"); }, LV_EVENT_CLICKED, this);
    }
    //     // Tạo nút đóng
    //     lv_obj_t *btnClose = lv_btn_create(popup);
    //     lv_obj_set_size(btnClose, 40, 40);
    //     lv_obj_align(btnClose, LV_ALIGN_TOP_RIGHT, -, 10);
    //     lv_obj_t *labelClose = lv_label_create(btnClose);
    //     lv_label_set_text(labelClose, "X");
    //     lv_obj_center(labelClose);
    //     lv_obj_add_event_cb(btnClose, [](lv_event_t *e) {
    //         lv_obj_t *popup = lv_event_get_target(e);
    //         lv_obj_del_async(popup); // Xóa popup
    //     }, LV_EVENT_CLICKED, popup);
    // }
    // Tạo nút đóng
    lv_obj_t *btnClose = lv_btn_create(popup);
    lv_obj_set_size(btnClose, 30, 30);
    lv_obj_align(btnClose, LV_ALIGN_TOP_RIGHT, -3, 3);
    lv_obj_t *labelClose = lv_label_create(btnClose);
    // Set the background color of the button to red
    lv_obj_set_style_bg_color(btnClose,  lv_color_hex(0xFF0000), LV_PART_MAIN);

    lv_label_set_text(labelClose, "X");
    lv_obj_center(labelClose);
    lv_obj_add_event_cb(btnClose, [](lv_event_t *e)
                        {
                            lv_obj_t *btnClose = lv_event_get_target(e);   // Lấy nút "Close"
                            lv_obj_t *popup = lv_obj_get_parent(btnClose); // Lấy cha của nút, tức là popup
                            lv_obj_del_async(popup);                       // Xóa popup
                        },
                        LV_EVENT_CLICKED, nullptr);
}

// // ver 1
// void DowntimeErrorPage::handleOption1()
// {
//     Serial.println("Option 1 Selected");
//     HTTPClient http;
//     String url = config->config.httpIP + (config->config.httpPort != 0 ? ":" + String(config->config.httpPort) : "")    + "/api/Production/option1";
//     http.begin(url);
//     http.addHeader("Content-Type", "application/json");

//     String payload = "{\"data\": \"value1\"}";
//     int httpCode = http.POST(payload);

//     if (httpCode == HTTP_CODE_OK)
//     {
//         String response = http.getString();
//         Serial.println("Response for Option 1: " + response);
//         showResult("Option 1 Success", response);
//     }
//     else
//     {
//         Serial.printf("Option 1 failed, code: %d\n", httpCode);
//         showResult("Option 1 Failed", "Error: " + String(httpCode));
//     }
//     http.end();
// }

// ver 2 làm khi đã có backend của quốc anh 

void DowntimeErrorPage::handleOption1(String rollPrefix)
{
    Serial.println("Option 1 Selected");
    HTTPClient http;

    http.setTimeout(3000);  // timeout trong 5000ms (5 giây)
    String url = config->config.httpIP + (config->config.httpPort != 0 ? ":" + String(config->config.httpPort) : "")  + "/api/rolls/submit-downtime";
    http.begin(url);
    http.addHeader("Content-Type", "application/json");

    // Tạo payload đúng định dạng DowntimeInputRequest
    String stationCode = roll.stationCode; // Thay thế bằng giá trị thực tế
    String downtimeInputId = currentItemData.id; // Thay thế bằng giá trị thực tế
    //String downtimeCode = currentItemData.downtimeCode; // Thay thế bằng giá trị thực tế

    DowntimeErrorItem *selectedItem = getSelectedItem();
    if (selectedItem != nullptr)
    {
        // Xử lý item được chọn
        String payload = "{";
        payload += "\"StationCode\": \"" + stationCode + "\",";
        payload += "\"DowntimeInputId\": \"" + downtimeInputId + "\",";
        payload += "\"DowntimeCode\": \"" + String(selectedItem->downtimeCode.c_str()) + "\",";
        payload += "\"RollPrefix\": \"" + rollPrefix + "\"";
        payload += "}";

        // Gửi request POST
        Serial.println("GGGGGGGGGSending payload for Option 1: " + payload);
        Serial.println("GGGGGGGGGGGURL: " + url);
        int httpCode = http.POST(payload);

        // Kiểm tra phản hồi từ server
        String response = http.getString();
        Serial.printf("HTTP Code: %d\n", httpCode);
        Serial.println("Server response: " + response);

        // bùa lại chổ này .
        //showResult("Success", "Nhap loi thanh cong");

        // Serial.println("showResult");

        //không reset vì nó gây lỗi tùm lum .xung đột với tự động refetch dùng 
        //productionPage->fetchProducts();
//        Serial.println("productionPage->init();: ");


        if (httpCode == HTTP_CODE_OK)
        {
            showResult("Thành công", response);

            productionPage->fetchProducts();

            lv_disp_load_scr(downtimePage->getPage());
        }
        else
        {
            showResult("Thất bại", "Lỗi: " + String(httpCode) + ", Tin nhắn: " + response);
        }


    }
    else
    {
        // Không có item nào được chọn
           showResult("Lỗi:", "Phải chọn ít nhất một lỗi!");
    }
    // String rollPrefix = rollPrefix; // Thay thế bằng giá trị thực tế

    http.end();
}

// cái này không còn dùng nữa . he he
void DowntimeErrorPage::handleOption2()
{
    Serial.println("Option 2 Selected");
    HTTPClient http;
    String url = config->config.httpIP + (config->config.httpPort != 0 ? ":" + String(config->config.httpPort) : "")    + "/api/Production/option2";
    http.begin(url);
    http.addHeader("Content-Type", "application/json");

    String payload = "{\"data\": \"value2\"}";
    int httpCode = http.POST(payload);

    if (httpCode == HTTP_CODE_OK)
    {
        String response = http.getString();
        Serial.println("Response for Option 2: " + response);
        showResult("Option 2 Success", response);
    }
    else
    {
        Serial.printf("Option 2 failed, code: %d\n", httpCode);
        showResult("Option 2 Failed", "Error: " + String(httpCode));
    }
    http.end();
}

//ver này chưa có tự dộng tắt 
// void DowntimeErrorPage::showResult(const String &title, const String &message)
// {
//     lv_obj_t *msgbox = lv_msgbox_create(NULL, title.c_str(), message.c_str(), NULL, true);
//     lv_obj_set_style_text_font(msgbox, &arial, 0);
//     lv_obj_center(msgbox);
//     Serial.println(title + ": " + message);
// }


//ver này đã có tự động tắt 
void DowntimeErrorPage::showResult(const String &title, const String &message)
{
    // Tạo hộp thoại thông báo
    lv_obj_t *msgbox = lv_msgbox_create(NULL, title.c_str(), message.c_str(), NULL, true);
    lv_obj_set_style_text_font(msgbox, &arial, 0);
    lv_obj_center(msgbox);
    /* Lấy nút Close */
lv_obj_t *close_btn = lv_msgbox_get_close_btn(msgbox);

/* Thay ký tự mặc định bằng '×' */
lv_label_set_text(lv_obj_get_child(close_btn, 0), "×");

    // // Tạo bộ hẹn giờ tự động xóa hộp thoại sau 2 giây
    // lv_timer_t *timer = lv_timer_create([](lv_timer_t *timer) {
    //     lv_obj_t *msgbox = static_cast<lv_obj_t *>(timer->user_data);
    //     if (msgbox) {
    //         lv_obj_del(msgbox);
    //     }
    // }, 2000, msgbox);

    //  // Tạo bộ hẹn giờ tự động nhấn nút "Close" sau 2 giây
    // lv_timer_t *timer = lv_timer_create([](lv_timer_t *timer) {
    //     lv_obj_t *msgbox = static_cast<lv_obj_t *>(timer->user_data);
    //     if (msgbox) {
    //         // Lấy nút "Close" của msgbox
    //         lv_obj_t *close_btn = lv_msgbox_get_close_btn(msgbox);
    //         if (close_btn) {
    //             // Gửi sự kiện nhấn nút
    //             lv_event_send(close_btn, LV_EVENT_CLICKED, NULL);
    //         }
    //     }
    // }, 2000, msgbox);

    // In ra Serial log
    Serial.println(title + ": " + message);
}

void DowntimeErrorPage::init()
{
    std::vector<DowntimeErrorItem> downtimeDetails{};
    Serial.println("DowntimeErrorPage::init");

    // lv_obj_t *container1 = lv_obj_create(page);            // Or use an appropriate parent
    // lv_obj_set_style_pad_all(container1, 0, LV_PART_MAIN); //
    // lv_obj_set_size(container1, LV_PCT(100), LV_PCT(100));

    content_container = lv_obj_create(page);
    lv_obj_set_size(content_container, LV_PCT(100), lv_obj_get_height(page) - 50);
    lv_obj_set_style_pad_all(content_container, 0, LV_PART_MAIN);
    lv_obj_clear_flag(content_container, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_clear_flag(content_container, LV_OBJ_FLAG_SCROLL_MOMENTUM); // Tắt cuộn quán tính cho trang chính
    lv_obj_align(content_container, LV_ALIGN_TOP_MID, 0, 50);          // Đặt container1 dưới thanh bar, bỏ 50px


     // Tạo container con sử dụng flex layout để sắp xếp các thành phần theo cột
    container = lv_obj_create(content_container);
    lv_obj_set_flex_flow(container, LV_FLEX_FLOW_COLUMN); // Sắp xếp theo cột
    lv_obj_set_flex_align(container, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
    lv_obj_set_size(container, LV_PCT(100), LV_PCT(100)); // Kích thước đầy đủ

    // Set container scrollable
    lv_obj_set_scrollbar_mode(container, LV_SCROLLBAR_MODE_ON);
    lv_obj_set_scroll_dir(container, LV_DIR_VER);              // Vertical scrolling
    lv_obj_clear_flag(container, LV_OBJ_FLAG_SCROLL_MOMENTUM); // Tắt cuộn quán tính cho trang chính

    // Thiết lập layout của container cha (container)
    lv_obj_set_layout(container, LV_LAYOUT_FLEX);
    lv_obj_set_flex_flow(container, LV_FLEX_FLOW_ROW_WRAP);                                          // Đặt chế độ FLEX theo hàng và bọc lại
    lv_obj_set_flex_align(container, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_START); // Căn chỉnh các item

    // Đảm bảo container có chiều rộng 100% của màn hình
    lv_obj_set_style_width(container, LV_PCT(100), 0);
 

    fetchDowntimeError();

    // Tạo nút cuối trang
    lv_obj_t *btnShowPopup = lv_btn_create(page);
    lv_obj_set_size(btnShowPopup, LV_PCT(80), 40);
    lv_obj_align(btnShowPopup, LV_ALIGN_BOTTOM_MID, 0, -10);
    lv_obj_t *label = lv_label_create(btnShowPopup);
    lv_label_set_text(label, "Gửi thông tin");
    lv_obj_set_style_text_font(label, &arial, 0);
    lv_obj_center(label);

    // Gắn sự kiện khi nhấn nút
    lv_obj_add_event_cb(btnShowPopup, [](lv_event_t *e)
                        {
        DowntimeErrorPage *page = (DowntimeErrorPage *)lv_event_get_user_data(e);
        page->showPopup(); }, LV_EVENT_CLICKED, this);
}
// void DowntimeErrorPage::fetchDowntimeError()
// {
//     HTTPClient http;
//     String url = config->config.httpIP + (config->config.httpPort != 0 ? ":" + String(config->config.httpPort) : "")    + config->config.pathGetProduct ;

//     Serial.println("Fetching Downtime Codes from: " + url);

//     try
//     {
//         http.begin(url);
//         int httpCode = http.GET();

//         if (httpCode == HTTP_CODE_OK)
//         {
//             String payload = http.getString();
//             Serial.println("Downtime Codes Payload: " + payload);

//             // Parse JSON
//             DynamicJsonDocument doc(5024); // Điều chỉnh kích thước tùy theo dữ liệu
//             DeserializationError error = deserializeJson(doc, payload);

//             if (error)
//             {
//                 Serial.print(F("deserializeJson() failed: "));
//                 Serial.println(error.f_str());
//                 return; // Thoát nếu phân giải JSON thất bại
//             }

//             // Duyệt qua danh sách downtime codes
//             JsonArray downtimeArray = doc.as<JsonArray>();
//             downtimeDetails.clear(); // Xóa danh sách cũ trước khi cập nhật

//             for (JsonObject downtime : downtimeArray)
//             {
//                 DowntimeErrorItem item;
//                 item.code = downtime["Code"].as<String>().c_str();
//                 item.name = downtime["Name"].as<String>().c_str();
//                 downtimeDetails.push_back(item);
//             }

//             if (downtimeDetails.empty())
//             {
//                 // Thêm một phần tử mặc định nếu danh sách rỗng
//                 DowntimeErrorItem defaultItem;
//                 defaultItem.code = "N/A";
//                 defaultItem.name = "No Downtime Codes Available";
//                 downtimeDetails.push_back(defaultItem);
//             }

//             // Gọi hàm cập nhật UI
//             updateUI();
//         }
//         else
//         {
//             Serial.printf("HTTP request failed, code: %d\n", httpCode);
//             showError("Failed to fetch downtime codes. Code: " + String(httpCode));
//         }
//     }
//     catch (...)
//     {
//         Serial.println("Exception caught during fetchDowntimeError.");
//         showError("An exception occurred while fetching downtime codes.");
//     }

//     http.end();
// }

void DowntimeErrorPage::fetchDowntimeError()
{
    HTTPClient http;
    String url = config->config.httpIP + (config->config.httpPort != 0 ? ":" + String(config->config.httpPort) : "")    + config->config.pathGetProduct;

    Serial.println("Fetching Downtime Codes from: " + url);

    try
    {
        http.begin(url);
        int httpCode = http.GET();

        if (httpCode == HTTP_CODE_OK)
        {
            String payload = http.getString();
            Serial.println("Downtime Codes Payload: " + payload);

            // Parse JSON
            DynamicJsonDocument doc(10024); // Tăng kích thước bộ đệm nếu cần
            DeserializationError error = deserializeJson(doc, payload);

            if (error)
            {
                Serial.print(F("deserializeJson() failed: "));
                Serial.println(error.f_str());
                return;
            }

            // Duyệt qua danh sách downtime codes
            JsonArray downtimeArray = doc.as<JsonArray>();
            downtimeDetails.clear();

            for (JsonObject downtime : downtimeArray)
            {
                DowntimeErrorItem item;
                item.downtimeCode = downtime["downtimeCode"] | ""; // Xử lý giá trị null
                item.downtimeName = downtime["downtimeName"] | "";
                item.description = downtime["description"] | "";
                item.createTime = downtime["createTime"] | 0;
                item.downtimeType = downtime["downtimeType"] | 0;
                item.downtimeGroup = downtime["downtimeGroup"] | "";
                item.userCreate = downtime["userCreate"] | "";
                item.isEnable = downtime["isEnable"] | false;
                item.selected = false;
                downtimeDetails.push_back(item);
            }

            if (downtimeDetails.empty())
            {
                // Thêm một phần tử mặc định nếu danh sách rỗng
                // DowntimeErrorItem defaultItem{"N/A", "No Downtime Codes Available", 0, 0, "", "", false, false};

                DowntimeErrorItem defaultItem = {"N/A","N/A", "No Downtime Codes Available", 0, 0, "", "", false, false};

                downtimeDetails.push_back(defaultItem);
            }

            // Gọi hàm cập nhật UI
            updateUI();
        }
        else
        {
            Serial.printf("HTTP request failed, code: %d\n", httpCode);
            // tạm thời không cần dùng
            // showError("Không thể  lấy Mã dừng máy. Code: " + String(httpCode));
        }
    }
    catch (...)
    {
        Serial.println("Exception caught during fetchDowntimeError.");
        showError("Đã xảy ra lỗi khi lấy mã dừng máy.");
    }

    http.end();
}

//ver không tự tắt 
// void DowntimeErrorPage::showError(const String &message)
// {
//     lv_obj_t *msgbox = lv_msgbox_create(NULL, "Lỗi", message.c_str(), NULL, true);
//     lv_obj_set_style_text_font(msgbox, &arial, 0);
//     lv_obj_center(msgbox);
//     Serial.println("Error: " + message);
// }


// ver có tự động tắt .
void DowntimeErrorPage::showError(const String &message)
{
    // Tạo hộp thoại thông báo
    lv_obj_t *msgbox = lv_msgbox_create(NULL, "Lỗi", message.c_str(), NULL, true);
    lv_obj_set_style_text_font(msgbox, &arial, 0);
    lv_obj_center(msgbox);

    /* Lấy nút Close */
lv_obj_t *close_btn = lv_msgbox_get_close_btn(msgbox);

/* Thay ký tự mặc định bằng '×' */
lv_label_set_text(lv_obj_get_child(close_btn, 0), "×");

    // // Tạo bộ hẹn giờ tự động xóa hộp thoại sau 2 giây
    // lv_timer_t *timer = lv_timer_create([](lv_timer_t *timer) {
    //     lv_obj_t *msgbox = static_cast<lv_obj_t *>(timer->user_data);
    //     if (msgbox) {
    //         lv_obj_del(msgbox);
    //     }
    // }, 2000, msgbox);

    // In ra Serial log
    Serial.println("Error: " + message);
}


// //ver 2 co do khi click
// // Callback sự kiện cho mỗi item
// static void item_event_cb(lv_event_t *e)
// {
//     lv_obj_t *itemContainer = lv_event_get_target(e);
//     lv_obj_set_style_bg_color(itemContainer, lv_palette_main(LV_PALETTE_RED), 0);
// }

// ver 3 là lựa chọn có select và app dụng vào field trong DowntimeErrorItem

// Callback sự kiện cho mỗi item
static void item_event_cb(lv_event_t *e)
{
    lv_obj_t *itemContainer = lv_event_get_target(e);


    // Lấy con trỏ đến item thông qua user data
    DowntimeErrorItem *itemData = static_cast<DowntimeErrorItem *>(lv_event_get_user_data(e));

    Serial.println("Click on item downtime code selected: " + String(itemData->selected));

    // Đổi trạng thái
    if (itemData->selected) {
        lv_obj_set_style_bg_color(itemContainer, lv_color_white(), 0); // Trở về màu trắng
        itemData->selected = false;                                   // Bỏ select
        Serial.println("unselect item downtime code");
    } else {
        lv_obj_set_style_bg_color(itemContainer, lv_palette_main(LV_PALETTE_RED), 0); // Đổi sang màu đỏ
        itemData->selected = true;                                                    // Select
        Serial.println("select item downtime code");
    }
}



void DowntimeErrorPage::updateUI()
{
      //==============
    
    // lv_obj_invalidate(container);

     if (container != NULL && lv_obj_is_valid(container))
    {
        // lv_obj_del(container); // Xóa tất cả các widget cũ trong container
        lv_obj_clean(container); // Xóa tất cả các widget cũ trong container

    }

    // Duyệt qua các item và tạo các container cho mỗi item
    for ( auto &item : downtimeDetails)
    {
        lv_obj_t *itemContainer = lv_obj_create(container);
        lv_obj_set_size(itemContainer, LV_PCT(47), LV_PCT(22)); // Mỗi item chiếm 47% chiều rộng của container

        // Thiết lập layout của itemContainer để các item trong mỗi container sắp xếp theo cột
        lv_obj_set_layout(itemContainer, LV_LAYOUT_FLEX);
        lv_obj_set_flex_flow(itemContainer, LV_FLEX_FLOW_COLUMN); // Sắp xếp theo cột trong mỗi item
        lv_obj_set_style_pad_all(itemContainer, 5, 0);
        lv_obj_set_style_align(itemContainer, LV_ALIGN_CENTER, 0);
        lv_obj_set_scrollbar_mode(itemContainer, LV_SCROLLBAR_MODE_OFF); // Tắt scrollbar để không cuộn ngang

        // Gán callback sự kiện
        // lv_obj_add_event_cb(itemContainer, item_event_cb, LV_EVENT_CLICKED, NULL);

        // Gán callback sự kiện và truyền con trỏ đến item
        lv_obj_add_event_cb(itemContainer, item_event_cb, LV_EVENT_CLICKED, static_cast<void *>(&item));
        // Tạo label cho mỗi item
        lv_obj_t *labelName = lv_label_create(itemContainer);
        lv_obj_set_style_text_font(labelName, &arial, 0);
        lv_label_set_text_fmt(labelName, "%s: %s", item.downtimeCode.c_str(), item.downtimeName.c_str());
        lv_obj_set_style_text_align(labelName, LV_TEXT_ALIGN_CENTER, 0);
        lv_obj_set_style_align(labelName, LV_ALIGN_CENTER, 0);
        lv_label_set_long_mode(labelName, LV_LABEL_LONG_WRAP); // Đảm bảo label tự động xuống dòng
        lv_obj_set_width(labelName, LV_PCT(100));              // Đặt chiều rộng của label để nó có thể xuống dòng
        lv_obj_clear_flag(labelName, LV_OBJ_FLAG_CLICKABLE);     // Xóa khả năng click của labelName 
    }

    // Căn chỉnh lại container nếu cần
    lv_obj_align(container, LV_ALIGN_TOP_MID, 0, 0);

     // Thêm padding dưới cùng để tránh bị che bởi bàn phím
    lv_obj_set_style_pad_bottom(container, 100, 0); // 100 pixels hoặc chiều cao của bàn phím
}

// void DowntimeErrorPage::eventHandler(lv_event_t *e)
// {
//     DowntimeErrorPage *page = (DowntimeErrorPage *)lv_event_get_user_data(e);
//     if (e->code == LV_EVENT_CLICKED)
//     {
//         if (e->target == page->btnSave)
//         {
//             page->onSave();
//         }
//     }
// }