// Geometric Tools LLC, Redmond WA 98052
// Copyright (c) 1998-2014
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
// File Version: 1.0.0 (2014/08/11)

Texture2D baseTexture;
SamplerState baseSampler;

struct PS_INPUT
{
    float4 vertexColor : COLOR0;
    float2 vertexTCoord : TEXCOORD0;
};

struct PS_OUTPUT
{
    float4 pixelColor0 : SV_TARGET0;
};

PS_OUTPUT PSMain(PS_INPUT input)
{
    PS_OUTPUT output;
    float4 gray = baseTexture.Sample(baseSampler, input.vertexTCoord).rrra;
    output.pixelColor0 = input.vertexColor * gray;
    return output;
};
