#include "processor.h"
#include "linux_parser.h"

float Processor::Utilization() { 
    long int total_jiffies = LinuxParser::Jiffies();
    long int active_jiffies = LinuxParser::ActiveJiffies();

    long int diff_total_jiffies = total_jiffies - prev_tot_jiffies_;
    long int diff_active_jiffies = active_jiffies - prev_active_jiffies_;
    prev_tot_jiffies_ = total_jiffies;
    prev_active_jiffies_     = active_jiffies;

    return float(diff_active_jiffies)/float(diff_total_jiffies);    
    }