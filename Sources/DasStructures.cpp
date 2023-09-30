// das2: Improved DENG asset manager library
// licence: Apache, see LICENCE file
// file: dastool.cmake - dastool utility program CMake configuration file
// author: Karl-Mihkel Ott

#include <iomanip>
#include <sstream>
#include <das2/Exceptions.h>
#include <das2/DasStructures.h>

namespace das2 {

    Buffer::Buffer(const Buffer& _buffer) {
        m_bStructure = _buffer.m_bStructure;
        if (_buffer.m_uLength) {
            m_uLength = _buffer.m_uLength;
            m_pData = new char[m_uLength];
            std::memcpy(m_pData, _buffer.m_pData, m_uLength);
        }
    }


    Buffer::Buffer(Buffer&& _buffer) noexcept {
        m_bStructure = _buffer.m_bStructure;
        m_uLength = _buffer.m_uLength;
        m_pData = _buffer.m_pData;

        _buffer.m_uLength = 0;
        _buffer.m_pData = nullptr;
    }


    Buffer& Buffer::operator=(Buffer&& _buffer) noexcept {
        delete[] m_pData;

        m_bStructure = _buffer.m_bStructure;
        m_uLength = _buffer.m_uLength;
        m_pData = _buffer.m_pData;

        _buffer.m_uLength = 0;
        _buffer.m_pData = nullptr;

        return *this;
    }


    Buffer::~Buffer() {
        if (m_uLength)
            free(m_pData);
    }


    void BinString::Read(std::istream& _stream) {
        _stream.read(reinterpret_cast<char*>(&m_uLength), sizeof(uint16_t));
        _stream.read(reinterpret_cast<char*>(&m_hshString), sizeof(cvar::hash_t));

        delete[] m_pData;
        m_pData = nullptr;
        if (m_uLength) {
            m_pData = new char[m_uLength + 1]{};
            _stream.read(m_pData, m_uLength);
        }
    }
    
    void BinString::Write(std::ostream& _stream) const {
        _stream.write(reinterpret_cast<const char*>(&m_uLength), sizeof(uint16_t));
        _stream.write(reinterpret_cast<const char*>(&m_hshString), sizeof(cvar::hash_t));
        if (m_pData && m_uLength) {
            _stream.write(m_pData, m_uLength);
        }
    }


    void Header::Read(std::istream& _stream) {
        _stream.read(reinterpret_cast<char*>(&m_uMagic), sizeof(uint64_t));
        if (!Verify()) {
            std::stringstream ss;
            ss << "[das2::Header] invalid magic number 0x" << std::setfill('0') << std::setw(16) << std::hex << m_uMagic; 
            throw MagicValueException(ss.str());
        }

        szAuthorName.Read(_stream);
        szComment.Read(_stream);
        _stream.read(reinterpret_cast<char*>(&uVerticesCount), sizeof(uint32_t));
        _stream.read(reinterpret_cast<char*>(&uMeshCount), sizeof(uint32_t));
        _stream.read(reinterpret_cast<char*>(&uAnimationCount), sizeof(uint32_t));
        _stream.read(reinterpret_cast<char*>(&uDefaultSceneIndex), sizeof(uint32_t));
        _stream.read(reinterpret_cast<char*>(&bZstdLevel), sizeof(uint8_t));
    }

    void Header::Write(std::ostream& _stream) const {
        _stream.write(reinterpret_cast<const char*>(&m_uMagic), sizeof(uint64_t));
        szAuthorName.Write(_stream);
        szComment.Write(_stream);
        _stream.write(reinterpret_cast<const char*>(&uVerticesCount), sizeof(uint32_t));
        _stream.write(reinterpret_cast<const char*>(&uMeshCount), sizeof(uint32_t));
        _stream.write(reinterpret_cast<const char*>(&uAnimationCount), sizeof(uint32_t));
        _stream.write(reinterpret_cast<const char*>(&uDefaultSceneIndex), sizeof(uint32_t));
        _stream.write(reinterpret_cast<const char*>(&bZstdLevel), sizeof(uint8_t));
    }


    void Buffer::Read(std::istream& _stream) {
        _stream.read(reinterpret_cast<char*>(&m_bStructure), sizeof(StructureIdentifier));
        if (!Verify()) {
            std::stringstream ss;
            ss << "[das2::Buffer] invalid magic number 0x" << std::setfill('0') << std::setw(2) << std::hex << m_bStructure;
            throw MagicValueException(ss.str());
        }

        _stream.read(reinterpret_cast<char*>(&m_uLength), sizeof(uint32_t));

        delete[] m_pData;
        m_pData = nullptr;

        if (m_uLength) {
            m_pData = new char[m_uLength];
            _stream.read(m_pData, m_uLength);
        }
    }

    void Buffer::Write(std::ostream& _stream) const {
        _stream.write(reinterpret_cast<const char*>(&m_bStructure), sizeof(StructureIdentifier));
        _stream.write(reinterpret_cast<const char*>(&m_uLength), sizeof(uint32_t));
        _stream.write(m_pData, m_uLength);
    }


    void MorphTarget::Read(std::istream& _stream) {
        _stream.read(reinterpret_cast<char*>(&m_bStructure), sizeof(StructureIdentifier));
        if (!Verify()) {
            std::stringstream ss;
            ss << "das2::MorphTarget: invalid magic number 0x" << std::setfill('0') << std::setw(2) << std::hex << m_bStructure;
            throw MagicValueException(ss.str());
        }

        _stream.read(reinterpret_cast<char*>(&uIndexBufferOffset), sizeof(uint32_t));
        _stream.read(reinterpret_cast<char*>(&uPositionVertexBufferOffset), sizeof(uint32_t));
        _stream.read(reinterpret_cast<char*>(&uVertexNormalBufferOffset), sizeof(uint32_t));
        _stream.read(reinterpret_cast<char*>(&arrUVBufferOffsets), sizeof(std::array<uint32_t, 8>));
        _stream.read(reinterpret_cast<char*>(&uColorMultiplierOffset), sizeof(uint32_t));
    }

    void MorphTarget::Write(std::ostream& _stream) const {
        _stream.write(reinterpret_cast<const char*>(&m_bStructure), sizeof(StructureIdentifier));
        _stream.write(reinterpret_cast<const char*>(&uIndexBufferOffset), sizeof(uint32_t));
        _stream.write(reinterpret_cast<const char*>(&uPositionVertexBufferOffset), sizeof(uint32_t));
        _stream.write(reinterpret_cast<const char*>(&uVertexNormalBufferOffset), sizeof(uint32_t));
        _stream.write(reinterpret_cast<const char*>(&arrUVBufferOffsets), sizeof(std::array<uint32_t, 8>));
        _stream.write(reinterpret_cast<const char*>(&uColorMultiplierOffset), sizeof(uint32_t));
    }

    
    void Mesh::Read(std::istream& _stream) {
        _stream.read(reinterpret_cast<char*>(&m_bStructure), sizeof(StructureIdentifier));
        if (!Verify()) {
            std::stringstream ss;
            ss << "das2::Mesh: invalid magic number 0x" << std::setfill('0') << std::setw(2) << std::hex << m_bStructure;
            throw MagicValueException(ss.str());
        }

        _stream.read(reinterpret_cast<char*>(&uIndexBufferOffset), sizeof(uint32_t));
        _stream.read(reinterpret_cast<char*>(&uDrawCount), sizeof(uint32_t));
        _stream.read(reinterpret_cast<char*>(&uPositionVertexBufferOffset), sizeof(uint32_t));
        _stream.read(reinterpret_cast<char*>(&uVertexNormalBufferOffset), sizeof(uint32_t));
        _stream.read(reinterpret_cast<char*>(arrUVBufferOffsets.data()), sizeof(std::array<uint32_t, 8>));
        _stream.read(reinterpret_cast<char*>(&uIndexBufferOffset), sizeof(uint32_t));
        _stream.read(reinterpret_cast<char*>(arrSkeletalJointIndexBufferOffsets.data()), sizeof(std::array<uint32_t, 8>));
        _stream.read(reinterpret_cast<char*>(arrSkeletalJointWeightBufferOffsets.data()), sizeof(std::array<uint32_t, 8>));
        _stream.read(reinterpret_cast<char*>(&bMaterialType), sizeof(MaterialType));
        _stream.read(reinterpret_cast<char*>(&uMaterialId), sizeof(uMaterialId));

        // read morph targets
        uint32_t uMorphTargetCount = 0;
        _stream.read(reinterpret_cast<char*>(&uMorphTargetCount), sizeof(uint32_t));
        morphTargets.resize(uMorphTargetCount);

        for (auto it = morphTargets.begin(); it != morphTargets.end(); it++) {
            it->Read(_stream);
        }

        // read multiple LODs
        uint32_t uMultipleLodCount = 0;
        _stream.read(reinterpret_cast<char*>(&uMultipleLodCount), sizeof(uint32_t));
        multipleLods.resize(uMultipleLodCount);

        for (auto it = multipleLods.begin(); it != multipleLods.end(); it++) {
            it->Read(_stream);
        }
    }

    void Mesh::Write(std::ostream& _stream) const {
        _stream.write(reinterpret_cast<const char*>(&m_bStructure), sizeof(StructureIdentifier));
        _stream.write(reinterpret_cast<const char*>(&uIndexBufferOffset), sizeof(uint32_t));
        _stream.write(reinterpret_cast<const char*>(&uDrawCount), sizeof(uint32_t));
        _stream.write(reinterpret_cast<const char*>(&uPositionVertexBufferOffset), sizeof(uint32_t));
        _stream.write(reinterpret_cast<const char*>(&uVertexNormalBufferOffset), sizeof(uint32_t));
        _stream.write(reinterpret_cast<const char*>(&arrUVBufferOffsets), sizeof(std::array<uint32_t, 8>));
        _stream.write(reinterpret_cast<const char*>(&uColorMultiplierOffset), sizeof(uint32_t));
        _stream.write(reinterpret_cast<const char*>(&arrSkeletalJointIndexBufferOffsets), sizeof(std::array<uint32_t, 8>));
        _stream.write(reinterpret_cast<const char*>(&arrSkeletalJointWeightBufferOffsets), sizeof(std::array<uint32_t, 8>));
        _stream.write(reinterpret_cast<const char*>(&bMaterialType), sizeof(MaterialType));
        _stream.write(reinterpret_cast<const char*>(&uMaterialId), sizeof(uint32_t));


        // output morph targets
        uint32_t uMorphTargetCount = static_cast<uint32_t>(morphTargets.size());
        _stream.write(reinterpret_cast<const char*>(&uMorphTargetCount), sizeof(uint32_t));
        for (auto it = morphTargets.begin(); it != morphTargets.end(); it++) {
            it->Write(_stream);
        }

        // output multiple LODs (recursive)
        uint32_t uMultipleLodCount = static_cast<uint32_t>(multipleLods.size());
        _stream.write(reinterpret_cast<const char*>(&uMultipleLodCount), sizeof(uint32_t));
        for (auto it = multipleLods.begin(); it != multipleLods.end(); it++) {
            it->Write(_stream);
        }
    }


    void MeshGroup::Read(std::istream& _stream) {
        _stream.read(reinterpret_cast<char*>(&m_bStructure), sizeof(StructureIdentifier));
        if (!Verify()) {
            std::stringstream ss;
            ss << "das2::MeshGroup: invalid magic number 0x" << std::setfill('0') << std::setw(2) << std::hex << m_bStructure;
            throw MagicValueException(ss.str());
        }

        szName.Read(_stream);

        uint32_t uMeshCount = 0;
        _stream.read(reinterpret_cast<char*>(&uMeshCount), sizeof(uint32_t));
        meshes.resize(uMeshCount);
        _stream.read(reinterpret_cast<char*>(meshes.data()), sizeof(uint32_t) * meshes.size());
    }

    void MeshGroup::Write(std::ostream& _stream) const {
        _stream.write(reinterpret_cast<const char*>(&m_bStructure), sizeof(StructureIdentifier));
        szName.Write(_stream);

        uint32_t uMeshCount = static_cast<uint32_t>(meshes.size());
        _stream.write(reinterpret_cast<const char*>(&uMeshCount), sizeof(uint32_t));
        _stream.write(reinterpret_cast<const char*>(meshes.data()), sizeof(uint32_t) * meshes.size());
    }


    void Node::Read(std::istream& _stream) {
        _stream.read(reinterpret_cast<char*>(&m_bStructure), sizeof(StructureIdentifier));
        if (!Verify()) {
            std::stringstream ss;
            ss << "das2::Node: invalid magic number 0x" << std::setfill('0') << std::setw(2) << std::hex << m_bStructure;
            throw MagicValueException(ss.str());
        }

        szName.Read(_stream);
        
        uint32_t uChildrenCount = 0;
        _stream.read(reinterpret_cast<char*>(&uChildrenCount), sizeof(uint32_t));
        children.resize(uChildrenCount);
        _stream.read(reinterpret_cast<char*>(children.data()), children.size() * sizeof(uint32_t));
        _stream.read(reinterpret_cast<char*>(&uMeshGroupId), sizeof(uint32_t));
        _stream.read(reinterpret_cast<char*>(&uSkeletonId), sizeof(uint32_t));
        _stream.read(reinterpret_cast<char*>(&mCustomTransform), sizeof(TRS::Matrix4<float>));
        _stream.read(reinterpret_cast<char*>(&qRotation), sizeof(TRS::Quaternion));
        _stream.read(reinterpret_cast<char*>(&vTranslation), sizeof(TRS::Vector3<float>));
        _stream.read(reinterpret_cast<char*>(&fScale), sizeof(float));
    }

    void Node::Write(std::ostream& _stream) const {
        _stream.write(reinterpret_cast<const char*>(&m_bStructure), sizeof(StructureIdentifier));
        szName.Write(_stream);
        
        uint32_t uChildrenCount = static_cast<uint32_t>(children.size());
        _stream.write(reinterpret_cast<const char*>(&uChildrenCount), sizeof(uint32_t));
        _stream.write(reinterpret_cast<const char*>(children.data()), sizeof(uint32_t) * children.size());

        _stream.write(reinterpret_cast<const char*>(&uMeshGroupId), sizeof(uint32_t));
        _stream.write(reinterpret_cast<const char*>(&uSkeletonId), sizeof(uint32_t));
        _stream.write(reinterpret_cast<const char*>(&mCustomTransform), sizeof(TRS::Matrix4<float>));
        _stream.write(reinterpret_cast<const char*>(&qRotation), sizeof(TRS::Quaternion));
        _stream.write(reinterpret_cast<const char*>(&vTranslation), sizeof(TRS::Vector3<float>));
        _stream.write(reinterpret_cast<const char*>(&fScale), sizeof(float));
    }


    void Scene::Read(std::istream& _stream) {
        _stream.read(reinterpret_cast<char*>(&m_bStructure), sizeof(StructureIdentifier));
        if (!Verify()) {
            std::stringstream ss;
            ss << "das2::Scene: invalid magic number 0x" << std::setfill('0') << std::setw(2) << std::hex << m_bStructure;
            throw MagicValueException(ss.str());
        }

        szName.Read(_stream);

        uint32_t uRootNodeCount = 0;
        _stream.read(reinterpret_cast<char*>(&uRootNodeCount), sizeof(uint32_t));
        rootNodes.resize(uRootNodeCount);
        _stream.read(reinterpret_cast<char*>(rootNodes.data()), sizeof(uint32_t) * rootNodes.size());
    }

    void Scene::Write(std::ostream& _stream) const {
        _stream.write(reinterpret_cast<const char*>(&m_bStructure), sizeof(StructureIdentifier));
        szName.Write(_stream);

        uint32_t uRootNodeCount = static_cast<uint32_t>(rootNodes.size());
        _stream.write(reinterpret_cast<const char*>(&uRootNodeCount), sizeof(uint32_t));
        _stream.write(reinterpret_cast<const char*>(rootNodes.data()), sizeof(uint32_t) * rootNodes.size());
    }


    void SkeletonJoint::Read(std::istream& _stream) {
        _stream.read(reinterpret_cast<char*>(&m_bStructure), sizeof(StructureIdentifier));
        if (!Verify()) {
            std::stringstream ss;
            ss << "das2::SkeletonJoint: invalid magic number 0x" << std::setfill('0') << std::setw(2) << std::hex << m_bStructure;
            throw MagicValueException(ss.str());
        }

        szName.Read(_stream);

        uint32_t uChildrenCount = 0;
        _stream.read(reinterpret_cast<char*>(&uChildrenCount), sizeof(uint32_t));
        children.resize(uChildrenCount);
        _stream.read(reinterpret_cast<char*>(children.data()), sizeof(uint32_t) * children.size());

        _stream.read(reinterpret_cast<char*>(&mInverseBindPos), sizeof(TRS::Matrix4<float>));
        _stream.read(reinterpret_cast<char*>(&qRotation), sizeof(TRS::Quaternion));
        _stream.read(reinterpret_cast<char*>(&vTranslation), sizeof(TRS::Vector3<float>));
        _stream.read(reinterpret_cast<char*>(&fScale), sizeof(float));
    }

    void SkeletonJoint::Write(std::ostream& _stream) const {
        _stream.write(reinterpret_cast<const char*>(&m_bStructure), sizeof(StructureIdentifier));
        szName.Write(_stream);

        uint32_t uChildrenCount = static_cast<uint32_t>(children.size());
        _stream.write(reinterpret_cast<const char*>(&uChildrenCount), sizeof(uint32_t));
        _stream.write(reinterpret_cast<const char*>(children.data()), sizeof(uint32_t) * children.size());
        
        _stream.write(reinterpret_cast<const char*>(&mInverseBindPos), sizeof(TRS::Matrix4<float>));
        _stream.write(reinterpret_cast<const char*>(&qRotation), sizeof(TRS::Quaternion));
        _stream.write(reinterpret_cast<const char*>(&vTranslation), sizeof(TRS::Vector3<float>));
        _stream.write(reinterpret_cast<const char*>(&fScale), sizeof(float));
    }


    void Skeleton::Read(std::istream& _stream) {
        _stream.read(reinterpret_cast<char*>(&m_bStructure), sizeof(StructureIdentifier));
        if (!Verify()) {
            std::stringstream ss;
            ss << "das2::Skeleton: invalid magic number 0x" << std::setfill('0') << std::setw(2) << std::hex << m_bStructure;
            throw MagicValueException(ss.str());
        }

        szName.Read(_stream);
        _stream.read(reinterpret_cast<char*>(&uParent), sizeof(uint32_t));
        
        uint32_t uJointCount = 0;
        _stream.read(reinterpret_cast<char*>(&uJointCount), sizeof(uint32_t));
        joints.resize(uJointCount);
        _stream.read(reinterpret_cast<char*>(joints.data()), joints.size() * sizeof(uint32_t));
    }

    void Skeleton::Write(std::ostream& _stream) const {
        _stream.write(reinterpret_cast<const char*>(&m_bStructure), sizeof(StructureIdentifier));
        szName.Write(_stream);
        _stream.write(reinterpret_cast<const char*>(&uParent), sizeof(uint32_t));
        
        uint32_t uJointCount = static_cast<uint32_t>(joints.size());
        _stream.write(reinterpret_cast<const char*>(&uJointCount), sizeof(uint32_t));
        _stream.write(reinterpret_cast<const char*>(joints.data()), sizeof(uint32_t) * joints.size());
    }


    void Animation::Read(std::istream& _stream) {
        _stream.read(reinterpret_cast<char*>(&m_bStructure), sizeof(StructureIdentifier));
        if (!Verify()) {
            std::stringstream ss;
            ss << "das2::Animation: invalid magic number 0x" << std::setfill('0') << std::setw(2) << std::hex << m_bStructure;
            throw MagicValueException(ss.str());
        }

        szName.Read(_stream);

        uint32_t uAnimationChannelCount = 0;
        _stream.read(reinterpret_cast<char*>(&uAnimationChannelCount), sizeof(uint32_t));
        animationChannels.resize(uAnimationChannelCount);
        _stream.read(reinterpret_cast<char*>(animationChannels.data()), sizeof(uint32_t) * animationChannels.size());
    }

    void Animation::Write(std::ostream& _stream) const {
        _stream.write(reinterpret_cast<const char*>(&m_bStructure), sizeof(StructureIdentifier));
        szName.Write(_stream);

        uint32_t uAnimationChannelCount = static_cast<uint32_t>(animationChannels.size());
        _stream.write(reinterpret_cast<const char*>(&uAnimationChannelCount), sizeof(uint32_t));
        _stream.write(reinterpret_cast<const char*>(animationChannels.data()), sizeof(uint32_t) * animationChannels.size());
    }


    void AnimationChannel::Read(std::istream& _stream) {
        _stream.read(reinterpret_cast<char*>(&m_bStructure), sizeof(StructureIdentifier));
        if (!Verify()) {
            std::stringstream ss;
            ss << "das2::AnimationChannel: invalid magic number 0x" << std::setfill('0') << std::setw(2) << std::hex << m_bStructure;
            throw MagicValueException(ss.str());
        }

        _stream.read(reinterpret_cast<char*>(&uNodePropertyId), sizeof(uint32_t));
        _stream.read(reinterpret_cast<char*>(&uJointPropertyId), sizeof(uint32_t));
        _stream.read(reinterpret_cast<char*>(&bAnimationTarget), sizeof(AnimationTarget));
        _stream.read(reinterpret_cast<char*>(&uWeightCount), sizeof(uint32_t));

        uint32_t uKeyframeCount = 0;
        _stream.read(reinterpret_cast<char*>(&uKeyframeCount), sizeof(uint32_t));
        keyframes.resize(uKeyframeCount);
        _stream.read(reinterpret_cast<char*>(keyframes.data()), sizeof(float) * keyframes.size());

        // read tangents
        if (bInterpolationType == InterpolationType_CubicSpline) {
            tangents.resize(uKeyframeCount);
            for (auto it = tangents.begin(); it != tangents.end(); it++) {
                switch (bAnimationTarget) {
                    case AnimationTarget_Rotation:
                        {
                            TRS::Quaternion q;
                            _stream.read(reinterpret_cast<char*>(&q), sizeof(TRS::Quaternion));
                            (*it)[0] = q;
                            _stream.read(reinterpret_cast<char*>(&q), sizeof(TRS::Quaternion));
                            (*it)[1] = q;
                        }
                        break;

                    case AnimationTarget_Translation:
                        {
                            TRS::Vector3<float> v;
                            _stream.read(reinterpret_cast<char*>(&v), sizeof(TRS::Vector3<float>));
                            (*it)[0] = v;
                            _stream.read(reinterpret_cast<char*>(&v), sizeof(TRS::Vector3<float>));
                            (*it)[1] = v;
                        }
                        break;

                    case AnimationTarget_Scale:
                        {
                            float fScale = 0.;
                            _stream.read(reinterpret_cast<char*>(&fScale), sizeof(float));
                            (*it)[0] = fScale;
                            _stream.read(reinterpret_cast<char*>(&fScale), sizeof(float));
                            (*it)[1] = fScale;
                        }
                        break;

                    case AnimationTarget_Weights:
                        {
                            (*it)[0].emplace<std::vector<float>>();
                            (*it)[1].emplace<std::vector<float>>();

                            auto& a1 = std::get<std::vector<float>>((*it)[0]);
                            auto& a2 = std::get<std::vector<float>>((*it)[1]);
                            a1.resize(uWeightCount);
                            a2.resize(uWeightCount);

                            _stream.read(reinterpret_cast<char*>(a1.data()), a1.size() * sizeof(float));
                            _stream.read(reinterpret_cast<char*>(a2.data()), a2.size() * sizeof(float));
                        }
                        break;

                    default:
                        break;
                }
            }
        }

        // read target values
        targetValues.resize(uKeyframeCount);
        for (auto it = targetValues.begin(); it != targetValues.end(); it++) {
            switch (bAnimationTarget) {
                case AnimationTarget_Rotation:
                    it->emplace<TRS::Quaternion>();
                    _stream.read(reinterpret_cast<char*>(&std::get<TRS::Quaternion>(*it)), sizeof(TRS::Quaternion));
                    break;

                case AnimationTarget_Translation:
                    it->emplace<TRS::Vector3<float>>();
                    _stream.read(reinterpret_cast<char*>(&std::get<TRS::Vector3<float>>(*it)), sizeof(TRS::Vector3<float>));
                    break;

                case AnimationTarget_Scale:
                    it->emplace<float>();
                    _stream.read(reinterpret_cast<char*>(&std::get<float>(*it)), sizeof(float));
                    break;

                case AnimationTarget_Weights:
                    {
                        it->emplace<std::vector<float>>();
                        auto& target = std::get<std::vector<float>>(*it);
                        target.resize(uWeightCount);
                        _stream.read(reinterpret_cast<char*>(target.data()), sizeof(float) * target.size());
                    }
                    break;

                default:
                    break;
            }
        }
    }

    void AnimationChannel::Write(std::ostream& _stream) const {
        _stream.write(reinterpret_cast<const char*>(&m_bStructure), sizeof(StructureIdentifier));
        _stream.write(reinterpret_cast<const char*>(&uNodePropertyId), sizeof(uint32_t));
        _stream.write(reinterpret_cast<const char*>(&uJointPropertyId), sizeof(uint32_t));
        _stream.write(reinterpret_cast<const char*>(&bAnimationTarget), sizeof(AnimationTarget));
        _stream.write(reinterpret_cast<const char*>(&bInterpolationType), sizeof(InterpolationType));
        _stream.write(reinterpret_cast<const char*>(&uWeightCount), sizeof(uint32_t));

        uint32_t uKeyframeCount = static_cast<uint32_t>(keyframes.size());
        _stream.write(reinterpret_cast<const char*>(&uKeyframeCount), sizeof(uint32_t));
        _stream.write(reinterpret_cast<const char*>(keyframes.data()), sizeof(float) * keyframes.size());

        // write tangents
        if (bInterpolationType == InterpolationType_CubicSpline) {
            for (auto it = tangents.begin(); it != tangents.end(); it++) {
                switch (bAnimationTarget) {
                    case AnimationTarget_Rotation:
                        _stream.write(reinterpret_cast<const char*>(&std::get<TRS::Quaternion>((*it)[0])), sizeof(TRS::Quaternion));
                        _stream.write(reinterpret_cast<const char*>(&std::get<TRS::Quaternion>((*it)[1])), sizeof(TRS::Quaternion));
                        break;

                    case AnimationTarget_Translation:
                        _stream.write(reinterpret_cast<const char*>(&std::get<TRS::Vector3<float>>((*it)[0])), sizeof(TRS::Vector3<float>));
                        _stream.write(reinterpret_cast<const char*>(&std::get<TRS::Vector3<float>>((*it)[1])), sizeof(TRS::Vector3<float>));
                        break;

                    case AnimationTarget_Scale:
                        _stream.write(reinterpret_cast<const char*>(&std::get<float>((*it)[0])), sizeof(float));
                        _stream.write(reinterpret_cast<const char*>(&std::get<float>((*it)[1])), sizeof(float));
                        break;

                    case AnimationTarget_Weights:
                        {
                            auto& a1 = std::get<std::vector<float>>((*it)[0]);
                            auto& a2 = std::get<std::vector<float>>((*it)[1]);
                            _stream.write(reinterpret_cast<const char*>(a1.data()), a1.size() * sizeof(float));
                            _stream.write(reinterpret_cast<const char*>(a2.data()), a2.size() * sizeof(float));
                        }
                        break;

                    default:
                        break;
                }
            }
        }

        // write target values
        for (auto it = targetValues.begin(); it != targetValues.end(); it++) {
            switch (bAnimationTarget) {
                case AnimationTarget_Rotation:
                    _stream.write(reinterpret_cast<const char*>(&std::get<TRS::Quaternion>(*it)), sizeof(TRS::Quaternion));
                    break;

                case AnimationTarget_Translation:
                    _stream.write(reinterpret_cast<const char*>(&std::get<TRS::Vector3<float>>(*it)), sizeof(TRS::Vector3<float>));
                    break;

                case AnimationTarget_Scale:
                    _stream.write(reinterpret_cast<const char*>(&std::get<float>(*it)), sizeof(float));
                    break;

                case AnimationTarget_Weights:
                    {
                        auto& w = std::get<std::vector<float>>(*it);
                        _stream.write(reinterpret_cast<const char*>(w.data()), sizeof(float) * w.size());
                    }
                    break;

                default:
                    break;
            }
        }
    }


    void MaterialPhong::Read(std::istream& _stream) {
        _stream.read(reinterpret_cast<char*>(&m_bStructure), sizeof(StructureIdentifier));
        if (!Verify()) {
            std::stringstream ss;
            ss << "das2::MaterialPhong: invalid magic number 0x" << std::setfill('0') << std::setw(2) << std::hex << m_bStructure;
            throw MagicValueException(ss.str());
        }
        szName.Read(_stream);

        _stream.read(reinterpret_cast<char*>(&vDiffuse), sizeof(TRS::Vector4<float>));
        _stream.read(reinterpret_cast<char*>(&vSpecular), sizeof(TRS::Vector4<float>));
        _stream.read(reinterpret_cast<char*>(&vEmission), sizeof(TRS::Vector4<float>));

        szDiffuseMapUri.Read(_stream);
        szSpecularMapUri.Read(_stream);
        szEmissionMapUri.Read(_stream);
    }

    void MaterialPhong::Write(std::ostream& _stream) const {
        _stream.write(reinterpret_cast<const char*>(&m_bStructure), sizeof(StructureIdentifier));
        szName.Write(_stream);
        _stream.write(reinterpret_cast<const char*>(&vDiffuse), sizeof(TRS::Vector4<float>));
        _stream.write(reinterpret_cast<const char*>(&vSpecular), sizeof(TRS::Vector4<float>));
        _stream.write(reinterpret_cast<const char*>(&vEmission), sizeof(TRS::Vector4<float>));
        
        szDiffuseMapUri.Write(_stream);
        szSpecularMapUri.Write(_stream);
        szEmissionMapUri.Write(_stream);
    }


    void MaterialPbr::Read(std::istream& _stream) {
        _stream.read(reinterpret_cast<char*>(&m_bStructure), sizeof(StructureIdentifier));
        if (!Verify()) {
            std::stringstream ss;
            ss << "das2::MaterialPbr: invalid magic number 0x" << std::setfill('0') << std::setw(2) << std::hex << m_bStructure;
            throw MagicValueException(ss.str());
        }

        szName.Read(_stream);
        _stream.read(reinterpret_cast<char*>(&vAlbedoFactor), sizeof(TRS::Vector4<float>));
        _stream.read(reinterpret_cast<char*>(&vEmissiveFactor), sizeof(TRS::Vector4<float>));
        _stream.read(reinterpret_cast<char*>(&fRoughness), sizeof(float));
        _stream.read(reinterpret_cast<char*>(&fMetallic), sizeof(float));
        _stream.read(reinterpret_cast<char*>(&fAmbientOcclusion), sizeof(float));

        szAlbedoMapUri.Read(_stream);
        szEmissionMapUri.Read(_stream);
        szMetallicMapUri.Read(_stream);
        szAmbientOcclusionMapUri.Read(_stream);
    }

    void MaterialPbr::Write(std::ostream& _stream) const {
        _stream.write(reinterpret_cast<const char*>(m_bStructure), sizeof(StructureIdentifier));
        szName.Write(_stream);
        _stream.write(reinterpret_cast<const char*>(&vAlbedoFactor), sizeof(TRS::Vector4<float>));
        _stream.write(reinterpret_cast<const char*>(&vEmissiveFactor), sizeof(TRS::Vector4<float>));
        _stream.write(reinterpret_cast<const char*>(&fRoughness), sizeof(float));
        _stream.write(reinterpret_cast<const char*>(&fMetallic), sizeof(float));
        _stream.write(reinterpret_cast<const char*>(&fAmbientOcclusion), sizeof(float));
        
        szAlbedoMapUri.Write(_stream);
        szEmissionMapUri.Write(_stream);
        szMetallicMapUri.Write(_stream);
        szAmbientOcclusionMapUri.Write(_stream);
    }
}
