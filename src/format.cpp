#include "format.h"

#include <iomanip>
#include <sstream>
#include <string>

using std::string;

string Format::NumToString(int number) {
  std::ostringstream o_stream;
  o_stream << std::setfill('0') << std::setw(2) << number;
  return o_stream.str();
}

string Format::ElapsedTime(long seconds) {
  int hours = seconds / 3600;
  int minutes = (seconds % 3600) / 60;
  int secs = (seconds % 3600) % 60;
  return NumToString(hours) + ":" + NumToString(minutes) + ":" + NumToString(secs);
}