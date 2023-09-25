// das2: Improved DENG asset manager library
// licence: Apache, see LICENCE file
// file: Serializer.cpp - das2 serializer class implementation file
// author: Karl-Mihkel Ott

#include <das2/Serializer.h>
#include <sstream>
#include <iostream>

#include <boost/iostreams/filtering_streambuf.hpp>
#include <boost/iostreams/copy.hpp>
#include <boost/iostreams/filter/zstd.hpp>

namespace das2 {

    void Serializer::_StreamUncompressed(std::ostream& _stream) {
        m_model.buffer.Write(_stream);
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
        namespace bio = boost::iostreams;

        std::stringstream origin;
        _StreamUncompressed(origin);

        bio::filtering_streambuf<bio::input> out;

        if (m_model.header.bZlibLevel == 9)
            out.push(bio::zstd_compressor(bio::zstd_params(bio::zstd::best_compression)));
        else if (m_model.header.bZlibLevel == 1)
            out.push(bio::zstd_compressor(bio::zstd_params(bio::zstd::default_compression)));
        else if (m_model.header.bZlibLevel == 255)
            out.push(bio::zstd_compressor(bio::zstd_params(bio::zstd::best_speed)));

        out.push(origin);
        bio::copy(out, m_stream);
    }


    void Serializer::Serialize() {
        m_model.header.Write(m_stream);
        if (m_model.header.bZlibLevel)
            _StreamCompressed();
        else _StreamUncompressed(m_stream);
    }
}
