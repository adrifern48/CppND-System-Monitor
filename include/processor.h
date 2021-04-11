#ifndef PROCESSOR_H
#define PROCESSOR_H

class Processor {
 public:
  float Utilization();

 private:
  float prevIdle_, prevTotal_ = 0.f;
};

#endif