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

#define LOG(message) std::cout << message << '\n';
#define LOG_FORMAT(message, ...) std::cout << std::format(message, __VA_ARGS__) << '\n';

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
void hash_combine(std::size_t &Seed, const T &V, const Rest &...rest)
{
    std::hash<T> hasher;
    Seed ^= hasher(V) + 0x9e3779b9 + (Seed << 6) + (Seed >> 2);
    hash_combine(Seed, rest...);
}

template<typename T> void append(std::vector<T> &A, std::vector<T> &B)
{
    A.insert(A.end(), B.begin(), B.end());
}