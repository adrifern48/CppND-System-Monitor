#include "processor.h"
#include "linux_parser.h"
#include <iostream>

using namespace std;

// TODO: Return the aggregate CPU utilization
float Processor::Utilization() {
    float utilization = 0.0f;
    auto cpuFields = LinuxParser::CpuUtilization();
    if (!cpuFields.empty()) {
        // std::cout << "test: " << cpuFields_[0] << "\n";
        unsigned long long int userTime, niceTime, systemTime, idleTime;
        unsigned long long int ioWait, irq, softIrq, steal, guest, guestNice;

        try {
            userTime = stoi(cpuFields[0]);
            niceTime = stoi(cpuFields[1]);
            systemTime = stoi(cpuFields[2]);
            idleTime = stoi(cpuFields[3]);
            ioWait = stoi(cpuFields[4]);
            irq = stoi(cpuFields[5]);
            softIrq = stoi(cpuFields[6]);
            steal = stoi(cpuFields[7]);
            guest = stoi(cpuFields[8]);
            guestNice = stoi(cpuFields[9]);

            // PrevIdle = previdle + previowait
            // Idle = idle + iowait

            // PrevNonIdle = prevuser + prevnice + prevsystem + previrq + prevsoftirq + prevsteal
            // NonIdle = user + nice + system + irq + softirq + steal

            // PrevTotal = PrevIdle + PrevNonIdle
            // Total = Idle + NonIdle

            // // differentiate: actual value minus the previous one
            // totald = Total - PrevTotal
            // idled = Idle - PrevIdle

            // totald = Total - PrevTotal
            // idled = Idle - PrevIdle

            // CPU_Percentage = (totald - idled)/totald

            float prevIdle = pIdle + pIoWait;
            float idle = idleTime + ioWait;
            float prevNonIdle = pUser + pNice + pSystem + pIrq + pSoftIrq + pSteal;
            float nonIdle = userTime = niceTime + systemTime + irq + softIrq + steal;
            auto prevTotal = prevIdle + prevNonIdle;
            auto total = idle + nonIdle;

            auto totalDelta = total - prevTotal;
            auto idleDelta = idle - prevIdle;
            utilization = (totalDelta - idleDelta) / totalDelta; 


            pUser = userTime;
            pNice = niceTime;
            pSystem = systemTime;
            pIdle = idleTime;
            pIoWait = ioWait;
            pIrq = irq;
            pSoftIrq = softIrq;
            pSteal = steal;
            pGuest = guest;
            pGuestNice = guestNice;



            // userTime = userTime - guest;
            // niceTime = niceTime - guestNice;

            // float idle = idleTime + ioWait;
            // float nonIdle = userTime + niceTime + systemTime + irq + softIrq + steal;
            // float totalTime = idle + nonIdle;

            // utilization_ = (float)((totalTime - idle) / totalTime);

           
        } catch (const std::out_of_range& e) {
            std::cout << "Could not parse CPU fields!";
        }
    }
    return utilization;
}

// cat /proc/stat
// output:
// cpu  65106 1770 14161 363939 5066 0 489 0 0 0
// add up the amount of time (number of jiffies) that system is working and divide by total number of jiffies (time passed)
// since metirics are since beginning of system being on, if system was dormant for a long time then utilization will be low (b/c averaged)
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
    