#include <dirent.h>
#include <unistd.h>
#include <sstream>
#include <string>
#include <vector>
#include <unistd.h>

#include "linux_parser.h"
#include "format.h"

using std::stof;
using std::stol;
using std::string;
using std::to_string;
using std::vector;

// Read and return OS "pretty name"
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

// Read and return kernel info
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

// Read and return all PIDs for processes on the system
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

// Read and return the system memory utilization
float LinuxParser::MemoryUtilization() { 
  float utilization = 0.f;
  std::ifstream stream(kProcDirectory + kMeminfoFilename);
  if (stream.is_open()) {
    string key, val, unit;
    float memTotal, memFree = 0.f;
    while (stream >> key >> val >> unit) {
      if ("MemTotal:" == key) memTotal = stof(val);
      else if ("MemFree:" == key) memFree = stof(val);
    }

    utilization = (memTotal - memFree) / memTotal;
  }

  return utilization; 
}

// Read and return system uptime in seconds
long LinuxParser::UpTime() { 
  std::ifstream stream(kProcDirectory + kUptimeFilename);
  if (stream.is_open()) {
    string line, uptime;
    std::getline(stream, line);
    std::istringstream linestream(line);
    linestream >> uptime;
    return stol(uptime);
  }

  return 0; 
}

// Read and return fields used to calculate system CPU Utilization
vector<string> LinuxParser::CpuUtilization() { 
  std::ifstream stream(kProcDirectory + kStatFilename);
  if (stream.is_open()) {
    string line, value;
    vector<string> cpuFields;
    std::getline(stream, line);
    std::istringstream linestream(line);
    linestream.ignore(string("cpu").length());

    for (int i =0; i < 10; ++i) {
      linestream >> value;
      cpuFields.emplace_back(value);
    }

    return cpuFields;
  }
  
  return {}; 
}

// Read and return the total number of processes on the system
int LinuxParser::TotalProcesses() {
  int totalProcesses = 0; 
  std::ifstream stream(kProcDirectory + kStatFilename);
  if (stream.is_open()) {
    string val, nextVal;
    while (stream >> val >> nextVal) {
      if ("procs_running" == nextVal) totalProcesses = stoi(val); // backward lookup
    }
  }
  return totalProcesses;
}

// Read and return the total number of running processes on the system
int LinuxParser::RunningProcesses() { 
  int runningProcesses = 0; 
  std::ifstream stream(kProcDirectory + kStatFilename);
  if (stream.is_open()) {
    string val, nextVal;
    while (stream >> val >> nextVal) {
      if ("procs_blocked" == nextVal) runningProcesses = stoi(val); // backward lookup
    }
  }
  return runningProcesses;
}

// Read and return the command associated with a process
string LinuxParser::Command(int pid) { 
  std::ifstream stream(kProcDirectory + to_string(pid) + kCmdlineFilename);
  string cmd;
  if (stream.is_open()) {
    string cmd;
    std::getline(stream, cmd);
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

//  Read and calculate Cpu Utilization for process given PID
float LinuxParser::Cpu(int pid) {
  std::ifstream stream(kProcDirectory + to_string(pid) + kStatFilename);
  if (stream.is_open()) {
    float uTime, sTime, cuTime, csTime, startTime;
    float totalTime, seconds;
    float uptime = (float)UpTime(); // system uptime in seconds
    string line, value;
    std::getline(stream, line);
    std::istringstream linestream(line);
    for (int i = 1; i < 23; ++i) {
      linestream >> value;
      switch (i)
      {
      case 14:
        // CPU time spent in user code
        uTime = stof(value);
        break;
      case 15:
        // CPU time spent in kernel code
        sTime = stof(value);
        break;
      case 16:
        // Waited-for children's CPU time spent in user code
        cuTime = stof(value);
        break;
      case 17:
        // Wait-for children's CPU time spent in kernel code
        csTime = stof(value);
        break;
      case 22:
        // Time when the process started
        startTime = stof(value);
        break;
      default:
        break;
      }
    }

    totalTime = uTime + sTime;
    totalTime = totalTime + cuTime + csTime;
    seconds = uptime - (startTime / sysconf(_SC_CLK_TCK));

    float cpu = ((totalTime / sysconf(_SC_CLK_TCK)) / seconds);
    
    return cpu;
  }
  
  return 0.f;
}

//  Read and return the user ID associated with a process
string LinuxParser::Uid(int pid) { 
  std::ifstream stream(kProcDirectory + to_string(pid) + kStatusFilename);
  string value;
  if (stream.is_open()) {
    string line;
    string key;
    while (std::getline(stream, line)) {
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
  std::ifstream stream(kPasswordPath);
  string user;
  if (stream.is_open()) {
    string uid = Uid(pid);
    string line;
    string password, potentialUid;
    while (std::getline(stream, line)) {
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
