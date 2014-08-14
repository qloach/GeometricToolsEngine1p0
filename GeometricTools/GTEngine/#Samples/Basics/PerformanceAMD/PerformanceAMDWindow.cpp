// Geometric Tools LLC, Redmond WA 98052
// Copyright (c) 1998-2014
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
// File Version: 1.0.0 (2014/08/11)

#include "PerformanceAMDWindow.h"
#include <iostream>

//----------------------------------------------------------------------------
PerformanceAMDWindow::~PerformanceAMDWindow()
{
}
//----------------------------------------------------------------------------
PerformanceAMDWindow::PerformanceAMDWindow(Parameters& parameters)
    :
    Window(parameters),
    mTextColor(0.0f, 0.0f, 0.0f, 1.0f),
    mPerformance(mEngine->GetDevice())
{
    std::string gtpath = mEnvironment.GetVariable("GTE_PATH");
    if (gtpath == "")
    {
        LogError("You must create the environment variable GTE_PATH.");
        parameters.created = false;
        return;
    }
    mEnvironment.Insert(gtpath + "/Samples/Basics/PerformanceAMD/Shaders/");
    mEnvironment.Insert(gtpath + "/Samples/#Data/");
    if (mEnvironment.GetPath("GenerateTexture.hlsl") == "")
    {
        LogError("Cannot find file GenerateTexture.hlsl.");
        parameters.created = false;
        return;
    }
    if (mEnvironment.GetPath("MedicineBag.jpg") == "")
    {
        LogError("Cannot find file MedicineBag.jpg.");
        parameters.created = false;
        return;
    }

    CreateCamera();
    CreateTextureGenerator();
    CreateScene();

    // Disable back-face culling.
    mNoCullingState.reset(new RasterizerState());
    mNoCullingState->cullMode = RasterizerState::CULL_NONE;
    mEngine->SetRasterizerState(mNoCullingState);

    mPerformance.SaveCounterInformation("AMD7970Counters.txt");
    mPerformance.Register(Listener);
    mPerformance.SetAllCounters();
}
//----------------------------------------------------------------------------
void PerformanceAMDWindow::OnIdle()
{
    MeasureTime();

    MoveCamera();
    UpdateConstants();

    mEngine->ClearBuffers();

    mPerformance.Profile([this]()
    {
        mEngine->Execute(mGenerateTexture, mNumXGroups, mNumYGroups, 1);
        mEngine->Draw(mTriangles);
    });

    // Compute the average measurements.  GetAverage allows you to access
    // the measurements during application run time.  SaveAverage calls
    // GetAverage and writes the results to a spreadsheet.
    std::vector<std::vector<AMDPerformance::Measurement>> measurements;
    if (mPerformance.GetNumProfileCalls() == 16)
    {
        mPerformance.GetAverage(measurements);
        mPerformance.SaveAverage("ProfileResults.csv");
    }

    DrawFrameRate(8, mYSize - 8, mTextColor);
    mEngine->DisplayColorBuffer(0);

    UpdateFrameCount();
}
//----------------------------------------------------------------------------
void PerformanceAMDWindow::CreateCamera()
{
    mCamera.SetFrustum(60.0f, GetAspectRatio(), 0.01f, 100.0f);
    Vector4<float> camPosition(0.0f, 0.0f, 4.0f, 1.0f);
    Vector4<float> camDVector(0.0f, 0.0f, -1.0f, 0.0f);
    Vector4<float> camUVector(0.0f, 1.0f, 0.0f, 0.0f);
    Vector4<float> camRVector = Cross(camDVector, camUVector);
    mCamera.SetFrame(camPosition, camDVector, camUVector, camRVector);
    EnableCameraMotion(0.005f, 0.002f, 2.0f, 2.0f);
}
//----------------------------------------------------------------------------
void PerformanceAMDWindow::CreateTextureGenerator()
{
    // Load the texture.
    std::string path = mEnvironment.GetPath("MedicineBag.jpg");
    mOriginalTexture.reset(WICFileIO::Load(path, false));

    // Create a texture that will be generated by a compute shader from the
    // original texture.
    unsigned int const width = mOriginalTexture->GetWidth();
    unsigned int const height = mOriginalTexture->GetHeight();
    mBlurredTexture.reset(new Texture2(DF_R8G8B8A8_UNORM, width, height));
    mBlurredTexture->SetUsage(Resource::SHADER_OUTPUT);

    // Create the compute shader for blurring the original texture.
    unsigned int const numThreads = 8;
    HLSLDefiner definer;
    definer.SetInt("DELTA", 3);
    definer.SetUnsignedInt("NUM_X_THREADS", numThreads);
    definer.SetUnsignedInt("NUM_Y_THREADS", numThreads);
    mNumXGroups = width / numThreads;
    mNumYGroups = height / numThreads;
    mGenerateTexture.reset(ShaderFactory::CreateCompute(
        mEnvironment.GetPath("GenerateTexture.hlsl"), definer));
    mGenerateTexture->Set("input", mOriginalTexture);
    mGenerateTexture->Set("output", mBlurredTexture);
}
//----------------------------------------------------------------------------
void PerformanceAMDWindow::CreateScene()
{
    struct Vertex
    {
        Vector3<float> position;
        Vector2<float> tcoord;
    };
    VertexFormat vformat;
    vformat.Bind(VA_POSITION, DF_R32G32B32_FLOAT, 0);
    vformat.Bind(VA_TEXCOORD, DF_R32G32_FLOAT, 0);
    unsigned int const numTriangles = 1024;
    unsigned int const numVertices = 3 * numTriangles;
    std::shared_ptr<VertexBuffer> vbuffer(new VertexBuffer(vformat,
        numVertices));

    // Randomly generate positions and texture coordinates.
    std::mt19937 mte;
    std::uniform_real_distribution<float> unirnd(0.0f, 1.0f);
    std::uniform_real_distribution<float> symrnd(-1.0f, 1.0f);
    Vertex* vertex = vbuffer->Get<Vertex>();
    for (unsigned int i = 0; i < numVertices; ++i)
    {
        for (int j = 0; j < 3; ++j)
        {
            vertex[i].position[j] = symrnd(mte);
        }
        for (int j = 0; j < 2; ++j)
        {
            vertex[i].tcoord[j] = unirnd(mte);
        }
    }

    // The vertices are not indexed.  Each consecutive triple is a triangle.
    std::shared_ptr<IndexBuffer> ibuffer(new IndexBuffer(IP_TRIMESH,
        numTriangles));

    // Use a standard texture effect.
    std::shared_ptr<Texture2Effect> effect(new Texture2Effect(
        mBlurredTexture, SamplerState::MIN_L_MAG_L_MIP_P,
        SamplerState::CLAMP, SamplerState::CLAMP));
    mPVWMatrix = effect->GetPVWMatrixConstant();

    mTriangles.reset(new Visual(vbuffer, ibuffer, effect));

    EnableObjectMotion();
}
//----------------------------------------------------------------------------
void PerformanceAMDWindow::UpdateConstants()
{
    Matrix4x4<float>& pvwMatrix = *mPVWMatrix->Get<Matrix4x4<float>>();
#if defined(GTE_USE_MAT_VEC)
    pvwMatrix = mCamera.GetProjectionViewMatrix()*mObjectTransform;
#else
    pvwMatrix = mObjectTransform*mCamera.GetViewMatrix();
#endif
    mEngine->Update(mPVWMatrix);
}
//----------------------------------------------------------------------------
void PerformanceAMDWindow::Listener(GPA_Logging_Type type,
    char const* message)
{
    switch (type)
    {
    case GPA_LOGGING_ERROR:
        std::cout << "GPA_LOGGING_ERROR: ";
        break;
    case GPA_LOGGING_MESSAGE:
        std::cout << "GPA_LOGGING_MESSAGE: ";
        break;
    case GPA_LOGGING_TRACE:
        std::cout << "GPA_LOGGING_TRACE: ";
        break;
    default:
        std::cout << "GPA_UNEXPECTED: ";
        break;
    }

    if (message)
    {
        std::cout << message;
    }
    std::cout << std::endl;
}
//----------------------------------------------------------------------------
