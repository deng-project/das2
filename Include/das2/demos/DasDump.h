// das2: Improved DENG asset manager library
// licence: Apache, see LICENCE file
// file: DasDump.h - header of demo program to dump information about das file
// author: Karl-Mihkel Ott

#pragma once

#include <string>
#include <das2/DasStructures.h>

namespace das2 {

    class DasDump {
        private:
            Model m_model;
        public:
            DasDump(const std::string& _sFileName);
            void PrintInfo();
    };
}