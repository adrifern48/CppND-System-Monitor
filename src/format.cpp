#include <string>

#include "format.h"

using std::string;

string Format::ElapsedTime(long seconds) { 
    if (seconds < 0) return string();

    int hours = seconds / 3600;
    seconds %= 3600;
    int mins = seconds / 60;
    seconds %= 60;
    int sec = seconds;

    string hh = std::to_string(hours);
    string mm = std::to_string(mins);
    string ss = std::to_string(sec);

    if (hours >= 0 && hours < 10) hh = "0"+hh;
    if (mins >= 0 && mins < 10) mm = "0"+mm;
    if (sec >= 0 && sec < 10) ss = "0"+ss;

    return hh+":"+mm+":"+ss;
}