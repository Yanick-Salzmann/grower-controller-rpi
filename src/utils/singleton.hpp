#ifndef GROWER_CONTROLLER_RPI_SINGLETON_HPP
#define GROWER_CONTROLLER_RPI_SINGLETON_HPP

#include <memory>

namespace grower::utils {
    template<typename T>
    class singleton {
    public:
        static std::shared_ptr<T> instance() {
            static const auto instance = std::make_shared<T>();
            return instance;
        }
    };
}

#endif //GROWER_CONTROLLER_RPI_SINGLETON_HPP
