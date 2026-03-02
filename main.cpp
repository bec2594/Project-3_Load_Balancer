#include "Simulate.h"
#include <iostream>

int main(int argc, char* argv[]) {
    std::string config_file = "config.yaml";
    if (argc > 1) {
        config_file = argv[1];
    }

    Simulate sim(config_file);

    if(!sim.initialize()) {
        std::cerr <<"Simulation failed to initialize." << std::endl;
        return 1;
    }
    sim.run();
    sim.output_summary();
    return 0;
}
