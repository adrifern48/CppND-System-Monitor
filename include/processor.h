#ifndef PROCESSOR_H
#define PROCESSOR_H

class Processor {
 public:
  float Utilization();  // TODO: See src/processor.cpp

 private:
  unsigned long long int pUser, pNice, pSystem, pIdle = 0;
  unsigned long long int pIoWait, pIrq, pSoftIrq, pSteal, pGuest, pGuestNice = 0;
  float utilization_;
};

#endif