// das2: Improved DENG asset manager library
// licence: Apache, see LICENCE file
// file: Serializer.cpp - das2 serializer class implementation file
// author: Karl-Mihkel Ott

#include <das2/Serializer.h>
#include <sstream>
#include <iostream>

namespace das2 {

    void Serializer::_StreamUncompressed(std::ostream& _stream) {
        _StreamUncompressedArray(m_model.buffers, _stream);
        _StreamUncompressedArray(m_model.meshes, _stream);
        _StreamUncompressedArray(m_model.meshGroups, _stream);
        _StreamUncompressedArray(m_model.nodes, _stream);
        _StreamUncompressedArray(m_model.scenes, _stream);
        _StreamUncompressedArray(m_model.skeletonJoints, _stream);
        _StreamUncompressedArray(m_model.skeletons, _stream);
        _StreamUncompressedArray(m_model.animations, _stream);
        _StreamUncompressedArray(m_model.animationChannels, _stream);
        _StreamUncompressedArray(m_model.phongMaterials, _stream);
        _StreamUncompressedArray(m_model.pbrMaterials, _stream);
    }


    void Serializer::_StreamCompressed() {
        std::stringstream sstream;
        _StreamUncompressed(sstream);
        std::cout << "string stream size: " << sstream.str().size() << '\n';
    }


    void Serializer::Serialize() {
        m_stream << m_model.header;
        if (m_model.header.bZlibLevel)
            _StreamCompressed();
        else _StreamUncompressed(m_stream);
    }
}
