#ifndef PROCESS_H
#define PROCESS_H

#include <string>
/*
Basic class for Process representation
It contains relevant attributes as shown below
*/
class Process {
 public:
  Process(int i);
  int Pid();                               // TODO: See src/process.cpp
  std::string User();                      // TODO: See src/process.cpp
  void GetUser();
  std::string Command();                   // TODO: See src/process.cpp
  void GetCommand();
  float CpuUtilization() const;                  // TODO: See src/process.cpp
  void SetCpuUtilization();
  void SetRam();
  std::string Ram();                       // TODO: See src/process.cpp
  void SetUpTime();
  long int UpTime();                       // TODO: See src/process.cpp
  bool operator<(Process const& a) const;  // TODO: See src/process.cpp

 private:
  int pid_;
  std::string command_;
  std::string user_;
  std::string memory_usage_;
  long uptime_sec_;
  float utilization_;

};

#endif
