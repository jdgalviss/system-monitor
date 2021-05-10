#include <unistd.h>
#include <cctype>
#include <sstream>
#include <string>
#include "process.h"
#include "linux_parser.h"

using std::string;
using std::to_string;
using std::vector;

int Process::Pid() { return pid_; }

float Process::CpuUtilization() { return cpu_utilization_; }

string Process::Command() { return LinuxParser::Command(pid_); }

string Process::Ram() { return LinuxParser::Ram(pid_); }

string Process::User() { return LinuxParser::User(pid_); }

long int Process::UpTime() { return LinuxParser::UpTime(pid_); }


void Process::CalculateProcessData(){
    CalculateCpuUtilization();
}


void Process::CalculateCpuUtilization(){
    std::vector<float> cpu_data = LinuxParser::CpuUtilization(pid_);
    float uptime = LinuxParser::UpTime();
    if (cpu_data.size() == 5) {
        float total_time = cpu_data[0] + cpu_data[1] + cpu_data[2] + cpu_data[3];

        float seconds = uptime - cpu_data[4];
        cpu_utilization_ = total_time/seconds;
    }
    else{
        cpu_utilization_ = 0;
    }
}

bool Process::operator<(Process const& a) const { return this->cpu_utilization_ > a.cpu_utilization_; }

// bool Process::operator<(Process const& a) const { return std::stof(this->memory_) > std::stof(a.memory_); }
