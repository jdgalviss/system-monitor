#include "linux_parser.h"

#include <dirent.h>
#include <unistd.h>

#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>
using std::stof;
using std::string;
using std::to_string;
using std::vector;

string LinuxParser::OperatingSystem() {
  string line;
  string key;
  string value;
  std::ifstream filestream(kOSPath);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::replace(line.begin(), line.end(), ' ', '_');
      std::replace(line.begin(), line.end(), '=', ' ');
      std::replace(line.begin(), line.end(), '"', ' ');
      std::istringstream linestream(line);
      while (linestream >> key >> value) {
        if (key == "PRETTY_NAME") {
          std::replace(value.begin(), value.end(), '_', ' ');
          return value;
        }
      }
    }
  }
  return value;
}

string LinuxParser::Kernel() {
  string os, kernel, version;
  string line;
  std::ifstream stream(kProcDirectory + kVersionFilename);
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    linestream >> os >> version >> kernel;
  }
  return kernel;
}

// BONUS: Update this to use std::filesystem
vector<int> LinuxParser::Pids() {
  vector<int> pids;
  DIR* directory = opendir(kProcDirectory.c_str());
  struct dirent* file;
  while ((file = readdir(directory)) != nullptr) {
    // Is this a directory?
    if (file->d_type == DT_DIR) {
      // Is every character of the name a digit?
      string filename(file->d_name);
      if (std::all_of(filename.begin(), filename.end(), isdigit)) {
        int pid = stoi(filename);
        pids.push_back(pid);
      }
    }
  }
  closedir(directory);
  return pids;
}

float LinuxParser::MemoryUtilization() {
  string memory_field, memory_value;
  float mem[6];
  string line;
  std::ifstream stream(kProcDirectory + kMeminfoFilename);
  if (stream.is_open()) {
    for (int i = 0; i < 6; i++) {
      std::getline(stream, line);
      std::istringstream linestream(line);
      linestream >> memory_field >> memory_value;
      mem[i] = std::stof(memory_value);
    }
  }
  float total_used_memory = mem[0] - mem[1];
  float non_cache_memory = total_used_memory - (mem[3] + mem[4]);
  return (non_cache_memory) / mem[0];
}

long LinuxParser::UpTime() {
  string line, uptime;
  std::ifstream stream(kProcDirectory + kUptimeFilename);
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    linestream >> uptime;
  }
  return std::stol(uptime);
}

long LinuxParser::Jiffies() { return IdleJiffies() + ActiveJiffies(); }

long LinuxParser::ActiveJiffies(int pid [[maybe_unused]]) { return 0; }

long LinuxParser::ActiveJiffies() {
  std::vector<string> cpu_data = CpuData();
  long int idle = std::stol(cpu_data[kUser_]) + std::stol(cpu_data[kSystem_]) +
                  std::stol(cpu_data[kNice_]) + std::stol(cpu_data[kIRQ_]) +
                  std::stol(cpu_data[kSoftIRQ_]) + std::stol(cpu_data[kSteal_]);
  return idle;
}

long LinuxParser::IdleJiffies() {
  std::vector<string> cpu_data = CpuData();
  long int idle = std::stol(cpu_data[CPUStates::kIdle_]) +
                  std::stol(cpu_data[CPUStates::kIOwait_]);
  return idle;
}

vector<string> LinuxParser::CpuData() {
  std::ifstream stream(kProcDirectory + kStatFilename);
  std::vector<string> cpu_data;
  string line, d;

  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    linestream >> d;
    for (int i = 0; i < 10; ++i) {
      linestream >> d;
      cpu_data.push_back(d);
    }
  }
  return cpu_data;
}

int LinuxParser::TotalProcesses() {
  std::vector<string> lines;
  std::ifstream stream(kProcDirectory + kStatFilename);
  string line, head, num_procs;
  if (stream.is_open()) {
    while (std::getline(stream, line)) {
      lines.push_back(line);
    }
  }
  line = lines.end()[-4];
  std::istringstream linestream(line);
  linestream >> head >> num_procs;
  return std::stoi(num_procs);
}

int LinuxParser::RunningProcesses() {
  std::vector<string> lines;
  std::ifstream stream(kProcDirectory + kStatFilename);
  string line, head, num_procs;
  if (stream.is_open()) {
    while (std::getline(stream, line)) {
      lines.push_back(line);
    }
  }
  line = lines.end()[-3];
  std::istringstream linestream(line);
  linestream >> head >> num_procs;
  return std::stoi(num_procs);
}

string LinuxParser::Command(int pid) {
  std::ifstream stream(kProcDirectory + std::to_string(pid) + kCmdlineFilename);
  string line, cmd;
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    linestream >> cmd;
  }
  return cmd;
}

vector<float> LinuxParser::CpuUtilization(int pid) {
  std::ifstream stream(kProcDirectory + std::to_string(pid) + kStatFilename);
  string line, d;
  vector<float> data;
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    for (int i = 0; i < 24; ++i) {
      linestream >> d;
      if (i == kUtime_ || i == kStime_ || i == kCUtime_ || i == kCStime_ ||
          i == kStarttime_) {
        float time = std::stof(d) / sysconf(_SC_CLK_TCK);
        data.push_back(time);
      }
    }
  }
  return data;
}

string LinuxParser::Ram(int pid) {
  std::ifstream stream(kProcDirectory + std::to_string(pid) + kStatusFilename);
  string line, data, memory;
  if (stream.is_open()) {
    for (int i = 0; i <= 17; i++) {
      std::getline(stream, line);
    }
    std::istringstream linestream(line);
    linestream >> data >> memory;
    double memory_f = std::stod(memory);
    memory_f = memory_f / 1e6;
    std::stringstream ss;
    ss << std::fixed << std::setprecision(1) << memory_f;
    std::string s = ss.str();
    return s;
  }
  return string();
}

string LinuxParser::Uid(int pid) {
  std::ifstream stream(kProcDirectory + std::to_string(pid) + kStatusFilename);
  string line, data, uid;
  if (stream.is_open()) {
    for (int i = 0; i <= 8; i++) {
      std::getline(stream, line);
    }
    std::istringstream linestream(line);
    linestream >> data >> uid;
    return uid;
  }
  return string();
}

string LinuxParser::User(int pid) {
  string uid = Uid(pid);
  std::ifstream stream(kPasswordPath);
  string line, user, x, _uid;
  if (stream.is_open()) {
    while (std::getline(stream, line)) {
      std::replace(line.begin(), line.end(), ':', ' ');
      std::istringstream linestream(line);
      linestream >> user >> x >> _uid;
      if (_uid == uid) return user;
    }
  }
  return string();
}

long LinuxParser::UpTime(int pid) {
  std::ifstream stream(kProcDirectory + std::to_string(pid) + kStatFilename);
  string line, d;
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    // linestream >> d;
    for (int i = 0; i <= 21; ++i) {
      linestream >> d;
    }
    return UpTime() - std::stol(d) / sysconf(_SC_CLK_TCK);
  }
  return 0;
}
