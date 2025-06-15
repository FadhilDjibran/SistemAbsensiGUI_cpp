#include "DateUtil.h"
#include <chrono>
#include <iomanip>
#include <sstream>
using namespace std;

string DateUtil::getCurrentDate() { 
    auto now = std::chrono::system_clock::now(); 
    auto in_time_t = std::chrono::system_clock::to_time_t(now); 
    stringstream ss; 
    ss << put_time(localtime(&in_time_t), "%Y-%m-%d"); 
    return ss.str();
}