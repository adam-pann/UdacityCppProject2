#ifndef PROCESSOR_H
#define PROCESSOR_H

class Processor {
 public:
  Processor();
  float Utilization();
  void Update(long jiffies, long active_jiffies, long prev_idle_jiffies);
 private:
    long prev_jiffies;
    long prev_active_jiffies;
    long prev_idle_jiffies;
    
};

#endif
