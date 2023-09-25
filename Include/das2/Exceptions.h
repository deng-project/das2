// das2: Improved DENG asset manager library
// licence: Apache, see LICENCE file
// file: Exceptions.h - header file containing possible das2 exeptions
// author: Karl-Mihkel Ott

#include <exception>
#include <string>

namespace das2 {

    class ConvertionException : std::exception {
        private:
            std::string m_sWhatMessage;

        public:
            ConvertionException(const std::string& _sWhat = "Unknown exception") :
                m_sWhatMessage(_sWhat) {}

            const char* what() const noexcept override {
                return m_sWhatMessage.c_str();
            }
    };


    class ConvertionLogicException : public ConvertionException {
        public:
            ConvertionLogicException(const std::string& _sWhat = "Unknown exception") :
                ConvertionException(_sWhat) {}
    };


    class MagicValueException : std::exception {
        private:
            std::string m_sWhatMessage;

        public:
            MagicValueException(const std::string& _sWhat = "Unknown exception") :
                m_sWhatMessage(_sWhat) {}

            const char* what() const noexcept override {
                return m_sWhatMessage.c_str();
            }
    };
}