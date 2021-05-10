#include "format.h"

#include <iomanip>
#include <sstream>
#include <string>

using std::string;

string Format::ToString(int value) {
  std::ostringstream os;
  os << std::setfill('0') << std::setw(2) << value;
  return os.str();
}

string Format::ElapsedTime(long seconds) {
  int hours = seconds / 3600;
  int minutes = (seconds % 3600) / 60;
  int secs = (seconds % 3600) % 60;
  return ToString(hours) + ":" + ToString(minutes) + ":" + ToString(secs);
}