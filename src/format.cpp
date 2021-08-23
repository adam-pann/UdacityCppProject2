#include <string>

#include "format.h"

using std::string;

// INPUT: Long int measuring seconds
// OUTPUT: HH:MM:SS
// REMOVE: [[maybe_unused]] once you define the function
string Format::ElapsedTime(long seconds) {
    int hour, minute, sec, remainder;
    hour = seconds / (60 * 60);
    remainder = seconds % (60 * 60);
    minute = remainder / 60 ;
    remainder = remainder % 60;
    sec = remainder;

    //create strings from int to ensure two digits are always used
    string hour_s, minute_s, sec_s;
    if (hour < 10) {
        hour_s = "0" + std::to_string(hour);
    }
    else {
        hour_s = std::to_string(hour);
    }

    if (minute < 10) {
        minute_s = "0" + std::to_string(minute);
    }
    else {
        minute_s = std::to_string(minute);
    }

    if (sec < 10) {
        sec_s = "0" + std::to_string(sec);
    }
    else {
        sec_s = std::to_string(sec);
    }

    return hour_s + ":" + minute_s + ":" + sec_s;
}
