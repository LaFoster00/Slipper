#pragma once

#ifdef LINUX
#    define MAX_PATH PATH_MAX
#    define DEBUG_BREAK raise(SIGTRAP);
#elif WINDOWS
#    define DEBUG_BREAK __debugbreak();
#endif

#define MAX_SCENE_OBJECT_NAME_LENGTH 256

inline static std::map<VkResult, std::string_view> ResultToString = {
    {VK_SUCCESS, "VK_SUCCESS"},
    {VK_NOT_READY, "VK_NOT_READY"},
    {VK_TIMEOUT, "VK_TIMEOUT"},
    {VK_EVENT_SET, "VK_EVENT_SET"},
    {VK_EVENT_RESET, "VK_EVENT_RESET"},
    {VK_INCOMPLETE, "VK_INCOMPLETE"},
    {VK_ERROR_OUT_OF_HOST_MEMORY, "VK_ERROR_OUT_OF_HOST_MEMORY"},
    {VK_ERROR_OUT_OF_DEVICE_MEMORY, "VK_ERROR_OUT_OF_DEVICE_MEMORY"},
    {VK_ERROR_INITIALIZATION_FAILED, "VK_ERROR_INITIALIZATION_FAILED"},
    {VK_ERROR_DEVICE_LOST, "VK_ERROR_DEVICE_LOST"},
    {VK_ERROR_MEMORY_MAP_FAILED, "VK_ERROR_MEMORY_MAP_FAILED"},
    {VK_ERROR_LAYER_NOT_PRESENT, "VK_ERROR_LAYER_NOT_PRESENT"},
    {VK_ERROR_EXTENSION_NOT_PRESENT, "VK_ERROR_EXTENSION_NOT_PRESENT"},
    {VK_ERROR_FEATURE_NOT_PRESENT, "VK_ERROR_FEATURE_NOT_PRESENT"},
    {VK_ERROR_INCOMPATIBLE_DRIVER, "VK_ERROR_INCOMPATIBLE_DRIVER"},
    {VK_ERROR_TOO_MANY_OBJECTS, "VK_ERROR_TOO_MANY_OBJECTS"},
    {VK_ERROR_FORMAT_NOT_SUPPORTED, "VK_ERROR_FORMAT_NOT_SUPPORTED"},
    {VK_ERROR_FRAGMENTED_POOL, "VK_ERROR_FRAGMENTED_POOL"},
    {VK_ERROR_UNKNOWN, "VK_ERROR_UNKNOWN"},
    {VK_ERROR_OUT_OF_POOL_MEMORY, "VK_ERROR_OUT_OF_POOL_MEMORY"},
    {VK_ERROR_INVALID_EXTERNAL_HANDLE, "VK_ERROR_INVALID_EXTERNAL_HANDLE"},
    {VK_ERROR_FRAGMENTATION, "VK_ERROR_FRAGMENTATION"},
    {VK_ERROR_INVALID_OPAQUE_CAPTURE_ADDRESS, "VK_ERROR_INVALID_OPAQUE_CAPTURE_ADDRESS"},
    {VK_PIPELINE_COMPILE_REQUIRED, "VK_PIPELINE_COMPILE_REQUIRED"},
    {VK_ERROR_SURFACE_LOST_KHR, "VK_ERROR_SURFACE_LOST_KHR"},
    {VK_ERROR_NATIVE_WINDOW_IN_USE_KHR, "VK_ERROR_NATIVE_WINDOW_IN_USE_KHR"},
    {VK_SUBOPTIMAL_KHR, "VK_SUBOPTIMAL_KHR"},
    {VK_ERROR_OUT_OF_DATE_KHR, "VK_ERROR_OUT_OF_DATE_KHR"},
    {VK_ERROR_INCOMPATIBLE_DISPLAY_KHR, "VK_ERROR_INCOMPATIBLE_DISPLAY_KHR"},
    {VK_ERROR_VALIDATION_FAILED_EXT, "VK_ERROR_VALIDATION_FAILED_EXT"},
    {VK_ERROR_INVALID_SHADER_NV, "VK_ERROR_INVALID_SHADER_NV"},
    {VK_ERROR_IMAGE_USAGE_NOT_SUPPORTED_KHR, "VK_ERROR_IMAGE_USAGE_NOT_SUPPORTED_KHR"},
    {VK_ERROR_VIDEO_PICTURE_LAYOUT_NOT_SUPPORTED_KHR,
     "VK_ERROR_VIDEO_PICTURE_LAYOUT_NOT_SUPPORTED_KHR"},
    {VK_ERROR_VIDEO_PROFILE_OPERATION_NOT_SUPPORTED_KHR,
     "VK_ERROR_VIDEO_PROFILE_OPERATION_NOT_SUPPORTED_KHR"},
    {VK_ERROR_VIDEO_PROFILE_FORMAT_NOT_SUPPORTED_KHR,
     "VK_ERROR_VIDEO_PROFILE_FORMAT_NOT_SUPPORTED_KHR"},
    {VK_ERROR_VIDEO_PROFILE_CODEC_NOT_SUPPORTED_KHR,
     "VK_ERROR_VIDEO_PROFILE_CODEC_NOT_SUPPORTED_KHR"},
    {VK_ERROR_VIDEO_STD_VERSION_NOT_SUPPORTED_KHR, "VK_ERROR_VIDEO_STD_VERSION_NOT_SUPPORTED_KHR"},
    {VK_ERROR_INVALID_DRM_FORMAT_MODIFIER_PLANE_LAYOUT_EXT,
     "VK_ERROR_INVALID_DRM_FORMAT_MODIFIER_PLANE_LAYOUT_EXT"},
    {VK_ERROR_NOT_PERMITTED_KHR, "VK_ERROR_NOT_PERMITTED_KHR"},
    {VK_ERROR_FULL_SCREEN_EXCLUSIVE_MODE_LOST_EXT, "VK_ERROR_FULL_SCREEN_EXCLUSIVE_MODE_LOST_EXT"},
    {VK_THREAD_IDLE_KHR, "VK_THREAD_IDLE_KHR"},
    {VK_THREAD_DONE_KHR, "VK_THREAD_DONE_KHR"},
    {VK_OPERATION_DEFERRED_KHR, "VK_OPERATION_DEFERRED_KHR"},
    {VK_OPERATION_NOT_DEFERRED_KHR, "VK_OPERATION_NOT_DEFERRED_KHR"},
    {VK_ERROR_COMPRESSION_EXHAUSTED_EXT, "VK_ERROR_COMPRESSION_EXHAUSTED_EXT"},
    {VK_ERROR_OUT_OF_POOL_MEMORY_KHR, "VK_ERROR_OUT_OF_POOL_MEMORY_KHR"},
    {VK_ERROR_INVALID_EXTERNAL_HANDLE_KHR, "VK_ERROR_INVALID_EXTERNAL_HANDLE_KHR"},
    {VK_ERROR_FRAGMENTATION_EXT, "VK_ERROR_FRAGMENTATION_EXT"},
    {VK_ERROR_NOT_PERMITTED_EXT, "VK_ERROR_NOT_PERMITTED_EXT"},
    {VK_ERROR_INVALID_DEVICE_ADDRESS_EXT, "VK_ERROR_INVALID_DEVICE_ADDRESS_EXT"},
    {VK_ERROR_INVALID_OPAQUE_CAPTURE_ADDRESS_KHR, "VK_ERROR_INVALID_OPAQUE_CAPTURE_ADDRESS_KHR"},
    {VK_PIPELINE_COMPILE_REQUIRED_EXT, "VK_PIPELINE_COMPILE_REQUIRED_EXT"},
    {VK_ERROR_PIPELINE_COMPILE_REQUIRED_EXT, "VK_ERROR_PIPELINE_COMPILE_REQUIRED_EXT"},
    {VK_RESULT_MAX_ENUM, "VK_RESULT_MAX_ENUM"}};

inline static std::map<VkDescriptorType, std::string_view> DescriptorTypeToString = {
    {VK_DESCRIPTOR_TYPE_SAMPLER, "VK_DESCRIPTOR_TYPE_SAMPLER"},
    {VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, "VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER"},
    {VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE, "VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE"},
    {VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, "VK_DESCRIPTOR_TYPE_STORAGE_IMAGE"},
    {VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER, "VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER"},
    {VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER, "VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER"},
    {VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, "VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER"},
    {VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, "VK_DESCRIPTOR_TYPE_STORAGE_BUFFER"},
    {VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC, "VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC"},
    {VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC, "VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC"},
    {VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT, "VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT"},
    {VK_DESCRIPTOR_TYPE_INLINE_UNIFORM_BLOCK, "VK_DESCRIPTOR_TYPE_INLINE_UNIFORM_BLOCK"},
    {VK_DESCRIPTOR_TYPE_ACCELERATION_STRUCTURE_KHR,
     "VK_DESCRIPTOR_TYPE_ACCELERATION_STRUCTURE_KHR"},
    {VK_DESCRIPTOR_TYPE_ACCELERATION_STRUCTURE_NV, "VK_DESCRIPTOR_TYPE_ACCELERATION_STRUCTURE_NV"},
    {VK_DESCRIPTOR_TYPE_SAMPLE_WEIGHT_IMAGE_QCOM, "VK_DESCRIPTOR_TYPE_SAMPLE_WEIGHT_IMAGE_QCOM"},
    {VK_DESCRIPTOR_TYPE_BLOCK_MATCH_IMAGE_QCOM, "VK_DESCRIPTOR_TYPE_BLOCK_MATCH_IMAGE_QCOM"},
    {VK_DESCRIPTOR_TYPE_MUTABLE_EXT, "VK_DESCRIPTOR_TYPE_MUTABLE_EXT"},
    {VK_DESCRIPTOR_TYPE_INLINE_UNIFORM_BLOCK_EXT, "VK_DESCRIPTOR_TYPE_INLINE_UNIFORM_BLOCK_EXT"},
    {VK_DESCRIPTOR_TYPE_MUTABLE_VALVE, "VK_DESCRIPTOR_TYPE_MUTABLE_VALVE"},
    {VK_DESCRIPTOR_TYPE_MAX_ENUM, "VK_DESCRIPTOR_TYPE_MAX_ENUM"}};


#define VK_ASSERT(func, message, ...)                                                          \
    if (func != VK_SUCCESS) {                                                                  \
        std::cout << "\nA Vulkan exepction occured at line: VkException '"                     \
                  << ResultToString.at(func) << "' '" << __LINE__ << "' in file '" << __FILE__ \
                  << "':\n";                                                                   \
        std::string formatedMessage = "\n\t";                                                  \
        formatedMessage += std::format(message, __VA_ARGS__);                                  \
        formatedMessage += '\n';                                                               \
        DEBUG_BREAK                                                                            \
        throw std::runtime_error(formatedMessage);                                             \
    }

#define LOG(message) std::cout << message << '\n';
#define LOG_FORMAT(message, ...) std::cout << std::format(message, __VA_ARGS__) << '\n';

#define ASSERT(statement, message, ...)                                                       \
    if (!statement) {                                                                         \
        std::string formatedMessage = "\n\t";                                                 \
        formatedMessage += std::format(message, __VA_ARGS__);                                 \
        formatedMessage += '\n';                                                              \
        std::cout << "\nAn error occured at line: '" << __LINE__ << "' in file '" << __FILE__ \
                  << "':\n\t" << formatedMessage;                                             \
        DEBUG_BREAK                                                                           \
        throw std::runtime_error(formatedMessage);                                            \
    }

#ifdef __cplusplus
#    define EXTERNC extern "C"
#else
#    define EXTERNC
#endif

#define BIT(x) (1 << x)

using StringViewHash = std::hash<std::string_view>;

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

// Returns the type info of the variant held object
template<class T> std::type_info const &var_type(T const &t)
{
    return std::visit([](auto &&x) -> decltype(auto) { return typeid(x); }, t);
}