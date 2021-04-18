#include <dirent.h>
#include <unistd.h>
#include <sstream>
#include <string>
#include <vector>
#include <unistd.h>

#include "linux_parser.h"
#include "format.h"

using std::stof;
using std::string;
using std::to_string;
using std::vector;

// DONE: An example of how to read data from the filesystem
string LinuxParser::OperatingSystem() {
  string line;
  string key;
  string value;
  std::ifstream filestream(kOSPath);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::replace(line.begin(), line.end(), ' ', '_');
      std::replace(line.begin(), line.end(), '=', ' ');
      std::replace(line.begin(), line.end(), '"', ' ');
      std::istringstream linestream(line);
      while (linestream >> key >> value) {
        if (key == "PRETTY_NAME") {
          std::replace(value.begin(), value.end(), '_', ' ');
          return value;
        }
      }
    }
  }
  return value;
}

// DONE: An example of how to read data from the filesystem
string LinuxParser::Kernel() {
  string os, kernel, version;
  string line;
  std::ifstream stream(kProcDirectory + kVersionFilename);
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    linestream >> os >> version >> kernel;
  }
  return kernel;
}

// BONUS: Update this to use std::filesystem
vector<int> LinuxParser::Pids() {
  vector<int> pids;
  DIR* directory = opendir(kProcDirectory.c_str());
  struct dirent* file;
  while ((file = readdir(directory)) != nullptr) {
    // Is this a directory?
    if (file->d_type == DT_DIR) {
      // Is every character of the name a digit?
      string filename(file->d_name);
      if (std::all_of(filename.begin(), filename.end(), isdigit)) {
        int pid = stoi(filename);
        pids.push_back(pid);
      }
    }
  }
  closedir(directory);
  return pids;
}

float LinuxParser::MemoryUtilization() { 
  float utilization = 0.f;
  std::ifstream stream(kProcDirectory + kMeminfoFilename);
  if (stream.is_open()) {
    string key, val, unit;
    float memTotal, memFree = 0.f;
    while (stream >> key >> val >> unit) {
      if ("MemTotal:" == key) memTotal = std::stof(val);
      else if ("MemFree:" == key) memFree = std::stof(val);
    }

    utilization = (memTotal - memFree) / memTotal;
  }

  return utilization; 
}

long LinuxParser::UpTime() { 
  std::ifstream stream(kProcDirectory + kUptimeFilename);
  if (stream.is_open()) {
    string line, uptime;
    std::getline(stream, line);
    std::istringstream linestream(line);
    linestream >> uptime;
    return std::stol(uptime);
  }

  return 0; 
}

// TODO: Read and return the number of jiffies for the system
long LinuxParser::Jiffies() { return 0; }

// TODO: Read and return the number of active jiffies for a PID
// REMOVE: [[maybe_unused]] once you define the function
long LinuxParser::ActiveJiffies(int pid[[maybe_unused]]) { return 0; }

// TODO: Read and return the number of active jiffies for the system
long LinuxParser::ActiveJiffies() { return 0; }

// TODO: Read and return the number of idle jiffies for the system
long LinuxParser::IdleJiffies() { return 0; }

vector<string> LinuxParser::CpuUtilization() { 
  std::ifstream stream(kProcDirectory + kStatFilename);
  if (stream.is_open()) {
    string user, nice, system, idle, iowait;
    string irq, softirq, steal, guest, guestNice;
    string line;
    vector<string> cpuFields;
    std::getline(stream, line);
    std::istringstream linestream(line);
    linestream.ignore(string("cpu").length());
    linestream >> user >> nice >> system >> idle >> iowait 
      >> irq >> softirq >> steal >> guest >> guestNice;
    cpuFields.push_back(user);
    cpuFields.push_back(nice);
    cpuFields.push_back(system);
    cpuFields.push_back(idle);
    cpuFields.push_back(iowait);
    cpuFields.push_back(irq);
    cpuFields.push_back(softirq);
    cpuFields.push_back(steal);
    cpuFields.push_back(guest);
    cpuFields.push_back(guestNice);

    return cpuFields;
  }
  
  return {}; 
}

int LinuxParser::TotalProcesses() {
  int totalProcesses = 0; 
  std::ifstream stream(kProcDirectory + kStatFilename);
  if (stream.is_open()) {
    string val, nextVal;
    while (stream >> val >> nextVal) {
      if ("procs_running" == nextVal) totalProcesses = std::stoi(val); // backward lookup
    }
  }
  return totalProcesses;
}

int LinuxParser::RunningProcesses() { 
  int runningProcesses = 0; 
  std::ifstream stream(kProcDirectory + kStatFilename);
  if (stream.is_open()) {
    string val, nextVal;
    while (stream >> val >> nextVal) {
      if ("procs_blocked" == nextVal) runningProcesses = std::stoi(val); // backward lookup
    }
  }
  return runningProcesses;
}

// Read and return the command associated with a process
string LinuxParser::Command(int pid) { 
  std::ifstream filestream(kProcDirectory + to_string(pid) + kCmdlineFilename);
  string cmd;
  if (filestream.is_open()) {
    string cmd;
    std::getline(filestream, cmd);
    return cmd;
  }

  return string();
}

// Read and return the memory used by a process
string LinuxParser::Ram(int pid) {
  string key, value, line;
  std::ifstream stream(kProcDirectory + to_string(pid) + kStatusFilename);
  if (stream.is_open()) {
    while (std::getline(stream, line)) {
      std::istringstream linestream(line);
      while (linestream >> key >> value) {
        if (key == "VmSize:")
          return to_string(stol(value) / 1024); // convert KB to MB
      }
    }
  }
  
  return string(); 

}

// TODO: figure out why processes are all displaying 0 cpu
float LinuxParser::Cpu(int pid) {
  std::ifstream stream(kProcDirectory + to_string(pid) + kStatFilename);
  if (stream.is_open()) {
    long uTime, sTime, cuTime, csTime, startTime;
    string line, value;
    std::getline(stream, line);
    std::istringstream linestream(line);
    for (int i = 1; i < 23; ++i) {
      linestream >> value;
      switch (i)
      {
      case 14:
        uTime = stol(value);
        break;
      case 15:
        sTime = stol(value);
        break;
      case 16:
        cuTime = stol(value);
        break;
      case 17:
        csTime = stol(value);
        break;
      case 22:
        startTime = stol(value);
        break;
      default:
        break;
      }
    }
    auto sysUptime = UpTime();
    auto totalTime = uTime + sTime;
    totalTime = totalTime + cuTime + csTime;
    auto seconds = sysUptime - (startTime / sysconf(_SC_CLK_TCK));
    float cpu = 100 * ((totalTime / sysconf(_SC_CLK_TCK)) / seconds);
    
    return cpu;
  }
  
  return 0.f;
}

//  Read and return the user ID associated with a process
string LinuxParser::Uid(int pid) { 
  std::ifstream filestream(kProcDirectory + to_string(pid) + kStatusFilename);
  string value;
  if (filestream.is_open()) {
    string line;
    string key;
    while (std::getline(filestream, line)) {
      std::istringstream linestream(line);
      while (linestream >> key >> value) {
        if (key == "Uid:") 
          return value; // capture the first integer on the "Uid:" line
      }
    }
  }
  
  return value; 
}

// Read and return the user associated with a process
string LinuxParser::User(int pid) { 
  std::ifstream filestream(kPasswordPath);
  string user;
  if (filestream.is_open()) {
    string uid = Uid(pid);
    string line;
    string password, potentialUid;
    while (std::getline(filestream, line)) {
      std::replace(line.begin(), line.end(), ':', ' ');
      std::istringstream linestream(line);
      while (linestream >> user >> password >> potentialUid) {
        if (uid == potentialUid) return user;  
      }
    }
  }
  
  return user;
}

// Read and return the uptime of a process
long LinuxParser::UpTime(int pid) {
  std::ifstream stream(kProcDirectory + to_string(pid) + kStatFilename);
  if (stream.is_open()) {
    string line, value;
    std::getline(stream, line);
    std::istringstream linestream(line);
    for (int i = 0; i < 22; ++i)
      linestream >> value;
    linestream >> value;
    long seconds = stol(value) / sysconf(_SC_CLK_TCK);
    return stol(Format::ElapsedTime(seconds));
  }
  
  return 0; 
}
