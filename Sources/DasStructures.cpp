// das2: Improved DENG asset manager library
// licence: Apache, see LICENCE file
// file: dastool.cmake - dastool utility program CMake configuration file
// author: Karl-Mihkel Ott

#include <das2/DasStructures.h>

namespace das2 {

    std::ostream& operator<<(std::ostream& _stream, BinString& _szData) {
        const uint16_t uLength = _szData.Length();
        const cvar::hash_t hshString = _szData.Hash();

        _stream.write(reinterpret_cast<const char*>(&uLength), sizeof(uint16_t));
        _stream.write(reinterpret_cast<const char*>(&hshString), sizeof(cvar::hash_t));
        _stream.write(_szData.CString(), uLength + 1);
        return _stream;
    }
    

    std::ostream& operator<<(std::ostream& _stream, const Header& _header) {
        _stream.write(reinterpret_cast<const char*>(&_header.m_uMagic), sizeof(uint64_t));
        _stream << _header.szAuthorName;
        _stream << _header.szComment;
        _stream.write(reinterpret_cast<const char*>(&_header.uVerticesCount), sizeof(uint32_t));
        _stream.write(reinterpret_cast<const char*>(&_header.uMeshCount), sizeof(uint32_t));
        _stream.write(reinterpret_cast<const char*>(&_header.uAnimationCount), sizeof(uint32_t));
        _stream.write(reinterpret_cast<const char*>(&_header.uDefaultSceneIndex), sizeof(uint32_t));
        _stream.write(reinterpret_cast<const char*>(&_header.uDefaultSceneIndex), sizeof(uint32_t));
        _stream.write(reinterpret_cast<const char*>(&_header.bZlibLevel), sizeof(uint8_t));
        return _stream;
    }


    std::ostream& operator<<(std::ostream& _stream, const Buffer& _buffer) {
        _stream.write(reinterpret_cast<const char*>(&_buffer.m_bStructure), sizeof(StructureIdentifier));
        _stream.write(reinterpret_cast<const char*>(&_buffer.m_uLength), sizeof(uint32_t));
        _stream.write(_buffer.m_pData, _buffer.m_uLength);
        return _stream;
    }

    std::ostream& operator<<(std::ostream& _stream, const MorphTarget& _morphTarget) {
        _stream.write(reinterpret_cast<const char*>(&_morphTarget), sizeof(MorphTarget));
        return _stream;
    }


    std::ostream& operator<<(std::ostream& _stream, const Mesh& _mesh) {
        _stream.write(reinterpret_cast<const char*>(&_mesh.bStructure), sizeof(StructureIdentifier));
        _stream.write(reinterpret_cast<const char*>(&_mesh.uIndexBufferId), sizeof(uint32_t));
        _stream.write(reinterpret_cast<const char*>(&_mesh.uIndexBufferOffset), sizeof(uint32_t));
        _stream.write(reinterpret_cast<const char*>(&_mesh.uDrawCount), sizeof(uint32_t));
        _stream.write(reinterpret_cast<const char*>(&_mesh.uPositionVertexBufferId), sizeof(uint32_t));
        _stream.write(reinterpret_cast<const char*>(&_mesh.uPositionVertexBufferOffset), sizeof(uint32_t));
        _stream.write(reinterpret_cast<const char*>(&_mesh.uSurfaceNormalBufferId), sizeof(uint32_t));
        _stream.write(reinterpret_cast<const char*>(&_mesh.uSurfaceNormalBufferOffset), sizeof(uint32_t));
        _stream.write(reinterpret_cast<const char*>(&_mesh.arrUVBufferIds), sizeof(std::array<uint32_t, 8>));
        _stream.write(reinterpret_cast<const char*>(&_mesh.arrUVBufferOffsets), sizeof(std::array<uint32_t, 8>));
        _stream.write(reinterpret_cast<const char*>(&_mesh.uColorMultiplierId), sizeof(uint32_t));
        _stream.write(reinterpret_cast<const char*>(&_mesh.uColorMultiplierOffset), sizeof(uint32_t));
        _stream.write(reinterpret_cast<const char*>(&_mesh.arrSkeletalJointIndexBufferIds), sizeof(std::array<uint32_t, 8>));
        _stream.write(reinterpret_cast<const char*>(&_mesh.arrSkeletalJointIndexBufferOffsets), sizeof(std::array<uint32_t, 8>));
        _stream.write(reinterpret_cast<const char*>(&_mesh.arrSkeletalJointWeightBufferIds), sizeof(std::array<uint32_t, 8>));
        _stream.write(reinterpret_cast<const char*>(&_mesh.arrSkeletalJointWeightBufferOffsets), sizeof(std::array<uint32_t, 8>));
        _stream.write(reinterpret_cast<const char*>(&_mesh.bMaterialType), sizeof(MaterialType));
        _stream.write(reinterpret_cast<const char*>(&_mesh.uMaterialId), sizeof(uint32_t));
        _stream.write(reinterpret_cast<const char*>(&_mesh.uMorphTargetCount), sizeof(uint32_t));

        for (uint32_t i = 0; i < _mesh.uMorphTargetCount; i++) {
            _stream << _mesh.pMorphTargets[i];
        }

        _stream.write(reinterpret_cast<const char*>(&_mesh.uMultipleLodCount), sizeof(uint32_t));
        
        for (uint32_t i = 0; i < _mesh.uMultipleLodCount; i++) {
            _stream << _mesh.pMultipleLods[i];
        }
        return _stream;
    }


    std::ostream& operator<<(std::ostream& _stream, const MeshGroup& _meshGroup) {
        _stream.write(reinterpret_cast<const char*>(&_meshGroup.bStructure), sizeof(StructureIdentifier));
        _stream << _meshGroup.szName;
        _stream.write(reinterpret_cast<const char*>(&_meshGroup.uMeshCount), sizeof(uint32_t));
        _stream.write(reinterpret_cast<const char*>(&_meshGroup.pMeshes), sizeof(uint32_t) * static_cast<size_t>(_meshGroup.uMeshCount));
        return _stream;
    }


    std::ostream& operator<<(std::ostream& _stream, const Node& _node) {
        _stream.write(reinterpret_cast<const char*>(&_node), sizeof(Node));
        return _stream;
    }


    std::ostream& operator<<(std::ostream& _stream, const Scene& _scene) {
        _stream.write(reinterpret_cast<const char*>(&_scene.bStructure), sizeof(StructureIdentifier));
        _stream << _scene.szName;
        _stream.write(reinterpret_cast<const char*>(_scene.uRootNodeCount), sizeof(uint32_t));
        _stream.write(reinterpret_cast<const char*>(_scene.pRootNodes), sizeof(uint32_t) * static_cast<size_t>(_scene.uRootNodeCount));
        return _stream;
    }


    std::ostream& operator<<(std::ostream& _stream, const SkeletonJoint& _skeletonJoint) {
        _stream.write(reinterpret_cast<const char*>(&_skeletonJoint.bStructure), sizeof(StructureIdentifier));
        _stream << _skeletonJoint.szName;
        _stream.write(reinterpret_cast<const char*>(&_skeletonJoint.uChildrenCount), sizeof(uint32_t));
        _stream.write(reinterpret_cast<const char*>(&_skeletonJoint.pChildren), sizeof(uint32_t) * static_cast<size_t>(_skeletonJoint.uChildrenCount));
        _stream.write(reinterpret_cast<const char*>(&_skeletonJoint.mInverseBindPos), sizeof(TRS::Matrix4<float>));
        _stream.write(reinterpret_cast<const char*>(&_skeletonJoint.qRotation), sizeof(TRS::Quaternion));
        _stream.write(reinterpret_cast<const char*>(&_skeletonJoint.vTranslation), sizeof(TRS::Vector3<float>));
        _stream.write(reinterpret_cast<const char*>(&_skeletonJoint.fScale), sizeof(float));
        return _stream;
    }


    std::ostream& operator<<(std::ostream& _stream, const Skeleton& _skeleton) {
        _stream.write(reinterpret_cast<const char*>(&_skeleton.bStructure), sizeof(StructureIdentifier));
        _stream << _skeleton.szName;
        _stream.write(reinterpret_cast<const char*>(&_skeleton.uParent), sizeof(uint32_t));
        _stream.write(reinterpret_cast<const char*>(&_skeleton.uJointCount), sizeof(uint32_t));
        _stream.write(reinterpret_cast<const char*>(&_skeleton.pJoints), sizeof(uint32_t) * static_cast<size_t>(_skeleton.uJointCount));
        return _stream;
    }

    std::ostream& operator<<(std::ostream& _stream, const Animation& _animation) {
        _stream.write(reinterpret_cast<const char*>(&_animation.bStructure), sizeof(StructureIdentifier));
        _stream << _animation.szName;
        _stream.write(reinterpret_cast<const char*>(&_animation.uAnimationChannelCount), sizeof(uint32_t));
        _stream.write(reinterpret_cast<const char*>(&_animation.pAnimationChannels), sizeof(uint32_t) * static_cast<size_t>(_animation.uAnimationChannelCount));
        return _stream;
    }

    std::ostream& operator<<(std::ostream& _stream, const AnimationChannel& _animationChannel) {
        _stream.write(reinterpret_cast<const char*>(&_animationChannel.bStructure), sizeof(StructureIdentifier));
        _stream.write(reinterpret_cast<const char*>(&_animationChannel.uNodePropertyId), sizeof(uint32_t));
        _stream.write(reinterpret_cast<const char*>(&_animationChannel.uJointPropertyId), sizeof(uint32_t));
        _stream.write(reinterpret_cast<const char*>(&_animationChannel.bAnimationTarget), sizeof(AnimationTarget));
        _stream.write(reinterpret_cast<const char*>(&_animationChannel.bInterpolationType), sizeof(InterpolationType));
        _stream.write(reinterpret_cast<const char*>(&_animationChannel.uKeyframeCount), sizeof(uint32_t));
        _stream.write(reinterpret_cast<const char*>(&_animationChannel.uWeightCount), sizeof(uint32_t));
        _stream.write(reinterpret_cast<const char*>(&_animationChannel.pKeyframes), sizeof(float) * static_cast<size_t>(_animationChannel.uKeyframeCount));

        if (_animationChannel.bInterpolationType == InterpolationType_CubicSpline) {
            switch (_animationChannel.bAnimationTarget) {
                case AnimationTarget_Weights:
                    _stream.write(_animationChannel.pTangents, 2 * sizeof(float) * static_cast<size_t>(_animationChannel.uWeightCount * _animationChannel.uKeyframeCount));
                    break;

                case AnimationTarget_Translation:
                    _stream.write(_animationChannel.pTangents, 2 * sizeof(TRS::Vector3<float>) * static_cast<size_t>(_animationChannel.uKeyframeCount));
                    break;

                case AnimationTarget_Rotation:
                    _stream.write(_animationChannel.pTangents, 2 * sizeof(TRS::Quaternion) * static_cast<size_t>(_animationChannel.uKeyframeCount));
                    break;

                case AnimationTarget_Scale:
                    _stream.write(_animationChannel.pTangents, 2 * sizeof(float) * static_cast<size_t>(_animationChannel.uKeyframeCount));
                    break;

                default:
                    break;
            }
        }


        switch (_animationChannel.bAnimationTarget) {
            case AnimationTarget_Weights:
                _stream.write(_animationChannel.pTargetValues, sizeof(float) * static_cast<size_t>(_animationChannel.uWeightCount * _animationChannel.uKeyframeCount));
                break;

            case AnimationTarget_Translation:
                _stream.write(_animationChannel.pTargetValues, sizeof(TRS::Vector3<float>) * static_cast<size_t>(_animationChannel.uKeyframeCount));
                break;

            case AnimationTarget_Rotation:
                _stream.write(_animationChannel.pTargetValues, sizeof(TRS::Quaternion) * static_cast<size_t>(_animationChannel.uKeyframeCount));
                break;

            case AnimationTarget_Scale:
                _stream.write(_animationChannel.pTargetValues, sizeof(float) * static_cast<size_t>(_animationChannel.uKeyframeCount));
                break;

            default:
                break;
        }

        return _stream;
    }


    std::ostream& operator<<(std::ostream& _stream, const MaterialPhong& _phongMaterial) {
        _stream.write(reinterpret_cast<const char*>(&_phongMaterial.bStructure), sizeof(StructureIdentifier));
        _stream << _phongMaterial.szName;
        _stream.write(reinterpret_cast<const char*>(&_phongMaterial.vDiffuse), sizeof(TRS::Vector4<float>));
        _stream.write(reinterpret_cast<const char*>(&_phongMaterial.vSpecular), sizeof(TRS::Vector4<float>));
        _stream.write(reinterpret_cast<const char*>(&_phongMaterial.vEmission), sizeof(TRS::Vector4<float>));
        _stream << _phongMaterial.szDiffuseMapUri;
        _stream << _phongMaterial.szSpecularMapUri;
        _stream << _phongMaterial.szEmissionMapUri;

        return _stream;
    }

    std::ostream& operator<<(std::ostream& _stream, const MaterialPbr& _pbrMaterial) {
        _stream.write(reinterpret_cast<const char*>(_pbrMaterial.bStructure), sizeof(StructureIdentifier));
        _stream << _pbrMaterial.szName;
        _stream.write(reinterpret_cast<const char*>(&_pbrMaterial.vAlbedoFactor), sizeof(TRS::Vector4<float>));
        _stream.write(reinterpret_cast<const char*>(&_pbrMaterial.vEmissiveFactor), sizeof(TRS::Vector4<float>));
        _stream.write(reinterpret_cast<const char*>(&_pbrMaterial.fRoughness), sizeof(float));
        _stream.write(reinterpret_cast<const char*>(&_pbrMaterial.fMetallic), sizeof(float));
        _stream.write(reinterpret_cast<const char*>(&_pbrMaterial.fAmbientOcclusion), sizeof(float));
        _stream << _pbrMaterial.szAlbedoMapUri;
        _stream << _pbrMaterial.szEmissionMapUri;
        _stream << _pbrMaterial.szRoughnessMapUri;
        _stream << _pbrMaterial.szMetallicMapUri;
        _stream << _pbrMaterial.szAmbientOcclusionMapUri;
        return _stream;
    }
}
