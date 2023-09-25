// das2: Improved DENG asset manager library
// licence: Apache, see LICENCE file
// file: Unserializer.h - das2 unserializer class implementation file
// author: Karl-Mihkel Ott

#include <sstream>

#include <das2/Exceptions.h>
#include <das2/Unserializer.h>

#include <boost/iostreams/filtering_streambuf.hpp>
#include <boost/iostreams/copy.hpp>
#include <boost/iostreams/filter/zstd.hpp>

namespace das2 {

    Unserializer::Unserializer(std::istream& _stream) :
        m_stream(_stream) {}


    void Unserializer::_ReadCompressed() {
        namespace bio = boost::iostreams;

        bio::filtering_streambuf<bio::input> in;
        if (m_model.header.bZlibLevel == 9)
            in.push(bio::zstd_decompressor(bio::zstd::best_compression));
        else if (m_model.header.bZlibLevel == 1)
            in.push(bio::zstd_decompressor(bio::zstd::best_speed));
        else
            in.push(bio::zstd_decompressor(bio::zstd::default_compression));

        in.push(m_stream);
        std::stringstream ss;
        bio::copy(in, ss);

        _ReadUncompressed(ss);
    }


    void Unserializer::_ReadUncompressed(std::istream& _stream) {
        // read all structures
        bool bBufferRead = false;
        while (!_stream.eof() && _stream.peek() != -1) {
            StructureIdentifier bIdentifier = static_cast<StructureIdentifier>(m_stream.peek());
            switch (bIdentifier) {
                case StructureIdentifier_Buffer:
                    if (bBufferRead)
                        throw SerializerException("Multiple buffers detected in das2 buffer. The specification states that only one buffer is allowed.");
                    
                    bBufferRead = true;
                    m_model.buffer.Read(_stream);
                    break;

                case StructureIdentifier_Mesh:
                    m_model.meshes.emplace_back();
                    m_model.meshes.back().Read(_stream);
                    break;

                case StructureIdentifier_MeshGroup:
                    m_model.meshGroups.emplace_back();
                    m_model.meshGroups.back().Read(_stream);
                    break;

                case StructureIdentifier_Node:
                    m_model.nodes.emplace_back();
                    m_model.nodes.back().Read(_stream);
                    break;

                case StructureIdentifier_Scene:
                    m_model.scenes.emplace_back();
                    m_model.scenes.back().Read(_stream);
                    break;

                case StructureIdentifier_SkeletonJoint:
                    m_model.skeletonJoints.emplace_back();
                    m_model.skeletonJoints.back().Read(_stream);
                    break;

                case StructureIdentifier_Skeleton:
                    m_model.skeletons.emplace_back();
                    m_model.skeletons.back().Read(_stream);
                    break;

                case StructureIdentifier_Animation:
                    m_model.animations.emplace_back();
                    m_model.animations.back().Read(_stream);
                    break;

                case StructureIdentifier_AnimationChannel:
                    m_model.animationChannels.emplace_back();
                    m_model.animationChannels.back().Read(_stream);
                    break;

                case StructureIdentifier_MaterialPhong:
                    m_model.phongMaterials.emplace_back();
                    m_model.phongMaterials.back().Read(_stream);
                    break;

                case StructureIdentifier_MaterialPbr:
                    m_model.pbrMaterials.emplace_back();
                    m_model.pbrMaterials.back().Read(_stream);
                    break;

                default:
                    throw SerializerException("Invalid magic byte");
                    break;
            }
        }
    }


    void Unserializer::Unserialize() {
        // read header
        m_model.header.Read(m_stream);

        // check if zlib compression is used
        if (m_model.header.bZlibLevel != 0)
            _ReadCompressed();
        else _ReadUncompressed(m_stream);
    }
}