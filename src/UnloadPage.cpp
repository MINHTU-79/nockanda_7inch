#include "production.hpp"
#include "UnloadPage.hpp"
#include "Data.hpp"


// extern ConfigManager configMgr;
extern lv_obj_t *mainPage;
extern ProductionPage *productionPage;
extern lv_obj_t *create_top_bar(lv_obj_t *parent, const char *title, lv_event_cb_t back_cb, void *back_cb_user_data, lv_event_cb_t refresh_cb, void *refresh_cb_user_data);

UnloadPage::UnloadPage(ConfigManager *configMgr)
    : config(configMgr)
{
    page = lv_obj_create(NULL);

    Serial.println("UnloadPage init");
    // LV_LOG_USER("Page created: %p", page);

    // Tạo thanh bar với tiêu đề "Unload"
    create_top_bar(
        page,
        "Xuống cuộn", // Tiêu đề
        [](lv_event_t *e)
        { lv_disp_load_scr(productionPage->getPage()); }, // Callback nút Back
        nullptr,                                          // Không cần user_data cho Back
        nullptr,                                          // Callback nút Refresh
        this);
}

// // Hàm mã hóa URL
// String URLEncode(const char *msg)
// {
//     const char *hex = "0123456789ABCDEF";
//     String encodedMsg = "";
//     while (*msg != '\0')
//     {
//         if (isalnum(*msg) || *msg == '-' || *msg == '_' || *msg == '.' || *msg == '~')
//         {
//             encodedMsg += *msg;
//         }
//         else
//         {
//             encodedMsg += '%';
//             encodedMsg += hex[*msg >> 4];
//             encodedMsg += hex[*msg & 15];
//         }
//         msg++;
//     }
//     return encodedMsg;
// }

// Hàm tạo giao diện
void UnloadPage::init()
{

    // Container chính bao phủ toàn bộ trang (thanh bar đã có từ constructor)
    lv_obj_t *content_container = lv_obj_create(page);
    lv_obj_set_size(content_container, LV_PCT(100), lv_obj_get_height(page) - 50);
    lv_obj_set_style_pad_all(content_container, 0, LV_PART_MAIN);
    lv_obj_clear_flag(content_container, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_clear_flag(content_container, LV_OBJ_FLAG_SCROLL_MOMENTUM); // Tắt cuộn quán tính cho trang chính
    lv_obj_align(content_container, LV_ALIGN_TOP_MID, 0, 50);          // Đặt container1 dưới thanh bar, bỏ 50px

    // Tạo container con sử dụng flex layout để sắp xếp các thành phần theo cột
    lv_obj_t *container = lv_obj_create(content_container);
    lv_obj_set_flex_flow(container, LV_FLEX_FLOW_COLUMN); // Sắp xếp theo cột
    lv_obj_set_flex_align(container, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
    lv_obj_set_size(container, LV_PCT(100), LV_PCT(100)); // Kích thước đầy đủ

    // Set container scrollable
    lv_obj_set_scrollbar_mode(container, LV_SCROLLBAR_MODE_ON);
    lv_obj_set_scroll_dir(container, LV_DIR_VER);              // Vertical scrolling
    lv_obj_clear_flag(container, LV_OBJ_FLAG_SCROLL_MOMENTUM); // Tắt cuộn quán tính cho trang chính

    // // ---------------------------------------------------------------------------------
    // // Thêm nút Back với Icon vào container1, sử dụng vị trí tuyệt đối
    // // ---------------------------------------------------------------------------------
    // lv_obj_t *btnBack = lv_btn_create(container1);
    // lv_obj_set_size(btnBack, 30, 30); // Kích thước nhỏ cho nút icon

    // // Đặt vị trí tuyệt đối trong container1
    // lv_obj_set_style_pad_all(btnBack, 0, 0);
    // lv_obj_set_pos(btnBack, 10, 10);              // Đặt ở góc trên bên trái
    // lv_obj_set_align(btnBack, LV_ALIGN_TOP_LEFT); // Căn lề trên cùng bên trái

    // lv_obj_add_event_cb(btnBack, [](lv_event_t *e)
    //                     {
    //                       lv_disp_load_scr(mainPage); // Điều hướng trở lại trang chính
    //                     },
    //                     LV_EVENT_CLICKED, NULL);

    // // Thêm icon vào nút Back (sử dụng LV_SYMBOL_LEFT)
    // lv_obj_t *btnBackIcon = lv_label_create(btnBack);
    // lv_label_set_text(btnBackIcon, LV_SYMBOL_LEFT); // Icon mũi tên trái
    // lv_obj_center(btnBackIcon);

    // // ---------------------------------------------------------------------------------
    // // Thêm nút Refetch với Icon vào container1, sử dụng vị trí tuyệt đối
    // // ---------------------------------------------------------------------------------
    // lv_obj_t *btnRefetch = lv_btn_create(container1); // Tạo nút Refetch
    // lv_obj_set_size(btnRefetch, 30, 30);              // Kích thước nhỏ cho nút icon

    // // Đặt vị trí tuyệt đối trong container1
    // lv_obj_set_style_pad_all(btnRefetch, 0, 0);
    // lv_obj_set_pos(btnRefetch, 0, 10);                // Đặt bên cạnh nút Back
    // lv_obj_set_align(btnRefetch, LV_ALIGN_TOP_RIGHT); // Căn lề trên cùng bên trái

    // // Sử dụng eventHandler với user_data là this
    // lv_obj_add_event_cb(btnRefetch, UnloadPage::refreshPage, LV_EVENT_CLICKED, this);

    // // Thêm icon vào nút Refetch (sử dụng LV_SYMBOL_REFRESH)
    // lv_obj_t *btnRefetchIcon = lv_label_create(btnRefetch);
    // lv_label_set_text(btnRefetchIcon, LV_SYMBOL_REFRESH); // Icon làm mới
    // lv_obj_center(btnRefetchIcon);

    // ---------------------------------------------------------------------------------
    // Thêm các thành phần từ đoạn mã đầu tiên vào container sử dụng flex layout
    // ---------------------------------------------------------------------------------

    // Tạo container cho thông tin Running Job
    lv_obj_t *jobInfoContainer = lv_obj_create(container);
    lv_obj_set_size(jobInfoContainer, LV_PCT(98), LV_SIZE_CONTENT);                                  // Chiều cao phù hợp
    lv_obj_set_style_bg_color(jobInfoContainer, lv_color_hex(0xF0F0F0), LV_PART_MAIN);               // Nền màu nhẹ
    lv_obj_set_style_radius(jobInfoContainer, 10, LV_PART_MAIN);                                     // Góc bo tròn
    lv_obj_set_style_pad_all(jobInfoContainer, 10, LV_PART_MAIN);                                    // Padding bên trong
    lv_obj_set_style_border_width(jobInfoContainer, 2, LV_PART_MAIN);                                // Độ dày viền
    lv_obj_set_style_border_color(jobInfoContainer, lv_palette_main(LV_PALETTE_BLUE), LV_PART_MAIN); // Màu viền

    // Tạo layout dạng lưới cho jobInfoContainer
    lv_obj_set_flex_flow(jobInfoContainer, LV_FLEX_FLOW_ROW_WRAP);
    lv_obj_set_flex_align(jobInfoContainer, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_START);

    // Tạo các label cho thông tin Job

    // Ma Hang
    labelSoMeter = lv_label_create(jobInfoContainer);
    lv_label_set_text(labelSoMeter, "Số mét chạy: N/A");
    lv_obj_set_style_text_font(labelSoMeter, &arial, 0);
    lv_obj_set_style_text_color(labelSoMeter, lv_color_hex(0x333333), 0);
    lv_obj_set_width(labelSoMeter, LV_PCT(98));

    //   // ProductCode
    //   labelProductCode = lv_label_create(jobInfoContainer);
    //   lv_label_set_text(labelProductCode, "ProductCode: N/A");
    //   lv_obj_set_style_text_font(labelProductCode, &lv_font_montserrat_14, 0);  // Font chữ lớn hơn
    //   lv_obj_set_style_text_color(labelProductCode, lv_color_hex(0x333333), 0); // Màu chữ tối hơn
    //   lv_obj_set_width(labelProductCode, LV_PCT(98));

    //======================================================================


    // Tạo label cho totalMeterRan
    lv_obj_t *labeltotalMeterRan = lv_label_create(container);
    lv_label_set_text(labeltotalMeterRan, "Điều chỉnh số mét chạy:");
    lv_obj_set_style_text_font(labeltotalMeterRan, &arial, 0);
    lv_obj_set_width(labeltotalMeterRan, LV_SIZE_CONTENT); // Đảm bảo độ rộng phù hợp với nội dung
    lv_obj_set_style_text_align(labeltotalMeterRan, LV_TEXT_ALIGN_LEFT, 0);

    // Tạo textarea cho totalMeterRan
    adjustmentMetRun = lv_textarea_create(container);
    //   lv_textarea_set_placeholder_text(adjustmentMetRun, "Search Station...");
    lv_textarea_set_one_line(adjustmentMetRun, true);
    lv_obj_set_width(adjustmentMetRun, LV_PCT(98));
    lv_obj_add_event_cb(adjustmentMetRun, ta_event_cbB, LV_EVENT_CLICKED, this);
    lv_obj_add_event_cb(adjustmentMetRun, ta_event_cbB, LV_EVENT_DEFOCUSED, this);

    // // Tạo label cho rollCount
    // lv_obj_t *labelrollCount = lv_label_create(container);
    // lv_label_set_text(labelrollCount, "Điều chỉnh số cuộn:");
    // lv_obj_set_style_text_font(labelrollCount, &arial, 0);
    // lv_obj_set_width(labelrollCount, LV_SIZE_CONTENT); // Đảm bảo độ rộng phù hợp với nội dung
    // lv_obj_set_style_text_align(labelrollCount, LV_TEXT_ALIGN_LEFT, 0);

    // // Tạo textarea cho rollCount
    // rollCount = lv_textarea_create(container);
    // // lv_textarea_set_placeholder_text(rollCount, "Search Station...");
    // lv_textarea_set_one_line(rollCount, true);
    // lv_obj_set_width(rollCount, LV_PCT(98));
    // lv_obj_add_event_cb(rollCount, ta_event_cbB, LV_EVENT_CLICKED, this);
    // lv_obj_add_event_cb(rollCount, ta_event_cbB, LV_EVENT_DEFOCUSED, this);

    // Tạo label cho rollCount
    lv_obj_t *labelrollCount = lv_label_create(container);
    lv_label_set_text(labelrollCount, "Điều chỉnh số cuộn:");
    lv_obj_set_style_text_font(labelrollCount, &arial, 0);
    lv_obj_set_width(labelrollCount, LV_SIZE_CONTENT); // Đảm bảo độ rộng phù hợp với nội dung
    lv_obj_set_style_text_align(labelrollCount, LV_TEXT_ALIGN_LEFT, 0);
    lv_obj_add_flag(labelrollCount, LV_OBJ_FLAG_HIDDEN); // Ẩn label khỏi giao diện

    // Tạo textarea cho rollCount
    rollCount = lv_textarea_create(container);
    // lv_textarea_set_placeholder_text(rollCount, "Search Station...");
    lv_textarea_set_one_line(rollCount, true);
    lv_obj_set_width(rollCount, LV_PCT(98));
    lv_obj_add_event_cb(rollCount, ta_event_cbB, LV_EVENT_CLICKED, this);
    lv_obj_add_event_cb(rollCount, ta_event_cbB, LV_EVENT_DEFOCUSED, this);
    lv_textarea_set_text(rollCount, "1");           // Đặt giá trị mặc định là 1
    lv_obj_add_flag(rollCount, LV_OBJ_FLAG_HIDDEN); // Ẩn textarea khỏi giao diện

    //--------------------
    // Tạo nút xuong cuon
    // ---------------------------------------------------------------------------------
    btnUnload = lv_btn_create(container);
    lv_obj_set_size(btnUnload, 150, 50);
    lv_obj_add_event_cb(btnUnload, eventHandlerR, LV_EVENT_CLICKED, this);

    lv_obj_t *btnUnloadLabel = lv_label_create(btnUnload);
    lv_label_set_text(btnUnloadLabel, "Xuống cuộn");
    lv_obj_set_style_text_font(btnUnloadLabel, &arial, 0);
    lv_obj_center(btnUnloadLabel);
    // ---------------------------------------------------------------------------------
    // Tạo Keyboard
    // ---------------------------------------------------------------------------------
    keyboardD = lv_keyboard_create(content_container);
    lv_obj_set_size(keyboardD, LV_PCT(100), LV_PCT(50));
    lv_obj_align(keyboardD, LV_ALIGN_BOTTOM_MID, 0, 0);
    lv_keyboard_set_mode(keyboardD, LV_KEYBOARD_MODE_NUMBER);
    lv_obj_add_flag(keyboardD, LV_OBJ_FLAG_HIDDEN | LV_OBJ_FLAG_FLOATING); // Ẩn ban đầu và làm floating

    // Thêm padding dưới cùng để tránh bị che bởi bàn phím
    lv_obj_set_style_pad_bottom(container, 100, 0); // 100 pixels hoặc chiều cao của bàn phím

    // ---------------------------------------------------------------------------------
    // Fetch data từ APIs
    // ---------------------------------------------------------------------------------

    // Fetch Running Job thông tin cho station từ config.fxStationCode
 //   fetchPage();
}




// void UnloadPage::fetchPage(const String &stationCode)
// {
//   HTTPClient http;
//   String url = config->config.httpIP + (config->config.httpPort != 0 ? ":" + String(config->config.httpPort) : "")     +
//                config->config.pathGetRunningProductOnStation + "?station=" + URLEncode(stationCode.c_str());

//   Serial.println("Fetching Running Job from: " + url); // Debugging URL

//   try
//   {
//     http.begin(url);
//     int httpCode = http.GET();

//     if (httpCode == HTTP_CODE_OK)
//     {
//       String payload = http.getString();
//       Serial.println("Running Job Payload: " + payload); // Debugging payload

//       // Parse JSON
//       DynamicJsonDocument doc(1024); // Dung lượng có thể điều chỉnh tùy nhu cầu
//       DeserializationError error = deserializeJson(doc, payload);

//       if (error)
//       {
//         Serial.print(F("deserializeJson() failed: "));
//         Serial.println(error.f_str());
//         return; // Thoát nếu phân giải JSON thất bại
//       }

//       // Chuyển đổi sang JsonObject
//       JsonObject job = doc.as<JsonObject>();

//       if (job.isNull())
//       {
//         Serial.println("No running job found.");
//         // Cập nhật UI để hiển thị không có job
//         lv_label_set_text(labelProductCode, "ProductCode: N/A");
//         lv_label_set_text(labelPlanQty, "PlanQty: N/A");
//         lv_label_set_text(labelJobStatus, "JobStatus: N/A");
//         lv_label_set_text(labelStationCode, ("StationCode: " + stationCode).c_str());
//         return;
//       }

//       // Cập nhật UI với dữ liệu job
//       updateJobInfo(job);
//     }
//     else if (httpCode == 404)
//     {
//       String errorMessage = http.getString();
//       Serial.println("No running job found (404): " + errorMessage);
//       // Cập nhật UI để hiển thị không có job
//       lv_label_set_text(labelProductCode, "ProductCode: N/A");
//       lv_label_set_text(labelPlanQty, "PlanQty: N/A");
//       lv_label_set_text(labelJobStatus, "JobStatus: N/A");
//       lv_label_set_text(labelStationCode, ("StationCode: " + stationCode).c_str());

//       // Hiển thị thông báo cho người dùng (tùy chọn)
//       lv_obj_t *msgbox = lv_msgbox_create(NULL, "Notification", errorMessage.c_str(), NULL, true);
//       lv_obj_center(msgbox);
//     }
//     else if (httpCode == 500)
//     {
//       String errorMessage = http.getString();
//       Serial.println("Server Error (500): " + errorMessage);
//       // Cập nhật UI để hiển thị không có job hoặc giữ nguyên trạng thái hiện tại

//       // Hiển thị thông báo lỗi cho người dùng
//       lv_obj_t *msgbox = lv_msgbox_create(NULL, "Server Error", errorMessage.c_str(), NULL, true);
//       lv_obj_center(msgbox);
//     }
//     else
//     {
//       Serial.println("Error in HTTP request: " + String(httpCode));
//       // Có thể thêm xử lý cho các mã lỗi khác nếu cần
//     }
//   }
//   catch (...)
//   {
//     Serial.println("Exception caught during fetchPage."); // Xử lý ngoại lệ
//     // Có thể hiển thị thông báo lỗi cho người dùng
//     lv_obj_t *msgbox = lv_msgbox_create(NULL, "Error", "An error occurred when connecting to the server.", NULL, true);
//     lv_obj_center(msgbox);
//   }

//   http.end();
// }

void UnloadPage::fetchPage()
{
    if (!roll.stationCode.isEmpty() && roll.meterRan != 0.0f)
    {
        Serial.println("Show meterRan");
        // Cập nhật UI để hiển thị không có job
        lv_label_set_text_fmt(labelSoMeter,("Số mét chạy: " + std::to_string(roll.meterRan)).c_str());

        return;
    }
}

void UnloadPage::fetchMeterRan() {
    const String stationCode = config->config.fxStationCode;

    HTTPClient http;

    http.setTimeout(5000); // timeout trong 5000ms (5 giây)

    String url = config->config.httpIP + 
                 (config->config.httpPort != 0 ? ":" + String(config->config.httpPort) : "") +
                 config->config.pathGetRunningProductOnStation + URLEncode(stationCode.c_str());

    Serial.println("Fetching Running Job from: " + url); // Debugging URL

    http.begin(url);
    int httpCode = http.GET();

    if (httpCode == HTTP_CODE_OK) {
        String payload = http.getString();
        Serial.println("Payload: " + payload); // Debugging payload

        DynamicJsonDocument doc(512); // Dung lượng nhỏ hơn vì chỉ cần lấy một giá trị
        if (deserializeJson(doc, payload) == DeserializationError::Ok) {
            meterRan = doc["meterRan"] | 0.0f; // Giá trị mặc định là 0.0 nếu không tìm thấy
            Serial.println("meterRan: " + String(meterRan));
        } else {
            Serial.println("Failed to parse JSON.");
        }
    } else {
        Serial.println("HTTP GET failed with code: " + String(httpCode));
    }

    http.end();
}

lv_obj_t *UnloadPage::getPage()
{

    // lấy meterran moi nhat
    fetchMeterRan();

    lv_textarea_set_text(adjustmentMetRun, String(meterRan).c_str()); // Đồng bộ nội dung với textarea

    return page;
}

// Callback cho các trường nhập liệu
void UnloadPage::ta_event_cbB(lv_event_t *e)
{
    UnloadPage *page = (UnloadPage *)lv_event_get_user_data(e);
    lv_obj_t *target = lv_event_get_target(e);
    lv_obj_t *container = lv_obj_get_parent(target); // Get container to scroll

    if (lv_event_get_code(e) == LV_EVENT_CLICKED)
    {
        // Đặt bàn phím hướng đến textarea được focus
        lv_keyboard_set_textarea(page->keyboardD, target);
        lv_obj_clear_flag(page->keyboardD, LV_OBJ_FLAG_HIDDEN);

        // Tính toán xem textarea có bị che bởi bàn phím không
        lv_coord_t ta_y = lv_obj_get_y(target);                   // Y position of textarea
        lv_coord_t ta_height = lv_obj_get_height(target);         // Height of textarea
        lv_coord_t kb_y = lv_obj_get_y(page->keyboardD);           // Y position of keyboard
        lv_coord_t kb_height = lv_obj_get_height(page->keyboardD); // Height of keyboard

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
        lv_obj_add_flag(page->keyboardD, LV_OBJ_FLAG_HIDDEN);

        // Khôi phục chiều cao ban đầu của container
        lv_obj_set_height(container, LV_PCT(100));
    }
}

void UnloadPage::eventHandlerR(lv_event_t *e)
{
    UnloadPage *page = (UnloadPage *)lv_event_get_user_data(e);
    if (e->code == LV_EVENT_CLICKED)
    {
        if (e->target == page->btnUnload)
        {
            page->onChangeProduct();
        }
    }
}


// void UnloadPage::onChangeProduct()
// {
//     Serial.printf("onChangeProduct: selectedStationIdx = %d, selectedProductIdx = %d\n", lv_dropdown_get_selected(ddStation), lv_dropdown_get_selected(ddProduct));
//     int selectedStationIdx = lv_dropdown_get_selected(ddStation);
//     int selectedProductIdx = lv_dropdown_get_selected(ddProduct);

//     int stationIdx, productIdx;

//     // Kiểm tra nếu có bộ lọc hoặc không có
//     if (filteredStationIndices.empty())
//     {
//         // Nếu không có lọc, sử dụng danh sách gốc
//         stationIdx = selectedStationIdx;
//     }
//     else
//     {
//         // Nếu có lọc, lấy chỉ số gốc từ danh sách đã lọc
//         if (selectedStationIdx < 0 || selectedStationIdx >= filteredStationIndices.size())
//         {
//             Serial.println("Invalid station selection");
//             return;
//         }
//         stationIdx = filteredStationIndices[selectedStationIdx];
//     }

//     if (filteredProductIndices.empty())
//     {
//         // Nếu không có lọc, sử dụng danh sách gốc
//         productIdx = selectedProductIdx;
//     }
//     else
//     {
//         // Nếu có lọc, lấy chỉ số gốc từ danh sách đã lọc
//         if (selectedProductIdx < 0 || selectedProductIdx >= filteredProductIndices.size())
//         {
//             Serial.println("Invalid product selection");
//             return;
//         }
//         productIdx = filteredProductIndices[selectedProductIdx];
//     }

//     // Kiểm tra chỉ số hợp lệ trong danh sách gốc
//     if (stationIdx < 0 || stationIdx >= stations.size() || productIdx < 0 || productIdx >= products.size())
//     {
//         Serial.println("Invalid original selection");
//         return;
//     }

//     // Lấy mã Station và Product từ danh sách gốc
//     String stationCode = stations[stationIdx].StationCode;
//     String productCode = products[productIdx].ProductCode;

//     // Serial.println("onChangeProduct: ");
//     // Serial.println("  Station index: " + String(stationIdx));
//     // Serial.println("  Product index: " + String(productIdx));
//     // Serial.println("  Station code: " + stationCode);
//     // Serial.println("  Product code: " + productCode);
//     // // Xây dựng URL và gửi yêu cầu POST
//     // String url = config->config.httpIP + (config->config.httpPort != 0 ? ":" + String(config->config.httpPort) : "")    +
//     //              config->config.pathPostChangedProduct +
//     //              "?station=" + stationCode + "&product=" + productCode;
//     // Serial.println("URL: " + url);

//     // Xây dựng URL và gửi yêu cầu POST
//     String stationCodeEncoded = URLEncode(stationCode.c_str());
//     String productCodeEncoded = URLEncode(productCode.c_str());
//     String url = config->config.httpIP + (config->config.httpPort != 0 ? ":" + String(config->config.httpPort) : "")  +
//                  config->config.pathPostChangedProduct +
//                  "?station=" + stationCodeEncoded + "&product=" + productCodeEncoded;
//     Serial.println("URL: " + url);

//     HTTPClient http;
//     http.begin(url);
//     http.addHeader("Content-Type", "application/json");
//     int httpCode = http.POST(""); // Assuming no body is needed

//     // if (httpCode == HTTP_CODE_OK || httpCode == HTTP_CODE_CREATED)
//     // {
//     //   Serial.println("Product changed successfully");
//     // }
//     // else
//     // {
//     //   Serial.println("Failed to change product");
//     // }
//     // http.end();

//     // Kiểm tra mã trạng thái phản hồi

//     String payload = ""; // Khai báo biến để lưu trữ nội dung trả về

//     // Kiểm tra mã phản hồi từ server
//     if (httpCode == HTTP_CODE_OK || httpCode == HTTP_CODE_CREATED)
//     {
//         // Lấy nội dung trả về từ server
//         payload = http.getString();
//         Serial.println("Server Response: " + payload); // In ra Serial để theo dõi

//         // Tạo message box để hiển thị nội dung phản hồi
//         lv_obj_t *msgbox = lv_msgbox_create(NULL, "Success", payload.c_str(), NULL, true); // Tạo msgbox
//         lv_obj_center(msgbox);                                                             // Căn giữa msgbox

//         Serial.println("Product changed successfully");
//     }
//     else
//     {
//         // Xử lý trường hợp lỗi
//         payload = http.getString(); // Lấy thông tin chi tiết lỗi từ server (nếu có)
//         Serial.println("Failed to change product. Server Response: " + payload);

//         // // Tạo message box để hiển thị thông báo lỗi
//         // lv_obj_t *msgbox = lv_msgbox_create(NULL, "Error", "Failed to change product", NULL, true); // Tạo msgbox
//         // lv_obj_center(msgbox);                                                                      // Căn giữa msgbox

//         // Tạo message box để hiển thị thông báo lỗi
//         lv_obj_t *msgbox = lv_msgbox_create(NULL, "Error", "Failed to change product", NULL, true); // Tạo msgbox
//         lv_obj_center(msgbox);                                                                      // Căn giữa msgbox

//         // Tạo style mới để đổi màu chữ
//         static lv_style_t style;
//         lv_style_init(&style);
//         lv_style_set_text_color(&style, lv_palette_main(LV_PALETTE_RED)); // Đặt màu chữ thành đỏ

//         // Áp dụng style cho nội dung message box
//         lv_obj_add_style(lv_msgbox_get_text(msgbox), &style, 0);
//     }

//     http.end(); // Kết thúc yêu cầu HTTP
// }


void UnloadPage::onChangeProduct()
{
    Serial.println("onChangeProduct: Preparing to unload rolls");

    // Lấy giá trị từ adjustmentMetRun và rollCount
    const char *adjustmentMetRunText = lv_textarea_get_text(adjustmentMetRun);
    const char *rollCountText = lv_textarea_get_text(rollCount);

    // Chuyển đổi adjustmentMetRunText sang double (nếu có)
    double totalMeterRan = 0.0;
    bool hasTotalMeterRan = false;
    if (strlen(adjustmentMetRunText) > 0)
    {
        totalMeterRan = atof(adjustmentMetRunText); // Chuyển sang số thực
        hasTotalMeterRan = true;
    }

    // Chuyển đổi rollCountText sang int và kiểm tra hợp lệ
    if (strlen(rollCountText) == 0)
    {
        lv_obj_t *msgbox = lv_msgbox_create(NULL, "Error", "Roll count is required!", NULL, true);
        lv_obj_center(msgbox);
        return; // Thoát nếu không có roll count
    }

    int rollCountValue = atoi(rollCountText);
 
    // Tạo URL
    String url = config->config.httpIP + (config->config.httpPort != 0 ? ":" + String(config->config.httpPort) : "")  + "/api/rolls/unload/" + URLEncode(config->config.fxStationCode.c_str());

    // Tạo payload JSON
    String payload = "{";
    if (hasTotalMeterRan)
    {
        payload += "\"TotalMeterRan\":" + String(totalMeterRan, 2) + ",";
    }
    else
    {
        payload += "\"TotalMeterRan\":null,";
    }
    payload += "\"RollCount\":" + String(rollCountValue);
    payload += "}";

    Serial.println("URL: " + url);
    Serial.println("Payload: " + payload);

    // Gửi yêu cầu POST
    HTTPClient http;
    http.begin(url);
    http.addHeader("Content-Type", "application/json");

    int httpCode = http.POST(payload);

    if (httpCode == HTTP_CODE_OK || httpCode == HTTP_CODE_CREATED)
    {
        String response = http.getString();
        Serial.println("Server Response: " + response);

        lv_obj_t *msgbox = lv_msgbox_create(NULL, "Success", "Unload completed successfully!", NULL, true);
        lv_obj_center(msgbox);
    }
    else
    {
        String response = http.getString();
        Serial.println("Failed to unload rolls. Server Response: " + response);

        lv_obj_t *msgbox = lv_msgbox_create(NULL, "Error", "Failed to unload rolls!", NULL, true);
        lv_obj_center(msgbox);
    }

    http.end();
}
