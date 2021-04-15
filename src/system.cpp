#include <unistd.h>
#include <cstddef>
#include <set>
#include <string>
#include <vector>

#include "linux_parser.h"
#include "process.h"
#include "processor.h"
#include "system.h"

#include <iostream> //used for testing

using std::set;
using std::size_t;
using std::string;
using std::vector;

Processor& System::Cpu() { 
    return cpu_; 
}

// TODO: Return a container composed of the system's processes
vector<Process>& System::Processes() { 
    // Get list of pids from filesytstem
    vector<int> pids(LinuxParser::Pids());
    // for (auto& proc : pids) {
    //     std::cout << proc << "\n";
    // }

    // To ensure no duplicate pids, use a set
    set<int> pids_set;
    for (Process& proc : processes_) {
        pids_set.insert(proc.Pid());
    }

    // Add any new processes since previous iteration
    for (const auto& pid : pids) {
        if (pids_set.find(pid) == pids_set.end())
            processes_.emplace_back(Process(pid));
    }

    // TODO: sort 

    return processes_;
 }

std::string System::Kernel() const { 
    return LinuxParser::Kernel(); 
}

float System::MemoryUtilization() const { 
    return LinuxParser::MemoryUtilization(); 
}

std::string System::OperatingSystem() const { 
    return LinuxParser::OperatingSystem(); 
}

int System::RunningProcesses() const { 
    return LinuxParser::RunningProcesses(); 
}

int System::TotalProcesses() const { 
    return LinuxParser::TotalProcesses(); 
}

long int System::UpTime() const { 
    return LinuxParser::UpTime(); 
}