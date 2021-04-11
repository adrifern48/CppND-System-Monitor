#include "processor.h"
#include "linux_parser.h"
#include <math.h>

using namespace std;

float Processor::Utilization() {
    float utilization = 0.0f;
    auto cpuFields = LinuxParser::CpuUtilization();
    if (!cpuFields.empty()) {
        // guest time is already accounted for in user time
        float user, nice, system, idle;
        float iowait, irq, softirq, steal; 

        try {
            user = stof(cpuFields[0]);
            nice = stof(cpuFields[1]);
            system = stof(cpuFields[2]);
            idle = stof(cpuFields[3]);
            iowait = stof(cpuFields[4]);
            irq = stof(cpuFields[5]);
            softirq = stof(cpuFields[6]);
            steal = stof(cpuFields[7]);

            float idleTime = idle + iowait;
            float nonIdle = user + nice + irq + softirq + system + steal;

            float total = idleTime + nonIdle;
            float totald = total - prevTotal_;
            float idled = idleTime - prevIdle_;

            // Persist values for next iteration
            prevIdle_ = idleTime;
            prevTotal_ = idleTime + nonIdle;  

            utilization = (totald - idled) / totald;           
        } catch (const std::out_of_range& e) {
            return NAN;
        }
    }
    return utilization;
}
    