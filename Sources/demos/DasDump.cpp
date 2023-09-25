// das2: Improved DENG asset manager library
// licence: Apache, see LICENCE file
// file: DasDump.cpp - implementation of demo program to dump information about das file
// author: Karl-Mihkel Ott

#include <iostream>
#include <fstream>
#include <das2/demos/DasDump.h>
#include <das2/Exceptions.h>
#include <das2/Unserializer.h>

namespace das2 {

    DasDump::DasDump(const std::string& _sFileName) {
        std::ifstream stream(_sFileName);
        
        try {
            Unserializer unserializer(stream);
            unserializer.Unserialize();
            m_model = unserializer.Get();
        }
        catch (const MagicValueException& _e) {
            std::cerr << "[MagicValueException] " << _e.what() << '\n';
            std::exit(1);
        }
        catch (const SerializerException& _e) {
            std::cerr << "[SerializerException] " << _e.what() << '\n';
            std::exit(1);
        }
    }


    void DasDump::PrintInfo() {
        std::cout << "---- das2::Header ----\n";
        std::cout << "Author name: \"" << m_model.header.szAuthorName.CString() << "\"\n";
        std::cout << "Comment: \"" << m_model.header.szComment.CString() << "\"\n";
        std::cout << "Vertices count: " << m_model.header.uVerticesCount << '\n';
        std::cout << "Mesh count: " << m_model.header.uMeshCount << '\n';
        std::cout << "Animation count: " << m_model.header.uAnimationCount << '\n';
        std::cout << "Default scene index: " << m_model.header.uDefaultSceneIndex << '\n';
        std::cout << "zstd compression mode: " << (int)m_model.header.bZlibLevel << '\n';

        std::cout << "---- das2::Buffer ----\n";
        std::cout << "Buffer length: " << m_model.buffer.Size() << '\n';
        std::cout.flush();
    }
}


int main(int argv, char* argc[]) {
    std::ios_base::sync_with_stdio(false);
    std::cin.tie(0);

    if (argv < 2) {
        std::cout << "Usage: ./DasDump <filename>\n";
        return 0;
    }

    das2::DasDump dumper(argc[1]);
    dumper.PrintInfo();
    return 0;
}