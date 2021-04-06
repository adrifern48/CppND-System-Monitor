#ifndef PROCESSOR_H
#define PROCESSOR_H

class Processor {
 public:
  float Utilization();  // TODO: See src/processor.cpp

 private:
  unsigned long long int userTime_, niceTime_, systemTime_, idleTime_;
  unsigned long long int ioWait_, irq_, softIrq_, steal_, guest_, guestNice_;
  float utilization_;
};

#endif