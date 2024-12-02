// help.cpp
#include "Help.hpp"
#include <ctime>
#include <sstream>
#include <iomanip>

//ver này sai múi giờ
// std::string convertEpochToTimeFormat(long long epochTime)
// {
//     // Chuyển đổi epochTime sang time_t
//     time_t rawTime = static_cast<time_t>(epochTime);

//     // Lấy thông tin thời gian từ rawTime
//     struct tm *timeInfo = localtime(&rawTime);

//     // Sử dụng stringstream để định dạng thời gian
//     std::ostringstream formattedTime;
//     // formattedTime << std::put_time(timeInfo, "%Y-%m-%d %H:%M:%S");
//     formattedTime << std::put_time(timeInfo, "%H:%M");

//     // Trả về thời gian đã định dạng
//     return formattedTime.str();
// }

//ver này đúng mui gio tphcm, nhưng không tra ve type String
// std::string convertEpochToTimeFormat(long long epochTime)
// {
//     // Chuyển đổi epochTime sang time_t
//     time_t rawTime = static_cast<time_t>(epochTime);

//     // Lấy thông tin thời gian UTC từ rawTime
//     struct tm *utcTimeInfo = gmtime(&rawTime);

//     // Thêm 7 giờ để chuyển sang GMT+07:00
//     utcTimeInfo->tm_hour += 7;

//     // Điều chỉnh ngày, giờ nếu vượt quá giới hạn
//     mktime(utcTimeInfo);

//     // Sử dụng stringstream để định dạng thời gian
//     std::ostringstream formattedTime;
//     formattedTime << std::put_time(utcTimeInfo, "%I:%M %p"); // Định dạng 12 giờ AM/PM

//     // Trả về thời gian đã định dạng
//     return formattedTime.str();
// }

// ver này trả về  type String

String convertEpochToTimeFormat(long long epochTime)
{
    // Chuyển đổi epochTime sang time_t
    time_t rawTime = static_cast<time_t>(epochTime);

    // Lấy thông tin thời gian UTC từ rawTime
    struct tm *utcTimeInfo = gmtime(&rawTime);

    // Thêm 7 giờ để chuyển sang GMT+07:00
    utcTimeInfo->tm_hour += 7;

    // Điều chỉnh ngày, giờ nếu vượt quá giới hạn
    mktime(utcTimeInfo);

    // Tạo chuỗi định dạng thời gian
    char buffer[16]; // Đủ để chứa định dạng giờ:phút AM/PM
    strftime(buffer, sizeof(buffer), "%I:%M %p", utcTimeInfo);

    // Trả về thời gian đã định dạng dưới dạng String
    return String(buffer);
}



// std::string convertSecondsToTimeFormat(int lossTime)
// {
//     // Xử lý số giây
//     int years = lossTime / (365 * 24 * 3600);
//     lossTime %= (365 * 24 * 3600);

//     int days = lossTime / (24 * 3600);
//     lossTime %= (24 * 3600);

//     int hours = lossTime / 3600;
//     lossTime %= 3600;

//     int minutes = lossTime / 60;
//     lossTime %= 60;

//     int seconds = lossTime;

//     // Tạo chuỗi kết quả
//     std::ostringstream result;

//     // Nếu có năm, hiển thị năm
//     if (years > 0)
//     {
//         result << years << ":";
//     }

//     // Nếu có ngày, hiển thị ngày
//     if (days > 0)
//     {
//         result << days << ":";
//     }

//     // Hiển thị giờ, phút, giây
//     result << std::setw(2) << std::setfill('0') << hours << ":"
//            << std::setw(2) << std::setfill('0') << minutes << ":"
//            << std::setw(2) << std::setfill('0') << seconds;

//     return result.str();
// }

String convertSecondsToTimeFormat(int lossTime) {
    // Xử lý số giây
    int years = lossTime / (365 * 24 * 3600);
    lossTime %= (365 * 24 * 3600);

    int days = lossTime / (24 * 3600);
    lossTime %= (24 * 3600);

    int hours = lossTime / 3600;
    lossTime %= 3600;

    int minutes = lossTime / 60;
    lossTime %= 60;

    int seconds = lossTime;

    // Tạo chuỗi kết quả
    String result;

    // Hiển thị các đơn vị nếu chúng có giá trị lớn hơn 0
    if (years > 0) {
        result += String(years) + ":";
    }
    if (days > 0 || years > 0) {
        result += String(days) + ":";
    }
    if (hours > 0 || days > 0 || years > 0) {
        result += String(hours) + ":";
    }
    if (minutes > 0 || hours > 0 || days > 0 || years > 0) {
        result += String(minutes) + ":";
    }
    result += String(seconds);

    return result;
}


// Hàm mã hóa URL
String URLEncode(const char *msg)
{
  const char *hex = "0123456789ABCDEF";
  String encodedMsg = "";
  while (*msg != '\0')
  {
    if (isalnum(*msg) || *msg == '-' || *msg == '_' || *msg == '.' || *msg == '~')
    {
      encodedMsg += *msg;
    }
    else
    {
      encodedMsg += '%';
      encodedMsg += hex[*msg >> 4];
      encodedMsg += hex[*msg & 15];
    }
    msg++;
  }
  return encodedMsg;
}