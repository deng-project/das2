// das2: Improved DENG asset manager library
// licence: Apache, see LICENCE file
// file: Data.h - header of data structure declarations for Wavefront Obj files
// author: Karl-Mihkel Ott

#pragma once

#include <cstdint>
#include <array>
#include <vector>
#include <trs/Vector.h>
#include <trs/Matrix.h>
#include <trs/Points.h>

namespace das2 {
    namespace obj {
        struct Vertices {
            std::vector<TRS::Vector4<float>> geometricVertices;     // v
            std::vector<TRS::Vector3<float>> paramSpaceVertices;    // vp
            std::vector<TRS::Vector3<float>> vertexNormals;         // vn
            std::vector<TRS::Vector3<float>> textureVertices;       // vt
        };

        // cstype property
        enum class FreeFormGeometryType {
            Unknown,
            BasisMatrix,
            Bezier,
            BSpline,
            Cardinal,
            Taylor
        };

        // free-form curve/surface attributes
        struct FreeFormAttributes {
            FreeFormGeometryType geomType = FreeFormGeometryType::Unknown;
            TRS::Vector2<float> vDegree;
            std::array<TRS::Matrix4<float>, 2> mBasis; // [0] - u dir; [1] - v dir
            TRS::Vector2<float> vStep;
        };

        struct Curve {
            TRS::Point2D<float> params;
            std::vector<uint32_t> controlVertices;
        };

        struct Surface {
            std::pair<float, float> fS;
            std::pair<float, float> fT;
            std::vector<TRS::Point3D<uint32_t>> controlVertices;
        };

        // elements are referencing vertices
        struct Elements {
            std::vector<uint32_t> points;                   // points       (p)
            std::vector<TRS::Point2D<uint32_t>> lines;      // line         (l)
            std::vector<TRS::Point3D<uint32_t>> faces;      // face         (f)
            std::vector<Curve> curves;                      // curve        (curv)
            std::vector<std::vector<uint32_t>> curves2D;    // 2D curve     (curv2)
            std::vector<Surface> surfaces;                  // surface      (surf)
        };


        struct Curve2D {
            std::pair<float, float> fU;
            uint32_t uCurv2D;
        };

        struct FreeFormBodyAttributes {
            std::vector<std::vector<float>> parametersU;            // parameter values (parm) u coordinate
            std::vector<std::vector<float>> parametersV;            // parameter values (parm) v coordinate
            std::vector<std::vector<Curve2D>> outerTrimmingLoops;     // outer trimming loop (trim)
            std::vector<std::vector<Curve2D>> innerTrimmingLoops;     // inner trimming loop (hole)
            std::vector<std::vector<Curve2D>> specialCurves;          // special curve (scrv)
            std::vector<std::vector<uint32_t>> specialPoints;       // special point (sp)
        };

        // connectivity (con)
        
        struct Grouping {
            // group name       (g)
            // smoothing group  (s)
            // merging group    (mg)
            // object name      (o)
        };


        struct RenderAttributes {
            // bevel interpolation              (bevel)
            // color interpolation              (c_interp)
            // dissolve interpolation           (d_interp)
            // level of detail                  (lod)
            // material name                    (usemtl)
            // material library                 (mtllib)
            // shadow casting                   (shadow_obj)
            // ray tracing                      (trace_obj)
            // curve approximation technique    (ctech)
            // surface approximation technique  (stech)
        };
    }
}
