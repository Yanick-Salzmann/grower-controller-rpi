#ifndef GROWER_CONTROLLER_RPI_SUBPROCESS_HPP
#define GROWER_CONTROLLER_RPI_SUBPROCESS_HPP

#include <string>
#include <vector>

namespace grower::utils {
    struct sub_process {
        int stdin_pipe;
        int stdout_pipe;
        int stderr_pipe;

        int process_id;
        int error_code;
    };

    sub_process launch_subprocess(const std::string& command, std::vector<std::string> arguments);
}

#endif //GROWER_CONTROLLER_RPI_SUBPROCESS_HPP
