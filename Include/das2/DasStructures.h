// das2: Improved DENG asset manager library
// licence: Apache, see LICENCE file
// file: dastool.cmake - dastool utility program CMake configuration file
// author: Karl-Mihkel Ott

#pragma once

#include <cstdint>
#include <string>
#include <array>
#include <cvar/CVarTypes.h>

#define DAS2_MAGIC 0x0000000032736164

namespace das2 {

    enum StructureIdentifier : char {
        StructureIdentifier_Unknown = 0x00,
        StructureIdentifier_Buffer = 0x01,
        StructureIdentifier_Mesh = 0x02,
        StructureIdentifier_MorphTarget = 0x03,
        StructureIdentifier_MeshGroup = 0x04,
        StructureIdentifier_Node = 0x05,
        StructureIdentifier_Scene = 0x06,
        StructureIdentifier_SkeletonJoint = 0x07,
        StructureIdentifier_Skeleton = 0x08,
        StructureIdentifier_Animation = 0x09,
        StructureIdentifier_AnimationChannel = 0x0a,
        StructureIdentifier_MaterialPhong = 0x0b,
        StructureIdentifier_MaterialPbr = 0x0c
    };

    enum MaterialType : char {
        MaterialType_Unknown,
        MaterialType_Phong,
        MaterialType_Pbr
    };

    enum AnimationTarget : char {
        AnimationTarget_Unknown,
        AnimationTarget_Weights,
        AnimationTarget_Translation,
        AnimationTarget_Rotation,
        AnimationTarget_Scale
    };

    enum InterpolationType : char {
        InterpolationType_Unknown,
        InterpolationType_Linear,
        InterpolationType_Step,
        InterpolationType_CubicSpline
    };

    struct Header {
        uint64_t uMagic = DAS2_MAGIC;
        CVar::String szAuthorName = "";
        CVar::String szComment = "";
        uint32_t uVerticesCount = 0;
        uint32_t uMeshCount = 0;
        uint32_t uAnimationCount = 0;
        uint32_t uDefaultSceneIndex = 0;
        uint32_t uZlibLevel = 0;
    };

    struct Buffer {
        StructureIdentifier bStructure = StructureIdentifier_Unknown;
        uint32_t uLength = 0;
        char* pData = nullptr;
    };


    struct MorphTarget {
        StructureIdentifier bStructure = StructureIdentifier_Unknown;
        uint32_t uIndexBufferId = static_cast<uint32_t>(-1);
        uint32_t uIndexBufferOffset = 0;
        uint32_t uPositionVertexBufferId = static_cast<uint32_t>(-1);
        uint32_t uPositionVertexBufferOffset = 0;
        uint32_t uSurfaceNormalBufferId = static_cast<uint32_t>(-1);
        uint32_t uSurfaceNormalBufferOffset = 0;
        std::array<uint32_t, 8> uUVBufferIds;
        std::array<uint32_t, 8> uUVBufferOffsets;
        uint32_t uColorMultiplierId = static_cast<uint32_t>(-1);
        uint32_t uColorMultiplierOffset = 0;
    };


    struct Mesh {
        StructureIdentifier bStructure = StructureIdentifier_Unknown;
        uint32_t uIndexBufferId = static_cast<uint32_t>(-1);
        uint32_t uIndexBufferOffset = 0;
        uint32_t uDrawCount = 0;
        uint32_t uPositionVertexBufferId = static_cast<uint32_t>(-1);
        uint32_t uPositionVertexBufferOffset = 0;
        uint32_t uSurfaceNormalBufferId = static_cast<uint32_t>(-1);
        uint32_t uSurfaceNormalBufferOffset = 0;
        std::array<uint32_t, 8> arrUVBufferIds;
        std::array<uint32_t, 8> arrUVBufferOffsets;
        uint32_t uColorMultiplierId = static_cast<uint32_t>(-1);
        uint32_t uColorMultiplierOffset = 0;
        std::array<uint32_t, 8> arrSkeletalJointIndexBufferIds;
        std::array<uint32_t, 8> arrSkeletalJointIndexBufferOffsets;
        std::array<uint32_t, 8> arrSkeletalJointWeightBufferIds;
        std::array<uint32_t, 8> arrSkeletalJointWeightBufferOffsets;
        MaterialType bMaterialType = MaterialType_Unknown;
        uint32_t uMaterialId = static_cast<uint32_t>(-1);
        uint32_t uMorphTargetCount = 0;
        MorphTarget* pMorphTargets = nullptr;
        uint32_t uMultipleLodCount = 0;
        Mesh* pMultipleLods = nullptr;
    };


    struct MeshGroup {
        StructureIdentifier bStructure = StructureIdentifier_Unknown;
        CVar::String szName = ""; 
        uint32_t uMeshCount = 0;
        uint32_t* pMeshes = nullptr;
    };


    struct Node {
        StructureIdentifier bStructure = StructureIdentifier_Unknown;
        CVar::String szName = "";
        uint32_t uChildrenCount = 0;
        uint32_t* pChildren = nullptr;
        uint32_t uMeshGroupId = static_cast<uint32_t>(-1);
        uint32_t uSkeletonId = static_cast<uint32_t>(-1);
        TRS::Matrix4<float> mCustomTransform;
        TRS::Quaterion qRotation;
        TRS::Vector3<float> vTranslation;
        float fScale = 1.f;
    };

    struct Scene {
        StructureIdentifier bStructure = StructureIdentifier_Unknown;
        CVar::String szName = "";
        uint32_t uRootNodeCount = 0;
        uint32_t* pRootNodes = nullptr;
    };


    struct SkeletonJoint {
        StructureIdentifier bStructure = StructureIdentifier_Unknown;
        CVar::String szName;
        uint32_t uChildrenCount = 0;
        uint32_t* pChildren = nullptr;
        TRS::Matrix4<float> mInverseBindPos;
        TRS::Quaterion qRotation;
        TRS::Vector3<float> vTranslation;
        float fScale = 1.f;
    };


    struct Skeleton {
        StructureIdentifier bStructure = StructureIdentifier_Unknown;
        CVar::String szName = "";
        uint32_t uParent = static_cast<uint32_t>(-1);
        uint32_t uJointCount = 0;
        uint32_t* pJoints = nullptr;
    };


    struct Animation {
        StructureIdentifier bStructure = StructureIdentifier_Unknown;
        uint32_t uAnimationChannelCount = 0;
        uint32_t* pAnimationChannels = nullptr;
    };


    struct AnimationChannel {
        StructureIdentifier bStructure = StructureIdentifier_Unknown;
        uint32_t uNodePropertyId = static_cast<uint32_t>(-1);
        uint32_t uJointPropertyId = static_cast<uint32_t>(-1);
        AnimationTarget bAnimationTarget = AnimationTarget_Unknown;
        InterpolationType bInterpolationType = InterpolationType_Unknown;
        uint32_t uKeyframeCount = 0;
        uint32_t uWeightCount = 0;
        float* pKeyframes = nullptr;
        char* pTangents = nullptr;
        char* pTargetValues = nullptr;
    };


    struct MaterialPhong {
        StructureIdentifier bStructure = StructureIdentifier_Unknown;
        String szName = "";
        TRS::Vector4<float> vDiffuse = { 0.f, 0.f, 0.f, 1.f };
        TRS::Vector4<float> vSpecular = { 0.f, 0.f, 0.f, 1.f };
        TRS::Vector4<float> vEmission = { 0.f, 0.f, 0.f, 1.f };
        CVar::String szDiffuseMapUri = "";
        CVar::String szSpecularMapUri = "";
        CVar::EmissionMapUri = "";
    };


    struct MaterialPbr {
        StructureIdentifier bStructure = StructureIdentifier_Unknown;
        String szName = "";
        TRS::Vector4<float> vAlbedoFactor = { 1.f, 1.f, 1.f, 1.f };
        TRS::Vector4<float> vEmissiveFactor = { 0.f, 0.f, 0.f, 1.f };
        float fRoughness = 0.f;
        float fMetallic = 0.f;
        float fAmbientOcclusion = 1.f;
        CVar::String szAlbedoMapUri = "";
        CVar::String szEmissionMapUri = "";
        CVar::String szRoughnessMapUri = "";
        CVar::String szMetallicMapUri = "";
        CVar::String szAmbientOcclusionMapUri = "";
    };
}
