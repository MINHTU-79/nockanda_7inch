#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"
#include "production.hpp"
#include "DowntimePage.hpp"
#include "DowntimeErrorPage.hpp"
#include "Data.hpp"
#include <esp_task_wdt.h>

// #include "mutex.h"

extern lv_obj_t *mainPage;
extern DowntimePage *downtimePage;
extern UnloadPage *unloadPage;
extern DowntimeErrorPage *downtimeErrorPage;

// extern SemaphoreHandle_t xMutexFetchProduction;

extern lv_obj_t *create_top_bar(lv_obj_t *parent, const char *title, lv_event_cb_t back_cb, void *back_cb_user_data, lv_event_cb_t refresh_cb, void *refresh_cb_user_data);

// Hàm kiểm tra xem spinner đang ẩn hay hiện
bool is_spinner_hidden(lv_obj_t *spinner) {
    return lv_obj_has_flag(spinner, LV_OBJ_FLAG_HIDDEN);
}

// Hàm cập nhật hiển thị spinner
void set_spinner_visibility(lv_obj_t *spinner, bool visible) {
    if (visible) {
      Serial.println("AAAAAAAAAAAShow spinner set clean");
        lv_obj_clear_flag(spinner, LV_OBJ_FLAG_HIDDEN); // Hiển thị spinner
    } else {
      Serial.println("AAAAAAAAAAAShow spinner set hindden");

        lv_obj_add_flag(spinner, LV_OBJ_FLAG_HIDDEN); // Ẩn spinner

        lv_obj_invalidate(spinner); // Yêu cầu cập nhật lại vùng hiển thị của spinner
    }
}

ProductionPage::ProductionPage(ConfigManager *configMgr)
    : config(configMgr)
{
  page = lv_obj_create(NULL);

  Serial.println("ProductionPage init");
  // LV_LOG_USER("Page created: %p", page);

  // Tạo thanh bar với tiêu đề "Production"
  create_top_bar(
      page,
      "Sản Xuất", // Tiêu đề
      [](lv_event_t *e)
      { lv_disp_load_scr(mainPage); }, // Callback nút Back
      nullptr,                         // Không cần user_data cho Back
      ProductionPage::refreshPage,     // Callback nút Refresh
      this);



  // Container chính bao phủ toàn bộ trang (thanh bar đã có từ constructor)
  content_container = lv_obj_create(page);
  lv_obj_set_size(content_container, LV_PCT(100), lv_obj_get_height(page) - 50);
  lv_obj_set_style_pad_all(content_container, 0, LV_PART_MAIN);
  lv_obj_clear_flag(content_container, LV_OBJ_FLAG_SCROLLABLE);
  lv_obj_clear_flag(content_container, LV_OBJ_FLAG_SCROLL_MOMENTUM); // Tắt cuộn quán tính cho trang chính
  lv_obj_align(content_container, LV_ALIGN_TOP_MID, 0, 50);          // Đặt container1 dưới thanh bar, bỏ 50px


}

// Hàm tạo giao diện
void ProductionPage::init()
{
  // Kiểm tra và xóa ObjSpinner nếu đã tồn tại
  // if (ObjSpinner != NULL && lv_obj_is_valid(ObjSpinner))
  // {
    
  // }

  
  // // Tạo một spinner (loader) để báo hiệu đang tải
  //   ObjSpinner = lv_spinner_create(page, 1000, 60);  // 1 giây cho một vòng quay
  //   lv_obj_set_size(ObjSpinner, 50, 50);             // Kích thước của spinner
  //   lv_obj_align(ObjSpinner, LV_ALIGN_CENTER, 0, 0); // Đặt spinner ở giữa màn hình
  //   // Ẩn spinner ban đầu
  //   lv_obj_add_flag(ObjSpinner, LV_OBJ_FLAG_HIDDEN);

  //   // Đưa spinner lên foreground nếu cần
  //   lv_obj_move_foreground(ObjSpinner);


  // lv_obj_clean(content_container); // Chỉf xóa nội dung, giữ lại top bar

  if (content_container != NULL && lv_obj_is_valid(content_container))
  {
    lv_obj_clean(content_container); // Chỉ xóa nội dung, giữ lại top bar
  }

  // Tạo container con sử dụng flex layout để sắp xếp các thành phần theo cột
  container = lv_obj_create(content_container);
  lv_obj_set_flex_flow(container, LV_FLEX_FLOW_COLUMN); // Sắp xếp theo cột
  lv_obj_set_flex_align(container, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
  lv_obj_set_size(container, LV_PCT(100), LV_PCT(100)); // Kích thước đầy đủ

  // Set container scrollable
  lv_obj_set_scrollbar_mode(container, LV_SCROLLBAR_MODE_ON);
  lv_obj_set_scroll_dir(container, LV_DIR_VER);              // Vertical scrolling
  lv_obj_clear_flag(container, LV_OBJ_FLAG_SCROLL_MOMENTUM); // Tắt cuộn quán tính cho trang chính
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

  // Khởi tạo các label cho thông tin Job bằng tiếng Việt
  // Nhan Vien
  labelNhanVien = lv_label_create(jobInfoContainer);
  lv_label_set_text(labelNhanVien, "Nhân viên: N/A");
  lv_obj_set_style_text_font(labelNhanVien, &arial, 0);
  lv_obj_set_style_text_color(labelNhanVien, lv_color_hex(0x333333), 0); // Màu chữ tối hơn
  lv_obj_set_width(labelNhanVien, LV_PCT(98));

  // Ma Hang
  labelMaHang = lv_label_create(jobInfoContainer);
  lv_label_set_text(labelMaHang, "Mã hàng: N/A");
  lv_obj_set_style_text_font(labelMaHang, &arial, 0);
  lv_obj_set_style_text_color(labelMaHang, lv_color_hex(0x333333), 0);
  lv_obj_set_width(labelMaHang, LV_PCT(98));

  // Da Chay
  labelDaChay = lv_label_create(jobInfoContainer);
  lv_label_set_text(labelDaChay, "Đã chạy: N/A");
  lv_obj_set_style_text_font(labelDaChay, &arial, 0);
  lv_obj_set_style_text_color(labelDaChay, lv_color_hex(0x333333), 0);
  lv_obj_set_width(labelDaChay, LV_PCT(98));

  // Counter
  labelCounter = lv_label_create(jobInfoContainer);
  lv_label_set_text(labelCounter, "Counter: N/A");
  lv_obj_set_style_text_font(labelCounter, &arial, 0);
  lv_obj_set_style_text_color(labelCounter, lv_color_hex(0x333333), 0);
  lv_obj_set_width(labelCounter, LV_PCT(98));

  //======================================================================
  // Các button navi
  //======================================================================

  // Tạo nút "Goi dung may"

  lv_obj_t *btnGoiDungMay = lv_btn_create(container);
  lv_obj_set_size(btnGoiDungMay, LV_PCT(90), 50);
  lv_obj_align(btnGoiDungMay, LV_ALIGN_TOP_MID, 0, 60);
  lv_obj_t *labelGoiDungMay = lv_label_create(btnGoiDungMay);
  lv_label_set_text(labelGoiDungMay, "Gọi dừng máy");
  lv_obj_set_style_text_font(labelGoiDungMay, &arial, 0);
  lv_obj_center(labelGoiDungMay);
  // lv_obj_add_event_cb(btnGoiDungMay, [](lv_event_t *e)
  //                     {
  //                       // goiDungMayPage->refreshPage();               // Làm lại trang trước khi hiện thị
  //                       // lv_disp_load_scr(goiDungMayPage->getPage()); // Hiện thị trang này
  //                       lv_disp_load_scr(downtimePage->getPage());
  //                     },
  //                     LV_EVENT_CLICKED, NULL);

  // Tạo nút "Production"

  lv_obj_t *btnProduction = lv_btn_create(container);
  lv_obj_set_size(btnProduction, LV_PCT(90), 50);
  lv_obj_align(btnProduction, LV_ALIGN_TOP_MID, 0, 120);
  lv_obj_t *labelProduction = lv_label_create(btnProduction);
  lv_label_set_text(labelProduction, "Lỗi gần nhất");
  lv_obj_set_style_text_font(labelProduction, &arial, 0);
  lv_obj_center(labelProduction);
  lv_obj_add_event_cb(btnProduction, [](lv_event_t *e)
                      {
                        // productionPage->refreshPage();               // Làm mới lại trang trước khi hiển thị
                        // lv_disp_load_scr(productionPage->getPage()); // Hiển thị trang mới
                         // Hiển thị trang DowntimePage
                        lv_disp_load_scr(downtimePage->getPage()); }, LV_EVENT_CLICKED, NULL);

  // Tạo nút "Cong nhan xuong cuon"

  lv_obj_t *btnCongNhanXuongCuon = lv_btn_create(container);
  lv_obj_set_size(btnCongNhanXuongCuon, LV_PCT(90), 50);
  lv_obj_align(btnCongNhanXuongCuon, LV_ALIGN_TOP_MID, 0, 180);
  lv_obj_t *labelCongNhanXuongCuon = lv_label_create(btnCongNhanXuongCuon);
  lv_label_set_text(labelCongNhanXuongCuon, "Công nhân xuống cuộn");
  lv_obj_set_style_text_font(labelCongNhanXuongCuon, &arial, 0);
  lv_obj_center(labelCongNhanXuongCuon);
  lv_obj_add_event_cb(btnCongNhanXuongCuon, [](lv_event_t *e)
                      { lv_disp_load_scr(unloadPage->getPage()); }, LV_EVENT_CLICKED, NULL);

  // Tạo nút "CheckList (Con 29 phut)"

  lv_obj_t *btnCheckList = lv_btn_create(container);
  lv_obj_set_size(btnCheckList, LV_PCT(90), 50);
  lv_obj_align(btnCheckList, LV_ALIGN_TOP_MID, 0, 240);
  lv_obj_t *labelCheckList = lv_label_create(btnCheckList);
  lv_label_set_text(labelCheckList, "CheckList");
  lv_obj_center(labelCheckList);
  // lv_obj_add_event_cb(btnCheckList, [](lv_event_t *e)
  //                     {
  //                       lv_disp_load_scr(checkListPage->getPage());
  //                     },
  //                     LV_EVENT_CLICKED, NULL);

  // // Tạo nút Change Product
  // btnChangeProduct = lv_btn_create(container);
  // lv_obj_set_size(btnChangeProduct, 150, 50);
  // lv_obj_add_event_cb(btnChangeProduct, eventHandler, LV_EVENT_CLICKED, this);

  // lv_obj_t *btnChangeProductLabel = lv_label_create(btnChangeProduct);
  // lv_label_set_text(btnChangeProductLabel, "Change Product");
  // lv_obj_center(btnChangeProductLabel);

  // ---------------------------------------------------------------------------------
  // Tạo Keyboard
  // ---------------------------------------------------------------------------------
  keyboard = lv_keyboard_create(content_container);
  lv_obj_set_size(keyboard, LV_PCT(100), LV_PCT(50));
  lv_obj_align(keyboard, LV_ALIGN_BOTTOM_MID, 0, 0);
  lv_keyboard_set_mode(keyboard, LV_KEYBOARD_MODE_TEXT_UPPER);
  lv_obj_add_flag(keyboard, LV_OBJ_FLAG_HIDDEN | LV_OBJ_FLAG_FLOATING); // Ẩn ban đầu và làm floating

  // không cần . vì h trang này không còn nhập keyboard
  // Thêm padding dưới cùng để tránh bị che bởi bàn phím
  // lv_obj_set_style_pad_bottom(container, 100, 0); // 100 pixels hoặc chiều cao của bàn phím

  // ---------------------------------------------------------------------------------
  // Fetch data từ APIs
  // ---------------------------------------------------------------------------------
  fetchProducts();
}

void ProductionPage::updateJobInfo(const JsonObject &job)
{
  String nhanVien = job["Employee"].as<String>();
  String maHang = job["ProductCode"].as<String>();
  String daChay = job["Progress"].as<String>();
  String counter = job["Counter"].as<String>();

  // Cập nhật các label với thông tin bằng tiếng Việt
  lv_label_set_text(labelNhanVien, ("Nhân viên: " + nhanVien).c_str());
  lv_label_set_text(labelMaHang, ("Mã hàng: " + maHang).c_str());
  lv_label_set_text(labelDaChay, ("Đã chạy: " + daChay).c_str());
  lv_label_set_text(labelCounter, ("Counter: " + counter).c_str());
}

void ProductionPage::filterDropdown(lv_obj_t *dropdown, const std::vector<Product> &dataList, const char *filter)
{
  lv_dropdown_clear_options(dropdown);
  filteredProductIndices.clear(); // Clear the previous filtered indices

  String filterStr = String(filter);
  filterStr.toLowerCase();

  for (size_t i = 0; i < dataList.size(); ++i)
  {
    const auto &item = dataList[i];
    String name = item.ProductName;
    String lowerName = name;
    lowerName.toLowerCase();
    if (lowerName.indexOf(filterStr) != -1)
    {
      lv_dropdown_add_option(dropdown, name.c_str(), LV_DROPDOWN_POS_LAST);
      filteredProductIndices.push_back(i); // Store the original index
    }
  }
}

lv_obj_t *ProductionPage::getPage()
{
  return page;
}


// ver này ghi ra RunningJob tạm trước .

void ProductionPage::fetchProducts()
{

  Serial.println("sssssssssssssssssssssssStart fetchProducts");

  // // Lấy mutex trước khi thực hiện đoạn code
  // if (xSemaphoreTake(xMutexFetchProduction, pdMS_TO_TICKS(3000)) == pdTRUE)
  // {
  // Serial.println("Get Mutex");
  const String stationCode = config->config.fxStationCode;

  HTTPClient http;

  http.setTimeout(3000); // timeout trong 5000ms (5 giây)

  String url = config->config.httpIP + (config->config.httpPort != 0 ? ":" + String(config->config.httpPort) : "") +
               config->config.pathGetRunningProductOnStation + URLEncode(stationCode.c_str());

  Serial.println("Fetching Running Job from: " + url); // Debugging URL

  // Reset watchdog (nếu cần)
  esp_task_wdt_reset();

  try
  {
    http.begin(url);
    int httpCode = http.GET();

    if (httpCode == HTTP_CODE_OK)
    {
      String payload = http.getString();
      // Serial.println("Running Job Payload: " + payload); // Debugging payload

      // Parse JSON
      DynamicJsonDocument doc(8192); // Tăng kích thước nếu payload lớn
      DeserializationError error = deserializeJson(doc, payload);

      if (error)
      {
        Serial.print(F("deserializeJson() failed: "));
        Serial.println(error.f_str());
        return;
      }

      // Tạo RunningJob tạm thời
      RunningJob tempRunningJob;

      // Extract main job data
      JsonObject job = doc.as<JsonObject>();

      Serial.println("Job JSON:");
      serializeJson(job, Serial);
      Serial.println();

      tempRunningJob.stationCode = job["stationCode"] | "N/A";
      tempRunningJob.employee = job["employee"] | "N/A";
      tempRunningJob.productCode = job["productCode"] | "N/A";
      tempRunningJob.meterRan = job["meterRan"] | 0.0f;
      tempRunningJob.totalMeter = job["totalMeter"] | 0.0f;
      tempRunningJob.spinCount = job["spinCount"] | 0;
      tempRunningJob.cavity = job["cavity"] | 0;
      // Extract and process downtime details
      JsonArray downtimes = job["downtimeInputs"].as<JsonArray>();

      if (!downtimes.isNull() && downtimes.size() > 0)
      {
        for (JsonObject downtime : downtimes)
        {
          DowntimeItem item;
          item.atMeter = downtime["atMeter"] | 0.0f;
          item.objectId = downtime["objectId"] | "N/A";
          item.id = downtime["id"] | "N/A";
          item.jobCode = downtime["jobCode"] | "N/A";
          item.stationCode = downtime["stationCode"] | "N/A";
          item.updateTime = downtime["updateTime"] | 0;
          item.startTime = downtime["startTime"] | 0;
          item.endTime = downtime["endTime"] | 0;
          item.downtimeCode = downtime["downtimeCode"] | "N/A";
          item.employeeCode = downtime["employeeCode"] | "N/A";
          item.description = downtime["description"] | "N/A";
          item.actualStopTime = downtime["actualStopTime"] | -1;
          item.approvalEmployeeCode = downtime["approvalEmployeeCode"] | 0;
          item.status = downtime["status"] | 1;
          item.temporarySolution = downtime["temporarySolution"] | "N/A";
          item.lossTime = downtime["lossTime"] | 0;
          item.rollCode = downtime["rollCode"] | "N/A";
          item.scheduleDowntime = downtime["scheduleDowntime"] | 0;
          item.unScheduleDowntime = downtime["unScheduleDowntime"] | 0;

          tempRunningJob.downtimeInputs.push_back(item);
        }
      }
      else
      {
        // No downtimes, create a default item
        DowntimeItem defaultItem;
        defaultItem.downtimeCode = "N/A";
        defaultItem.description = "No downtime recorded";
        defaultItem.lossTime = 0;
        tempRunningJob.downtimeInputs.push_back(defaultItem);
      }

      // Giải phóng tài nguyên cũ trước khi gán
      roll.clear();

      // Nếu tất cả dữ liệu hợp lệ, cập nhật vào roll
      roll = tempRunningJob;


      // Update UI with the new roll
      lv_label_set_text(labelNhanVien, ("Nhân viên: " + roll.employee).c_str());
      lv_label_set_text(labelMaHang, ("Mã hàng: " + roll.productCode).c_str());

      String meterText = String(roll.meterRan, 2) + " / " + String(roll.totalMeter, 2);
      lv_label_set_text(labelDaChay, ("Đã chạy: " + meterText).c_str());

      lv_label_set_text(labelCounter, ("Counter: " + String(roll.spinCount)).c_str());

      // Reset watchdog (nếu cần)
      esp_task_wdt_reset();

      // Update DowntimePage UI
      downtimePage->setDowntimeDetails(roll.downtimeInputs);

      // Reset watchdog (nếu cần)
      esp_task_wdt_reset();

      // Update unloadPage UI
      unloadPage->fetchPage();

      // Reset watchdog (nếu cần)
      esp_task_wdt_reset();

      // Update
      downtimeErrorPage->fetchDowntimeError();
    }
    else if (httpCode == 404)
    {
      Serial.println("Không có công việc nào đang chạy (404).");
      lv_label_set_text(labelNhanVien, "Nhân viên: N/A");
      lv_label_set_text(labelMaHang, "Mã hàng: N/A");
      lv_label_set_text(labelDaChay, "Đã chạy: N/A");
      lv_label_set_text(labelCounter, "Counter: N/A");
    }
    else if (httpCode == 500)
    {
      Serial.println("Lỗi server (500).");
      lv_obj_t *msgbox = lv_msgbox_create(NULL, "Server Error", "Lỗi từ server", NULL, true);
      lv_obj_set_style_text_font(msgbox, &arial, 0);
      lv_obj_center(msgbox);
    }
    else
    {
      Serial.printf("Error in HTTP request: %d\n", httpCode);
    }
  }
  catch (...)
  {
    Serial.println("Exception caught during fetchRunningJob.");
    lv_obj_t *msgbox = lv_msgbox_create(NULL, "Error", "Đã xảy ra lỗi khi kết nối server.", NULL, true);
    lv_obj_set_style_text_font(msgbox, &arial, 0);
    lv_obj_center(msgbox);
  }

  http.end();

  //   xSemaphoreGive(xMutexFetchProduction);
  //   Serial.println("Give Mutex");
  // }
  // else
  // {
  //   // Code xử lý khi không lấy được semaphore trong 1 giây
  //   Serial.println("Semaphore not found within 1 second.");
  // }

  Serial.println("Eeeeeeeeeeeeeeeeeeend fetchProducts");

  // Serial.println("ObjSpinner is hidden: " + String(is_spinner_hidden(ObjSpinner)));
  // if (!is_spinner_hidden(ObjSpinner))
  // {
  //   Serial.println("Hide spinner");
  //   set_spinner_visibility(ObjSpinner, false); // Ẩn spinner nếu nó đang hiện
  // }
}

// Callback cho các trường nhập liệu
void ProductionPage::ta_event_cb(lv_event_t *e)
{
  ProductionPage *page = (ProductionPage *)lv_event_get_user_data(e);
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

void ProductionPage::eventHandler(lv_event_t *e)
{
  ProductionPage *page = (ProductionPage *)lv_event_get_user_data(e);
  if (e->code == LV_EVENT_CLICKED)
  {
    if (e->target == page->btnChangeProduct)
    {
      page->onChangeProduct();
    }
  }
}

// Hàm này sẽ được gọi khi nhấn nút "Close" để đóng msgbox

void ProductionPage::onChangeProduct()
{
  Serial.printf("onChangeProduct: selectedStationIdx = %d, selectedProductIdx = %d\n", lv_dropdown_get_selected(ddStation), lv_dropdown_get_selected(ddProduct));
  int selectedStationIdx = lv_dropdown_get_selected(ddStation);
  int selectedProductIdx = lv_dropdown_get_selected(ddProduct);

  int stationIdx, productIdx;

  // Kiểm tra nếu có bộ lọc hoặc không có
  if (filteredStationIndices.empty())
  {
    // Nếu không có lọc, sử dụng danh sách gốc
    stationIdx = selectedStationIdx;
  }
  else
  {
    // Nếu có lọc, lấy chỉ số gốc từ danh sách đã lọc
    if (selectedStationIdx < 0 || selectedStationIdx >= filteredStationIndices.size())
    {
      Serial.println("Invalid station selection");
      return;
    }
    stationIdx = filteredStationIndices[selectedStationIdx];
  }

  if (filteredProductIndices.empty())
  {
    // Nếu không có lọc, sử dụng danh sách gốc
    productIdx = selectedProductIdx;
  }
  else
  {
    // Nếu có lọc, lấy chỉ số gốc từ danh sách đã lọc
    if (selectedProductIdx < 0 || selectedProductIdx >= filteredProductIndices.size())
    {
      Serial.println("Invalid product selection");
      return;
    }
    productIdx = filteredProductIndices[selectedProductIdx];
  }

  // Kiểm tra chỉ số hợp lệ trong danh sách gốc
  if (stationIdx < 0 || stationIdx >= stations.size() || productIdx < 0 || productIdx >= products.size())
  {
    Serial.println("Invalid original selection");
    return;
  }

  // Lấy mã Station và Product từ danh sách gốc
  String stationCode = stations[stationIdx].StationCode;
  String productCode = products[productIdx].ProductCode;

  // Serial.println("onChangeProduct: ");
  // Serial.println("  Station index: " + String(stationIdx));
  // Serial.println("  Product index: " + String(productIdx));
  // Serial.println("  Station code: " + stationCode);
  // Serial.println("  Product code: " + productCode);
  // // Xây dựng URL và gửi yêu cầu POST
  // String url = config->config.httpIP + (config->config.httpPort != 0 ? ":" + String(config->config.httpPort) : "")     +
  //              config->config.pathPostChangedProduct +
  //              "?station=" + stationCode + "&product=" + productCode;
  // Serial.println("URL: " + url);

  // Xây dựng URL và gửi yêu cầu POST
  String stationCodeEncoded = URLEncode(stationCode.c_str());
  String productCodeEncoded = URLEncode(productCode.c_str());
  String url = config->config.httpIP + (config->config.httpPort != 0 ? ":" + String(config->config.httpPort) : "") +
               config->config.pathPostChangedProduct +
               "?station=" + stationCodeEncoded + "&product=" + productCodeEncoded;
  Serial.println("URL: " + url);

  HTTPClient http;
  http.begin(url);
  http.addHeader("Content-Type", "application/json");
  int httpCode = http.POST(""); // Assuming no body is needed

  // if (httpCode == HTTP_CODE_OK || httpCode == HTTP_CODE_CREATED)
  // {
  //   Serial.println("Product changed successfully");
  // }
  // else
  // {
  //   Serial.println("Failed to change product");
  // }
  // http.end();

  // Kiểm tra mã trạng thái phản hồi

  String payload = ""; // Khai báo biến để lưu trữ nội dung trả về

  // Kiểm tra mã phản hồi từ server
  if (httpCode == HTTP_CODE_OK || httpCode == HTTP_CODE_CREATED)
  {
    // Lấy nội dung trả về từ server
    payload = http.getString();
    Serial.println("Server Response: " + payload); // In ra Serial để theo dõi

    // Tạo message box để hiển thị nội dung phản hồi
    lv_obj_t *msgbox = lv_msgbox_create(NULL, "Thành công", payload.c_str(), NULL, true); // Tạo msgbox
    lv_obj_set_style_text_font(msgbox, &arial, 0);
    lv_obj_center(msgbox); // Căn giữa msgbox

    Serial.println("Product changed successfully");
  }
  else
  {
    // Xử lý trường hợp lỗi
    payload = http.getString(); // Lấy thông tin chi tiết lỗi từ server (nếu có)
    Serial.println("Failed to change product. Server Response: " + payload);

    // // Tạo message box để hiển thị thông báo lỗi
    // lv_obj_t *msgbox = lv_msgbox_create(NULL, "Error", "Failed to change product", NULL, true); // Tạo msgbox
    // lv_obj_center(msgbox);                                                                      // Căn giữa msgbox

    // Tạo message box để hiển thị thông báo lỗi
    lv_obj_t *msgbox = lv_msgbox_create(NULL, "Lỗi", "Không thể thay đổi sản phẩm", NULL, true); // Tạo msgbox
    lv_obj_set_style_text_font(msgbox, &arial, 0);
    lv_obj_center(msgbox); // Căn giữa msgbox

    // Tạo style mới để đổi màu chữ
    static lv_style_t style;
    lv_style_init(&style);
    lv_style_set_text_color(&style, lv_palette_main(LV_PALETTE_RED)); // Đặt màu chữ thành đỏ

    // Áp dụng style cho nội dung message box
    lv_obj_add_style(lv_msgbox_get_text(msgbox), &style, 0);
  }

  http.end(); // Kết thúc yêu cầu HTTP
}

// void ProductionPage::refreshPage(lv_event_t *e)
// {
//   ProductionPage *page = (ProductionPage *)lv_event_get_user_data(e); // Lấy dữ liệu user_data là con trỏ tới lớp ProductionPage



//   if (is_spinner_hidden(page->ObjSpinner))
//   {
//     set_spinner_visibility(page->ObjSpinner, true); // Hiển thị spinner nếu nó đang ẩn
//     Serial.println("AAAAAAAAAAAAAAShow spinner");
//   }

//   page->init();
 
//   // // Tạo một timer để chờ trong khi spinner hoạt động
//   // lv_timer_t *timer = lv_timer_create([](lv_timer_t *t)
//   //                                     {
//   //                                       // Lấy user_data từ timer
//   //                                       ProductionPage *page = (ProductionPage *)t->user_data; // Sử dụng t->user_data để lấy dữ liệu user_data
//   //                                       // lv_obj_clean(page->page);                              // Xoá sạch tất cả các đối tượng trên trang
//   //                                       page->init();    // Tạo lại giao diện
//   //                                       lv_timer_del(t); // Xoá timer khi xong
//   //                                     },
//   //                                     1000, page); // Tạo timer với thời gian tương ứng với thời gian của spinner

//   // // Gán trực tiếp user_data
//   // timer->user_data = page; // Gán user_data trực tiếp thông qua cấu trúc lv_timer_t
// }

// ver này là hiện text thông bào khi refetch.
// void ProductionPage::refreshPage(lv_event_t *e)
// {
//   ProductionPage *page = (ProductionPage *)lv_event_get_user_data(e); // Lấy dữ liệu user_data là con trỏ tới lớp ProductionPage

//   // Tạo một timer để làm mới trang
//   lv_timer_t *timer = lv_timer_create([](lv_timer_t *t)
//                                       {
//                                         // Lấy user_data từ timer
//                                         ProductionPage *page = (ProductionPage *)t->user_data;

//                                         // Làm mới giao diện
//                                         page->init();

//                                         // Hiển thị thông báo "Refetched successfully"
//                                         lv_obj_t *label = lv_label_create(page->page);
//                                         lv_label_set_text(label, "Refetched successfully");
//                                         lv_obj_align(label, LV_ALIGN_CENTER, 0, 0); // Đặt thông báo ở giữa màn hình

//                                         // Tạo một timer khác để xóa thông báo sau 1 giây
//                                         lv_timer_t *remove_timer = lv_timer_create([](lv_timer_t *t)
//                                                                                    {
//                                                                                      lv_obj_t *label = (lv_obj_t *)t->user_data;
//                                                                                      lv_obj_del(label); // Xóa thông báo
//                                                                                      lv_timer_del(t);   // Xóa timer
//                                                                                    },
//                                                                                    1000, label);

//                                         lv_timer_del(t); // Xóa timer ban đầu
//                                       },
//                                       0, page); // Gán user_data cho timer
// }


// ver này là hiện text thông bào khi refetch.
void ProductionPage::refreshPage(lv_event_t *e)
{
  ProductionPage *page = (ProductionPage *)lv_event_get_user_data(e); // Lấy dữ liệu user_data là con trỏ tới lớp ProductionPage

  // Tạo một timer để làm mới trang
  lv_timer_t *timer = lv_timer_create([](lv_timer_t *t)
                                      {
                                        // Lấy user_data từ timer
                                        ProductionPage *page = (ProductionPage *)t->user_data;

                                        // Làm mới giao diện
                                        page->init();

                                        // Hiển thị thông báo "Refetched successfully"
                                        lv_obj_t *label = lv_label_create(page->page);
                                        lv_label_set_text(label, "Tải lại thành công");
                                        lv_obj_set_style_text_font(label, &arial, 0);

                                        // Set the label's background color to blue and text color to white
                                        lv_obj_set_style_bg_color(label, lv_color_hex(0xFFFFFF), 0);   // Blue background
                                        lv_obj_set_style_text_color(label, lv_color_hex(0x000000), 0); // White text
                                        lv_obj_set_style_bg_opa(label, LV_OPA_COVER, 0);               // Make background opaque

                                        // Set border width, color, and round the corners
                                        lv_obj_set_style_border_width(label, 2, 0);                      // Set border width
                                        lv_obj_set_style_border_color(label, lv_color_hex(0x000000), 0); // White border

                                        // Use lv_obj_set_style_radius to set rounded corners
                                        lv_obj_set_style_radius(label, 9, 0); // Rounded corners with radius 10

                                        // Add padding around the text
                                        lv_obj_set_style_pad_all(label, 10, 0); // Padding of 10px on all sides

                                        lv_obj_align(label, LV_ALIGN_CENTER, 0, 0); // Đặt thông báo ở giữa màn hình

                                        

                                        // Tạo một timer khác để xóa thông báo sau 1 giây
                                        lv_timer_t *remove_timer = lv_timer_create([](lv_timer_t *t)
                                                                                   {
                                                                                     lv_obj_t *label = (lv_obj_t *)t->user_data;
                                                                                     lv_obj_del(label); // Xóa thông báo
                                                                                     lv_timer_del(t);   // Xóa timer
                                                                                   },
                                                                                   2000, label);

                                        lv_timer_del(t); // Xóa timer ban đầu
                                      },
                                      0, page); // Gán user_data cho timer
}
