// help.h
#ifndef HELP_H
#define HELP_H

#include <ctime>
#include <string>
#include <Arduino.h>

// Hàm chuyển đổi thời gian từ epoch sang định dạng thời gian
// std::string convertEpochToTimeFormat(long long epochTime);

String convertEpochToTimeFormat(long long epochTime);


// Hàm chuyển đổi số giây thành chuỗi có định dạng (giờ, phút, giây, ngày, năm)
String convertSecondsToTimeFormat(int lossTime);

String URLEncode(const char *msg);

#endif // HELP_H
