#include "processor.h"

// TODO: Return the aggregate CPU utilization


// cat /proc/stat
// output:
// cpu  65106 1770 14161 363939 5066 0 489 0 0 0
// add up the amount of time (number of jiffies) that system is working and divide by total number of jiffies (time passed)
// since metirics are since beginning of system being on, if system was dormant for a long time then utilization will be low (b/c averaged)
float Processor::Utilization() { return utilization_; }
//       user    nice   system  idle      iowait irq   softirq  steal  guest  guest_nice
//cpu    74608   2520   24433   1117073   6176   4054  0        0      0      0    
// Guest time is already accounted in usertime
// usertime = usertime - guest;                             # As you see here, it subtracts guest from user time
// nicetime = nicetime - guestnice;                         # and guest_nice from nice time
// // Fields existing on kernels >= 2.6
// // (and RHEL's patched kernel 2.4...)
// unsigned long long int idlealltime = idletime + ioWait;  # ioWait is added in the idleTime
// unsigned long long int systemalltime = systemtime + irq + softIrq;
// unsigned long long int virtalltime = guest + guestnice;
// unsigned long long int totaltime = usertime + nicetime + systemalltime + idlealltime + steal + virtalltime;

// PrevIdle = previdle + previowait
// Idle = idle + iowait

// PrevNonIdle = prevuser + prevnice + prevsystem + previrq + prevsoftirq + prevsteal
// NonIdle = user + nice + system + irq + softirq + steal

// PrevTotal = PrevIdle + PrevNonIdle
// Total = Idle + NonIdle

// // differentiate: actual value minus the previous one
// totald = Total - PrevTotal
// idled = Idle - PrevIdle

// CPU_Percentage = (totald - idled)/totald
    