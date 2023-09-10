// das2: Improved DENG asset manager library
// licence: Apache, see LICENCE file
// file: Data.h - header of data structure declarations for Wavefront Obj files
// author: Karl-Mihkel Ott

#pragma once

#include <cstdint>
#include <array>
#include <vector>
#include <unordered_map>

#include <trs/Vector.h>
#include <trs/Matrix.h>
#include <trs/Points.h>

#include <das2/DasStructures.h>

namespace das2 {
    namespace obj {
        enum class KeywordToken : unsigned char {
            GeometryVertex,         // v
            TextureVertex,          // vt
            VertexNormal,           // vn
            ParameterSpaceVertex,   // vp
            CSType,                 // cstype
            Degree,                 // deg
            BasisMatrix,            // bmat
            StepSize,               // step
            Point,                  // p
            Line,                   // l
            Face,                   // f
            Curve,                  // curv
            Curve2D,                // curv2
            Surface,                // surf
            ParameterValues,        // parm
            OuterTrimmingLoop,      // trim
            InnerTrimmingLoop,      // hole
            SpecialCurve,           // scrv
            SpecialPoint,           // sp
            EndStatement,           // end
            Connect,                // con
            GroupName,              // g
            SmoothingGroup,         // s
            MergingGroup,           // mg
            ObjectName,             // o
            BevelInterpolation,     // bevel
            ColorInterpolation,     // c_interp
            DissolveInterpolation,  // d_interp,
            LevelOfDetail,          // lod
            MaterialName,           // usemtl
            MaterialLibrary,        // mtllib
            ShadowCasting,          // shadow_obj
            RayTracing,             // trace_obj
            CurveApprox,            // ctech
            SurfaceApprox,          // stech
        };

        struct Vertices {
            std::vector<TRS::Vector4<float>> geometricVertices;     // v 
            std::vector<TRS::Vector3<float>> vertexNormals;         // vn
            std::vector<TRS::Vector3<float>> textureVertices;       // vt
        };

        // elements are referencing vertices
        struct Elements {
            std::vector<std::vector<TRS::Point3D<uint32_t>>> faces;      // f
        };

        struct Group {
            BinString szMaterialName = "";          // (structural)
            std::vector<BinString> groupNames;      // g
            BinString sMaterialName;                // usemtl
            bool bSmoothing;                        // s
            Elements elements;                      // (structural)
        };

        enum class MTLToken : unsigned char {
            Phong_Ambient,          // Ka
            Phong_Diffuse,          // Kd
            Phong_Specular,         // Ks
            Phong_SpecularExp,      // Ns
            Transparency,           // d
            Phong_AmbientMapUri,    // map_Ka
            Phong_DiffuseMapUri,    // map_Kd
            Phong_SpecularMapUri,   // map_Ks
            Phong_SpecularExpUri,   // map_Ns
            TransparencyMapUri,     // map_d

            PBR_Roughness,          // Pr
            PBR_Metallic,           // Pm
            PBR_Sheen,              // Ps
            PBR_ClearcoatThickness, // Pc
            PBR_ClearcoatRoughness, // Pcr
            PBR_Emissive,           // Ke
            PBR_Anisotropy,         // aniso
            PBR_AnisotropyRotation, // anisor
            PBR_RoughnessMapUri,    // map_Pr
            PBR_MetallicMapUri,     // map_Pm
            PBR_SheenMapUri,        // map_Ps
            PBR_EmissionMapUri,     // map_Ke
            PBR_NormalMapUri        // norm
        };

        struct Material {
            /* Blinn/Phong shading model */
            TRS::Vector3<float> vKa;    // Blinn/Phong shading ambient color
            TRS::Vector3<float> vKd;    // Blinn/Phong shading diffuse color
            TRS::Vector3<float> vKs;    // Blinn/Phong shading specular color
            float fNs = 1.f;            // Blinn/Phong shading specular exponent
            float fD = 1.f;             // Transparency parameter
            BinString szMapKaUri = "";  // Blinn/Phong shading ambient map uri
            BinString szMapKdUri = "";  // Blinn/Phong shading diffuse map uri
            BinString szMapKsUri = "";  // Blinn/Phong shading specular map uri
            BinString szMapNsUri = "";  // Blinn/Phong shading specular exponent map uri
            BinString szMapDUri = "";   // Transparency parameter map uri

            /* PBR shading model */
            float fPr;                          // PBR roughness parameter
            float fPm;                          // PBR metallic parameter
            float fSheen;                       // PBR sheen parameter
            float fPc;                          // PBR clearcoat thickness
            float fPcr;                         // PBR clearcoat roughness
            TRS::Vector3<float> vEmissive;      // PBR emissive component
            float fAnisotropy;                  // PBR anisotropy parameter
            float fAnisor;                      // PBR anisotropy rotation

            BinString szMapPrUri = "";          // PBR roughness map uri
            BinString szMapPmUri = "";          // PBR metallic map uri
            BinString szMapSheenUri = "";       // PBR sheen map uri
            BinString szEmissionMapUri = "";    // PBR emissive map
            BinString szNormalMapUri = "";      // PBR normal map
        };


        struct Object {
            Vertices vertices;
            std::vector<Group> groups;
            std::unordered_map<BinString, Material> materials;
        };
    }
}
