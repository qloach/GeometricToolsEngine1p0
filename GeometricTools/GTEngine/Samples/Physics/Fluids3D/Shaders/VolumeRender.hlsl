// Geometric Tools LLC, Redmond WA 98052
// Copyright (c) 1998-2014
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
// File Version: 1.0.0 (2014/08/11)

cbuffer PVWMatrix
{
    float4x4 pvwMatrix;
};

struct VS_INPUT
{
    float3 modelPosition : POSITION;
    float3 modelTCoord : TEXCOORD0;
};

struct VS_OUTPUT
{
    float3 vertexTCoord : TEXCOORD0;
    float4 clipPosition : SV_POSITION;
};

VS_OUTPUT VSMain (VS_INPUT input)
{
    VS_OUTPUT output;
#if defined(GTE_USE_MAT_VEC)
    output.clipPosition = mul(pvwMatrix, float4(input.modelPosition, 1.0f));
#else
    output.clipPosition = mul(float4(input.modelPosition, 1.0f), pvwMatrix);
#endif
    output.vertexTCoord = input.modelTCoord;
    return output;
}

Texture3D<float4> volumeTexture;
SamplerState trilinearClampSampler;

struct PS_INPUT
{
    float3 vertexTCoord : TEXCOORD0;
};

struct PS_OUTPUT
{
    float4 pixelColor0 : SV_TARGET0;
};

PS_OUTPUT PSMain(PS_INPUT input)
{
    PS_OUTPUT output;
    float4 color = volumeTexture.Sample(trilinearClampSampler, input.vertexTCoord);
    output.pixelColor0 = float4(color.w*color.rgb, 0.5f*color.w);
    return output;
};
