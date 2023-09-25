// das2: Improved DENG asset manager library
// licence: Apache, see LICENCE file
// file: DasDump.cpp - implementation of demo program to dump information about das file
// author: Karl-Mihkel Ott

#include <iostream>
#include <fstream>
#include <das2/demos/DasDump.h>
#include <das2/Exceptions.h>
#include <das2/Unserializer.h>

using namespace std;

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
        cout << "---- das2::Header ----\n";
        cout << "Author name: \"" << m_model.header.szAuthorName.CString() << "\"\n";
        cout << "Comment: \"" << m_model.header.szComment.CString() << "\"\n";
        cout << "Vertices count: " << m_model.header.uVerticesCount << '\n';
        cout << "Mesh count: " << m_model.header.uMeshCount << '\n';
        cout << "Animation count: " << m_model.header.uAnimationCount << '\n';
        cout << "Default scene index: " << m_model.header.uDefaultSceneIndex << '\n';
        cout << "zstd compression mode: " << (int)m_model.header.bZlibLevel << '\n';

        cout << "---- das2::Buffer ----\n";
        cout << "Buffer length: " << m_model.buffer.Size() << '\n';
    }
}


int main(int argv, char* argc[]) {
    ios_base::sync_with_stdio(false);
    cin.tie(0);

    if (argv < 2) {
        cout << "Usage: ./DasDump <filename>\n";
        return 0;
    }

    das2::DasDump dumper(argc[1]);
    dumper.PrintInfo();
    return 0;
}