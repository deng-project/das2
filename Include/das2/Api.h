// das2: Improved DENG asset manager library
// licence: Apache, see LICENCE file
// file: Api.h - DAS2_API macro definition
// author: Karl-Mihkel Ott

#pragma once

#if defined(_MSC_BUILD) && !defined(DAS2_BUILD_STATIC)
    #ifdef DAS2_EXPORT_LIBRARY
        #define DAS2_API __declspec(dllexport)
    #else
        #define DAS2_API __declspec(dllimport)
    #endif
#else
    #define DAS2_API
#endif
