// das2: Improved DENG asset manager library
// licence: Apache, see LICENCE file
// file: ConvertWavefrontObj.h - implementation of demo program to convert wavefront obj files into das2 files
// author: Karl-Mihkel Ott

#include <iostream>
#include <fstream>
#include <das2/Exceptions.h>
#include <das2/converters/obj/demos/ConvertWavefrontObj.h>

namespace das2 {
    namespace demo {

        ConvertWavefrontObj::ConvertWavefrontObj(const char* _szDestFileName, const char* _szInputFileName, uint8_t _bZlibCompression) {
            std::ifstream m_fileInputStream(_szInputFileName, std::ios_base::binary);

            try {
                m_pUnserializer = new das2::obj::Unserializer(m_fileInputStream);
            }
            catch (const cvar::SyntaxErrorException& e) {
                std::cerr << "[SyntaxErrorException] " << e.what() << '\n';
                std::exit(1);
            }
            catch (const cvar::UnexpectedEOFException& e) {
                std::cerr << "[UnexpectedEOFException] " << e.what() << '\n';
                std::exit(2);
            }
            
            das2::obj::Object obj = m_pUnserializer->Get();
            m_fileInputStream.close();

            try {
                m_pDasConverter = new das2::obj::DasConverter(obj, "", "", _bZlibCompression);
            }
            catch (const das2::ConvertionLogicException& e) {
                std::cerr << "[ConvertionLogicException] " << e.what() << '\n';
                std::exit(3);
            }

            das2::Model model = m_pDasConverter->GetModel();

            std::ofstream m_fileDestinationStream (_szDestFileName, std::ios_base::binary);
            m_pSerializer = new das2::Serializer(m_fileDestinationStream, model);
            m_pSerializer->Serialize();

            m_fileDestinationStream.close();
        }


        ConvertWavefrontObj::~ConvertWavefrontObj() {
            delete m_pSerializer;
            delete m_pDasConverter;
            delete m_pUnserializer;
        }
    }
}


int main (int argc, char* argv[]) {
    uint8_t bZlibLevel = 0;

    if (argc < 3) {
        std::cout << "Usage: ./ConvertWavefrontObj <input-file> <output-file> [zlib level]\n";
        return 0;
    }

    if (argc >= 4) {
        bZlibLevel = static_cast<uint8_t>(std::stoi(argv[3]));
    }

    static_cast<void>(das2::demo::ConvertWavefrontObj(argv[2], argv[1], bZlibLevel));
    
    return 0;
}