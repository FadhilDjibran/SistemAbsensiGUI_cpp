#include "DateUtil.h"
#include <chrono>
#include <iomanip>
#include <sstream>

std::string DateUtil::getCurrentDate() {
    auto now = std::chrono::system_clock::now();
    auto in_time_t = std::chrono::system_clock::to_time_t(now);
    std::stringstream ss;
    #pragma warning(suppress : 4996) // Non-standard, but works for MinGW/MSVC to suppress localtime warning
    ss << std::put_time(std::localtime(&in_time_t), "%Y-%m-%d");
    return ss.str();
}