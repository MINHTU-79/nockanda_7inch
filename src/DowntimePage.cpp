#include "DowntimePage.hpp"
#include "DowntimeErrorPage.hpp"
#include "production.hpp"
#include "Data.hpp"

extern ProductionPage *productionPage;
extern bool inited;
extern DowntimeErrorPage *downtimeErrorPage;
extern lv_obj_t *create_top_bar(lv_obj_t *parent, const char *title, lv_event_cb_t back_cb, void *back_cb_user_data, lv_event_cb_t refresh_cb, void *refresh_cb_user_data);

DowntimePage::DowntimePage(ConfigManager *configMgr)
    : config(configMgr)
{
    // Khởi tạo trang
    page = lv_obj_create(NULL);

    // Tạo thanh bar với tiêu đề "Configuration"
    create_top_bar(
        page,
        "Chi tiết dừng máy", // Tiêu đề
        [](lv_event_t *e)
        { lv_disp_load_scr(productionPage->getPage()); }, // Callback nút Back
        nullptr,                                          // Không cần user_data cho Back
        nullptr,                                          // Callback nút Refresh
        nullptr);

    // lv_obj_set_size(page, LV_HOR_RES, LV_VER_RES);
    // container = lv_obj_create(page);
    // lv_obj_set_size(container, LV_PCT(100), LV_PCT(100));
    // lv_obj_set_layout(container, LV_LAYOUT_FLEX);
    // lv_obj_set_flex_flow(container, LV_FLEX_FLOW_COLUMN);
    // lv_obj_set_scroll_snap_y(container, LV_SCROLL_SNAP_CENTER);


}

lv_obj_t *DowntimePage::getPage()
{
    return page;
}

void DowntimePage::init()
{
    std::vector<DowntimeItem> downtimeDetails{};
    Serial.println("DowntimePage::init");

    // lv_obj_t *container1 = lv_obj_create(page);            // Or use an appropriate parent
    // lv_obj_set_style_pad_all(container1, 0, LV_PART_MAIN); //
    // lv_obj_set_size(container1, LV_PCT(100), LV_PCT(100));

    content_container = lv_obj_create(page);
    lv_obj_set_size(content_container, LV_PCT(100), lv_obj_get_height(page) - 50);
    lv_obj_set_style_pad_all(content_container, 0, LV_PART_MAIN);
    lv_obj_clear_flag(content_container, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_clear_flag(content_container, LV_OBJ_FLAG_SCROLL_MOMENTUM); // Tắt cuộn quán tính cho trang chính
    lv_obj_align(content_container, LV_ALIGN_TOP_MID, 0, 50);          // Đặt container1 dưới thanh bar, bỏ 50px

    //    Tạo container con sử dụng flex layout để sắp xếp các thành phần theo cột
    container = lv_obj_create(content_container);
    lv_obj_set_flex_flow(container, LV_FLEX_FLOW_COLUMN); // Sắp xếp theo cột
    lv_obj_set_flex_align(container, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
    lv_obj_set_size(container, LV_PCT(100), LV_PCT(100)); // Kích thước đầy đủ

    // Set container scrollable
    lv_obj_set_scrollbar_mode(container, LV_SCROLLBAR_MODE_ON);
    lv_obj_set_scroll_dir(container, LV_DIR_VER);              // Vertical scrolling
    lv_obj_clear_flag(container, LV_OBJ_FLAG_SCROLL_MOMENTUM); // Tắt cuộn quán tính cho trang chính

}

void DowntimePage::setDowntimeDetails(const std::vector<DowntimeItem> &details)
{
    Serial.println("Set downtime details:");
    // for (const auto &item : details)
    // {
    //     Serial.printf("  Code: %s, Pos: %s, Time: %s, Duration: %s\n", item.code.c_str(), item.position.c_str(), item.time.c_str(), item.duration.c_str());
    // }

    // downtimeDetails = details; // Sao chép giá trị

   
    if (!downtimeDetails.empty())
    {
        downtimeDetails.clear();
    }

    for (const auto &item : details)
    {
        // Serial.println("itemdetails push_back downtimeDetails");
        downtimeDetails.push_back(item);
    }

    if(!inited)
    delay(100);

    Serial.println("downtimeDetails = details");

    updateUI();
}

// void DowntimePage::eventHandler(lv_event_t *e)
// {
//     DowntimePage *page = (DowntimePage *)lv_event_get_user_data(e);
//     if (e->code == LV_EVENT_CLICKED)
//     {
//         if (e->target == page->btnSave)
//         {
//             page->onSave();
//         }
//     }
// }

void DowntimePage::updateUI()
{
      //==============
    // Xóa các mục cũ trên container
    if (container != NULL && lv_obj_is_valid(container) )
    {
        // Xóa các mục cũ
        lv_obj_clean(container);
    }

    // lv_obj_invalidate(container);

    // Khởi tạo các label cho thông tin Job bằng tiếng Việt
    // Nhan Vien
    labelMet = lv_label_create(container);
    lv_label_set_text(labelMet, "Số mét N/A");
    lv_obj_set_style_text_font(labelMet, &arial, 0);  // Font chữ lớn hơn
    lv_obj_set_style_text_color(labelMet, lv_color_hex(0x333333), 0); // Màu chữ tối hơn
    lv_obj_set_width(labelMet, LV_PCT(98));

    for (const auto &item : roll.downtimeInputs)
    {
        lv_obj_t *itemContainer = lv_obj_create(container);
        lv_obj_set_size(itemContainer, LV_PCT(100), LV_SIZE_CONTENT);
        lv_obj_set_layout(itemContainer, LV_LAYOUT_FLEX);
        lv_obj_set_flex_flow(itemContainer, LV_FLEX_FLOW_COLUMN); // Bố trí cột
        lv_obj_set_style_pad_all(itemContainer, 5, 0);            // Thêm khoảng cách

        // Chuyển downtimeCode về chữ thường
        std::string downtimeCodeLower = item.downtimeCode.c_str();
        std::transform(downtimeCodeLower.begin(), downtimeCodeLower.end(), downtimeCodeLower.begin(), ::tolower);

        if (downtimeCodeLower == "default")
        {
            // Đặt màu nền đỏ
            lv_obj_set_style_bg_color(itemContainer, lv_color_hex(0xFF7D7D), 0); // Màu đỏ
        }
        else
        {
            // Đặt màu nền mặc định (nếu cần)
            lv_obj_set_style_bg_color(itemContainer, lv_color_hex(0xFFFFFF), 0); // Màu trắng (hoặc thay bằng màu khác)
        }

        // bản có log

        // Lưu trữ thông tin `item.code` và chuỗi `b` vào đối tượng
        lv_obj_set_user_data(itemContainer, (void *)&item); // Lưu trữ con trỏ tới item

        // lv_label_set_text(labelMet, ("So may: " + roll.meterRan).c_str());

        lv_label_set_text(labelMet, ("Số mét: " + String(roll.meterRan)).c_str());
        lv_obj_set_style_text_font(labelMet, &arial, 0);
        // Gắn sự kiện click
        lv_obj_add_event_cb(itemContainer, [](lv_event_t *e)
                            {
    lv_obj_t *target = lv_event_get_target(e); // Lấy đối tượng bị click

    // Lấy dữ liệu đã lưu trữ trong đối tượng
    const auto *itemData = static_cast<const DowntimeItem *>(lv_obj_get_user_data(target));

            if (itemData)
            {
                // Debug: In ra mã lỗi và thông tin item
                Serial.print("Item clicked: ");

                // Gọi hàm downtimeErrorPage->getPage
                Serial.println("Calling downtimeErrorPage->getPage...");
                lv_disp_load_scr(downtimeErrorPage->getPage(*itemData));
    } else {
        Serial.println("Error: Item data is null");
    } }, LV_EVENT_CLICKED, NULL);

        // Container con cho dòng đầu tiên
        lv_obj_t *row1 = lv_obj_create(itemContainer);
        lv_obj_set_size(row1, LV_PCT(100), LV_SIZE_CONTENT);
        lv_obj_set_layout(row1, LV_LAYOUT_FLEX);
        lv_obj_set_flex_flow(row1, LV_FLEX_FLOW_ROW);
        lv_obj_set_style_pad_all(row1, 2, 0);            // Thêm padding
        lv_obj_clear_flag(row1, LV_OBJ_FLAG_SCROLLABLE); // Loại bỏ cuộn
        lv_obj_set_style_bg_opa(row1, LV_OPA_TRANSP, 0); // Làm trong suốt nền
        lv_obj_set_style_border_width(row1, 0, 0);       // Loại bỏ viền

        // Hiển thị mã lỗi
        lv_obj_t *labelCode = lv_label_create(row1);
        lv_label_set_text_fmt(labelCode, "Mã dừng: %s", item.downtimeCode.c_str());
        lv_obj_set_style_text_font(labelCode, &arial, 0);
        lv_obj_set_style_text_align(labelCode, LV_TEXT_ALIGN_LEFT, 0); // Căn trái
        lv_obj_set_flex_grow(labelCode, 1);                            // Chiếm không gian bên trái

        // Hiển thị vị trí
        lv_obj_t *labelPosition = lv_label_create(row1);
        lv_label_set_text_fmt(labelPosition, "Vị trí: %d m", item.atMeter);
        lv_obj_set_style_text_font(labelPosition, &arial, 0);
        lv_obj_set_style_text_align(labelPosition, LV_TEXT_ALIGN_RIGHT, 0); // Căn phải

        // Container con cho dòng thứ hai
        lv_obj_t *row2 = lv_obj_create(itemContainer);
        lv_obj_set_size(row2, LV_PCT(100), LV_SIZE_CONTENT);
        lv_obj_set_layout(row2, LV_LAYOUT_FLEX);
        lv_obj_set_flex_flow(row2, LV_FLEX_FLOW_ROW);
        lv_obj_set_style_pad_all(row2, 2, 0);            // Thêm padding
        lv_obj_clear_flag(row2, LV_OBJ_FLAG_SCROLLABLE); // Loại bỏ cuộn
        lv_obj_set_style_bg_opa(row2, LV_OPA_TRANSP, 0); // Làm trong suốt nền
        lv_obj_set_style_border_width(row2, 0, 0);       // Loại bỏ viền

        if (row2 == NULL)
        {
            Serial.println("Error: row2 is NULL!");
        }

        // Hiển thị thời gian
        lv_obj_t *labelTime = lv_label_create(row2);
        // Chuyển đổi thời gian từ epoch sang định dạng thời gian
        String formattedTimestartTime = convertEpochToTimeFormat(item.startTime);

        // Hiển thị thời gian đã định dạng trên label
        lv_label_set_text_fmt(labelTime, "Lúc: %s", formattedTimestartTime.c_str());
        lv_obj_set_style_text_font(labelTime, &arial, 0);
        lv_obj_set_style_text_align(labelTime, LV_TEXT_ALIGN_LEFT, 0); // Căn trái
        lv_obj_set_flex_grow(labelTime, 1);                            // Chiếm không gian bên trái

        // Hiển thị thời lượng
        lv_obj_t *labelDuration = lv_label_create(row2);
        // lv_label_set_text_fmt(labelDuration, "Dung: %s phut", item.lossTime);

        // Chuyển đổi số giây thành định dạng thời gian
        String formattedTimelossTime = convertSecondsToTimeFormat(item.lossTime);

        // Hiển thị thời gian đã định dạng trên label
        lv_label_set_text_fmt(labelDuration, "Dừng: %s", formattedTimelossTime.c_str());
        lv_obj_set_style_text_font(labelDuration, &arial, 0);
        lv_obj_set_style_text_align(labelDuration, LV_TEXT_ALIGN_RIGHT, 0); // Căn phải

        lv_obj_clear_flag(row1, LV_OBJ_FLAG_CLICKABLE); // Xóa khả năng click của row1
        lv_obj_clear_flag(row2, LV_OBJ_FLAG_CLICKABLE); // Xóa khả năng click của row2

        lv_obj_clear_flag(labelCode, LV_OBJ_FLAG_CLICKABLE);     // Xóa khả năng click của labelCode
        lv_obj_clear_flag(labelPosition, LV_OBJ_FLAG_CLICKABLE); // Xóa khả năng click của labelPosition
        lv_obj_clear_flag(labelTime, LV_OBJ_FLAG_CLICKABLE);     // Xóa khả năng click của labelTime
        lv_obj_clear_flag(labelDuration, LV_OBJ_FLAG_CLICKABLE); // Xóa khả năng click của labelDuration
    }
}
