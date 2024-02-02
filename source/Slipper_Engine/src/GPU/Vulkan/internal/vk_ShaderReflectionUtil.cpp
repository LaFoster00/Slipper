#include "vk_ShaderReflectionUtil.h"

namespace Slipper
{
namespace ShaderReflectionUtil
{

std::string to_string_shader_stage(SpvReflectShaderStageFlagBits Stage)
{
    switch (Stage) {
        case SPV_REFLECT_SHADER_STAGE_VERTEX_BIT:
            return "VS";
        case SPV_REFLECT_SHADER_STAGE_TESSELLATION_CONTROL_BIT:
            return "HS";
        case SPV_REFLECT_SHADER_STAGE_TESSELLATION_EVALUATION_BIT:
            return "DS";
        case SPV_REFLECT_SHADER_STAGE_GEOMETRY_BIT:
            return "GS";
        case SPV_REFLECT_SHADER_STAGE_FRAGMENT_BIT:
            return "PS";
        case SPV_REFLECT_SHADER_STAGE_COMPUTE_BIT:
            return "CS";
        case SPV_REFLECT_SHADER_STAGE_TASK_BIT_NV:
            return "TASK";
        case SPV_REFLECT_SHADER_STAGE_MESH_BIT_NV:
            return "MESH";
        case SPV_REFLECT_SHADER_STAGE_RAYGEN_BIT_KHR:
            return "RAYGEN";
        case SPV_REFLECT_SHADER_STAGE_ANY_HIT_BIT_KHR:
            return "ANY_HIT";
        case SPV_REFLECT_SHADER_STAGE_CLOSEST_HIT_BIT_KHR:
            return "CLOSEST_HIT";
        case SPV_REFLECT_SHADER_STAGE_MISS_BIT_KHR:
            return "MISS";
        case SPV_REFLECT_SHADER_STAGE_INTERSECTION_BIT_KHR:
            return "INTERSECTION";
        case SPV_REFLECT_SHADER_STAGE_CALLABLE_BIT_KHR:
            return "CALLABLE";
    }
    // Unhandled SpvReflectShaderStageFlagBits enum value
    return "???";
}

VkDescriptorType to_vk_descriptor_type(SpvReflectDescriptorType Type)
{
    switch (Type) {
        case SPV_REFLECT_DESCRIPTOR_TYPE_SAMPLER:
            return VK_DESCRIPTOR_TYPE_SAMPLER;
        case SPV_REFLECT_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER:
            return VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
        case SPV_REFLECT_DESCRIPTOR_TYPE_SAMPLED_IMAGE:
            return VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE;
        case SPV_REFLECT_DESCRIPTOR_TYPE_STORAGE_IMAGE:
            return VK_DESCRIPTOR_TYPE_STORAGE_IMAGE;
        case SPV_REFLECT_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER:
            return VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER;
        case SPV_REFLECT_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER:
            return VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER;
        case SPV_REFLECT_DESCRIPTOR_TYPE_UNIFORM_BUFFER:
            return VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        case SPV_REFLECT_DESCRIPTOR_TYPE_STORAGE_BUFFER:
            return VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
        case SPV_REFLECT_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC:
            return VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC;
        case SPV_REFLECT_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC:
            return VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC;
        case SPV_REFLECT_DESCRIPTOR_TYPE_INPUT_ATTACHMENT:
            return VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT;
        case SPV_REFLECT_DESCRIPTOR_TYPE_ACCELERATION_STRUCTURE_KHR:
            return VK_DESCRIPTOR_TYPE_ACCELERATION_STRUCTURE_KHR;
    }
    return VK_DESCRIPTOR_TYPE_MAX_ENUM;
}

ShaderMemberType to_shader_member_type(const SpvReflectTypeDescription &Type)
{
    switch (Type.op) {
        case SpvOpTypeVoid:
            return ShaderMemberType::VOID;
        case SpvOpTypeBool:
            return ShaderMemberType::BOOL;
        case SpvOpTypeInt:
            if (Type.traits.numeric.scalar.signedness)
                return ShaderMemberType::INT;
            else
                return ShaderMemberType::UINT;
        case SpvOpTypeFloat: {
            switch (Type.traits.numeric.scalar.width) {
                case 32:
                    return ShaderMemberType::FLOAT;
                case 64:
                    return ShaderMemberType::DOUBLE;
            }
        }
        case SpvOpTypeVector:
            switch (Type.traits.numeric.scalar.width) {
                case 32:
                    switch (Type.traits.numeric.vector.component_count) {
                        case 2:
                            return ShaderMemberType::VEC2;
                        case 3:
                            return ShaderMemberType::VEC3;
                        case 4:
                            return ShaderMemberType::VEC4;
                    }
                    break;
                case 64:
                    switch (Type.traits.numeric.vector.component_count) {
                        case 2:
                            return ShaderMemberType::DVEC2;
                        case 3:
                            return ShaderMemberType::DVEC3;
                        case 4:
                            return ShaderMemberType::DVEC4;
                    }
                    break;
            }
            break;
        case SpvOpTypeMatrix:
            switch (Type.traits.numeric.matrix.column_count) {
                case 2:
                    return ShaderMemberType::MATRIX2;
                case 3:
                    return ShaderMemberType::MATRIX3;
                case 4:
                    return ShaderMemberType::MATRIX4;
            }
            break;
        case SpvOpTypeImage:
            return ShaderMemberType::IMAGE;
        case SpvOpTypeSampler:
            return ShaderMemberType::SAMPLER;
        case SpvOpTypeSampledImage:
            return ShaderMemberType::SAMPLED_IMAGE;
        case SpvOpTypeAccelerationStructureKHR:
            return ShaderMemberType::ACCELERATION_STRUCTURE;
        case SpvOpTypeStruct:
            return ShaderMemberType::STRUCT;
        case SpvOpTypeArray:
            return ShaderMemberType::ARRAY;
    }
    return ShaderMemberType::UNDEFINED;
}

ShaderType to_shader_type(SpvReflectShaderStageFlagBits Stage)
{
    switch (Stage) {
        case SPV_REFLECT_SHADER_STAGE_VERTEX_BIT:
            return ShaderType::VERTEX;
        case SPV_REFLECT_SHADER_STAGE_TESSELLATION_CONTROL_BIT:
            break;
        case SPV_REFLECT_SHADER_STAGE_TESSELLATION_EVALUATION_BIT:
            break;
        case SPV_REFLECT_SHADER_STAGE_GEOMETRY_BIT:
            break;
        case SPV_REFLECT_SHADER_STAGE_FRAGMENT_BIT:
            return ShaderType::FRAGMENT;
        case SPV_REFLECT_SHADER_STAGE_COMPUTE_BIT:
            return ShaderType::COMPUTE;
        case SPV_REFLECT_SHADER_STAGE_TASK_BIT_NV:
            break;
        case SPV_REFLECT_SHADER_STAGE_MESH_BIT_NV:
            break;
        case SPV_REFLECT_SHADER_STAGE_RAYGEN_BIT_KHR:
            break;
        case SPV_REFLECT_SHADER_STAGE_ANY_HIT_BIT_KHR:
            break;
        case SPV_REFLECT_SHADER_STAGE_CLOSEST_HIT_BIT_KHR:
            break;
        case SPV_REFLECT_SHADER_STAGE_MISS_BIT_KHR:
            break;
        case SPV_REFLECT_SHADER_STAGE_INTERSECTION_BIT_KHR:
            break;
        case SPV_REFLECT_SHADER_STAGE_CALLABLE_BIT_KHR:
            break;
    }
    return ShaderType::UNDEFINED;
}

VkShaderStageFlags to_shader_stage_flag(ShaderType Type)
{
    switch (Type) {
        case ShaderType::UNDEFINED:
            return VK_SHADER_STAGE_FLAG_BITS_MAX_ENUM;
        case ShaderType::VERTEX:
            return VK_SHADER_STAGE_VERTEX_BIT;
        case ShaderType::FRAGMENT:
            return VK_SHADER_STAGE_FRAGMENT_BIT;
        case ShaderType::COMPUTE:
            return VK_SHADER_STAGE_COMPUTE_BIT;
        default:
            return VK_SHADER_STAGE_FLAG_BITS_MAX_ENUM;
    }
}

std::string to_string_spv_storage_class(SpvStorageClass StorageClass)
{
    switch (StorageClass) {
        case SpvStorageClassUniformConstant:
            return "UniformConstant";
        case SpvStorageClassInput:
            return "Input";
        case SpvStorageClassUniform:
            return "Uniform";
        case SpvStorageClassOutput:
            return "Output";
        case SpvStorageClassWorkgroup:
            return "Workgroup";
        case SpvStorageClassCrossWorkgroup:
            return "CrossWorkgroup";
        case SpvStorageClassPrivate:
            return "Private";
        case SpvStorageClassFunction:
            return "Function";
        case SpvStorageClassGeneric:
            return "Generic";
        case SpvStorageClassPushConstant:
            return "PushConstant";
        case SpvStorageClassAtomicCounter:
            return "AtomicCounter";
        case SpvStorageClassImage:
            return "Image";
        case SpvStorageClassStorageBuffer:
            return "StorageBuffer";
        case SpvStorageClassCallableDataKHR:
            return "CallableDataKHR";
        case SpvStorageClassIncomingCallableDataKHR:
            return "IncomingCallableDataKHR";
        case SpvStorageClassRayPayloadKHR:
            return "RayPayloadKHR";
        case SpvStorageClassHitAttributeKHR:
            return "HitAttributeKHR";
        case SpvStorageClassIncomingRayPayloadKHR:
            return "IncomingRayPayloadKHR";
        case SpvStorageClassShaderRecordBufferKHR:
            return "ShaderRecordBufferKHR";
        case SpvStorageClassPhysicalStorageBuffer:
            return "PhysicalStorageBuffer";
        case SpvStorageClassCodeSectionINTEL:
            return "CodeSectionINTEL";
        case SpvStorageClassDeviceOnlyINTEL:
            return "DeviceOnlyINTEL";
        case SpvStorageClassHostOnlyINTEL:
            return "HostOnlyINTEL";
        case SpvStorageClassMax:
            break;
    }

    // Special case: this specific "unhandled" value does actually seem to show up.
    if (StorageClass == (SpvStorageClass)-1) {
        return "NOT APPLICABLE";
    }

    // unhandled SpvStorageClass enum value
    return "???";
}

std::string to_string_spv_dim(SpvDim Dim)
{
    switch (Dim) {
        case SpvDim1D:
            return "1D";
        case SpvDim2D:
            return "2D";
        case SpvDim3D:
            return "3D";
        case SpvDimCube:
            return "Cube";
        case SpvDimRect:
            return "Rect";
        case SpvDimBuffer:
            return "Buffer";
        case SpvDimSubpassData:
            return "SubpassData";

        case SpvDimMax:
            break;
    }
    // unhandled SpvDim enum value
    return "???";
}

std::string to_string_resource_type(SpvReflectResourceType res_type)
{
    switch (res_type) {
        case SPV_REFLECT_RESOURCE_FLAG_UNDEFINED:
            return "UNDEFINED";
        case SPV_REFLECT_RESOURCE_FLAG_SAMPLER:
            return "SAMPLER";
        case SPV_REFLECT_RESOURCE_FLAG_CBV:
            return "CBV";
        case SPV_REFLECT_RESOURCE_FLAG_SRV:
            return "SRV";
        case SPV_REFLECT_RESOURCE_FLAG_UAV:
            return "UAV";
    }
    // unhandled SpvReflectResourceType enum value
    return "???";
}

std::string to_string_descriptor_type(SpvReflectDescriptorType Value)
{
    switch (Value) {
        case SPV_REFLECT_DESCRIPTOR_TYPE_SAMPLER:
            return "VK_DESCRIPTOR_TYPE_SAMPLER";
        case SPV_REFLECT_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER:
            return "VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER";
        case SPV_REFLECT_DESCRIPTOR_TYPE_SAMPLED_IMAGE:
            return "VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE";
        case SPV_REFLECT_DESCRIPTOR_TYPE_STORAGE_IMAGE:
            return "VK_DESCRIPTOR_TYPE_STORAGE_IMAGE";
        case SPV_REFLECT_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER:
            return "VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER";
        case SPV_REFLECT_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER:
            return "VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER";
        case SPV_REFLECT_DESCRIPTOR_TYPE_UNIFORM_BUFFER:
            return "VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER";
        case SPV_REFLECT_DESCRIPTOR_TYPE_STORAGE_BUFFER:
            return "VK_DESCRIPTOR_TYPE_STORAGE_BUFFER";
        case SPV_REFLECT_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC:
            return "VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC";
        case SPV_REFLECT_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC:
            return "VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC";
        case SPV_REFLECT_DESCRIPTOR_TYPE_INPUT_ATTACHMENT:
            return "VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT";
        case SPV_REFLECT_DESCRIPTOR_TYPE_ACCELERATION_STRUCTURE_KHR:
            return "VK_DESCRIPTOR_TYPE_ACCELERATION_STRUCTURE_KHR";
    }
    // unhandled SpvReflectDescriptorType enum value
    return "VK_DESCRIPTOR_TYPE_???";
}

std::string to_string_spv_built_in(const SpvBuiltIn BuiltIn)
{
    switch (BuiltIn) {
        case SpvBuiltInPosition:
            return "Position";
        case SpvBuiltInPointSize:
            return "PointSize";
        case SpvBuiltInClipDistance:
            return "ClipDistance";
        case SpvBuiltInCullDistance:
            return "CullDistance";
        case SpvBuiltInVertexId:
            return "VertexId";
        case SpvBuiltInInstanceId:
            return "InstanceId";
        case SpvBuiltInPrimitiveId:
            return "PrimitiveId";
        case SpvBuiltInInvocationId:
            return "InvocationId";
        case SpvBuiltInLayer:
            return "Layer";
        case SpvBuiltInViewportIndex:
            return "ViewportIndex";
        case SpvBuiltInTessLevelOuter:
            return "TessLevelOuter";
        case SpvBuiltInTessLevelInner:
            return "TessLevelInner";
        case SpvBuiltInTessCoord:
            return "TessCoord";
        case SpvBuiltInPatchVertices:
            return "PatchVertices";
        case SpvBuiltInFragCoord:
            return "FragCoord";
        case SpvBuiltInPointCoord:
            return "PointCoord";
        case SpvBuiltInFrontFacing:
            return "FrontFacing";
        case SpvBuiltInSampleId:
            return "SampleId";
        case SpvBuiltInSamplePosition:
            return "SamplePosition";
        case SpvBuiltInSampleMask:
            return "SampleMask";
        case SpvBuiltInFragDepth:
            return "FragDepth";
        case SpvBuiltInHelperInvocation:
            return "HelperInvocation";
        case SpvBuiltInNumWorkgroups:
            return "NumWorkgroups";
        case SpvBuiltInWorkgroupSize:
            return "WorkgroupSize";
        case SpvBuiltInWorkgroupId:
            return "WorkgroupId";
        case SpvBuiltInLocalInvocationId:
            return "LocalInvocationId";
        case SpvBuiltInGlobalInvocationId:
            return "GlobalInvocationId";
        case SpvBuiltInLocalInvocationIndex:
            return "LocalInvocationIndex";
        case SpvBuiltInWorkDim:
            return "WorkDim";
        case SpvBuiltInGlobalSize:
            return "GlobalSize";
        case SpvBuiltInEnqueuedWorkgroupSize:
            return "EnqueuedWorkgroupSize";
        case SpvBuiltInGlobalOffset:
            return "GlobalOffset";
        case SpvBuiltInGlobalLinearId:
            return "GlobalLinearId";
        case SpvBuiltInSubgroupSize:
            return "SubgroupSize";
        case SpvBuiltInSubgroupMaxSize:
            return "SubgroupMaxSize";
        case SpvBuiltInNumSubgroups:
            return "NumSubgroups";
        case SpvBuiltInNumEnqueuedSubgroups:
            return "NumEnqueuedSubgroups";
        case SpvBuiltInSubgroupId:
            return "SubgroupId";
        case SpvBuiltInSubgroupLocalInvocationId:
            return "SubgroupLocalInvocationId";
        case SpvBuiltInVertexIndex:
            return "VertexIndex";
        case SpvBuiltInInstanceIndex:
            return "InstanceIndex";
        case SpvBuiltInSubgroupEqMaskKHR:
            return "SubgroupEqMaskKHR";
        case SpvBuiltInSubgroupGeMaskKHR:
            return "SubgroupGeMaskKHR";
        case SpvBuiltInSubgroupGtMaskKHR:
            return "SubgroupGtMaskKHR";
        case SpvBuiltInSubgroupLeMaskKHR:
            return "SubgroupLeMaskKHR";
        case SpvBuiltInSubgroupLtMaskKHR:
            return "SubgroupLtMaskKHR";
        case SpvBuiltInBaseVertex:
            return "BaseVertex";
        case SpvBuiltInBaseInstance:
            return "BaseInstance";
        case SpvBuiltInDrawIndex:
            return "DrawIndex";
        case SpvBuiltInDeviceIndex:
            return "DeviceIndex";
        case SpvBuiltInViewIndex:
            return "ViewIndex";
        case SpvBuiltInBaryCoordNoPerspAMD:
            return "BaryCoordNoPerspAMD";
        case SpvBuiltInBaryCoordNoPerspCentroidAMD:
            return "BaryCoordNoPerspCentroidAMD";
        case SpvBuiltInBaryCoordNoPerspSampleAMD:
            return "BaryCoordNoPerspSampleAMD";
        case SpvBuiltInBaryCoordSmoothAMD:
            return "BaryCoordSmoothAMD";
        case SpvBuiltInBaryCoordSmoothCentroidAMD:
            return "BaryCoordSmoothCentroidAMD";
        case SpvBuiltInBaryCoordSmoothSampleAMD:
            return "BaryCoordSmoothSampleAMD";
        case SpvBuiltInBaryCoordPullModelAMD:
            return "BaryCoordPullModelAMD";
        case SpvBuiltInFragStencilRefEXT:
            return "FragStencilRefEXT";
        case SpvBuiltInViewportMaskNV:
            return "ViewportMaskNV";
        case SpvBuiltInSecondaryPositionNV:
            return "SecondaryPositionNV";
        case SpvBuiltInSecondaryViewportMaskNV:
            return "SecondaryViewportMaskNV";
        case SpvBuiltInPositionPerViewNV:
            return "PositionPerViewNV";
        case SpvBuiltInViewportMaskPerViewNV:
            return "ViewportMaskPerViewNV";
        case SpvBuiltInLaunchIdKHR:
            return "InLaunchIdKHR";
        case SpvBuiltInLaunchSizeKHR:
            return "InLaunchSizeKHR";
        case SpvBuiltInWorldRayOriginKHR:
            return "InWorldRayOriginKHR";
        case SpvBuiltInWorldRayDirectionKHR:
            return "InWorldRayDirectionKHR";
        case SpvBuiltInObjectRayOriginKHR:
            return "InObjectRayOriginKHR";
        case SpvBuiltInObjectRayDirectionKHR:
            return "InObjectRayDirectionKHR";
        case SpvBuiltInRayTminKHR:
            return "InRayTminKHR";
        case SpvBuiltInRayTmaxKHR:
            return "InRayTmaxKHR";
        case SpvBuiltInInstanceCustomIndexKHR:
            return "InInstanceCustomIndexKHR";
        case SpvBuiltInObjectToWorldKHR:
            return "InObjectToWorldKHR";
        case SpvBuiltInWorldToObjectKHR:
            return "InWorldToObjectKHR";
        case SpvBuiltInHitTNV:
            return "InHitTNV";
        case SpvBuiltInHitKindKHR:
            return "InHitKindKHR";
        case SpvBuiltInIncomingRayFlagsKHR:
            return "InIncomingRayFlagsKHR";
        case SpvBuiltInRayGeometryIndexKHR:
            return "InRayGeometryIndexKHR";

        case SpvBuiltInMax:
        default:
            break;
    }
    // unhandled SpvBuiltIn enum value
    std::stringstream ss;
    ss << "??? (" << BuiltIn << ")";
    return ss.str();
}

std::string to_string_spv_image_format(SpvImageFormat Fmt)
{
    switch (Fmt) {
        case SpvImageFormatUnknown:
            return "Unknown";
        case SpvImageFormatRgba32f:
            return "Rgba32f";
        case SpvImageFormatRgba16f:
            return "Rgba16f";
        case SpvImageFormatR32f:
            return "R32f";
        case SpvImageFormatRgba8:
            return "Rgba8";
        case SpvImageFormatRgba8Snorm:
            return "Rgba8Snorm";
        case SpvImageFormatRg32f:
            return "Rg32f";
        case SpvImageFormatRg16f:
            return "Rg16f";
        case SpvImageFormatR11fG11fB10f:
            return "R11fG11fB10f";
        case SpvImageFormatR16f:
            return "R16f";
        case SpvImageFormatRgba16:
            return "Rgba16";
        case SpvImageFormatRgb10A2:
            return "Rgb10A2";
        case SpvImageFormatRg16:
            return "Rg16";
        case SpvImageFormatRg8:
            return "Rg8";
        case SpvImageFormatR16:
            return "R16";
        case SpvImageFormatR8:
            return "R8";
        case SpvImageFormatRgba16Snorm:
            return "Rgba16Snorm";
        case SpvImageFormatRg16Snorm:
            return "Rg16Snorm";
        case SpvImageFormatRg8Snorm:
            return "Rg8Snorm";
        case SpvImageFormatR16Snorm:
            return "R16Snorm";
        case SpvImageFormatR8Snorm:
            return "R8Snorm";
        case SpvImageFormatRgba32i:
            return "Rgba32i";
        case SpvImageFormatRgba16i:
            return "Rgba16i";
        case SpvImageFormatRgba8i:
            return "Rgba8i";
        case SpvImageFormatR32i:
            return "R32i";
        case SpvImageFormatRg32i:
            return "Rg32i";
        case SpvImageFormatRg16i:
            return "Rg16i";
        case SpvImageFormatRg8i:
            return "Rg8i";
        case SpvImageFormatR16i:
            return "R16i";
        case SpvImageFormatR8i:
            return "R8i";
        case SpvImageFormatRgba32ui:
            return "Rgba32ui";
        case SpvImageFormatRgba16ui:
            return "Rgba16ui";
        case SpvImageFormatRgba8ui:
            return "Rgba8ui";
        case SpvImageFormatR32ui:
            return "R32ui";
        case SpvImageFormatRgb10a2ui:
            return "Rgb10a2ui";
        case SpvImageFormatRg32ui:
            return "Rg32ui";
        case SpvImageFormatRg16ui:
            return "Rg16ui";
        case SpvImageFormatRg8ui:
            return "Rg8ui";
        case SpvImageFormatR16ui:
            return "R16ui";
        case SpvImageFormatR8ui:
            return "R8ui";
        case SpvImageFormatR64ui:
            return "R64ui";
        case SpvImageFormatR64i:
            return "R64i";

        case SpvImageFormatMax:
            break;
    }
    // unhandled SpvImageFormat enum value
    return "???";
}

std::string to_string_type_flags(SpvReflectTypeFlags TypeFlags)
{
    if (TypeFlags == SPV_REFLECT_TYPE_FLAG_UNDEFINED) {
        return "UNDEFINED";
    }

#define PRINT_AND_CLEAR_TYPE_FLAG(stream, flags, bit)                                 \
    if (((flags) & (SPV_REFLECT_TYPE_FLAG_##bit)) == (SPV_REFLECT_TYPE_FLAG_##bit)) { \
        stream << #bit << " ";                                                        \
        flags ^= SPV_REFLECT_TYPE_FLAG_##bit;                                         \
    }
    std::stringstream sstream;
    PRINT_AND_CLEAR_TYPE_FLAG(sstream, TypeFlags, ARRAY);
    PRINT_AND_CLEAR_TYPE_FLAG(sstream, TypeFlags, STRUCT);
    PRINT_AND_CLEAR_TYPE_FLAG(sstream, TypeFlags, EXTERNAL_MASK);
    PRINT_AND_CLEAR_TYPE_FLAG(sstream, TypeFlags, EXTERNAL_BLOCK);
    PRINT_AND_CLEAR_TYPE_FLAG(sstream, TypeFlags, EXTERNAL_SAMPLED_IMAGE);
    PRINT_AND_CLEAR_TYPE_FLAG(sstream, TypeFlags, EXTERNAL_SAMPLER);
    PRINT_AND_CLEAR_TYPE_FLAG(sstream, TypeFlags, EXTERNAL_IMAGE);
    PRINT_AND_CLEAR_TYPE_FLAG(sstream, TypeFlags, MATRIX);
    PRINT_AND_CLEAR_TYPE_FLAG(sstream, TypeFlags, VECTOR);
    PRINT_AND_CLEAR_TYPE_FLAG(sstream, TypeFlags, FLOAT);
    PRINT_AND_CLEAR_TYPE_FLAG(sstream, TypeFlags, INT);
    PRINT_AND_CLEAR_TYPE_FLAG(sstream, TypeFlags, BOOL);
    PRINT_AND_CLEAR_TYPE_FLAG(sstream, TypeFlags, VOID);
#undef PRINT_AND_CLEAR_TYPE_FLAG
    if (TypeFlags != 0) {
        // Unhandled SpvReflectTypeFlags bit
        sstream << "???";
    }
    return sstream.str();
}

std::string to_string_format(SpvReflectFormat Fmt)
{
    switch (Fmt) {
        case SPV_REFLECT_FORMAT_UNDEFINED:
            return "VK_FORMAT_UNDEFINED";
        case SPV_REFLECT_FORMAT_R32_UINT:
            return "VK_FORMAT_R32_UINT";
        case SPV_REFLECT_FORMAT_R32_SINT:
            return "VK_FORMAT_R32_SINT";
        case SPV_REFLECT_FORMAT_R32_SFLOAT:
            return "VK_FORMAT_R32_SFLOAT";
        case SPV_REFLECT_FORMAT_R32G32_UINT:
            return "VK_FORMAT_R32G32_UINT";
        case SPV_REFLECT_FORMAT_R32G32_SINT:
            return "VK_FORMAT_R32G32_SINT";
        case SPV_REFLECT_FORMAT_R32G32_SFLOAT:
            return "VK_FORMAT_R32G32_SFLOAT";
        case SPV_REFLECT_FORMAT_R32G32B32_UINT:
            return "VK_FORMAT_R32G32B32_UINT";
        case SPV_REFLECT_FORMAT_R32G32B32_SINT:
            return "VK_FORMAT_R32G32B32_SINT";
        case SPV_REFLECT_FORMAT_R32G32B32_SFLOAT:
            return "VK_FORMAT_R32G32B32_SFLOAT";
        case SPV_REFLECT_FORMAT_R32G32B32A32_UINT:
            return "VK_FORMAT_R32G32B32A32_UINT";
        case SPV_REFLECT_FORMAT_R32G32B32A32_SINT:
            return "VK_FORMAT_R32G32B32A32_SINT";
        case SPV_REFLECT_FORMAT_R32G32B32A32_SFLOAT:
            return "VK_FORMAT_R32G32B32A32_SFLOAT";
        case SPV_REFLECT_FORMAT_R64_UINT:
            return "VK_FORMAT_R64_UINT";
        case SPV_REFLECT_FORMAT_R64_SINT:
            return "VK_FORMAT_R64_SINT";
        case SPV_REFLECT_FORMAT_R64_SFLOAT:
            return "VK_FORMAT_R64_SFLOAT";
        case SPV_REFLECT_FORMAT_R64G64_UINT:
            return "VK_FORMAT_R64G64_UINT";
        case SPV_REFLECT_FORMAT_R64G64_SINT:
            return "VK_FORMAT_R64G64_SINT";
        case SPV_REFLECT_FORMAT_R64G64_SFLOAT:
            return "VK_FORMAT_R64G64_SFLOAT";
        case SPV_REFLECT_FORMAT_R64G64B64_UINT:
            return "VK_FORMAT_R64G64B64_UINT";
        case SPV_REFLECT_FORMAT_R64G64B64_SINT:
            return "VK_FORMAT_R64G64B64_SINT";
        case SPV_REFLECT_FORMAT_R64G64B64_SFLOAT:
            return "VK_FORMAT_R64G64B64_SFLOAT";
        case SPV_REFLECT_FORMAT_R64G64B64A64_UINT:
            return "VK_FORMAT_R64G64B64A64_UINT";
        case SPV_REFLECT_FORMAT_R64G64B64A64_SINT:
            return "VK_FORMAT_R64G64B64A64_SINT";
        case SPV_REFLECT_FORMAT_R64G64B64A64_SFLOAT:
            return "VK_FORMAT_R64G64B64A64_SFLOAT";
    }
    // unhandled SpvReflectFormat enum value
    return "VK_FORMAT_???";
}

static std::string ToStringScalarType(const SpvReflectTypeDescription &type)
{
    switch (type.op) {
        case SpvOpTypeVoid: {
            return "void";
            break;
        }
        case SpvOpTypeBool: {
            return "bool";
            break;
        }
        case SpvOpTypeInt: {
            if (type.traits.numeric.scalar.signedness)
                return "int";
            else
                return "uint";
        }
        case SpvOpTypeFloat: {
            switch (type.traits.numeric.scalar.width) {
                case 32:
                    return "float";
                case 64:
                    return "double";
                default:
                    break;
            }
        }
        case SpvOpTypeStruct: {
            return "struct";
        }
        default: {
            break;
        }
    }
    return "";
}

static std::string ToStringGlslType(const SpvReflectTypeDescription &type)
{
    switch (type.op) {
        case SpvOpTypeVector: {
            switch (type.traits.numeric.scalar.width) {
                case 32: {
                    switch (type.traits.numeric.vector.component_count) {
                        case 2:
                            return "vec2";
                        case 3:
                            return "vec3";
                        case 4:
                            return "vec4";
                    }
                } break;

                case 64: {
                    switch (type.traits.numeric.vector.component_count) {
                        case 2:
                            return "dvec2";
                        case 3:
                            return "dvec3";
                        case 4:
                            return "dvec4";
                    }
                } break;
            }
        } break;
        default:
            break;
    }
    return ToStringScalarType(type);
}

static std::string ToStringHlslType(const SpvReflectTypeDescription &type)
{
    switch (type.op) {
        case SpvOpTypeVector: {
            switch (type.traits.numeric.scalar.width) {
                case 32: {
                    switch (type.traits.numeric.vector.component_count) {
                        case 2:
                            return "float2";
                        case 3:
                            return "float3";
                        case 4:
                            return "float4";
                    }
                } break;

                case 64: {
                    switch (type.traits.numeric.vector.component_count) {
                        case 2:
                            return "double2";
                        case 3:
                            return "double3";
                        case 4:
                            return "double4";
                    }
                } break;
            }
        } break;

        default:
            break;
    }
    return ToStringScalarType(type);
}

std::string to_string_type(SpvSourceLanguage src_lang, const SpvReflectTypeDescription &Type)
{
    if (src_lang == SpvSourceLanguageHLSL) {
        return ToStringHlslType(Type);
    }

    return ToStringGlslType(Type);
}

std::string to_string_component_type(const SpvReflectTypeDescription &Type,
                                     uint32_t MemberDecorationFlags)
{
    uint32_t masked_type = Type.type_flags & 0xF;
    if (masked_type == 0) {
        return "";
    }

    std::stringstream ss;

    if (Type.type_flags & SPV_REFLECT_TYPE_FLAG_MATRIX) {
        if (MemberDecorationFlags & SPV_REFLECT_DECORATION_COLUMN_MAJOR) {
            ss << "column_major"
               << " ";
        }
        else if (MemberDecorationFlags & SPV_REFLECT_DECORATION_ROW_MAJOR) {
            ss << "row_major"
               << " ";
        }
    }

    switch (masked_type) {
        default:
            ASSERT(true, "Unsupported component type.");
            break;
        case SPV_REFLECT_TYPE_FLAG_BOOL:
            ss << "bool";
            break;
        case SPV_REFLECT_TYPE_FLAG_INT:
            ss << (Type.traits.numeric.scalar.signedness ? "int" : "uint");
            break;
        case SPV_REFLECT_TYPE_FLAG_FLOAT:
            ss << "float";
            break;
    }

    if (Type.type_flags & SPV_REFLECT_TYPE_FLAG_MATRIX) {
        ss << Type.traits.numeric.matrix.row_count;
        ss << "x";
        ss << Type.traits.numeric.matrix.column_count;
    }
    else if (Type.type_flags & SPV_REFLECT_TYPE_FLAG_VECTOR) {
        ss << Type.traits.numeric.vector.component_count;
    }

    return ss.str();
}

void print_module_info(std::ostream &Os,
                       const SpvReflectShaderModule &Obj,
                       const char * /*indent*/)
{
    Os << "entry point     : " << Obj.entry_point_name << "\n";
    Os << "source lang     : " << spvReflectSourceLanguage(Obj.source_language) << "\n";
    Os << "source lang ver : " << Obj.source_language_version << "\n";
    if (Obj.source_language == SpvSourceLanguageHLSL) {
        Os << "stage           : ";
        Os << to_string_shader_stage(Obj.shader_stage);
    }
}

void print_descriptor_set(std::ostream &Os, const SpvReflectDescriptorSet &Obj, const char *Indent)
{
    const char *t = Indent;
    std::string tt = std::string(Indent) + "  ";
    std::string ttttt = std::string(Indent) + "    ";

    Os << t << "set           : " << Obj.set << "\n";
    Os << t << "binding count : " << Obj.binding_count;
    Os << "\n";
    for (uint32_t i = 0; i < Obj.binding_count; ++i) {
        const SpvReflectDescriptorBinding &binding = *Obj.bindings[i];
        Os << tt << i << ":"
           << "\n";
        print_descriptor_binding(Os, binding, false, ttttt.c_str());
        if (i < (Obj.binding_count - 1)) {
            Os << "\n";
        }
    }
}

void print_descriptor_binding(std::ostream &Os,
                              const SpvReflectDescriptorBinding &Obj,
                              bool WriteSet,
                              const char *Indent)
{
    const char *t = Indent;
    Os << t << "binding : " << Obj.binding << "\n";
    if (WriteSet) {
        Os << t << "set     : " << Obj.set << "\n";
    }
    Os << t << "type    : " << to_string_descriptor_type(Obj.descriptor_type) << "\n";

    // array
    if (Obj.array.dims_count > 0) {
        Os << t << "array   : ";
        for (uint32_t dim_index = 0; dim_index < Obj.array.dims_count; ++dim_index) {
            Os << "[" << Obj.array.dims[dim_index] << "]";
        }
        Os << "\n";
    }

    // counter
    if (Obj.uav_counter_binding != nullptr) {
        Os << t << "counter : ";
        Os << "(";
        Os << "set=" << Obj.uav_counter_binding->set << ", ";
        Os << "binding=" << Obj.uav_counter_binding->binding << ", ";
        Os << "name=" << Obj.uav_counter_binding->name;
        Os << ");";
        Os << "\n";
    }

    Os << t << "name    : " << Obj.name;
    if ((Obj.type_description->type_name != nullptr) &&
        (strlen(Obj.type_description->type_name) > 0)) {
        Os << " "
           << "(" << Obj.type_description->type_name << ")";
    }
}

void print_interface_variable(std::ostream &Os,
                              SpvSourceLanguage SrcLang,
                              const SpvReflectInterfaceVariable &Obj,
                              const char *Indent)
{
    const char *t = Indent;
    Os << t << "location  : ";
    if (Obj.decoration_flags & SPV_REFLECT_DECORATION_BUILT_IN) {
        Os << to_string_spv_built_in(Obj.built_in) << " "
           << "(built-in)";
    }
    else {
        Os << Obj.location;
    }
    Os << "\n";
    if (Obj.semantic != nullptr) {
        Os << t << "semantic  : " << Obj.semantic << "\n";
    }
    Os << t << "type      : " << to_string_type(SrcLang, *Obj.type_description) << "\n";
    Os << t << "format    : " << to_string_format(Obj.format) << "\n";
    Os << t << "qualifier : ";
    if (Obj.decoration_flags & SPV_REFLECT_DECORATION_FLAT) {
        Os << "flat";
    }
    else if (Obj.decoration_flags & SPV_REFLECT_DECORATION_NOPERSPECTIVE) {
        Os << "noperspective";
    }
    Os << "\n";

    Os << t << "name      : " << Obj.name;
    if ((Obj.type_description->type_name != nullptr) &&
        (strlen(Obj.type_description->type_name) > 0)) {
        Os << " "
           << "(" << Obj.type_description->type_name << ")";
    }
}
}  // namespace ShaderReflectionUtil
}  // namespace Slipper