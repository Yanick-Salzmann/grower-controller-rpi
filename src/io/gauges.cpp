#include <cstdio>
#include "gauges.hpp"

namespace grower::io {

    float read_temperature() {
        FILE* f = fopen("/sys/class/thermal/thermal_zone0/temp", "r");
        if(!f) {
            return -1.0f;
        }

        float temp = 0.0f;
        if(fscanf(f, "%f", &temp) != 1) {
            fclose(f);
            return -1.0f;
        }

        fclose(f);
        return temp / 1000.0f;
    }
}