#include <unistd.h>
#include <cctype>
#include <sstream>
#include <string>
#include <vector>

#include "linux_parser.h"
#include "process.h"

using std::string;
using std::to_string;
using std::vector;

Process::Process(int i) : pid_(i) {
    GetCommand();
    GetUser();
    SetRam();
    SetUpTime();
    SetCpuUtilization();
}

int Process::Pid() { return pid_; }

void Process::SetCpuUtilization() {
    long uptime = LinuxParser::UpTime();
    float seconds = uptime - LinuxParser::StartTime(Pid());
    float process_active = LinuxParser::ActiveJiffies(Pid());

    if (seconds != 0){
        utilization_ = (process_active / seconds);
    }
    else {
        utilization_ = 0;
    }
}

float Process::CpuUtilization() const { 
    return utilization_;
}

string Process::Command() { return command_; }

void Process::GetCommand() {
   command_ = LinuxParser::Command(Pid()); 
}

void Process::SetRam() {
    memory_usage_ = LinuxParser::Ram(Pid());
}

string Process::Ram() { return memory_usage_; }

string Process::User() { return user_; }

void Process::GetUser(){
    user_ = LinuxParser::User(Pid());
}

void Process::SetUpTime(){
    uptime_sec_ = LinuxParser::UpTime() - LinuxParser::UpTime(Pid());
}

long int Process::UpTime() {
    return uptime_sec_; 
}

bool Process::operator<(Process const& a) const { 
    return CpuUtilization() > a.CpuUtilization(); // swap to > to makee sort on greatest first.
}
