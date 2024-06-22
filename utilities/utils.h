//
// Created by root on 4/8/24.
//

#ifndef GOOGOM_UTILS_H
#define GOOGOM_UTILS_H


#include <iostream>
#include <chrono>
#include <iostream>

#include <boost/archive/iterators/base64_from_binary.hpp>
#include <boost/archive/iterators/transform_width.hpp>
#include <boost/archive/iterators/binary_from_base64.hpp>
#include <boost/algorithm/string.hpp>

class Utils {

public:
    static unsigned long long getCurrentMicroseconds();

    static std::string decode64(const std::string &val) {
        using namespace boost::archive::iterators;
        using It = transform_width<binary_from_base64<std::string::const_iterator>, 8, 6>;
        return boost::algorithm::trim_right_copy_if(std::string(It(std::begin(val)), It(std::end(val))), [](char c) {
            return c == '\0';
        });
    }

    static std::string encode64(const std::string &val) {
        using namespace boost::archive::iterators;
        using It = base64_from_binary<transform_width<std::string::const_iterator, 6, 8>>;
        auto tmp = std::string(It(std::begin(val)), It(std::end(val)));
        return tmp.append((3 - val.size() % 3) % 3, '=');
    }

    static std::string encode_base64(const std::vector<uint8_t>& data) {
        using namespace boost::archive::iterators;
        using It = base64_from_binary<transform_width<std::vector<uint8_t>::const_iterator, 6, 8>>;
        auto tmp = std::string(It(std::begin(data)), It(std::end(data)));
        return tmp.append((3 - data.size() % 3) % 3, '=');
    }
};

#endif // GOOGOM_UTILS_H
