#ifndef INIT_STRUCT_HPP
#define INIT_STRUCT_HPP


#include <Arduino.h>
#include <vector>

struct Station
{
  String StationCode;
  String StationName;
};

struct Product
{
  String ProductCode;
  String ProductName;
};

struct DowntimeItem {
    int atMeter;
    String objectId;
    String id;
    String jobCode;
    String stationCode;
    uint32_t updateTime;
    uint32_t startTime;
    uint32_t endTime;
    String downtimeCode;
    String employeeCode;
    String description;
    int actualStopTime;
    int approvalEmployeeCode;
    int status;
    String temporarySolution;
    int lossTime;
    String rollCode;
    int scheduleDowntime;
    int unScheduleDowntime;
};

struct RunningJob {
    String stationCode;
    String employee;
    String productCode;
    float meterRan;
    int spinCount;
    int totalMeter;
    std::vector<DowntimeItem> downtimeInputs;
    int cavity;
     // Phương thức giải phóng bộ nhớ
    void clear() {
        downtimeInputs.clear();
        downtimeInputs.shrink_to_fit(); // Giải phóng bộ nhớ bị giữ lại
    }
};


// struct DowntimeErrorItem {
//     std::string code;
//     std::string name;
// };

// struct DowntimeErrorItem {
//     std::string downtimeCode;    // "DowntimeCode"
//     std::string description;     // "Description"
//     long createTime;             // "CreateTime"
//     int downtimeType;            // "DowntimeType"
//     std::string downtimeGroup;   // "DowntimeGroup"
//     std::string userCreate;      // "UserCreate"
//     bool isEnable;               // "IsEnable"
// };


struct DowntimeErrorItem {
    std::string downtimeCode;    // "DowntimeCode"
    std::string downtimeName;    // "DowntimeName"
    std::string description;     // "Description"
    long createTime;             // "CreateTime"
    int downtimeType;            // "DowntimeType"
    std::string downtimeGroup;   // "DowntimeGroup"
    std::string userCreate;      // "UserCreate"
    bool isEnable;               // "IsEnable"
    bool selected;               // "Selected"

    
};



#endif // INIT_STRUCT_HPP
