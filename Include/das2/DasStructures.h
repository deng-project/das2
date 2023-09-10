// das2: Improved DENG asset manager library
// licence: Apache, see LICENCE file
// file: DasStructures.h - header of C++ structures for das2
// author: Karl-Mihkel Ott

#pragma once

#include <cstdint>
#include <cstring>
#include <cmath>
#include <string>
#include <array>
#include <vector>
#include <ostream>

#include <das2/Api.h>
#include <cvar/SID.h>
#include <trs/Vector.h>
#include <trs/Matrix.h>
#include <trs/Quaternion.h>

#define DAS2_MAGIC 0x0000000032736164

namespace das2 {

    class BinString {
        private:
            uint16_t m_uLength = 0;
            CVar::hash_t m_hshString = 0;
            char* m_pData = nullptr;

        public:
            BinString() = default;
            ~BinString() {
                delete[] m_pData;
            }

            BinString(const char* _szData) {
                m_uLength = static_cast<uint16_t>(std::strlen(_szData));
                m_pData = new char[m_uLength+1]{};
                std::memcpy(m_pData, _szData, m_uLength);
                m_hshString = RUNTIME_CRC(m_pData);
            }

            BinString(const std::string& _str) {
                m_uLength = static_cast<uint16_t>(_str.size());
                m_pData = new char[m_uLength+1]{};
                std::memcpy(m_pData, _str.data(), _str.size());
                m_hshString = RUNTIME_CRC(m_pData);
            }

            BinString(const BinString& _str) :
                m_uLength(_str.m_uLength),
                m_hshString(_str.m_hshString)
            {
                m_pData = new char[m_uLength+1]{};
                std::memcpy(m_pData, _str.m_pData, m_uLength);
            }

            BinString(BinString&& _str) :
                m_uLength(_str.m_uLength),
                m_pData(_str.m_pData),
                m_hshString(_str.m_hshString)
            {
                _str.m_uLength = 0;
                _str.m_pData = nullptr;
                _str.m_hshString = 0;
            }

            inline bool operator==(const BinString& _str) const {
                return m_hshString == _str.m_hshString;
            }

            inline bool operator!=(const BinString& _str) const {
                return m_hshString != _str.m_hshString;
            }

            inline const char* CString() const {
                return m_pData;
            }

            inline uint16_t Length() const {
                return m_uLength;
            }

            inline CVar::hash_t Hash() const {
                return m_hshString;
            }
    };
}


namespace std {
    template<>
    struct hash<das2::BinString> {
        bool operator()(const das2::BinString& _str) const {
            return _str.Hash();
        }
    };
}

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
        uint64_t uMagic = 0;
        BinString szAuthorName = "";
        BinString szComment = "";
        uint32_t uVerticesCount = 0;
        uint32_t uMeshCount = 0;
        uint32_t uAnimationCount = 0;
        uint32_t uDefaultSceneIndex = 0;
        uint8_t bZlibLevel = 0;
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
        std::array<uint32_t, 8> arrUVBufferIds;
        std::array<uint32_t, 8> arrUVBufferOffsets;
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
        BinString szName = nullptr; 
        uint32_t uMeshCount = 0;
        uint32_t* pMeshes = nullptr;
    };


    struct Node {
        StructureIdentifier bStructure = StructureIdentifier_Unknown;
        BinString szName = "";
        uint32_t uChildrenCount = 0;
        uint32_t* pChildren = nullptr;
        uint32_t uMeshGroupId = static_cast<uint32_t>(-1);
        uint32_t uSkeletonId = static_cast<uint32_t>(-1);
        TRS::Matrix4<float> mCustomTransform;
        TRS::Quaternion qRotation;
        TRS::Vector3<float> vTranslation;
        float fScale = 1.f;
    };

    struct Scene {
        StructureIdentifier bStructure = StructureIdentifier_Unknown;
        BinString szName = "";
        uint32_t uRootNodeCount = 0;
        uint32_t* pRootNodes = nullptr;
    };


    struct SkeletonJoint {
        StructureIdentifier bStructure = StructureIdentifier_Unknown;
        BinString szName;
        uint32_t uChildrenCount = 0;
        uint32_t* pChildren = nullptr;
        TRS::Matrix4<float> mInverseBindPos;
        TRS::Quaternion qRotation;
        TRS::Vector3<float> vTranslation;
        float fScale = 1.f;
    };


    struct Skeleton {
        StructureIdentifier bStructure = StructureIdentifier_Unknown;
        BinString szName = "";
        uint32_t uParent = static_cast<uint32_t>(-1);
        uint32_t uJointCount = 0;
        uint32_t* pJoints = nullptr;
    };


    struct Animation {
        StructureIdentifier bStructure = StructureIdentifier_Unknown;
        BinString szName = "";
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
        BinString szName = "";
        TRS::Vector4<float> vDiffuse = { 0.f, 0.f, 0.f, 1.f };
        TRS::Vector4<float> vSpecular = { 0.f, 0.f, 0.f, 1.f };
        TRS::Vector4<float> vEmission = { 0.f, 0.f, 0.f, 1.f };
        BinString szDiffuseMapUri = "";
        BinString szSpecularMapUri = "";
        BinString szEmissionMapUri = "";
    };


    struct MaterialPbr {
        StructureIdentifier bStructure = StructureIdentifier_Unknown;
        BinString szName = "";
        TRS::Vector4<float> vAlbedoFactor = { 1.f, 1.f, 1.f, 1.f };
        TRS::Vector4<float> vEmissiveFactor = { 0.f, 0.f, 0.f, 1.f };
        float fRoughness = 0.f;
        float fMetallic = 0.f;
        float fAmbientOcclusion = 1.f;
        BinString szAlbedoMapUri = "";
        BinString szEmissionMapUri = "";
        BinString szRoughnessMapUri = "";
        BinString szMetallicMapUri = "";
        BinString szAmbientOcclusionMapUri = "";
    };

    struct Model {
        Header header;
        std::vector<Buffer> buffers;
        std::vector<Mesh> meshes;
        std::vector<MeshGroup> meshGroups;
        std::vector<Node> nodes;
        std::vector<Scene> scenes;
        std::vector<SkeletonJoint> skeletonJoints;
        std::vector<Skeleton> skeletons;
        std::vector<Animation> animations;
        std::vector<AnimationChannel> animationChannels;
        std::vector<MaterialPhong> phongMaterials;
        std::vector<MaterialPbr> pbrMaterials;
    };

    DAS2_API std::ostream& operator<<(std::ostream& _stream, const BinString& _bString);
    DAS2_API std::ostream& operator<<(std::ostream& _stream, const Header& _header);
    DAS2_API std::ostream& operator<<(std::ostream& _stream, const Buffer& _buffer);
    DAS2_API std::ostream& operator<<(std::ostream& _stream, const MorphTarget& _morphTarget);
    DAS2_API std::ostream& operator<<(std::ostream& _stream, const Mesh& _meshes);
    DAS2_API std::ostream& operator<<(std::ostream& _stream, const MeshGroup& _meshGroup);
    DAS2_API std::ostream& operator<<(std::ostream& _stream, const Node& _node);
    DAS2_API std::ostream& operator<<(std::ostream& _stream, const Scene& _scene);
    DAS2_API std::ostream& operator<<(std::ostream& _stream, const SkeletonJoint& _skeletonJoint);
    DAS2_API std::ostream& operator<<(std::ostream& _stream, const Skeleton& _skeleton);
    DAS2_API std::ostream& operator<<(std::ostream& _stream, const Animation& _animation);
    DAS2_API std::ostream& operator<<(std::ostream& _stream, const AnimationChannel& _animationChannel);
    DAS2_API std::ostream& operator<<(std::ostream& _stream, const MaterialPhong& _phongMaterial);
    DAS2_API std::ostream& operator<<(std::ostream& _stream, const MaterialPbr& _pbrMaterial);
}
