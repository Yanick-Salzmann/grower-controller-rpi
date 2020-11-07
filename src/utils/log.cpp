#include "log.hpp"

#include <spdlog/sinks/stdout_color_sinks.h>

#ifdef _MSC_VER
#include <dbghelp.h>
#else
#include <cxxabi.h>
#endif

namespace grower::utils {
#ifndef _MSC_VER
    std::string demangle(const char* name) {
        int status = -1;

        std::unique_ptr<char, void(*)(void*)> res {
                abi::__cxa_demangle(name, nullptr, nullptr, &status),
                std::free
        };

        return (status==0) ? res.get() : name ;
    }
#else

    std::string demangle(const char *name) {
        char out_name[4096]{};
        const auto num_chars = UnDecorateSymbolName(name, out_name, sizeof out_name, UNDNAME_NAME_ONLY);
        if(num_chars > 0) {
            const auto num_read = std::min<std::size_t>(num_chars, (sizeof out_name) - 1);
            out_name[num_read] = '\0';
            return std::string{out_name, out_name + num_read};
        }

        return name;
    }

#endif

    void logger::init_logger(const std::string &name) {
        _logger = spdlog::stdout_color_mt(name);
        _logger->set_pattern("[%Y-%m-%d %H:%M:%S.%e] [%P:%t] [%n] %^[%l]%$ %v");
#ifndef NDEBUG
        _logger->set_level(spdlog::level::debug);
#else
        _logger->set_level (spdlog::level::debug);
#endif
    }

    std::string logger::sanitize_type_name(const std::string &name) {
        auto actual_name = demangle(name.c_str());

        const auto idx_space = actual_name.find(' ');
        if (idx_space == std::string::npos) {
            return actual_name;
        }

        return actual_name.substr(idx_space + 1);
    }
}