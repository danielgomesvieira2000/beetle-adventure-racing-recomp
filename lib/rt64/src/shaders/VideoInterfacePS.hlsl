//
// RT64
//

#include "shared/rt64_video_interface.h"

[[vk::push_constant]] ConstantBuffer<VideoInterfaceCB> gConstants : register(b0);
Texture2D<float4> gInput : register(t1);
SamplerState gSampler : register(s2);

// Limit texture sampling to the area the VI can sample of the texture.

float4 SampleInput(float2 uv) {
    const float2 LowerRight = gConstants.videoResolution / gConstants.textureResolution;
    const float2 HalfPixel = float2(0.5f, 0.5f) / gConstants.textureResolution;
    float2 outsideBorder = step(LowerRight, uv);
    float4 sampledColor = gInput.SampleLevel(gSampler, clamp(uv, HalfPixel, LowerRight - HalfPixel), 0);
    float4 gammaCorrectedColor = pow(sampledColor, gConstants.gamma);
    gammaCorrectedColor.rgb *= max(1.0f - outsideBorder.x - outsideBorder.y, 0.0f);
    gammaCorrectedColor.a = 1.0f;
    return gammaCorrectedColor;
}

// BAR seam fix (TODO #2B): N64 VI "divot" filter. Per-channel horizontal median-of-3 at native
// input-texel spacing — fills the 1px see-through holes the RDP leaves where coplanar/abutting
// polygons meet (hardware fills these at scanout). Gated so it only replaces a center texel that is
// an outlier vs BOTH horizontal neighbors while those neighbors agree, preserving genuine 1px detail.
float4 SampleInputDivot(float2 uv) {
    float4 c = SampleInput(uv);
    if (gConstants.divotFilter == 0) {
        return c;
    }
    const float2 dx = float2(1.0f / gConstants.textureResolution.x, 0.0f);
    float3 l = SampleInput(uv - dx).rgb;
    float3 r = SampleInput(uv + dx).rgb;
    float3 med = max(min(l, r), min(max(l, r), c.rgb));   // per-channel median of {l, c, r}
    float3 neighborsAgree = step(abs(l - r), gConstants.divotThreshold);
    float3 centerIsOutlier = step(gConstants.divotThreshold, abs(c.rgb - med));
    float3 useMedian = neighborsAgree * centerIsOutlier;
    return float4(lerp(c.rgb, med, useMedian), c.a);
}

//
// Sourced from https://www.shadertoy.com/view/csX3RH
//
float4 PixelAntialiasing(float2 uv) {
    float2 uvTexspace = uv * gConstants.videoResolution;
    float2 seam = floor(uvTexspace + 0.5f);
    uvTexspace = (uvTexspace - seam) / fwidth(uvTexspace) + seam;
    uvTexspace = clamp(uvTexspace, seam - 0.5f, seam + 0.5f);
    return SampleInputDivot(uvTexspace / gConstants.textureResolution);
}

float4 PSMain(in float4 pos : SV_Position, in float2 uv : TEXCOORD0) : SV_TARGET {
#ifdef PIXEL_ANTIALIASING
    return PixelAntialiasing(uv);
#else
    return SampleInputDivot((uv / gConstants.textureResolution) * gConstants.videoResolution);
#endif
}