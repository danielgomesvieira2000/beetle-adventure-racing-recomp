//
// RT64
//

#pragma once

#include "shared/rt64_hlsl.h"

#ifdef HLSL_CPU
namespace interop {
#endif
    struct VideoInterfaceCB {
        float2 videoResolution;
        float2 textureResolution;
        float gamma;
        uint divotFilter;       // BAR seam fix: 1 = apply the VI divot median, 0 = off
        float divotThreshold;   // outlier gate for the divot (linear 0..1)
    };
#ifdef HLSL_CPU
};
#endif