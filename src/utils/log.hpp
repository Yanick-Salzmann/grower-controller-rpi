#ifndef GROWER_CONTROLLER_RPI_LOG_HPP
#define GROWER_CONTROLLER_RPI_LOG_HPP

#include <memory>
#include <string>
#include <spdlog/spdlog.h>

namespace grower::utils {
    class logger {
        std::shared_ptr<spdlog::logger> _logger;

        void init_logger (const std::string& name);
        std::string static sanitize_type_name(const std::string& name);

    public:
        template<typename T>
        struct type_tag {

        };

        template<typename T>
        explicit logger(type_tag<T>&& tag) noexcept {
            init_logger (sanitize_type_name(typeid(T).name()));
        }

        explicit logger(const std::string& name) {
            init_logger (name);
        }

        const std::shared_ptr<spdlog::logger>& operator->() const {
            return _logger;
        }
    };
}

#define LOGGER static grower::utils::logger log;
#define LOGGER_IMPL(T) grower::utils::logger T::log{grower::utils::logger::type_tag<T>{}}

#endif //GROWER_CONTROLLER_RPI_LOG_HPP
