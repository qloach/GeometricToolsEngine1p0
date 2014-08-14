// Geometric Tools LLC, Redmond WA 98052
// Copyright (c) 1998-2014
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
// File Version: 1.0.0 (2014/08/11)

Texture2D myTexture;
SamplerState mySampler;

struct PS_INPUT
{
    float2 vertexTCoord : TEXCOORD0;
};

struct PS_OUTPUT
{
    float4 pixelColor0 : SV_TARGET0;
};

PS_OUTPUT PSMain(PS_INPUT input)
{
    PS_OUTPUT output;
    float4 color = myTexture.Sample(mySampler, input.vertexTCoord);
        if (color.z > 0.0f)
        {
            output.pixelColor0 = float4(0.0f, color.z, 0.0f, 1.0f);
        }
        else
        {
            output.pixelColor0 = float4(0.0f, 0.0f, 1.0f, 1.0f);
        }
    return output;
};
