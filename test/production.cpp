#include "UploadPage.hpp"
#include "production.hpp"


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
      "Xuong Cuon", // Tiêu đề
      [](lv_event_t *e)
      { lv_disp_load_scr(productionPage->getPage()); }, // Callback nút Back
      nullptr,                         // Không cần user_data cho Back
      nullptr,     // Callback nút Refresh
      this);
}

// // Hàm mã hóa URL
// String URLEncode(const char *msg)
// {
//   const char *hex = "0123456789ABCDEF";
//   String encodedMsg = "";
//   while (*msg != '\0')
//   {
//     if (isalnum(*msg) || *msg == '-' || *msg == '_' || *msg == '.' || *msg == '~')
//     {
//       encodedMsg += *msg;
//     }
//     else
//     {
//       encodedMsg += '%';
//       encodedMsg += hex[*msg >> 4];
//       encodedMsg += hex[*msg & 15];
//     }
//     msg++;
//   }
//   return encodedMsg;
// }

// Hàm tạo giao diện
void UnloadPage::init()
{

  // Container chính bao phủ toàn bộ trang (thanh bar đã có từ constructor)
  lv_obj_t *content_container = lv_obj_create(page);
  lv_obj_set_size(content_container, LV_PCT(100), lv_obj_get_height(page) - 50);
  lv_obj_set_style_pad_all(content_container, 0, LV_PART_MAIN);
  lv_obj_clear_flag(content_container, LV_OBJ_FLAG_SCROLLABLE);
  lv_obj_clear_flag(content_container, LV_OBJ_FLAG_SCROLL_MOMENTUM);  // Tắt cuộn quán tính cho trang chính
  lv_obj_align(content_container, LV_ALIGN_TOP_MID, 0, 50); // Đặt container1 dưới thanh bar, bỏ 50px

  // Tạo container con sử dụng flex layout để sắp xếp các thành phần theo cột
  lv_obj_t *container = lv_obj_create(content_container);
  lv_obj_set_flex_flow(container, LV_FLEX_FLOW_COLUMN); // Sắp xếp theo cột
  lv_obj_set_flex_align(container, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
  lv_obj_set_size(container, LV_PCT(100), LV_PCT(100)); // Kích thước đầy đủ

  // Set container scrollable
  lv_obj_set_scrollbar_mode(container, LV_SCROLLBAR_MODE_ON);
  lv_obj_set_scroll_dir(container, LV_DIR_VER);         // Vertical scrolling
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

  // Tạo các label cho thông tin Job
  // ProductCode
  labelProductCode = lv_label_create(jobInfoContainer);
  lv_label_set_text(labelProductCode, "ProductCode: N/A");
  lv_obj_set_style_text_font(labelProductCode, &lv_font_montserrat_14, 0);  // Font chữ lớn hơn
  lv_obj_set_style_text_color(labelProductCode, lv_color_hex(0x333333), 0); // Màu chữ tối hơn
  lv_obj_set_width(labelProductCode, LV_PCT(98));

  // PlanQty
  labelPlanQty = lv_label_create(jobInfoContainer);
  lv_label_set_text(labelPlanQty, "PlanQty: N/A");
  lv_obj_set_style_text_font(labelPlanQty, &lv_font_montserrat_14, 0);  // Font chữ lớn hơn
  lv_obj_set_style_text_color(labelPlanQty, lv_color_hex(0x333333), 0); // Màu chữ tối hơn
  lv_obj_set_width(labelPlanQty, LV_PCT(98));

  // JobStatus
  labelJobStatus = lv_label_create(jobInfoContainer);
  lv_label_set_text(labelJobStatus, "JobStatus: N/A");
  lv_obj_set_style_text_font(labelJobStatus, &lv_font_montserrat_14, 0);  // Font chữ lớn hơn
  lv_obj_set_style_text_color(labelJobStatus, lv_color_hex(0x333333), 0); // Màu chữ tối hơn
  lv_obj_set_width(labelJobStatus, LV_PCT(98));

  // StationCode
  labelStationCode = lv_label_create(jobInfoContainer);
  lv_label_set_text(labelStationCode, "StationCode: N/A");
  lv_obj_set_style_text_font(labelStationCode, &lv_font_montserrat_14, 0);  // Font chữ lớn hơn
  lv_obj_set_style_text_color(labelStationCode, lv_color_hex(0x333333), 0); // Màu chữ tối hơn
  lv_obj_set_width(labelStationCode, LV_PCT(98));

  //======================================================================

  // Tạo label cho Station
  lv_obj_t *labelStation = lv_label_create(container);
  lv_label_set_text(labelStation, "Select Station:");
  lv_obj_set_width(labelStation, LV_SIZE_CONTENT); // Đảm bảo độ rộng phù hợp với nội dung
  lv_obj_set_style_text_align(labelStation, LV_TEXT_ALIGN_LEFT, 0);

  // Tạo textarea cho tìm kiếm Station
  lv_obj_t *taStationSearch = lv_textarea_create(container);
  lv_textarea_set_placeholder_text(taStationSearch, "Search Station...");
  lv_textarea_set_one_line(taStationSearch, true);
  lv_obj_set_width(taStationSearch, LV_PCT(98));
  lv_obj_add_event_cb(taStationSearch, ta_event_cb, LV_EVENT_CLICKED, this);
  lv_obj_add_event_cb(taStationSearch, ta_event_cb, LV_EVENT_DEFOCUSED, this);

  // Tạo dropdown cho Stations
  ddStation = lv_dropdown_create(container);
  lv_obj_set_width(ddStation, LV_PCT(98));
  lv_obj_add_event_cb(ddStation, eventHandler, LV_EVENT_VALUE_CHANGED, this);

  // Tạo sự kiện tìm kiếm cho textarea Station
  lv_obj_add_event_cb(taStationSearch, [](lv_event_t *e)
                      {
    UnloadPage *page = (UnloadPage *)lv_event_get_user_data(e);
    page->filterDropdown(page->ddStation, page->stations, lv_textarea_get_text(e->target)); }, LV_EVENT_VALUE_CHANGED, this);

  // Tạo label cho Product
  lv_obj_t *labelProduct = lv_label_create(container);
  lv_label_set_text(labelProduct, "Select Product:");
  lv_obj_set_width(labelProduct, LV_SIZE_CONTENT);
  lv_obj_set_style_text_align(labelProduct, LV_TEXT_ALIGN_LEFT, 0);

  // Tạo textarea cho tìm kiếm Product
  lv_obj_t *taProductSearch = lv_textarea_create(container);
  lv_textarea_set_placeholder_text(taProductSearch, "Search Product...");
  lv_textarea_set_one_line(taProductSearch, true);
  lv_obj_set_width(taProductSearch, LV_PCT(98));
  lv_obj_add_event_cb(taProductSearch, ta_event_cb, LV_EVENT_CLICKED, this);
  lv_obj_add_event_cb(taProductSearch, ta_event_cb, LV_EVENT_DEFOCUSED, this);

  // Tạo dropdown cho Products
  ddProduct = lv_dropdown_create(container);
  lv_obj_set_width(ddProduct, LV_PCT(98));
  lv_obj_add_event_cb(ddProduct, eventHandler, LV_EVENT_VALUE_CHANGED, this);

  // Tạo sự kiện tìm kiếm cho textarea Product
  lv_obj_add_event_cb(taProductSearch, [](lv_event_t *e)
                      {
    UnloadPage *page = (UnloadPage *)lv_event_get_user_data(e);
    page->filterDropdown(page->ddProduct, page->products, lv_textarea_get_text(e->target)); }, LV_EVENT_VALUE_CHANGED, this);

  // Tạo nút Change Product
  btnChangeProduct = lv_btn_create(container);
  lv_obj_set_size(btnChangeProduct, 150, 50);
  lv_obj_add_event_cb(btnChangeProduct, eventHandler, LV_EVENT_CLICKED, this);

  lv_obj_t *btnChangeProductLabel = lv_label_create(btnChangeProduct);
  lv_label_set_text(btnChangeProductLabel, "Change Product");
  lv_obj_center(btnChangeProductLabel);
  // ---------------------------------------------------------------------------------
  // Tạo Keyboard
  // ---------------------------------------------------------------------------------
  keyboard = lv_keyboard_create(content_container);
  lv_obj_set_size(keyboard, LV_PCT(100), LV_PCT(50));
  lv_obj_align(keyboard, LV_ALIGN_BOTTOM_MID, 0, 0);
  lv_keyboard_set_mode(keyboard, LV_KEYBOARD_MODE_TEXT_UPPER);
  lv_obj_add_flag(keyboard, LV_OBJ_FLAG_HIDDEN | LV_OBJ_FLAG_FLOATING); // Ẩn ban đầu và làm floating

  // Thêm padding dưới cùng để tránh bị che bởi bàn phím
  lv_obj_set_style_pad_bottom(container, 100, 0); // 100 pixels hoặc chiều cao của bàn phím

  // ---------------------------------------------------------------------------------
  // Fetch data từ APIs
  // ---------------------------------------------------------------------------------
  fetchStations();
  fetchProducts();

  // Fetch Running Job thông tin cho station từ config.fxStationCode
  fetchRunningJob(config->config.fxStationCode);
}

void UnloadPage::fetchRunningJob(const String &stationCode)
{
  HTTPClient http;

  http.setTimeout(5000); // timeout trong 5000ms (5 giây)

  String url = config->config.httpIP + (config->config.httpPort != 0 ? ":" + String(config->config.httpPort) : "")     +
               config->config.pathGetRunningProductOnStation + "?station=" + URLEncode(stationCode.c_str());

  Serial.println("Fetching Running Job from: " + url); // Debugging URL

  try
  {
    http.begin(url);
    int httpCode = http.GET();

    if (httpCode == HTTP_CODE_OK)
    {
      String payload = http.getString();
      Serial.println("Running Job Payload: " + payload); // Debugging payload

      // Parse JSON
      DynamicJsonDocument doc(1024); // Dung lượng có thể điều chỉnh tùy nhu cầu
      DeserializationError error = deserializeJson(doc, payload);

      if (error)
      {
        Serial.print(F("deserializeJson() failed: "));
        Serial.println(error.f_str());
        return; // Thoát nếu phân giải JSON thất bại
      }

      // Chuyển đổi sang JsonObject
      JsonObject job = doc.as<JsonObject>();

      if (job.isNull())
      {
        Serial.println("No running job found.");
        // Cập nhật UI để hiển thị không có job
        lv_label_set_text(labelProductCode, "ProductCode: N/A");
        lv_label_set_text(labelPlanQty, "PlanQty: N/A");
        lv_label_set_text(labelJobStatus, "JobStatus: N/A");
        lv_label_set_text(labelStationCode, ("StationCode: " + stationCode).c_str());
        return;
      }

      // Cập nhật UI với dữ liệu job
      updateJobInfo(job);
    }
    else if (httpCode == 404)
    {
      String errorMessage = http.getString();
      Serial.println("No running job found (404): " + errorMessage);
      // Cập nhật UI để hiển thị không có job
      lv_label_set_text(labelProductCode, "ProductCode: N/A");
      lv_label_set_text(labelPlanQty, "PlanQty: N/A");
      lv_label_set_text(labelJobStatus, "JobStatus: N/A");
      lv_label_set_text(labelStationCode, ("StationCode: " + stationCode).c_str());

      // Hiển thị thông báo cho người dùng (tùy chọn)
      lv_obj_t *msgbox = lv_msgbox_create(NULL, "Notification", errorMessage.c_str(), NULL, true);
      lv_obj_center(msgbox);
    }
    else if (httpCode == 500)
    {
      String errorMessage = http.getString();
      Serial.println("Server Error (500): " + errorMessage);
      // Cập nhật UI để hiển thị không có job hoặc giữ nguyên trạng thái hiện tại

      // Hiển thị thông báo lỗi cho người dùng
      lv_obj_t *msgbox = lv_msgbox_create(NULL, "Server Error", errorMessage.c_str(), NULL, true);
      lv_obj_center(msgbox);
    }
    else
    {
      Serial.println("Error in HTTP request: " + String(httpCode));
      // Có thể thêm xử lý cho các mã lỗi khác nếu cần
    }
  }
  catch (...)
  {
    Serial.println("Exception caught during fetchRunningJob."); // Xử lý ngoại lệ
    // Có thể hiển thị thông báo lỗi cho người dùng
    lv_obj_t *msgbox = lv_msgbox_create(NULL, "Error", "An error occurred when connecting to the server.", NULL, true);
    lv_obj_center(msgbox);
  }

  http.end();
}

void UnloadPage::updateJobInfo(const JsonObject &job)
{
  String productCode = job["ProductCode"].as<String>();
  int planQty = job["PlanQty"].as<int>();
  String jobStatus = job["JobStatus"].as<String>();
  String stationCode = job["StationCode"].as<String>();

  // Cập nhật các label
  lv_label_set_text(labelProductCode, ("ProductCode: " + productCode).c_str());
  lv_label_set_text_fmt(labelPlanQty, "PlanQty: %d", planQty);
  lv_label_set_text(labelJobStatus, ("JobStatus: " + jobStatus).c_str());
  lv_label_set_text(labelStationCode, ("StationCode: " + stationCode).c_str());
}

// bản này có lưu lại các list dang sách sau khi đã filter.
void UnloadPage::filterDropdown(lv_obj_t *dropdown, const std::vector<Station> &dataList, const char *filter)
{
  lv_dropdown_clear_options(dropdown);
  filteredStationIndices.clear(); // Clear the previous filtered indices

  String filterStr = String(filter);
  filterStr.toLowerCase();

  for (size_t i = 0; i < dataList.size(); ++i)
  {
    const auto &item = dataList[i];
    String name = item.StationName;
    String lowerName = name;
    lowerName.toLowerCase();
    if (lowerName.indexOf(filterStr) != -1)
    {
      lv_dropdown_add_option(dropdown, name.c_str(), LV_DROPDOWN_POS_LAST);
      // filteredStationIndices.push_back(i); // Store the original index
      // size_t reversedIndex = dataList.size() - 1 - i;  // Đảo ngược chỉ số
      filteredStationIndices.push_back(i); // Lưu chỉ số đã đảo ngược
    }
  }
  // Serial.print("filteredStationIndices: [");
  // for (size_t i = 0; i < filteredStationIndices.size(); ++i)
  // {
  //   Serial.print(filteredStationIndices[i]);
  //   if (i < filteredStationIndices.size() - 1)
  //   {
  //     Serial.print(", ");
  //   }
  // }
  // Serial.println("]");
}

void UnloadPage::filterDropdown(lv_obj_t *dropdown, const std::vector<Product> &dataList, const char *filter)
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

lv_obj_t *UnloadPage::getPage()
{
  return page;
}

void UnloadPage::fetchStations()
{
  HTTPClient http;
  String url = config->config.httpIP + (config->config.httpPort != 0 ? ":" + String(config->config.httpPort) : "")    + config->config.pathGetStation;

  Serial.println("http Station: " + url); // Debugging URL

  try
  {
    http.begin(url);
    int httpCode = http.GET();

    if (httpCode == HTTP_CODE_OK)
    {
      String payload = http.getString();
      // Serial.println("payload Station: " + payload); // Debugging payload

      // Parse JSON
      DynamicJsonDocument doc(8192);
      DeserializationError error = deserializeJson(doc, payload);

      if (error)
      {
        Serial.print(F("deserializeJson() failed: "));
        Serial.println(error.f_str());
        return; // Exit if JSON deserialization fails
      }

      // Now convert doc to a JsonArray
      JsonArray arr = doc.as<JsonArray>();
      stations.clear();

      // Serial.println("stations doc: ");
      // serializeJson(doc, Serial); // Debugging serialized JSON
      // Serial.println();

      lv_dropdown_clear_options(ddStation); // Clear dropdown options
      // Serial.println("stations size: " + String(arr.size())); // Debugging array size

      for (JsonVariant item : arr)
      {
        Station s;
        s.StationCode = item["StationCode"].as<String>();
        s.StationName = item["StationName"].as<String>();
        // Serial.println("StationCode: " + s.StationCode + ", StationName: " + s.StationName);
        stations.push_back(s);
        lv_dropdown_add_option(ddStation, s.StationName.c_str(), 0); // Add option to dropdown
      }

      // // Debugging all stations
      // Serial.print("Stations all: ");
      // for (const auto &station : stations)
      // {
      //   Serial.println("StationCode: " + station.StationCode + ", StationName: " + station.StationName);
      // }
      // Serial.println();
    }
    else
    {
      Serial.println("Error in HTTP request: " + String(httpCode)); // Handle HTTP errors
    }
  }
  catch (...)
  {
    Serial.println("Exception caught during fetchStations."); // Handle exceptions
  }

  http.end();
}

void UnloadPage::fetchProducts()
{
  HTTPClient http;
  String url = config->config.httpIP + (config->config.httpPort != 0 ? ":" + String(config->config.httpPort) : "")    + config->config.pathGetProduct;

  Serial.println("http product: " + url); // Debugging URL

  try
  {
    http.begin(url);
    int httpCode = http.GET();

    if (httpCode == HTTP_CODE_OK)
    {
      String payload = http.getString();
      // Serial.println("payload Product: " + payload); // Debugging payload

      // Parse JSON
      DynamicJsonDocument doc(8192);
      DeserializationError error = deserializeJson(doc, payload);

      if (error)
      {
        Serial.print(F("deserializeJson() failed: "));
        Serial.println(error.f_str());
        return; // Exit if JSON deserialization fails
      }

      // Now convert doc to a JsonArray
      JsonArray arr = doc.as<JsonArray>();
      products.clear();

      lv_dropdown_clear_options(ddProduct); // Clear dropdown options

      // Serial.println("Products size: " + String(arr.size())); // Debugging array size

      for (JsonVariant item : arr)
      {
        Product p;
        p.ProductCode = item["ProductCode"].as<String>();
        p.ProductName = item["ProductName"].as<String>();
        // Serial.println("ProductCode: " + p.ProductCode + ", ProductName: " + p.ProductName); // Debugging each product
        products.push_back(p);
        lv_dropdown_add_option(ddProduct, p.ProductName.c_str(), 0); // Add option to dropdown
      }

      // // Debugging all products
      // Serial.print("Products all: ");
      // for (const auto &product : products)
      // {
      //   Serial.println("ProductCode: " + product.ProductCode + ", ProductName: " + product.ProductName);
      // }
      // Serial.println();
    }
    else
    {
      Serial.println("Error in HTTP request: " + String(httpCode)); // Handle HTTP errors
    }
  }
  catch (...)
  {
    Serial.println("Exception caught during fetchProducts."); // Handle exceptions
  }

  http.end();
}

// Callback cho các trường nhập liệu
void UnloadPage::ta_event_cb(lv_event_t *e)
{
  UnloadPage *page = (UnloadPage *)lv_event_get_user_data(e);
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

void UnloadPage::eventHandler(lv_event_t *e)
{
  UnloadPage *page = (UnloadPage *)lv_event_get_user_data(e);
  if (e->code == LV_EVENT_CLICKED)
  {
    if (e->target == page->btnChangeProduct)
    {
      page->onChangeProduct();
    }
  }
}

// Hàm này sẽ được gọi khi nhấn nút "Close" để đóng msgbox

void UnloadPage::onChangeProduct()
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
  // String url = config->config.httpIP + (config->config.httpPort != 0 ? ":" + String(config->config.httpPort) : "")    +
  //              config->config.pathPostChangedProduct +
  //              "?station=" + stationCode + "&product=" + productCode;
  // Serial.println("URL: " + url);

  // Xây dựng URL và gửi yêu cầu POST
  String stationCodeEncoded = URLEncode(stationCode.c_str());
  String productCodeEncoded = URLEncode(productCode.c_str());
  String url = config->config.httpIP + (config->config.httpPort != 0 ? ":" + String(config->config.httpPort) : "")    +
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
    lv_obj_t *msgbox = lv_msgbox_create(NULL, "Success", payload.c_str(), NULL, true); // Tạo msgbox
    lv_obj_center(msgbox);                                                             // Căn giữa msgbox

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
    lv_obj_t *msgbox = lv_msgbox_create(NULL, "Error", "Failed to change product", NULL, true); // Tạo msgbox
    lv_obj_center(msgbox);                                                                      // Căn giữa msgbox

    // Tạo style mới để đổi màu chữ
    static lv_style_t style;
    lv_style_init(&style);
    lv_style_set_text_color(&style, lv_palette_main(LV_PALETTE_RED)); // Đặt màu chữ thành đỏ

    // Áp dụng style cho nội dung message box
    lv_obj_add_style(lv_msgbox_get_text(msgbox), &style, 0);
  }

  http.end(); // Kết thúc yêu cầu HTTP
}

// void UnloadPage::refreshPage()
// {
//   // Xoá tất cả các đối tượng con của trang hiện tại
//   lv_obj_clean(page); // Xoá sạch tất cả các đối tượng trên trang

//   // Tạo lại giao diện cho trang Unload
//   init(); // Gọi lại hàm init() để tạo lại tất cả các đối tượng trên trang
// }

// Cập nhật hàm refreshPage
// void UnloadPage::refreshPage(lv_event_t *e)
// {
//   UnloadPage *page = (UnloadPage *)lv_event_get_user_data(e);  // Lấy dữ liệu user_data là con trỏ tới lớp UnloadPage

//   // Xoá tất cả các đối tượng con của trang hiện tại
//   lv_obj_clean(page->page); // Xoá sạch tất cả các đối tượng trên trang

//   // Tạo lại giao diện cho trang Unload
//   page->init(); // Gọi lại hàm init() để tạo lại tất cả các đối tượng trên trang
// }
void UnloadPage::refreshPage(lv_event_t *e)
{
  UnloadPage *page = (UnloadPage *)lv_event_get_user_data(e); // Lấy dữ liệu user_data là con trỏ tới lớp UnloadPage

  // Tạo một spinner (loader) để báo hiệu đang tải
  lv_obj_t *spinner = lv_spinner_create(page->page, 1000, 60); // 1 giây cho một vòng quay
  lv_obj_set_size(spinner, 50, 50);                            // Kích thước của spinner
  lv_obj_align(spinner, LV_ALIGN_CENTER, 0, 0);                // Đặt spinner ở giữa màn hình

  // Tạo một timer để chờ trong khi spinner hoạt động
  lv_timer_t *timer = lv_timer_create([](lv_timer_t *t)
                                      {
                                        // Lấy user_data từ timer
                                        UnloadPage *page = (UnloadPage *)t->user_data; // Sử dụng t->user_data để lấy dữ liệu user_data
                                        lv_obj_clean(page->page);                              // Xoá sạch tất cả các đối tượng trên trang
                                        page->init();                                          // Tạo lại giao diện
                                        lv_timer_del(t);                                       // Xoá timer khi xong
                                      },
                                      1000, page); // Tạo timer với thời gian tương ứng với thời gian của spinner

  // Gán trực tiếp user_data
  timer->user_data = page; // Gán user_data trực tiếp thông qua cấu trúc lv_timer_t
}
