// Geometric Tools LLC, Redmond WA 98052
// Copyright (c) 1998-2014
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
// File Version: 1.0.0 (2014/08/11)

#include "MinimumAreaBox2DWindow.h"

//----------------------------------------------------------------------------
MinimumAreaBox2DWindow::~MinimumAreaBox2DWindow()
{
}
//----------------------------------------------------------------------------
MinimumAreaBox2DWindow::MinimumAreaBox2DWindow(Parameters& parameters)
    :
    Window(parameters),
    mTextColor(0.0f, 0.0f, 0.0f, 1.0f),
    mVertices(NUM_POINTS)
{
#if 1
    // Randomly generated points.
    std::mt19937 mte;
    std::uniform_real_distribution<float> rnd(0.0f, 1.0f);
    Vector2<float> center(0.5f*mXSize, 0.5f*mYSize);
    Vector2<float> extent(0.25f*mXSize, 0.125f*mYSize);
    Vector2<float> axis[2] = {
        Vector2<float>(1.0f, 1.0f),
        Vector2<float>(-1.0f, 1.0f)
    };
    Normalize(axis[0]);
    Normalize(axis[1]);
    rnd(mte);
    for (auto& v : mVertices)
    {
        float angle = rnd(mte) * (float)GTE_C_TWO_PI;
        float radius = rnd(mte);
        float u[2] = { extent[0] * cos(angle), extent[1] * sin(angle) };
        v = center + radius * (u[0] * axis[0] + u[1] * axis[1]);
    }

    mMAB2(NUM_POINTS, &mVertices[0], 0.0f, mMinimalBox);
    mHull = mMAB2.GetHull();

    mMAB2.DoOrderNSqr(NUM_POINTS, &mVertices[0],
        static_cast<int>(mHull.size()), &mHull[0], mSlowMinimalBox);
#endif

#if 0
    std::ifstream input("convexpolygon.txt");
    int numVertices;
    input >> numVertices;
    mVertices.resize(numVertices);
    std::vector<int> indices(numVertices);
    for (int i = 0; i < numVertices; ++i)
    {
        input >> mVertices[i][0];
        input >> mVertices[i][1];
        indices[i] = i;
    }
    input.close();

#if 1
    mMAB2(numVertices, &mVertices[0], numVertices, &indices[0], mMinimalBox);
    mHull = mMAB2.GetHull();

    mMAB2.DoOrderNSqr(numVertices, &mVertices[0], numVertices, &indices[0],
        mSlowMinimalBox);
#else
    mMAB2(numVertices, &mVertices[0], 0, nullptr, mMinimalBox);
    mHull = mMAB2.GetHull();

    mMAB2.DoOrderNSqr(numVertices, &mVertices[0], 0, nullptr,
        mSlowMinimalBox);
#endif

#endif

#if 0
    // This data set leads to an intermediate bounding box for which point 0
    // supports two edges of the box and point 5 supports the other two
    // edges.  Point 0 and point 5 are at box corners that are diagonally
    // opposite.  The example led to fixing a bug in the update of the
    // extremes when the intermediate box is rotated.
    std::ifstream input("projection.raw", std::ios::in | std::ios::binary);
    std::vector<Vector2<double>> temp(9);
    mVertices.resize(9);
    input.read((char*)&temp[0], 9 * 2 * sizeof(double));
    input.close();
    for (int k = 0; k < 9; ++k)
    {
        mVertices[k][0] = 256.0f + 4096.0f * (float)temp[k][0];
        mVertices[k][1] = 256.0f + 4096.0f * (float)temp[k][1];
    }

    mMAB2(9, &mVertices[0], 0, nullptr, mMinimalBox);
    mHull = mMAB2.GetHull();
    mMAB2.DoOrderNSqr(9, &mVertices[0],
        static_cast<int>(mHull.size()), &mHull[0], mSlowMinimalBox);
#endif

    mOverlay.reset(new OverlayEffect(mXSize, mYSize, mXSize, mYSize,
        SamplerState::MIN_P_MAG_P_MIP_P, SamplerState::CLAMP,
        SamplerState::CLAMP, true));

    mScreenTexture.reset(new Texture2(DF_R8G8B8A8_UNORM, mXSize, mYSize));
    mScreenTexture->SetUsage(Resource::DYNAMIC_UPDATE);
    mOverlay->SetTexture(mScreenTexture);
    mScreenTexels = mScreenTexture->Get<unsigned int>();
}
//----------------------------------------------------------------------------
void MinimumAreaBox2DWindow::OnDisplay()
{
    // Clear the screen to white.
    memset(mScreenTexture->GetData(), 0xFF, mScreenTexture->GetNumBytes());

    // Draw the convex hull.
    int i0, i1, x0, y0, x1, y1;
    int numHull = static_cast<int>(mHull.size());
    for (i0 = numHull - 1, i1 = 0; i1 < numHull; i0 = i1++)
    {
        x0 = static_cast<int>(mVertices[mHull[i0]][0] + 0.5f);
        y0 = static_cast<int>(mVertices[mHull[i0]][1] + 0.5f);
        x1 = static_cast<int>(mVertices[mHull[i1]][0] + 0.5f);
        y1 = static_cast<int>(mVertices[mHull[i1]][1] + 0.5f);
        DrawLine(x0, y0, x1, y1, 0xFF0000FF);
    }

    // Draw the O(n^2) minimum area box.
    Vector2<float> vertex[4];
    mSlowMinimalBox.GetVertices(vertex);
    for (i0 = 3, i1 = 0; i1 < 4; i0 = i1++)
    {
        x0 = static_cast<int>(vertex[i0][0] + 0.5f);
        y0 = static_cast<int>(vertex[i0][1] + 0.5f);
        x1 = static_cast<int>(vertex[i1][0] + 0.5f);
        y1 = static_cast<int>(vertex[i1][1] + 0.5f);
        DrawLine(x0, y0, x1, y1, 0xFF00FF00);
    }

    // Draw the minimum area box.
    mMinimalBox.GetVertices(vertex);
    for (i0 = 3, i1 = 0; i1 < 4; i0 = i1++)
    {
        x0 = static_cast<int>(vertex[i0][0] + 0.5f);
        y0 = static_cast<int>(vertex[i0][1] + 0.5f);
        x1 = static_cast<int>(vertex[i1][0] + 0.5f);
        y1 = static_cast<int>(vertex[i1][1] + 0.5f);
        DrawLine(x0, y0, x1, y1, 0xFFFF0000);
    }

    // Draw the input points.
    for (auto const& v : mVertices)
    {
        int x = static_cast<int>(v[0] + 0.5f);
        int y = static_cast<int>(v[1] + 0.5f);
        DrawPoint(x, y, 0xFF808080);
    }

    mEngine->Update(mScreenTexture);
    mEngine->Draw(mOverlay);
    mEngine->DisplayColorBuffer(0);
}
//----------------------------------------------------------------------------
void MinimumAreaBox2DWindow::DrawPoint(int x, int y, unsigned int color)
{
    y = mYSize - 1 - y;
    for (int dy = -1; dy <= 1; ++dy)
    {
        for (int dx = -1; dx <= 1; ++dx)
        {
            mScreenTexels[(x + dx) + mXSize*(y + dy)] = color;
        }
    }
}
//----------------------------------------------------------------------------
void MinimumAreaBox2DWindow::DrawLine(int x0, int y0, int x1, int y1,
    unsigned int color)
{
    ImageUtility2::DrawLine(x0, y0, x1, y1,
        [this, color](int x, int y)
        {
            y = mYSize - 1 - y;
            mScreenTexels[x + mXSize*y] = color;
        }
    );
}
//----------------------------------------------------------------------------
