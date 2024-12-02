#pragma once
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
