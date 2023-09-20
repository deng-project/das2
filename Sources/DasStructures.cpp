// das2: Improved DENG asset manager library
// licence: Apache, see LICENCE file
// file: dastool.cmake - dastool utility program CMake configuration file
// author: Karl-Mihkel Ott

#include <das2/DasStructures.h>

namespace das2 {

    std::ostream& operator<<(std::ostream& _stream, const BinString& _szData) {
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
        _stream.write(reinterpret_cast<const char*>(&_mesh.m_bStructure), sizeof(StructureIdentifier));
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
        
        // output morph targets
        uint32_t uMorphTargetCount = static_cast<uint32_t>(_mesh.morphTargets.size());
        _stream.write(reinterpret_cast<const char*>(&uMorphTargetCount), sizeof(uint32_t));
        if (_mesh.morphTargets.size())
            _stream.write(reinterpret_cast<const char*>(_mesh.morphTargets.data()), sizeof(MorphTarget) * _mesh.morphTargets.size());

        // output multiple LODs (recursive)
        uint32_t uMultipleLodCount = static_cast<uint32_t>(_mesh.multipleLods.size());
        _stream.write(reinterpret_cast<const char*>(&uMultipleLodCount), sizeof(uint32_t));
        for (size_t i = 0; i < _mesh.multipleLods.size(); i++) {
            _stream << _mesh.multipleLods[i];
        }

        return _stream;
    }


    std::ostream& operator<<(std::ostream& _stream, const MeshGroup& _meshGroup) {
        _stream.write(reinterpret_cast<const char*>(&_meshGroup.m_bStructure), sizeof(StructureIdentifier));
        _stream << _meshGroup.szName;

        uint32_t uMeshCount = static_cast<uint32_t>(_meshGroup.meshes.size());
        _stream.write(reinterpret_cast<const char*>(&uMeshCount), sizeof(uint32_t));
        _stream.write(reinterpret_cast<const char*>(_meshGroup.meshes.data()), sizeof(uint32_t) * _meshGroup.meshes.size());
        return _stream;
    }


    std::ostream& operator<<(std::ostream& _stream, const Node& _node) {
        _stream.write(reinterpret_cast<const char*>(&_node.m_bStructure), sizeof(StructureIdentifier));
        _stream << _node.szName;
        
        uint32_t uChildrenCount = static_cast<uint32_t>(_node.children.size());
        _stream.write(reinterpret_cast<const char*>(&uChildrenCount), sizeof(uint32_t));
        _stream.write(reinterpret_cast<const char*>(_node.children.data()), sizeof(uint32_t) * _node.children.size());

        _stream.write(reinterpret_cast<const char*>(&_node.uMeshGroupId), sizeof(uint32_t));
        _stream.write(reinterpret_cast<const char*>(&_node.uSkeletonId), sizeof(uint32_t));
        _stream.write(reinterpret_cast<const char*>(&_node.mCustomTransform), sizeof(TRS::Matrix4<float>));
        _stream.write(reinterpret_cast<const char*>(&_node.qRotation), sizeof(TRS::Quaternion));
        _stream.write(reinterpret_cast<const char*>(&_node.vTranslation), sizeof(TRS::Vector3<float>));
        _stream.write(reinterpret_cast<const char*>(&_node.fScale), sizeof(float));

        return _stream;
    }


    std::ostream& operator<<(std::ostream& _stream, const Scene& _scene) {
        _stream.write(reinterpret_cast<const char*>(&_scene.m_bStructure), sizeof(StructureIdentifier));
        _stream << _scene.szName;

        uint32_t uRootNodeCount = static_cast<uint32_t>(_scene.rootNodes.size());
        _stream.write(reinterpret_cast<const char*>(&uRootNodeCount), sizeof(uint32_t));
        _stream.write(reinterpret_cast<const char*>(_scene.rootNodes.data()), sizeof(uint32_t) * _scene.rootNodes.size());
        return _stream;
    }


    std::ostream& operator<<(std::ostream& _stream, const SkeletonJoint& _skeletonJoint) {
        _stream.write(reinterpret_cast<const char*>(&_skeletonJoint.m_bStructure), sizeof(StructureIdentifier));
        _stream << _skeletonJoint.szName;

        uint32_t uChildrenCount = static_cast<uint32_t>(_skeletonJoint.children.size());
        _stream.write(reinterpret_cast<const char*>(&uChildrenCount), sizeof(uint32_t));
        _stream.write(reinterpret_cast<const char*>(_skeletonJoint.children.data()), sizeof(uint32_t) * _skeletonJoint.children.size());
        
        _stream.write(reinterpret_cast<const char*>(&_skeletonJoint.mInverseBindPos), sizeof(TRS::Matrix4<float>));
        _stream.write(reinterpret_cast<const char*>(&_skeletonJoint.qRotation), sizeof(TRS::Quaternion));
        _stream.write(reinterpret_cast<const char*>(&_skeletonJoint.vTranslation), sizeof(TRS::Vector3<float>));
        _stream.write(reinterpret_cast<const char*>(&_skeletonJoint.fScale), sizeof(float));
        return _stream;
    }


    std::ostream& operator<<(std::ostream& _stream, const Skeleton& _skeleton) {
        _stream.write(reinterpret_cast<const char*>(&_skeleton.m_bStructure), sizeof(StructureIdentifier));
        _stream << _skeleton.szName;
        _stream.write(reinterpret_cast<const char*>(&_skeleton.uParent), sizeof(uint32_t));
        
        uint32_t uJointCount = static_cast<uint32_t>(_skeleton.joints.size());
        _stream.write(reinterpret_cast<const char*>(&uJointCount), sizeof(uint32_t));
        _stream.write(reinterpret_cast<const char*>(_skeleton.joints.data()), sizeof(uint32_t) * _skeleton.joints.size());
        return _stream;
    }

    std::ostream& operator<<(std::ostream& _stream, const Animation& _animation) {
        _stream.write(reinterpret_cast<const char*>(&_animation.m_bStructure), sizeof(StructureIdentifier));
        _stream << _animation.szName;

        uint32_t uAnimationChannelCount = static_cast<uint32_t>(_animation.animationChannels.size());
        _stream.write(reinterpret_cast<const char*>(&uAnimationChannelCount), sizeof(uint32_t));
        _stream.write(reinterpret_cast<const char*>(_animation.animationChannels.data()), sizeof(uint32_t) * _animation.animationChannels.size());
        return _stream;
    }

    std::ostream& operator<<(std::ostream& _stream, const AnimationChannel& _animationChannel) {
        _stream.write(reinterpret_cast<const char*>(&_animationChannel.m_bStructure), sizeof(StructureIdentifier));
        _stream.write(reinterpret_cast<const char*>(&_animationChannel.uNodePropertyId), sizeof(uint32_t));
        _stream.write(reinterpret_cast<const char*>(&_animationChannel.uJointPropertyId), sizeof(uint32_t));
        _stream.write(reinterpret_cast<const char*>(&_animationChannel.bAnimationTarget), sizeof(AnimationTarget));
        _stream.write(reinterpret_cast<const char*>(&_animationChannel.bInterpolationType), sizeof(InterpolationType));
        _stream.write(reinterpret_cast<const char*>(&_animationChannel.uWeightCount), sizeof(uint32_t));

        uint32_t uKeyframeCount = static_cast<uint32_t>(_animationChannel.keyframes.size());
        _stream.write(reinterpret_cast<const char*>(&uKeyframeCount), sizeof(uint32_t));
        _stream.write(reinterpret_cast<const char*>(_animationChannel.keyframes.data()), sizeof(float) * _animationChannel.keyframes.size());

        // write tangents
        if (_animationChannel.bInterpolationType == InterpolationType_CubicSpline) {
            for (auto it = _animationChannel.tangents.begin(); it != _animationChannel.tangents.end(); it++) {
                switch (_animationChannel.bAnimationTarget) {
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
        for (auto it = _animationChannel.targetValues.begin(); it != _animationChannel.targetValues.end(); it++) {
            switch (_animationChannel.bAnimationTarget) {
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

        return _stream;
    }


    std::ostream& operator<<(std::ostream& _stream, const MaterialPhong& _phongMaterial) {
        _stream.write(reinterpret_cast<const char*>(&_phongMaterial.m_bStructure), sizeof(StructureIdentifier));
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
        _stream.write(reinterpret_cast<const char*>(_pbrMaterial.m_bStructure), sizeof(StructureIdentifier));
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
