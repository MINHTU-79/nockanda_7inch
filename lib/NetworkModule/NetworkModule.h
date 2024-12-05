
#ifndef NETWORK_MODULE_H
#define NETWORK_MODULE_H

#include <WiFi.h>

inline const char *ssid;
inline const char *password;

inline int *serverPort; // Cổng server tcp/ip
inline WiFiServer server;
inline WiFiClient currentClient;
inline bool isClientConnected = false;

// inline extern const char *ssid = "idiot";
// inline extern const char *password = "01215153846";

// inline extern const char *ssid = "KST Group PLV";
// inline extern const char *password = "KST_123789";

// Biến cấu hình IP
inline String local_ip_str;   // Địa chỉ IP tĩnh
inline String gateway_str;      // Gateway
inline String subnet_str;     // Subnet mask
inline String primaryDNS_str;       // DNS chính (Primary DNS)
inline String secondaryDNS_str;     // DNS phụ (Secondary DNS)

class NetworkModule
{
public:
    void initConnect();
    void disConnect();
    void reConnect();
    bool isConnected();
};

#endif