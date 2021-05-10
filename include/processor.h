#ifndef PROCESSOR_H
#define PROCESSOR_H

class Processor {
 public:
  float Utilization();

 private:
    long int prev_tot_jiffies_;
    long int prev_active_jiffies_;
};

#endif