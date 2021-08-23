#include "processor.h"
#include "linux_parser.h"

float Processor::Utilization() { 
    long jiffies = LinuxParser::Jiffies();
    long active_jiffies = LinuxParser::ActiveJiffies();
    long idle_jiffies = LinuxParser::IdleJiffies();
    long jiffies_delta = jiffies - prev_jiffies;
    long idle_jiffies_delta = idle_jiffies - prev_idle_jiffies;
    float utilization = float(jiffies_delta - idle_jiffies_delta) / jiffies_delta;

    Update(jiffies, active_jiffies, idle_jiffies);
    return utilization;   
}

void Processor::Update(long jiffies, long active_jiffies, long idle_jiffies) {
    prev_jiffies = jiffies;
    prev_active_jiffies = active_jiffies;
    prev_idle_jiffies = idle_jiffies;
}

Processor::Processor() : prev_jiffies(0), prev_active_jiffies(0), prev_idle_jiffies(0) {}
