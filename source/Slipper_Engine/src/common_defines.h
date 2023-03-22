#pragma once

#ifdef LINUX
#    define MAX_PATH PATH_MAX
#    define DEBUG_BREAK raise(SIGTRAP);
#elif WINDOWS
#    define DEBUG_BREAK __debugbreak();
#endif

#define VK_ASSERT(func, message, ...)                                                       \
    if (func != VK_SUCCESS) {                                                               \
        std::cout << "\nA Vulkan exepction occured at line: '" << __LINE__ << "' in file '" \
                  << __FILE__ << "':\n";                                                    \
        std::string formatedMessage = "\n\t";                                               \
        formatedMessage += std::format(message, __VA_ARGS__);                               \
        formatedMessage += '\n';                                                            \
        DEBUG_BREAK                                                                         \
        throw std::runtime_error(formatedMessage);                                          \
    }

#define LOG(message) std::cout << message;

#define ASSERT(statement, message, ...)                                                       \
    if (statement) {                                                                          \
        std::cout << "\nAn error occured at line: '" << __LINE__ << "' in file '" << __FILE__ \
                  << "':\n";                                                                  \
        std::string formatedMessage = "\n\t";                                                 \
        formatedMessage += std::format(message, __VA_ARGS__);                                 \
        formatedMessage += '\n';                                                              \
        DEBUG_BREAK                                                                           \
        throw std::runtime_error(formatedMessage);                                            \
    }

#ifdef __cplusplus
#    define EXTERNC extern "C"
#else
#    define EXTERNC
#endif

#define BIT(x) (1 << x)

inline void hash_combine(std::size_t &seed)
{
}

template<typename T, typename... Rest>
inline void hash_combine(std::size_t &seed, const T &v, const Rest &... rest)
{
    std::hash<T> hasher;
    seed ^= hasher(v) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
    hash_combine(seed, rest...);
}