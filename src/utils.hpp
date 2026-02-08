#pragma once

#include <string>
#include <vector>
#include <inttypes.h>
#include <sstream>
#include <math.h>
#include <chrono>
#include <iostream>
#include <map>

#define __stdcall __attribute__((stdcall))
#define __fastcall __attribute__((fastcall))
#define __thiscall __attribute__((thiscall))
#define __cdecl __attribute__((__cdecl__))

static std::vector<std::string> logs = {};

#define __FILENAME__ std::max<const char *>(__FILE__, \
                                            std::max(strrchr(__FILE__, '\\') + 1, strrchr(__FILE__, '/') + 1))
#define Log(...) _Log(__FILENAME__, "::", __FUNCTION__, " : ", __VA_ARGS__)

template <typename... Args>
void _Log(Args... args)
{
    std::stringstream ss;
    auto now = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
    char buf[100] = {0};
    std::strftime(buf, sizeof(buf), "%H:%M:%S", std::localtime(&now));
    ss << buf << " - ";
    (ss << ... << args);
    logs.push_back(ss.str());
    if (logs.size() > 10000)
        logs.clear();
    std::cout << "LOG - " << ss.str() << std::endl;
}

template <typename T>
std::string to_hex(T val)
{
    std::stringstream stream;
    stream << std::hex << val;
    return "0x" + stream.str();
}

struct Vector3d
{
    float x, y, z;
    Vector3d(float _x = 0.0f, float _y = 0.0f, float _z = 0.0f)
    {
        x = _x;
        y = _y;
        z = _z;
    };
    Vector3d(float *v)
    {
        x = v[0];
        y = v[1];
        z = v[2];
    };

    float distance(Vector3d v)
    {
        float f = std::sqrt(std::pow(this->x - v.x, 2) +
                            std::pow(this->y - v.y, 2) +
                            std::pow(this->z - v.z, 2));
        return f;
    }


    float distance_2d(Vector3d v)
    {
        float f = std::sqrt(std::pow(this->x - v.x, 2) +
                            std::pow(this->y - v.y, 2));
        return f;
    }

    float length()
    {
        return distance({0, 0, 0});
    }

    std::string to_str()
    {
        return "(" + std::to_string(x) + ", " + std::to_string(y) + ", " + std::to_string(z) + ")";
    }

    float *data()
    {
        return (float *)(&x);
    }

    float operator[](unsigned int i)
    {
        return data()[i];
    }
};

template <typename Return, uintptr_t addr, typename... Args>
Return invoke_cdecl(Args... args)
{
    return reinterpret_cast<Return(__cdecl *)(Args...)>(addr)(std::forward<Args>(args)...);
}
template <typename Return, uintptr_t addr, typename... Args>
Return invoke_stdcall(Args... args)
{
    return reinterpret_cast<Return(__stdcall *)(Args...)>(addr)(std::forward<Args>(args)...);
}
template <typename Return, uintptr_t addr, typename... Args>
Return invoke_fastcall(Args... args)
{
    return reinterpret_cast<Return(__fastcall *)(Args...)>(addr)(std::forward<Args>(args)...);
}
template <typename Return, uintptr_t addr, typename... Args>
Return invoke_thiscall(Args... args)
{
    return reinterpret_cast<Return(__thiscall *)(Args...)>(addr)(std::forward<Args>(args)...);
}

template <typename T = uintptr_t>
T read_address(uintptr_t address)
{
    return *reinterpret_cast<T *>(address);
}

template <typename T = uintptr_t>
void write_address(uintptr_t address, T value)
{
    *reinterpret_cast<T *>(address) = value;
}

template <typename T, typename P>
std::vector<T> filter(std::vector<T> vec, P cmp)
{
    std::vector<T> result;
    std::copy_if(vec.begin(), vec.end(), std::back_inserter(result), cmp);
    return result;
}

template <typename K, typename V>
std::vector<K> get_keys(std::map<K, V> m)
{
    std::vector<K> keys;
    for (auto &[k, v] : m)
        keys.push_back(k);
    return keys;
}

template <typename K, typename V>
std::vector<V> get_values(std::map<K, V> m)
{
    std::vector<V> values;
    for (auto &[k, v] : m)
        values.push_back(v);
    return values;
}
