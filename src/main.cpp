#include <Arduino.h>
#include <lvgl.h>
#include "esp_task_wdt.h"

#include "arial.c"

LV_FONT_DECLARE(arial);

static lv_style_t style; // Khai báo kiểu toàn cục

//kiểm soát không touch trong bao lâu thì bắt đầu định kỳ refetch.
static uint32_t last_touch_time = 0; // Lưu thời gian lần chạm cuối cùng
#define INACTIVITY_TIMEOUT 17000    // Thời gian chờ không tương tác (ms)

static uint32_t lastFetchTime = millis();

extern bool inited = false;


// #include "mutex.h"
#include "config.h"
#include "production.hpp"
#include "DowntimePage.hpp"
#include "UnloadPage.hpp"
#include "DowntimeErrorPage.hpp"
#include "config_page.hpp"
#include "NetworkModule.h"
#include "nvs_flash.h"

#include "PanelLan.h"

extern ConfigManager configMgr;

// cái này dùng cho "mutex.h"
// SemaphoreHandle_t xMutexFetchProduction = NULL;

//inline SemaphoreHandle_t xMutexFetchProduction;


PanelLan tft(BOARD_SC07);

NetworkModule networkModule;
Preferences preferences;

/*Change to your screen resolution*/
// static const uint16_t screenWidth  = 480;
// static const uint16_t screenHeight = 272;

static const uint16_t screenWidth = 272; // Đổi ngang thành dọc
static const uint16_t screenHeight = 480;

static lv_disp_draw_buf_t draw_buf;
static lv_color_t buf[2][screenWidth * 10];

void set_default_font(void) {
    lv_style_init(&style);
    lv_style_set_text_font(&style, &arial);

    // Áp dụng kiểu này cho toàn bộ giao diện người dùng
    lv_obj_add_style(lv_scr_act(), &style, LV_PART_MAIN);
}

/* Display flushing */
void my_disp_flush(lv_disp_drv_t *disp, const lv_area_t *area, lv_color_t *color_p)
{
  if (tft.getStartCount() == 0)
  { // Processing if not yet started
    tft.startWrite();
  }
  tft.pushImageDMA(area->x1, area->y1, area->x2 - area->x1 + 1, area->y2 - area->y1 + 1, (lgfx::swap565_t *)&color_p->full);
  lv_disp_flush_ready(disp);
}

// // chưa có kiểm tar định kỳ touch hay không để refetch
// /*Read the touchpad*/
// void my_touchpad_read(lv_indev_drv_t *indev_driver, lv_indev_data_t *data)
// {
//   uint16_t touchX, touchY;

//   data->state = LV_INDEV_STATE_REL;

//   if (tft.getTouch(&touchX, &touchY))
//   {
//     data->state = LV_INDEV_STATE_PR;

//     /*Set the coordinates*/
//     data->point.x = touchX;
//     data->point.y = touchY;
//   }
// }


void my_touchpad_read(lv_indev_drv_t *indev_driver, lv_indev_data_t *data)
{
    uint16_t touchX, touchY;

    data->state = LV_INDEV_STATE_REL;

    if (tft.getTouch(&touchX, &touchY))
    {
        data->state = LV_INDEV_STATE_PR;

        /*Set the coordinates*/
        data->point.x = touchX;
        data->point.y = touchY;

        // Cập nhật thời gian chạm cuối cùng
        last_touch_time = millis();
    }
}
bool check_inactivity()
{
    if (millis() - last_touch_time > INACTIVITY_TIMEOUT)
    {
        // Hành động khi không tương tác trong thời gian dài
        // Serial.println("Không có thao tác, thực hiện hành động...");
        
        // // Ví dụ: reset giao diện, hiển thị màn hình chờ, v.v.
        // lv_label_set_text(my_label, "Đã lâu không thao tác!");
        
        // Reset lại thời gian để tránh lặp lại liên tục
        last_touch_time = millis();
        
        return true;
    }
    return false;
}


inline lv_obj_t *mainPage;

// Page Instances
inline ProductionPage *productionPage;
inline DowntimePage *downtimePage;
inline UnloadPage *unloadPage;
inline DowntimeErrorPage *downtimeErrorPage;
inline ConfigurationPage *configPage;

// Biến toàn cục cho overlay
lv_obj_t *overlay_layer;
lv_obj_t *wifi_label;

void create_overlay_layer()
{
  // Tạo đối tượng overlay gắn trực tiếp vào display
  overlay_layer = lv_obj_create(lv_layer_top());            // Gắn vào lv_layer_top để luôn nằm trên
  lv_obj_remove_style_all(overlay_layer);                   // Bỏ các style mặc định
  lv_obj_set_size(overlay_layer, LV_HOR_RES, LV_VER_RES);   // Kích thước toàn màn hình
  lv_obj_set_style_bg_opa(overlay_layer, LV_OPA_TRANSP, 0); // Trong suốt để không che UI bên dưới

  // Đặt cờ không nhận sự kiện chạm trên overlay
  lv_obj_clear_flag(overlay_layer, LV_OBJ_FLAG_CLICKABLE);

  // Tạo label thông báo WiFi
  wifi_label = lv_label_create(overlay_layer);
  lv_label_set_text(wifi_label, "No WiFi");                           // Văn bản ngắn gọn tiếng Anh
  lv_obj_set_style_text_color(wifi_label, lv_color_hex(0xFF0000), 0); // Đặt màu chữ thành đỏ
  lv_obj_align(wifi_label, LV_ALIGN_TOP_MID, 0, 10);                  // Đặt vị trí cố định
}

void update_wifi_status(bool connected)
{
  if (connected)
  {
    lv_obj_add_flag(wifi_label, LV_OBJ_FLAG_HIDDEN); // Ẩn thông báo khi đã kết nối WiFi
  }
  else
  {
    lv_obj_clear_flag(wifi_label, LV_OBJ_FLAG_HIDDEN); // Hiện thông báo khi không có WiFi
  }
}

#pragma region layout tiêu đề

// Hàm tạo thanh bar
lv_obj_t *create_top_bar(lv_obj_t *parent, const char *title, lv_event_cb_t back_cb, void *back_cb_user_data, lv_event_cb_t refresh_cb, void *refresh_cb_user_data)
{
  // Tạo thanh bar chính
  lv_obj_t *bar = lv_obj_create(parent);
  lv_obj_set_size(bar, LV_PCT(100), 50); // Chiều cao 50px
  lv_obj_set_style_pad_all(bar, 1, 0);
  lv_obj_set_style_bg_color(bar, lv_palette_main(LV_PALETTE_BLUE), 0);
  lv_obj_set_style_radius(bar, 0, 0);
  lv_obj_clear_flag(bar, LV_OBJ_FLAG_SCROLLABLE); // Không cuộn

  // Nút Back
  if (back_cb != NULL)
  {
    lv_obj_t *back_btn = lv_btn_create(bar);
    lv_obj_set_size(back_btn, 30, 50);
    lv_obj_align(back_btn, LV_ALIGN_LEFT_MID, 0, 0);
    lv_obj_add_event_cb(back_btn, back_cb, LV_EVENT_CLICKED, back_cb_user_data);

    lv_obj_t *back_label = lv_label_create(back_btn);
    lv_label_set_text(back_label, LV_SYMBOL_LEFT);
    lv_obj_center(back_label);
  }

  // Tiêu đề
  lv_obj_t *title_label = lv_label_create(bar);
  lv_label_set_text(title_label, title);
  lv_obj_set_style_text_font(title_label, &arial, 0);
  lv_obj_align(title_label, LV_ALIGN_CENTER, 0, 0);

  // hiện tại không dùng cái này vì nó không ổn cho việt refetch hàng h . 
  // Nút Refresh
  if (refresh_cb != NULL)
  {
    lv_obj_t *refresh_btn = lv_btn_create(bar);
    lv_obj_set_size(refresh_btn, 30, 50);
    lv_obj_align(refresh_btn, LV_ALIGN_RIGHT_MID, 0, 0);
    lv_obj_add_event_cb(refresh_btn, refresh_cb, LV_EVENT_CLICKED, refresh_cb_user_data);

    lv_obj_t *refresh_label = lv_label_create(refresh_btn);
    lv_label_set_text(refresh_label, LV_SYMBOL_REFRESH);
    lv_obj_center(refresh_label);
  }

  // Nút List - Quay về mainPage
  lv_obj_t *list_btn = lv_btn_create(bar);
  lv_obj_set_size(list_btn, 30, 50);
  lv_obj_align(list_btn, LV_ALIGN_RIGHT_MID, -30, 0); // Đặt ở giữa bên trái
  lv_obj_add_event_cb(list_btn, [](lv_event_t *e)
                      {
        // Quay về mainPage khi nhấn nút
        lv_scr_load(mainPage); }, LV_EVENT_CLICKED, NULL);

  lv_obj_t *list_label = lv_label_create(list_btn);
  lv_label_set_text(list_label, LV_SYMBOL_LIST);
  lv_obj_center(list_label);

  // Nút Home - Quay về mainPage
  lv_obj_t *home_btn = lv_btn_create(bar);
  lv_obj_set_size(home_btn, 30, 50);
  lv_obj_align(home_btn, LV_ALIGN_LEFT_MID, 30, 0); // Đặt không giữa bên trái
  lv_obj_add_event_cb(home_btn, [](lv_event_t *e)
                      {
        // Quay về mainPage khi nhấn nút
        lv_disp_load_scr(productionPage->getPage()); }, LV_EVENT_CLICKED, NULL);

  lv_obj_t *home_label = lv_label_create(home_btn);
  lv_label_set_text(home_label, LV_SYMBOL_HOME);
  lv_obj_center(home_label);

  return bar;
}

#pragma endregion layout tiêu đề

#pragma region xử lý nhập mật khẩu
// Biểu tượng tích hợp sẵn của LVGL cho "Show" và "Hide"
#define ICON_EYE LV_SYMBOL_EYE_OPEN
#define ICON_EYE_CLOSE LV_SYMBOL_EYE_CLOSE

// Biến toàn cục để lưu trạng thái hiển thị mật khẩu
bool passwordVisible = false;
// Mật khẩu đúng (thay "1234" thành mật khẩu mong muốn)

// Hàm xử lý chuyển đổi hiển thị/ẩn mật khẩu
void togglePasswordVisibility(lv_event_t *e)
{
  lv_obj_t *textarea = (lv_obj_t *)lv_event_get_user_data(e);
  lv_obj_t *btnToggle = lv_event_get_target(e);
  lv_obj_t *labelToggle = lv_obj_get_child(btnToggle, 0); // Lấy nhãn trong nút

  // Kiểm tra chế độ hiện tại và chuyển đổi
  if (passwordVisible)
  {
    lv_textarea_set_password_mode(textarea, true); // Ẩn mật khẩu
    lv_label_set_text(labelToggle, ICON_EYE);      // Đổi biểu tượng thành mắt mở
    passwordVisible = false;
  }
  else
  {
    lv_textarea_set_password_mode(textarea, false); // Hiển thị mật khẩu
    lv_label_set_text(labelToggle, ICON_EYE_CLOSE); // Đổi biểu tượng thành mắt đóng
    passwordVisible = true;
  }
}

// Hàm xử lý xác nhận mật khẩu
void checkPassword(lv_obj_t *textarea)
{
  const char *input = lv_textarea_get_text(textarea);
  const char *correctPassword = "123456"; // Mật khẩu đúng

  if (strcmp(input, correctPassword) == 0)
  {
    // Nếu mật khẩu đúng, chuyển đến trang cấu hình
    lv_disp_load_scr(configPage->getPage());
  }
  else
  {
    // Nếu mật khẩu sai, hiển thị thông báo lỗi căn giữa
    lv_obj_t *msgbox = lv_msgbox_create(NULL, "Error", "Mat khau khong chinh xac", NULL, true);
    lv_obj_center(msgbox); // Căn giữa thông báo
  }
}

// Hàm xử lý sự kiện của keyboard
void keyboardEventHandler(lv_event_t *e)
{
  lv_obj_t *container = (lv_obj_t *)lv_event_get_user_data(e);      // Lấy container để xóa sau khi xử lý
  lv_obj_t *textarea = (lv_obj_t *)lv_obj_get_user_data(container); // Lấy textarea từ container
  checkPassword(textarea);

  // Đóng hộp thoại nhập mật khẩu sau khi nhấn Enter
  lv_obj_del(container);
}

// Hàm hiển thị hộp thoại nhập mật khẩu
void showPasswordPrompt(lv_event_t *e)
{
  // Tạo container cho msgbox và bàn phím
  lv_obj_t *container = lv_obj_create(lv_scr_act());
  lv_obj_set_size(container, LV_PCT(100), LV_PCT(100));
  lv_obj_align(container, LV_ALIGN_CENTER, 0, 0);

  // Tạo msgbox nhập mật khẩu với kích thước nhỏ gọn
  lv_obj_t *msgbox = lv_msgbox_create(container, NULL, "Enter password:", NULL, false);
  lv_obj_set_size(msgbox, 270, 130); // Điều chỉnh kích thước để thêm nút chuyển đổi
  lv_obj_align(msgbox, LV_ALIGN_TOP_MID, 0, 20);

  // Tạo textarea để nhập mật khẩu (một dòng duy nhất)
  lv_obj_t *textarea = lv_textarea_create(msgbox);
  lv_textarea_set_password_mode(textarea, true);
  lv_textarea_set_one_line(textarea, true); // Chế độ một dòng
  lv_textarea_set_max_length(textarea, 16);
  lv_obj_set_width(textarea, 180);
  lv_textarea_set_placeholder_text(textarea, "Enter password...");
  lv_obj_align(textarea, LV_ALIGN_CENTER, 0, -10);

  // Gắn userdata cho container để truy xuất textarea trong sự kiện keyboard
  lv_obj_set_user_data(container, textarea);

  // Tạo nút chuyển đổi để hiển thị/ẩn mật khẩu với biểu tượng
  lv_obj_t *btnToggle = lv_btn_create(msgbox);
  lv_obj_set_size(btnToggle, 40, 40); // Thay vì căn chỉnh theo msgbox, căn chỉnh nút theo textarea
  lv_obj_align_to(btnToggle, textarea, LV_ALIGN_OUT_RIGHT_MID, 10, 0);
  lv_obj_set_style_radius(btnToggle, LV_RADIUS_CIRCLE, 0); // Nút tròn

  // Tạo nhãn cho nút với biểu tượng "Show"
  lv_obj_t *labelToggle = lv_label_create(btnToggle);
  lv_label_set_text(labelToggle, ICON_EYE); // Biểu tượng mắt mở
  lv_label_set_recolor(labelToggle, true);
  lv_label_set_long_mode(labelToggle, LV_LABEL_LONG_WRAP);           // Sử dụng hằng số đúng
  lv_obj_set_style_text_align(labelToggle, LV_TEXT_ALIGN_CENTER, 0); // Sử dụng hàm đúng
  lv_obj_center(labelToggle);

  // Gán sự kiện cho nút chuyển đổi hiển thị/ẩn mật khẩu
  lv_obj_add_event_cb(btnToggle, togglePasswordVisibility, LV_EVENT_CLICKED, textarea);

  // Tạo bàn phím số và đặt ở dưới cùng màn hình
  lv_obj_t *keyboard = lv_keyboard_create(container);
  lv_obj_set_size(keyboard, LV_PCT(100), LV_PCT(40));
  lv_obj_align(keyboard, LV_ALIGN_BOTTOM_MID, 0, 0);
  lv_keyboard_set_mode(keyboard, LV_KEYBOARD_MODE_NUMBER); // Chỉ chế độ số
  lv_keyboard_set_textarea(keyboard, textarea);            // Liên kết với textarea

  // Gán sự kiện cho nút Enter trên keyboard
  lv_obj_add_event_cb(keyboard, keyboardEventHandler, LV_EVENT_READY, container);
}
#pragma endregion xử lý nhập mật khẩu

void ui_main()
{
  mainPage = lv_obj_create(NULL);

  // // Tạo nút "Configuration"
  // lv_obj_t *btnConfig = lv_btn_create(mainPage);
  // lv_obj_set_size(btnConfig, LV_PCT(90), 50);
  // lv_obj_align(btnConfig, LV_ALIGN_TOP_MID, 0, 50);
  // lv_obj_t *labelConfig = lv_label_create(btnConfig);
  // lv_obj_set_style_text_font(labelConfig, &arial, 0);
  // lv_label_set_text(labelConfig, "Cấu hình");
  // lv_obj_center(labelConfig);

  // lv_obj_add_event_cb(btnConfig, showPasswordPrompt, LV_EVENT_CLICKED, NULL);

  // Tạo nút "Production"
  lv_obj_t *btnProduction = lv_btn_create(mainPage);
  lv_obj_set_size(btnProduction, LV_PCT(90), 50);
  lv_obj_align(btnProduction, LV_ALIGN_TOP_MID, 0, 120);
  lv_obj_t *labelProduction = lv_label_create(btnProduction);
  lv_obj_set_style_text_font(labelProduction, &arial, 0);
  lv_label_set_text(labelProduction, "Sản xuất");
  lv_obj_center(labelProduction);
  lv_obj_add_event_cb(btnProduction, [](lv_event_t *e)
                      {
                        // productionPage->refreshPage();               // Làm mới lại trang trước khi hiển thị
                        lv_disp_load_scr(productionPage->getPage()); // Hiển thị trang mới
                      },
                      LV_EVENT_CLICKED, NULL);

  // Tạo nút "Other"
  lv_obj_t *btnOther = lv_btn_create(mainPage);
  lv_obj_set_size(btnOther, LV_PCT(90), 50);
  lv_obj_align(btnOther, LV_ALIGN_TOP_MID, 0, 190);
  lv_obj_t *labelOther = lv_label_create(btnOther);
  lv_obj_set_style_text_font(labelOther, &arial, 0);
  lv_label_set_text(labelOther, "Khác");
  lv_obj_center(labelOther);
  lv_obj_add_event_cb(btnOther, [](lv_event_t *e)
                      {
                        lv_obj_t* msg = lv_msgbox_create(NULL, "Info", "Trang chua duoc trien khai.", NULL, true);
                        lv_obj_center(msg);
                        lv_obj_add_event_cb(msg, [](lv_event_t* e){
                          lv_obj_del(lv_event_get_target(e));
                        }, LV_EVENT_CLICKED, NULL); }, LV_EVENT_CLICKED, NULL);

  // Hiển thị màn hình chính
  lv_disp_load_scr(mainPage);
}

void ElegantOTARUN(void *pvParameter)
{
  while (1)
  {
    ElegantOTA.loop();
    esp_task_wdt_reset();
  }
}

TaskHandle_t memoryTaskHandle = NULL;

// Function to print memory usage
void printMemoryUsage()
{
  String message = String("Free Heap (xPort): ") + String((unsigned long)xPortGetFreeHeapSize()) + " bytes, Free Heap (ESP): " + String((unsigned long)ESP.getFreeHeap()) + " bytes, Stack High Water Mark: " + String(uxTaskGetStackHighWaterMark(NULL) * 4) + " bytes." + "\n";
  Serial.print(message);
}

// Task to run printMemoryUsage every 3 seconds
void memoryTask(void *pvParameters)
{
  while (1)
  {
    printMemoryUsage();

    // // fetch lại production
    // if (check_inactivity())
    // {
    //  if (inited == true)
    // {
    //   productionPage->fetchProducts();
    // }
    // }
    vTaskDelay(pdMS_TO_TICKS(5000)); // Delay for 3 seconds (3000 ms)
  }
}

TaskHandle_t LGVLRunTaskHandle = NULL;


void LGVLRunTask(void *pvParameters)
{
  while (1)
  {
    // lv_timer_handler(); /* let the GUI do its work */
     lv_timer_handler_run_in_period(7);
    // Serial.println("tick");
    vTaskDelay(pdMS_TO_TICKS(5)); // Delay for 3 seconds (3000 ms)

    if (inited == true)
    {
      // if (check_inactivity())
      {
        if (millis() > lastFetchTime)
        {

          Serial.println("start lastFetchTime: " + String(lastFetchTime) + " millis: " + String(millis()));
          productionPage->fetchProducts();
          lastFetchTime += 10000; // Thêm khoảng thời gian 10 giây vào mốc thời gian kế tiếp

          Serial.println("end lastFetchTime: " + String(lastFetchTime) + " millis: " + String(millis()));

           String message = String("Free Heap (xPort): ") + String((unsigned long)xPortGetFreeHeapSize()) + " bytes, Free Heap (ESP): " + String((unsigned long)ESP.getFreeHeap()) + " bytes, Stack High Water Mark: " + String(uxTaskGetStackHighWaterMark(NULL) * 4) + " bytes." + "\n";

           Serial.print("\n\n\nMMMMMMMMMMMMMMMMMM" + message);
        }
        vTaskDelay(pdMS_TO_TICKS(5));
      }
    }
    // else
    // {
    //   // Serial.println("not inited");
    // }
  }
}

TaskHandle_t TaskCheckAndReConnectCode_handle;
void TaskCheckAndReConnectCode(void *pvParameters)
{

  networkModule.reConnect();

  for (;;)
  {
    // Vòng lặp vô hạn
  }
}

// Lưu config với Preferences

bool stringToBool(String str)
{
  str.toLowerCase(); // Chuyển đổi chuỗi thành chữ thường
  if (str == "true" || str == "1")
  {
    return true;
  }
  else if (str == "false" || str == "0")
  {
    return false;
  }

  // Xử lý chuỗi không hợp lệ theo cách bạn muốn
  return false; // hoặc ném lỗi, tùy vào yêu cầu
}

bool saveConfig(const String &key, const String &value)
{
  preferences.begin("esp32_config", false);
  bool isSaved = preferences.putString(key.c_str(), value.c_str());
  preferences.end();
  return isSaved;
}

void handleConfig(const String &line, Print &output)
{
  bool isSaved = false;

  if (line.startsWith("ssid="))
  {
    isSaved = saveConfig("ssid", line.substring(5));
    output.print("SSID: ");
    output.println(line.substring(5));
  }
  else if (line.startsWith("password="))
  {
    isSaved = saveConfig("password", line.substring(9));
    output.print("Password: ");
    output.println(line.substring(9));
  }
  else if (line.startsWith("isBroadcastWifi="))
  {
    // Mở NVS với namespace "esp32_config"
    preferences.begin("esp32_config", false);
    // Lưu giá trị isBroadcastWifi với key "isBroWifi"
    isSaved = preferences.putBool("isBroadCW", stringToBool(line.substring(16)));

    // Đóng NVS
    preferences.end();

    output.print("isBroadcastWifi: ");
    output.println(line.substring(16));
  }
  else if (line.startsWith("serverURL="))
  {
    isSaved = saveConfig("serverURL", line.substring(10));
    output.print("Server URL: ");
    output.println(line.substring(10));
  }

  else if (line.startsWith("FirmwareVersion"))
  {
    output.println("Firmware Version: " + String(FIRMWARE_VERSION));
  }

  else if (line.startsWith("localIP="))
  {
    output.print("IP Local: ");
    output.println(WiFi.localIP());
  }
  else if (line.startsWith("macAddress="))
  {
    // Lấy địa chỉ MAC của ESP32
    uint8_t mac[6];
    esp_read_mac(mac, ESP_MAC_WIFI_STA);

    // Convert địa chỉ MAC sang chuỗi
    char tempBuffer[30];
    sprintf(tempBuffer, "%02X:%02X:%02X:%02X:%02X:%02X", mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);

    output.print("MAC Address: ");
    output.println(tempBuffer);
  }
  else if (line.startsWith("softAPIP="))
  {
    output.print("IP softAP: ");
    output.println(WiFi.softAPIP());
  }

  else if (line.startsWith("WifiStrength"))
  {
    output.print("WifiStrength : ");
    output.println("WifiStrength : " + String(WiFi.RSSI()) + " | BSSIDstr " + WiFi.BSSIDstr());
  }
  else if (line.startsWith("DeleteNVS"))
  {
    output.print("DeleteNVS : ");
    nvs_flash_erase(); // erase the NVS partition and...
    nvs_flash_init();  // initialize the NVS partition.
    output.println("Deleted NVS memory");
  }
  else if (line.startsWith("GetSizePreferencesNameSpace="))
  {
    // Tách chuỗi để lấy namespace và partitionLabel
    // String line = "some_input_string_here";
    String customData = line.substring(28);
    int separatorIndex = customData.indexOf('|');

    String customNameSpace;
    String partitionLabel;

    if (separatorIndex != -1)
    {
      customNameSpace = customData.substring(0, separatorIndex);
      partitionLabel = customData.substring(separatorIndex + 1);
    }
    else
    {
      customNameSpace = customData;
      partitionLabel = ""; // Sử dụng giá trị mặc định nếu không có partitionLabel
    }

    // Open the preferences with the given namespace and partition label
    preferences.begin(customNameSpace.c_str(), true, partitionLabel.c_str());

    // Initialize NVS
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND)
    {
      ESP_ERROR_CHECK(nvs_flash_erase());
      ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);

    // Get NVS statistics
    nvs_stats_t nvs_stats;
    ret = nvs_get_stats(partitionLabel.length() > 0 ? partitionLabel.c_str() : NULL, &nvs_stats);
    ESP_ERROR_CHECK(ret);

    // Print NVS statistics
    output.printf("Namespace %s on partition %s:\n", customNameSpace.c_str(), partitionLabel.c_str());
    output.printf("Used entries: %d\n", nvs_stats.used_entries);
    output.printf("Free entries: %d\n", nvs_stats.free_entries);
    output.printf("Total entries: %d\n", nvs_stats.total_entries);
    output.printf("Namespace count: %d\n", nvs_stats.namespace_count);

    // NVS iterator to find entries
    nvs_iterator_t it = nvs_entry_find(partitionLabel.length() > 0 ? partitionLabel.c_str() : NVS_DEFAULT_PART_NAME, NULL, NVS_TYPE_ANY);
    if (it == NULL)
    {
      output.println("No namespaces found.");
      return;
    }

    output.println("Namespaces and their entries:");

    // Use a vector to track printed namespaces
    std::vector<String> printedNamespaces;

    while (it != NULL)
    {
      nvs_entry_info_t info;
      nvs_entry_info(it, &info);

      // Only print namespace if it hasn't been printed before
      bool namespacePrinted = false;
      for (size_t i = 0; i < printedNamespaces.size(); i++)
      {
        if (printedNamespaces[i] == info.namespace_name)
        {
          namespacePrinted = true;
          break;
        }
      }

      if (!namespacePrinted)
      {
        output.printf("Namespace: %s\n", info.namespace_name);
        printedNamespaces.push_back(info.namespace_name);

        // Find and print entries for the current namespace
        nvs_iterator_t entry_it = nvs_entry_find(partitionLabel.length() > 0 ? partitionLabel.c_str() : NVS_DEFAULT_PART_NAME, info.namespace_name, NVS_TYPE_ANY);
        while (entry_it != NULL)
        {
          nvs_entry_info_t entry_info;
          nvs_entry_info(entry_it, &entry_info);
          output.printf("  Key: %s, Type: %d\n", entry_info.key, entry_info.type);

          // Print the value based on the type
          if (entry_info.type == NVS_TYPE_U8)
          {
            uint8_t value = preferences.getUChar(entry_info.key);
            output.printf("Value: %u\n", value);
          }
          else if (entry_info.type == NVS_TYPE_I32)
          {
            int32_t value = preferences.getInt(entry_info.key);
            output.printf("Value: %d\n", value);
          }
          else if (entry_info.type == NVS_TYPE_STR)
          {
            String value = preferences.getString(entry_info.key);
            output.printf("Value: %s\n", value.c_str());
          }
          else if (entry_info.type == NVS_TYPE_BLOB)
          {
            size_t required_size = preferences.getBytesLength(entry_info.key);
            uint8_t *value = (uint8_t *)malloc(required_size);
            preferences.getBytes(entry_info.key, value, required_size);
            output.printf("Value: ");
            for (size_t i = 0; i < required_size; i++)
            {
              output.printf("%02x ", value[i]);
            }
            output.println();
            free(value);
          }
          else
          {
            output.println("Value: (Unknown type)");
          }

          entry_it = nvs_entry_next(entry_it);
        }
        nvs_release_iterator(entry_it);
      }

      it = nvs_entry_next(it);
    }
    nvs_release_iterator(it);

    // Đóng Preferences
    preferences.end();
  }
  else if (line.startsWith("DeletePreferencespartitionLabel="))
  {
    String partitionLabel = line.substring(32);

    const esp_partition_t *partition = esp_partition_find_first(ESP_PARTITION_TYPE_DATA, ESP_PARTITION_SUBTYPE_DATA_NVS, partitionLabel.c_str());

    if (partition != NULL)
    {
      esp_err_t err = esp_partition_erase_range(partition, 0, partition->size);
      if (err == ESP_OK)
      {
        output.println("NVS partition erased successfully");
      }
      else
      {
        output.printf("Failed to erase NVS partition: %s\n", esp_err_to_name(err));
      }
    }
    else
    {
      output.println("Partition not found");
    }

    output.println("Removed preferences partition: " + partitionLabel);
  }
  else if (line.startsWith("DeletePreferences[esp32_config]Key="))
  {
    preferences.begin("esp32_config", false);
    // Giả sử 'preferences' đã được khởi tạo trước đó
    String customKey = line.substring(35);         // Lấy tên khóa tùy chỉnh từ dòng lệnh
    const char *customKeyChar = customKey.c_str(); // Chuyển đổi sang kiểu const char *
    preferences.remove(customKeyChar);             // Xóa cặp khóa-giá trị với khóa tùy chỉnh
    output.println("Removed preferences value: " + customKey);
    preferences.end();
  }
  else if (line.startsWith("DeletePreferences[log_day1,LogCounter]Key="))
  {
    // preferences.begin("esp32_config", false);
    preferences.begin("LogCounter", false, "log_day1");
    // Giả sử 'preferences' đã được khởi tạo trước đó
    String customKey = line.substring(42);         // Lấy tên khóa tùy chỉnh từ dòng lệnh
    const char *customKeyChar = customKey.c_str(); // Chuyển đổi sang kiểu const char *
    preferences.remove(customKeyChar);             // Xóa cặp khóa-giá trị với khóa tùy chỉnh
    output.println("Removed preferences value: " + customKey);
    preferences.end();
  }
  else if (line.startsWith("showAllConfig"))
  {
    // // Bạn cần khởi tạo các biến loadedSsid, loadedPassword, loadedserverURL, loadedServerEventName, loadedServerCounterName, loadedhighSignalDur, loadedCounterPins[].
    // output.println();
    // output.println();
    // output.println("SSID: " + String(loadedSsid));
    // output.println("Password: " + String(loadedPassword));
    // output.println("serverURL: " + String(loadedserverURL));
    // output.println("Server Name: " + String(loadedServerEventName));
    // output.println("Server Counter Name: " + String(loadedServerCounterName));
    // output.println("High Signal Duration: " + String(loadedhighSignalDur));
    // output.println("Alive Signal Duration: " + String(loadedAliveSignalDur));
    // output.println("Alive Signal Name: " + String(NowsensorNamesAlive));

    // output.println("Counter Pins: " + String(loadedCounterPins[0]) + ", " +
    //                String(loadedCounterPins[1]) + ", " +
    //                String(loadedCounterPins[2]) + ", " +
    //                String(loadedCounterPins[3]));
  }
  else if (line.startsWith("wifiSSID="))
  {
    preferences.begin("app_config", false);
    isSaved = preferences.putString("wifiSSID", line.substring(9));
    preferences.end();

    output.print("WiFi SSID: ");
    output.println(line.substring(9));
  }
  else if (line.startsWith("wifiPassword="))
  {
    preferences.begin("app_config", false);
    isSaved = preferences.putString("wifiPassword", line.substring(13));
    preferences.end();

    output.print("WiFi Password: ");
    output.println(line.substring(13));
  }
  else if (line.startsWith("httpIP="))
  {
    preferences.begin("app_config", false);
    isSaved = preferences.putString("httpIP", line.substring(7));
    preferences.end();

    output.print("HTTP IP: ");
    output.println(line.substring(7));
  }
  else if (line.startsWith("httpPort="))
  {
    preferences.begin("app_config", false);
    isSaved = preferences.putUShort("httpPort", (uint16_t)line.substring(9).toInt());
    preferences.end();

    output.print("HTTP Port: ");
    output.println(line.substring(9));
  }
  else if (line.startsWith("fxStationCode="))
  {
    preferences.begin("app_config", false);
    isSaved = preferences.putString("fxStationCode", line.substring(14));
    preferences.end();

    output.print("FX Station Code: ");
    output.println(line.substring(14));
  }
  else if (line.startsWith("pathGetStation="))
  {
    preferences.begin("app_config", false);
    isSaved = preferences.putString("pathGetStation", line.substring(15));
    preferences.end();

    output.print("Path Get Station: ");
    output.println(line.substring(15));
  }
  else if (line.startsWith("pathGetProduct="))
  {
    preferences.begin("app_config", false);
    isSaved = preferences.putString("pathGetProduct", line.substring(15));
    preferences.end();

    output.print("Path Get Product: ");
    output.println(line.substring(15));
  }
  else if (line.startsWith("pathGRPOnS="))
  {
    preferences.begin("app_config", false);
    isSaved = preferences.putString("pathGRPOnS", line.substring(11));
    preferences.end();

    output.print("Path Get Running Product On Station: ");
    output.println(line.substring(11));
  }
  else if (line.startsWith("pathPChaP="))
  {
    preferences.begin("app_config", false);
    isSaved = preferences.putString("pathPChaP", line.substring(10));
    preferences.end();

    output.print("Path Post Changed Product: ");
    output.println(line.substring(10));
  }

  else
  {
    output.println("Not a configuration command.");
  }

  if (isSaved)
  {
    output.println("Saved configuration successfully.");
  }
  else
  {
    output.println("Save configuration failed.");
  }
}

void TaskUARTReceiveProgram()
{
  if (Serial.available())
  {
    String line = Serial.readStringUntil('\n');
    Serial.println(line);
    handleConfig(line, Serial);
  }
}

TaskHandle_t TaskUARTReceive_handle;
void TaskUARTReceive(void *pvParameters)
{

  // Vòng lặp vô tận để task không kết thúc
  for (;;)
  {
    // Thêm mã nguồn nếu cần
    TaskUARTReceiveProgram();
    vTaskDelay(100 / portTICK_PERIOD_MS);
    // esp_task_wdt_reset();
  }
}

unsigned long main_t = 0;

void setup()
{

  // // Tạo mutex
  // xMutexFetchProduction = xSemaphoreCreateMutex();
  // if (xMutexFetchProduction == NULL)
  // {
  //   Serial.println("Failed to create mutex! Restarting...");
  //   ESP.restart(); // Reset ESP32
  // }

  Serial.begin(115200); /* prepare for possible serial debug */

  Serial.println("Booting....");
  // setup lvgl

  // Cấu hình LVGL để sử dụng hàm callback
  // lv_log_register_print_cb(my_log_cb);

  tft.begin();

  tft.setRotation(1); // Thử 1 hoặc 2 tùy vào hướng dọc mong muốn

  lv_init();

  // Thiết lập phông chữ mặc định
  //set_default_font();

  lv_disp_draw_buf_init(&draw_buf, buf[0], buf[1], screenWidth * 10);

  /*Initialize the display*/
  static lv_disp_drv_t disp_drv;
  lv_disp_drv_init(&disp_drv);
  /*Change the following line to your display resolution*/
  disp_drv.hor_res = screenWidth;
  disp_drv.ver_res = screenHeight;
  disp_drv.flush_cb = my_disp_flush;
  disp_drv.draw_buf = &draw_buf;
  lv_disp_drv_register(&disp_drv);

  /*Initialize the input device driver*/
  static lv_indev_drv_t indev_drv;
  lv_indev_drv_init(&indev_drv);
  indev_drv.type = LV_INDEV_TYPE_POINTER;
  indev_drv.read_cb = my_touchpad_read;
  lv_indev_drv_register(&indev_drv);


     
  xTaskCreate(
      LGVLRunTask,       // Function that implements the task
      "LVGL run Task",    // Name of the task
      8048,             // Stack size in words (adjust if necessary)
      NULL,             // Task input parameter (not used here)
      1,                // Priority of the task
      &LGVLRunTaskHandle // Task handle to track the task
  );


  xTaskCreate(TaskUARTReceive, "TaskUARTReceive", 5040, NULL, 1, &TaskUARTReceive_handle);

  // Load config from NVS
  configMgr.loadConfig();
  

  create_overlay_layer(); // Gọi hàm tạo overlay layer

  networkModule.initConnect();

  initOtaModule();

  // Initialize Pages
  unloadPage = new UnloadPage(&configMgr);
  unloadPage->init();
 
  downtimePage = new DowntimePage(&configMgr);
  downtimePage->init();

  downtimeErrorPage = new DowntimeErrorPage(&configMgr);
  downtimeErrorPage->init();

  productionPage = new ProductionPage(&configMgr);
  productionPage->init();

  // configPage = new ConfigurationPage(&configMgr);
  // configPage->init();

  ui_main();

  // // Create FreeRTOS task for LVGL
  // xTaskCreatePinnedToCore(taskLVGL, "LVGL Task", 19096, NULL, 1, NULL, 0);
  xTaskCreate(ElegantOTARUN, "ElegantOTARUN Task", 2040, NULL, 1, NULL);

  // Create the FreeRTOS task

  xTaskCreate(TaskCheckAndReConnectCode, "TaskCheckAndReConnectCode", 2040, NULL, 1, &TaskCheckAndReConnectCode_handle);

  xTaskCreate(
      memoryTask,       // Function that implements the task
      "Memory Task",    // Name of the task
      2048,             // Stack size in words (adjust if necessary)
      NULL,             // Task input parameter (not used here)
      1,                // Priority of the task
      &memoryTaskHandle // Task handle to track the task
  );

  lv_disp_load_scr(productionPage->getPage());

  inited = true;
  
}

void loop()
{
  // lv_timer_handler(); /* let the GUI do its work */

  // lv_timer_handler_run_in_period(500); /* chạy lv_timer_handler() mỗi 5ms */
  // delay(5);
  //  Serial.println("TTT");
  // // ver có khắc phục
  // if ((uint32_t)(millis() - lastFetchTime) > 10000)
  // {
  //   productionPage->fetchProducts();
  //   lastFetchTime = millis();
  // }

  // if (millis() > lastFetchTime)
  // {

  //   Serial.println("starrt lastFetchTime: " + String(lastFetchTime) + " millis: " + String(millis()));
  //   productionPage->fetchProducts();
  //   lastFetchTime += 10000; // Thêm khoảng thời gian 10 giây vào mốc thời gian kế tiếp

  //   Serial.println("end lastFetchTime: " + String(lastFetchTime) + " millis: " + String(millis()));
  // }
  // delay(2);
  // Serial.println("MMM");
}
