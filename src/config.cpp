#include "config.h"

ConfigManager::ConfigManager()
{
  config.wifiSSID = "KST-Extend";
  config.wifiPassword = "KST_123789";
  config.httpIP = "192.168.1.179";
  config.httpPort = 5003;
  config.local_ip_str = "192.168.1.189";
  config.gateway_str = "192.168.1.1";
  config.subnet_str = "255.255.255.0";
  config.primaryDNS_str = "8.8.8.8";
  config.secondaryDNS_str = "8.8.4.4";
  config.fxStationCode = "";
  config.pathGetStation = "/api/Station/AllPrCodeName";
  config.pathGetProduct = "/api/Product/AllPrCodeName";
  config.pathGetRunningProductOnStation = "/api/Job/GetRunningJob";
  config.pathPostChangedProduct = "/api/Job/StartJob";
}

bool ConfigManager::loadConfig()
{
  preferences.begin(storageKey, true);
  config.wifiSSID = preferences.getString("wifiSSID", "KST-Extend");
  Serial.println("wifiSSID: " + config.wifiSSID);
  config.wifiPassword = preferences.getString("wifiPassword", "KST_123789");
  Serial.println("wifiPassword: " + config.wifiPassword);
  config.httpIP = preferences.getString("httpIP", "192.168.1.149");
  Serial.println("httpIP: " + config.httpIP);
  config.httpPort = preferences.getUShort("httpPort", 5002);
  Serial.println("httpPort: " + String(config.httpPort));
  config.local_ip_str = preferences.getString("local_ip", "192.168.1.189");
  Serial.println("local_ip_str: " + config.local_ip_str);
  config.gateway_str = preferences.getString("gateway", "192.168.1.1");
  Serial.println("gateway_str: " + config.gateway_str);
  config.subnet_str = preferences.getString("subnet", "255.255.255.0");
  Serial.println("subnet_str: " + config.subnet_str);
  config.primaryDNS_str = preferences.getString("priDNS", "8.8.8.8");
  Serial.println("primaryDNS_str: " + config.primaryDNS_str);
  config.secondaryDNS_str = preferences.getString("secDNS", "8.8.4.4");
  Serial.println("secondaryDNS_str: " + config.secondaryDNS_str);

  config.fxStationCode = preferences.getString("fxStationCode", "");
  Serial.println("fxStationCode: " + config.fxStationCode);
  config.pathGetStation = preferences.getString("pathGetStation", "/api/Station/AllPrCodeName");
  Serial.println("pathGetStation: " + config.pathGetStation);
  config.pathGetProduct = preferences.getString("pathGetProduct", "/api/Product/All");
  Serial.println("pathGetProduct: " + config.pathGetProduct);
  config.pathGetRunningProductOnStation = preferences.getString("pathGRPOnS", "/api/RunningProduct");
  Serial.println("pathGetRunningProductOnStation: " + config.pathGetRunningProductOnStation);
  config.pathPostChangedProduct = preferences.getString("pathPChaP", "/api/Job/StartJob");
  Serial.println("pathPostChangedProduct: " + config.pathPostChangedProduct);
  config.isBroadcastWifi = preferences.getBool("isBroadCW", false);
  Serial.println("isBroadcastWifi: " + String(config.isBroadcastWifi ? "true" : "false"));
  config.previouslyConnected = false;
  preferences.end();
  return true;
}

bool ConfigManager::saveConfig()
{
  preferences.begin(storageKey, false);
  preferences.putString("wifiSSID", config.wifiSSID);
  preferences.putString("wifiPassword", config.wifiPassword);
  preferences.putString("httpIP", config.httpIP);
  preferences.putUShort("httpPort", config.httpPort);
  preferences.putString("local_ip", config.local_ip_str);
  preferences.putString("gateway", config.gateway_str);
  preferences.putString("subnet", config.subnet_str);
  preferences.putString("priDNS", config.primaryDNS_str);
  preferences.putString("secDNS", config.secondaryDNS_str);

  preferences.putString("fxStationCode", config.fxStationCode);
  preferences.putString("pathGetStation", config.pathGetStation);
  preferences.putString("pathGetProduct", config.pathGetProduct);
  preferences.putString("pathGRPOnS", config.pathGetRunningProductOnStation);
  preferences.putString("pathPChaP", config.pathPostChangedProduct);
  preferences.end();
  return true;
}

AsyncWebServer serverOta(8080);

// Biến lưu trữ cấu hình hiện tại
String loadedSsid;
String loadedPassword;
String loadedserverURL;

// Thông tin xác thực OTA
const char *ota_username = "viot";
const char *ota_password = "Viot123!";

// HTML cho thông báo thành công và khởi động lại
const char *HTML_CODE_SUC = R"rawliteral(
<!DOCTYPE html>
<html lang="en">
<head>
<meta charset="UTF-8">
<meta name="viewport" content="width=device-width, initial-scale=1.0">
<title>Notification Message</title>
<style>
  .notification {
    padding: 20px;
    background-color: #000000; /* Đen */
    color: white;
    margin-bottom: 15px;
  }

  /* N��t đóng */
  .closebtn {
    margin-left: 15px;
    color: white;
    font-weight: bold;
    float: right;
    font-size: 22px;
    line-height: 20px;
    cursor: pointer;
    transition: 0.3s;
  }

  /* Khi di chuột vào nút đóng */
  .closebtn:hover {
    color: black;
  }
</style>
</head>
<body>

<div class="notification">
    <span class="closebtn" onclick="this.parentElement.style.display='none';">&times;</span> 
    <button onclick="restartESP()">Reset Now</button>
</div>

<p id="statusMessage"></p> <!-- Placeholder cho thông báo khởi động lại -->

<script>
function restartESP() {
    // Hiển thị thông báo "Restarting..."
    document.getElementById("statusMessage").innerText = "Restarting...";
    
    // Gửi yêu cầu đến ESP32 để khởi động lại sau một khoảng thời gian ngắn
    setTimeout(function() {
        var xhr = new XMLHttpRequest();
        xhr.open('GET', '/restart', true);
        xhr.send();
    }, 500); // Điều chỉnh thời gian trễ nếu cần (500ms trong ví dụ này)
}
</script>

</body>
</html>
)rawliteral";

// HTML cho trang cấu hình
const char *HTML_CODE = R"rawliteral(
<!DOCTYPE html>
<html>
    <head>
        <title>Update Config</title>
        <meta name="viewport" content="width=device-width,initial-scale=1">
        <style>
            body {
                font-family: Arial, sans-serif;
                background-color: #f4f4f4;
                margin: 0;
                padding: 0;
                display: flex;
                justify-content: center;
                align-items: center;
            }
            .container {
                background-color: #fff;
                padding: 20px;
                border-radius: 5px;
                box-shadow: 0 2px 5px rgba(0,0,0,0.2);
                display: flex;
                justify-content: center;
                align-items: center;
                height: auto;
                flex-direction: column;
                width: 90%;
                max-width: 500px;
            }
            .password-input {
                position: relative;
                display: flex;
                align-items: center;
                width: 100%;
            }

            .toggle-password {
                margin-left: 10px;
                padding: 10px;
                background-color: #000000; /* Đen */
                color: white;
                border: none;
                border-radius: 4px;
                cursor: pointer;
                text-align: center;
                text-decoration: none;
                display: inline-block;
                font-size: 16px;
                transition: background-color 0.3s;
            }

            .toggle-password:hover {
                background-color: #8B0000; /* Đỏ đậm khi hover */
                color: white;
            }

            h2 {
                color: #333;
                text-align: center;
            }

            form {
                display: flex;
                flex-direction: column;
                width: 100%;
            }
            label {
                margin-top: 10px;
                align-self: flex-start;
            }
            input[type="text"],
            input[type="password"],
            textarea,
            input[type="number"] {
                padding: 10px;
                margin-top: 5px;
                border: 1px solid #ddd;
                border-radius: 4px;
                box-sizing: border-box;
                width: 100%;
            }
            input[type="submit"] {
                background-color: #000000; /* Đen */
                color: white;
                cursor: pointer;
                border: none;
                margin-top: 30px;
                padding: 10px;
                border-radius: 4px;
                font-size: 16px;
            }

            input[type="submit"]:hover {
                background-color: #8B0000; /* Đỏ đậm khi hover */
                color: white;
            }
        </style>
    </head>
    <body>
        <div class="container">
            <form action="/updateConfig" method="post">
                <h2>Update Configuration</h2>

                <label for="macAddress">MAC Address:</label>
                <input type="text" id="macAddress" name="macAddress" value="%MACADDRESS%" readonly />
                
                <label for="firmWareVersion">Firmware Version:</label>
                <input type="text" id="firmWareVersion" name="firmWareVersion" value="%FIRMWAREVERSION%" readonly />

                <label for="localIPstr">Local IP str:</label>
                <input type="text" id="localIPstr" name="localIPstr" value="%LOCALIPSTR%"/>

                <label for="gatewaystr">Gateway str:</label>
                <input type="text" id="gatewaystr" name="gatewaystr" value="%GATEWAYSTR%"/>

                <label for="subnetstr">Subnet str:</label>
                <input type="text" id="subnetstr" name="subnetstr" value="%SUBNETSTR%"/>

                <label for="primaryDNSstr">Primary DNS str:</label>
                <input type="text" id="primaryDNSstr" name="primaryDNSstr" value="%PRIMARYDNSSTR%"/>

                <label for="secondaryDNSstr">Secondary DNS str:</label>
                <input type="text" id="secondaryDNSstr" name="secondaryDNSstr" value="%SECONDARYDNSSTR%"/>

                <label for="ssid">New SSID:</label>
                <input type="text" id="ssid" name="ssid" value="%SSID%" required />

                <label for="password">New Password:</label>
                <div class="password-input">
                    <input type="password" id="password" name="password" value="%PASSWORD%" required />
                    <button type="button" class="toggle-password" onclick="togglePasswordVisibility()">Show</button>
                </div>

                <label for="httpIP">HTTP IP:</label>
                <input type="text" id="httpIP" name="httpIP" value="%HTTPIP%" required />

                <label for="httpPort">HTTP Port:</label>
                <input type="number" id="httpPort" name="httpPort" value="%HTTPPORT%" required />

                 <label for="localIP">New SSID:</label>
                <input type="text" id="ssid" name="ssid" value="%SSID%" required />

                <label for="fxStationCode">FX Station Code:</label>
                <input type="text" id="fxStationCode" name="fxStationCode" value="%FXSTATIONCODE%" />

                <label for="pathGetStation">Path Get Station:</label>
                <input type="text" id="pathGetStation" name="pathGetStation" value="%PATHGETSTATION%" required />

                <label for="pathGetProduct">Path Get Product:</label>
                <input type="text" id="pathGetProduct" name="pathGetProduct" value="%PATHGETPRODUCT%" required />

                <label for="pathGetRunningProductOnStation">Path Get Running Product On Station:</label>
                <input type="text" id="pathGetRunningProductOnStation" name="pathGetRunningProductOnStation" value="%PATHGETRUNNINGPRODUCTONSTATION%" required />

                <label for="pathPostChangedProduct">Path Post Changed Product:</label>
                <input type="text" id="pathPostChangedProduct" name="pathPostChangedProduct" value="%PATHPOSTCHANGEDPRODUCT%" required />

                <input type="submit" value="Save Configuration" />
            </form>
        </div>
    </body>
    <script>
        function togglePasswordVisibility() {
            const passwordInput = document.getElementById("password");
            const toggleButton = document.querySelector(".toggle-password");
            if (passwordInput.type === "password") {
                passwordInput.type = "text";
                toggleButton.textContent = "Hide";
            } else {
                passwordInput.type = "password";
                toggleButton.textContent = "Show";
            }
        }
    </script>
</html>

)rawliteral";

// Hàm xử lý cập nhật cấu hình
void onUpdateRoute(AsyncWebServerRequest *request)
{
  if (!request->authenticate(ota_username, ota_password))
  {
    return request->requestAuthentication();
  }

  if (request->method() == HTTP_POST)
  {
    // Đọc các tham số từ form
    String newSSID = request->getParam("ssid", true)->value();
    String newPassword = request->getParam("password", true)->value();
    String newHttpIP = request->getParam("httpIP", true)->value();
    String newHttpPortStr = request->getParam("httpPort", true)->value();
    uint16_t newHttpPort = newHttpPortStr.toInt();
    String newlocalIP = request->getParam("localIPstr", true)->value();
    String newgatewayIP = request->getParam("gatewaystr", true)->value();
    String newsubnetMask = request->getParam("subnetstr", true)->value();
    String newprimaryDNS = request->getParam("primaryDNSstr", true)->value();
    String newsecondaryDNS = request->getParam("secondaryDNSstr", true)->value();

    String newFxStationCode = request->getParam("fxStationCode", true)->value();
    String newPathGetStation = request->getParam("pathGetStation", true)->value();
    String newPathGetProduct = request->getParam("pathGetProduct", true)->value();
    String newPathGetRunningProductOnStation = request->getParam("pathGetRunningProductOnStation", true)->value();
    String newPathPostChangedProduct = request->getParam("pathPostChangedProduct", true)->value();

    // Cập nhật cấu hình
    configMgr.config.wifiSSID = newSSID;
    configMgr.config.wifiPassword = newPassword;
    configMgr.config.httpIP = newHttpIP;
    configMgr.config.httpPort = newHttpPort;
    configMgr.config.local_ip_str = newlocalIP;
    configMgr.config.gateway_str = newgatewayIP;
    configMgr.config.subnet_str = newsubnetMask;
    configMgr.config.primaryDNS_str = newprimaryDNS;
    configMgr.config.secondaryDNS_str = newsecondaryDNS;

    configMgr.config.fxStationCode = newFxStationCode;
    configMgr.config.pathGetStation = newPathGetStation;
    configMgr.config.pathGetProduct = newPathGetProduct;
    configMgr.config.pathGetRunningProductOnStation = newPathGetRunningProductOnStation;
    configMgr.config.pathPostChangedProduct = newPathPostChangedProduct;

    // Lưu vào Preferences
    if (configMgr.saveConfig())
    {
      // Gửi trang thành công
      request->send(200, "text/html", HTML_CODE_SUC);
      // Khởi động lại ESP để áp dụng cấu hình mới
      delay(1000);
      ESP.restart();
    }
    else
    {
      // Gửi thông báo lỗi
      request->send(500, "text/plain", "Failed to save configuration.");
    }
  }
  else
  {
    request->send(405, "text/plain", "Method Not Allowed");
  }
}

// Hàm xử lý trang cấu hình
void handleConfig(AsyncWebServerRequest *request)
{
  if (!request->authenticate(ota_username, ota_password))
  {
    return request->requestAuthentication();
  }

  String html = HTML_CODE;
  // Thay thế các placeholder bằng cấu hình hiện tại
  html.replace("%MACADDRESS%", WiFi.macAddress());
  html.replace("%FIRMWAREVERSION%", String(FIRMWARE_VERSION));
  html.replace("%LOCALIP%", WiFi.localIP().toString());
  html.replace("%SSID%", configMgr.config.wifiSSID);
  html.replace("%PASSWORD%", configMgr.config.wifiPassword);
  html.replace("%HTTPIP%", configMgr.config.httpIP);
  html.replace("%HTTPPORT%", String(configMgr.config.httpPort));
  html.replace("%LOCALIPSTR%", configMgr.config.local_ip_str);
  html.replace("%GATEWAYSTR%", configMgr.config.gateway_str);
  html.replace("%SUBNETSTR%", configMgr.config.subnet_str);
  html.replace("%PRIMARYDNSSTR%", configMgr.config.primaryDNS_str);
  html.replace("%SECONDARYDNSSTR%", configMgr.config.secondaryDNS_str);
  html.replace("%FXSTATIONCODE%", configMgr.config.fxStationCode);
  html.replace("%PATHGETSTATION%", configMgr.config.pathGetStation);
  html.replace("%PATHGETPRODUCT%", configMgr.config.pathGetProduct);
  html.replace("%PATHGETRUNNINGPRODUCTONSTATION%", configMgr.config.pathGetRunningProductOnStation);
  html.replace("%PATHPOSTCHANGEDPRODUCT%", configMgr.config.pathPostChangedProduct);

  request->send(200, "text/html", html);
}

// Callback cho OTA bắt đầu
void onOTAStart()
{
  Serial.println("OTA Update Start");
}

// Callback cho tiến độ OTA
void onOTAProgress(unsigned int progress, unsigned int total)
{
  Serial.printf("OTA Progress: %u%%\n", (progress / (total / 100)));
}

// Callback cho kết thúc OTA

void onOTAEnd(bool success)
{
  // Log when OTA has finished
  if (success)
  {
    Serial.println("OTA update finished successfully!");
  }
  else
  {
    Serial.println("There was an error during OTA update!");
  }
  // <Add your own code here>
  // Chân GPIO kết nối với chân RESET của ESP32
  const int resetPin = 5; // Ví dụ chân GPIO0
  pinMode(resetPin, OUTPUT);
  digitalWrite(resetPin, LOW);  // Kéo chân RESET xuống
  delay(100);                   // Thời gian kéo chân RESET thấp
  digitalWrite(resetPin, HIGH); // Trở lại trạng thái bình thường
}
// Hàm khởi tạo module OTA và máy chủ web
void initOtaModule()
{
  // Định nghĩa các route
  serverOta.on("/", HTTP_GET, [](AsyncWebServerRequest *request)
               { request->send(200, "text/plain", "Hi! This is ElegantOTA AsyncDemo."); });

  // Route cho cập nhật cấu hình
  // serverOta.on("/updateConfig", HTTP_POST, [](AsyncWebServerRequest *request)
  //              {
  //                  onUpdateRoute(request);
  //              }, NULL, onUpdateRoute);

  // serverOta.on("/updateConfig", HTTP_GET, onUpdateRoute);
  serverOta.on("/updateConfig", HTTP_POST, onUpdateRoute);
  // Route cho trang cấu hình
  serverOta.on("/Board-config", HTTP_GET, handleConfig);

  // Route cho khởi động lại
  serverOta.on("/restart", HTTP_GET, [](AsyncWebServerRequest *request)
               {
        request->send(200, "text/plain", "Restarting...");
        delay(1000);
        ESP.restart(); });

  // Khởi động ElegantOTA
  ElegantOTA.begin(&serverOta); // Bắt đầu ElegantOTA
  ElegantOTA.setAuth(ota_username, ota_password);

  // Thiết lập các callback cho ElegantOTA
  ElegantOTA.onStart(onOTAStart);
  ElegantOTA.onProgress(onOTAProgress);
  ElegantOTA.onEnd(onOTAEnd);

  // Bắt đầu máy chủ
  serverOta.begin();
  Serial.println("HTTP server started ver2.0 updated");
}
