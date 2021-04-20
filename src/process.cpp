#include <unistd.h>
#include <cctype>
#include <string>

#include "process.h"
#include "linux_parser.h"

using std::string;

Process::Process(const int pid) : pid_(pid) { }

int Process::Pid() const { return pid_; }

float Process::CpuUtilization() const { return LinuxParser::Cpu(pid_); }

string Process::Command() const { return LinuxParser::Command(pid_); }

string Process::Ram() const { return LinuxParser::Ram(pid_); }

string Process::User() const { return LinuxParser::User(pid_); }

long int Process::UpTime() const { return LinuxParser::UpTime(pid_); }

bool Process::operator<(Process const& a) const { 
    return this->CpuUtilization() > a.CpuUtilization(); 
}