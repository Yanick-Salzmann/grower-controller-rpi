#ifndef GROWER_CONTROLLER_RPI_STRING_HPP
#define GROWER_CONTROLLER_RPI_STRING_HPP

#include <string>
#include <vector>
#include <sstream>
#include <memory>

namespace grower::utils {
    struct string_deleter {
    public:
        void operator () (char* ptr) {
            free(ptr);
        }
    };

    std::unique_ptr<char, string_deleter> heap_copy(const std::string& value);

    std::string trim_left(const std::string &value);

    std::string trim_right(const std::string &value);

    inline std::string trim(const std::string &value) {
        return trim_left(trim_right(value));
    }

    auto to_lower(const std::string &value) -> std::string;

    std::vector<std::string> split(const std::string &value, const char &delimiter);

    void split(const std::string &value, const char &delimiter, std::vector<std::string> &parts);

    std::string replace_all(const std::string &str, const char &chr, const char &replace);

    bool starts_with(const std::string &str, const std::string &pattern, bool ignore_case = true);

    inline bool contains(const std::string& str, const std::string& pattern) {
        return str.find(pattern) != std::string::npos;
    }

    template<typename T>
    std::string join(T container, const std::string &delimiter) {
        std::stringstream stream;
        auto is_first = true;

        for (const auto &elem: container) {
            if (is_first) {
                is_first = false;
            } else {
                stream << delimiter;
            }

            stream << elem;
        }

        return stream.str();
    }
}

#endif //GROWER_CONTROLLER_RPI_STRING_HPP
