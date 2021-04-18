#ifndef PROCESS_H
#define PROCESS_H

#include <string>
/*
Basic class for Process representation
It contains relevant attributes as shown below
*/
class Process {
 public:
  Process(const int pid);
  int Pid() const;
  std::string User();
  std::string Command();
  float CpuUtilization();                  // TODO: See src/process.cpp
  std::string Ram();
  long int UpTime();
  bool operator<(Process const& a) const;  // TODO: See src/process.cpp

  // TODO: Declare any necessary private members
 private:
  int pid_;
  long int uptime_;
  float cpu_;
  std::string user_;
  std::string ram_;
  std::string cmd_;
};

#endif