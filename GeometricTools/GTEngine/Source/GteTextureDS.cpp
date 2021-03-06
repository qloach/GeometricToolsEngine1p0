// Geometric Tools LLC, Redmond WA 98052
// Copyright (c) 1998-2014
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
// File Version: 1.0.0 (2014/08/11)

#include "GTEnginePCH.h"
#include "GteTextureDS.h"
using namespace gte;

//----------------------------------------------------------------------------
TextureDS::TextureDS(DFType format, unsigned int width, unsigned int height,
    bool createStorage)
    :
    Texture2(DataFormat::IsDepth(format) ? format : DF_D24_UNORM_S8_UINT,
        width, height, false, createStorage)
{
    mType = GT_TEXTURE_DS;
}
//----------------------------------------------------------------------------
