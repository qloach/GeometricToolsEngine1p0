// Geometric Tools LLC, Redmond WA 98052
// Copyright (c) 1998-2014
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
// File Version: 1.0.0 (2014/08/11)

#include "MinimumVolumeSphere3DWindow.h"

int main(int, char const*[])
{
    LogReporter reporter(
        "LogReport.txt",
        Logger::Listener::LISTEN_FOR_ALL,
        Logger::Listener::LISTEN_FOR_ALL,
        Logger::Listener::LISTEN_FOR_ALL,
        Logger::Listener::LISTEN_FOR_ALL);

    Window::Parameters parameters;
    parameters.title = L"MinimumVolumeSphere3DWindow";
    parameters.xOrigin = 0;
    parameters.yOrigin = 0;
    parameters.xSize = 1024;
    parameters.ySize = 1024;
    parameters.allowResize = true;
    parameters.deviceCreationFlags = D3D11_CREATE_DEVICE_DEBUG;

    MinimumVolumeSphere3DWindow* window =
        TheWindowSystem.Create<MinimumVolumeSphere3DWindow>(parameters);

    if (window)
    {
        HWND handle = window->GetHandle();
        ShowWindow(handle, SW_SHOW);
        UpdateWindow(handle);

        for (;;)
        {
            MSG msg;
            if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
            {
                if (msg.message == WM_QUIT)
                {
                    break;
                }

                TranslateMessage(&msg);
                DispatchMessage(&msg);
            }
            else
            {
                if (!window->IsMinimized())
                {
                    window->OnIdle();
                }
            }
        }

        TheWindowSystem.Destroy<MinimumVolumeSphere3DWindow>(window);
    }

    return 0;
}
