#ifndef SMS_BASE_HPP
#define SMS_BASE_HPP

#include "vector"
#include "algorithm"

template<typename T>
bool contains(T object, std::vector<T> list) {
    if (std::find(list.begin(), list.end(), object) != list.end())
        return true;
    else
        return false;
}

#endif //SMS_BASE_HPP
