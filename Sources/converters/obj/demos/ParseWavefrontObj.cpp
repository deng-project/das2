// das2: Improved DENG asset manager library
// licence: Apache, see LICENCE file
// file: ParseWavefrontObj.cpp - wavefront obj parser application class entry point
// author: Karl-Mihkel Ott

#include <fstream>
#include <iostream>
#include <das2/converters/obj/demos/ParseWavefrontObj.h>

namespace das2 {
    namespace demo {
        
        ParseWavefrontObj::ParseWavefrontObj(const char* _szFileName) {
            std::ifstream stream(_szFileName);

            try {
                m_pUnserializer = new obj::Unserializer(stream);
            } catch (cvar::SyntaxErrorException& e) {
                std::cerr << "[SyntaxErrorException] " << e.what() << '\n';
                std::exit(1);
            } catch (cvar::UnexpectedEOFException& e) {
                std::cerr << "[UnexpectedEOFException] " << e.what() << '\n';
                std::exit(2);
            }

            stream.close();
        }


        ParseWavefrontObj::~ParseWavefrontObj() {
            delete m_pUnserializer;
        }


        void ParseWavefrontObj::PrintOutput() {
            // for each geometry vertex
            auto object = m_pUnserializer->Get();
            for (auto it = object.vertices.geometricVertices.begin(); it != object.vertices.geometricVertices.end(); it++) {
                std::cout << "v " << it->first << ' ' << it->second << ' ' << it->third << ' ' << it->fourth << '\n';                 
            }

            std::cout << "-----------------------\n";
            // for each vertex normal
            for (auto it = object.vertices.vertexNormals.begin(); it != object.vertices.vertexNormals.end(); it++)
                std::cout << "vn " << it->first << ' ' << it->second << ' ' << it->third << '\n';

            std::cout << "-----------------------\n";
            // for each texture vertex
            for (auto it = object.vertices.textureVertices.begin(); it != object.vertices.textureVertices.end(); it++)
                std::cout << "vt " << it->first << ' ' << it->second << ' ' << it->third << '\n';

            std::cout << "-----------------------\n";
            // groups
            for (auto it = object.groups.begin(); it != object.groups.end(); it++) {
                std::cout << "***********************\n";
                std::cout << "szMaterialName: \"" << it->szMaterialName.CString() << "\"\n";
                std::cout << "groupNames: [";
                for (auto it2 = it->groupNames.begin(); it2 != it->groupNames.end(); it2++) {
                    std::cout << it2->CString();
                    if (std::next(it2) != it->groupNames.end())
                        std::cout << ',';
                }
                std::cout << "]\n";
                std::cout << "bSmoothing: " << (it->bSmoothing ? "true\n" : "false\n");
                std::cout << "elements:\n";

                for (auto it2 = it->elements.faces.begin(); it2 != it->elements.faces.end(); it2++) {
                    std::cout << "f ";
                    for (auto it3 = it2->begin(); it3 != it2->end(); it3++) {
                        std::cout << it3->x << '/' << it3->y << '/' << it3->z;
                        if (std::next(it3) == it2->end())
                            std::cout << '\n';
                        else std::cout << ' ';
                    }
                }
            }
        }

    }
}


int main(int argc, char* argv[]) {
    std::ios_base::sync_with_stdio(false);
    std::cin.tie(0);

    if (argc < 2) {
        std::cout << "Usage: ./ParseWavefrontObj <file>\n";
        return 3;
    }

    try {
        das2::demo::ParseWavefrontObj application(argv[1]);
        application.PrintOutput();
    }
    catch (const cvar::SyntaxErrorException& e) {
        std::cerr << e.what() << '\n';
    }
    catch (const cvar::UnexpectedEOFException& e) {
        std::cerr << e.what() << '\n';
    }
    return 0;
}
