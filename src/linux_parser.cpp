#include <dirent.h>
#include <unistd.h>
#include <sstream>
#include <string>
#include <vector>

#include "linux_parser.h"

//remove later
#include <iostream>

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
  string line;
  string key;
  string value;

  int memtotal= 0;
  int memfree = 0;
  float usedMemory = 0;

  std::ifstream filestream(kProcDirectory + kMeminfoFilename);
  if (filestream.is_open()) {
      while (std::getline(filestream, line)) {
          std::replace(line.begin(), line.end(), ':', ' ');
          std::istringstream linestream(line);
          while (linestream >> key >> value) {
              if (key == "MemTotal") {
                  memtotal = std::stoi(value);
              }
              else if (key == "MemFree"){
                  memfree = std::stoi(value);
              }
          }
      }
  }
  usedMemory = memtotal - memfree;
  if (memtotal != 0){ // safety check if file isn't opened
      return usedMemory / memtotal;
  }
  else {
      return 0;
  }
}

long LinuxParser::UpTime() { 
    string uptime_str;
    string line;
    long uptime = 0;
    std::ifstream stream(kProcDirectory + kUptimeFilename);
    if (stream.is_open()) {
        std::getline(stream, line);
        std::istringstream linestream(line);
        linestream >> uptime_str;
    }
    uptime = std::stoll(uptime_str);
    return uptime;
}

// Read and return the number of jiffies for the system
long LinuxParser::Jiffies() { 
    return LinuxParser::ActiveJiffies() + LinuxParser::IdleJiffies();
}

// Read and return the number of active jiffies for a PID
long LinuxParser::ActiveJiffies(int pid) { 
    // From https://man7.org/linux/man-pages/man5/proc.5.html
    // We need to find utime (14), stime (15), cutime (16), cstime (17) from /proc/[pid]/stat
    string line;
    string element;
    vector<string> value;

    long count = 0;
    std::ifstream filestream(kProcDirectory + std::to_string(pid) + kStatFilename);
    if (filestream.is_open()){
       while (std::getline(filestream, line)){
            std::istringstream linestream(line);
            while (linestream >> element) {
                value.push_back(element);
            }
       }
    }
    if (value.size() >= 16){
        count += std::stol(value[13]); //utime 
        count += std::stol(value[14]); //stime 
        count += std::stol(value[15]); //cutime 
        count += std::stol(value[16]); //cstime 
    }

    return count / sysconf(_SC_CLK_TCK);
}

// Read and return the number of active jiffies for the system
long LinuxParser::ActiveJiffies() { 
    long count = 0;
    vector<string> jiffies;
    jiffies = LinuxParser::CpuUtilization();

    count += std::stol(jiffies[kUser_]);
    count += std::stol(jiffies[kNice_]);
    count += std::stol(jiffies[kSystem_]);
    count += std::stol(jiffies[kIRQ_]);
    count += std::stol(jiffies[kSoftIRQ_]);
    count += std::stol(jiffies[kSteal_]);

    return count / sysconf(_SC_CLK_TCK);
}

// Read and return the number of idle jiffies for the system
long LinuxParser::IdleJiffies() { 
    long count = 0;
    vector<string> jiffies;
    jiffies = LinuxParser::CpuUtilization();

    count += std::stol(jiffies[3]);  // idle time
    count += std::stol(jiffies[4]);  // iowait time

    return count / sysconf(_SC_CLK_TCK);
}

// Read and return CPU utilization
vector<string> LinuxParser::CpuUtilization() { 
    string line;
    string cpu;
    string element;
    vector<string> jiffies = {};

    std::ifstream filestream(kProcDirectory + kStatFilename);
    if (filestream.is_open()){
        std::getline(filestream, line);
        std::istringstream linestream(line);
        linestream >> cpu;
        while (linestream >> element) {
          jiffies.push_back(element);
        }
    }

    return jiffies;
}

int LinuxParser::TotalProcesses() { 
    string line;
    string key;
    string value;
    std::ifstream filestream(kProcDirectory + kStatFilename);
    if (filestream.is_open()) {
        while (std::getline(filestream, line)) {
            std::istringstream linestream(line);
            while (linestream >> key >> value) {
                if (key == "processes"){
                    return std::stoi(value);
                }
            }
        }
    }
    return 0;
}

int LinuxParser::RunningProcesses() {
    string line;
    string key;
    string value;
    std::ifstream filestream(kProcDirectory + kStatFilename);
    if (filestream.is_open()) {
        while (std::getline(filestream, line)) {
            std::istringstream linestream(line);
            while (linestream >> key >> value) {
                if (key == "procs_running"){
                    return std::stoi(value);
                }
            }
        }
    }
    return 0;
}

string LinuxParser::Command(int pid){ 
    string line;
    std::ifstream filestream(kProcDirectory + std::to_string(pid) + kCmdlineFilename);
    if (filestream.is_open()) {
        std::getline(filestream, line);
        return line;
    }
    return string(); 
}

// Read and return the memory used by a process
string LinuxParser::Ram(int pid) { 
    string line;
    string key;
    string value = "";
    long memory;

    std::ifstream filestream(kProcDirectory + std::to_string(pid) + kStatusFilename);
    if (filestream.is_open()){
        while (std::getline(filestream, line)){
            std::istringstream linestream(line);
            linestream >> key;
            if (key == "VmSize:"){ // Found the line for memory size
                linestream >> memory;  // Value is in kb
                return std::to_string(memory / 1000); // convert to megabytes & return
            }
        }
    }
    return string();
}

// Read and return the user ID associated with a process
string LinuxParser::Uid(int pid) { 
    string line;
    string key;
    string value = "";

    std::ifstream filestream(kProcDirectory + std::to_string(pid) + kStatusFilename);
    if (filestream.is_open()){
        while (std::getline(filestream, line)){
            std::istringstream linestream(line);
            linestream >> key;
            if (key == "Uid:"){
                linestream >> value;
                return value;
            }
        }
    }


    return value; }

// Read and return the user associated with a process
string LinuxParser::User(int pid) { 
    string Uid = LinuxParser::Uid(pid);
    string line;
    string username, passwd, uid;
    std::ifstream filestream(kPasswordPath);
    if (filestream.is_open()){
        while (std::getline(filestream, line)) {
            std::replace(line.begin(), line.end(), ':', ' ');
            std::istringstream linestream(line);
            linestream >> username >> passwd >> uid;
            if (uid == Uid) {
                return username;
            }
        }
    }
    return string();
}

long LinuxParser::UpTime(int pid) { 
    // From https://man7.org/linux/man-pages/man5/proc.5.html
    // We need to find utime (14), stime (15), cutime (16), cstime (17) from /proc/[pid]/stat
    string line;
    string element;
    vector<string> value;

    long uptime = 0;
    std::ifstream filestream(kProcDirectory + std::to_string(pid) + kStatFilename);
    if (filestream.is_open()){
       while (std::getline(filestream, line)){
            std::istringstream linestream(line);
            while (linestream >> element) {
                value.push_back(element);
            }
       }
    }
    uptime = std::stol(value[21]);

    return uptime / sysconf(_SC_CLK_TCK); //convert uptime from ticks to seconds
}


long LinuxParser::StartTime(int pid){
    string line;
    string element;
    vector<string> value;

    long starttime_ticks = 0;
    std::ifstream filestream(kProcDirectory + std::to_string(pid) + kStatFilename);
    if (filestream.is_open()){
       while (std::getline(filestream, line)){
            std::istringstream linestream(line);
            while (linestream >> element) {
                value.push_back(element);
            }
       }
    }
    /* if (value.size() >= 22){ */
    starttime_ticks = std::stol(value[21]); //starttime 
    /* } */

    return starttime_ticks / sysconf(_SC_CLK_TCK);
}

