#include <unistd.h>
#include <cstddef>
#include <vector>

#include "linux_parser.h"
#include "process.h"
#include "processor.h"
#include "system.h"

using std::vector;

Processor& System::Cpu() { return cpu_; }

vector<Process>& System::Processes() { 
    // Get list of pids from filesytstem
    vector<int> pids(LinuxParser::Pids());
    processes_.clear(); // clear all previous processes
    for (const auto& pid : pids) 
        processes_.emplace_back(Process(pid));

    // Sort by CPU utilization in descending order
    std::sort(processes_.begin(), processes_.end());

    return processes_;
 }

std::string System::Kernel() const { return LinuxParser::Kernel(); }

float System::MemoryUtilization() const { return LinuxParser::MemoryUtilization(); }

std::string System::OperatingSystem() const { return LinuxParser::OperatingSystem(); }

int System::RunningProcesses() const { return LinuxParser::RunningProcesses(); }

int System::TotalProcesses() const { return LinuxParser::TotalProcesses(); }

long int System::UpTime() const { return LinuxParser::UpTime(); }