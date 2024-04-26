#include "Game/Game.h"

Game::Game(_In_ PCWSTR pszGameName)
    : m_pszGameName(pszGameName)
    , m_mainWindow(std::make_unique<MainWindow>())
    , m_renderer(std::make_unique<Renderer>())
    , ModelPtrContainer()
    , MaterialPtrContainer()
    , LightPtrContainer()
    , LightCount(0u)
{
}

HRESULT Game::Initialize(_In_ HINSTANCE hInstance, _In_ INT nCmdShow)
{
    HRESULT hr = m_mainWindow->InitWindow(hInstance, nCmdShow, m_pszGameName);
    if (FAILED(hr))
    {
        return hr;
    }

    hr = m_renderer->InitDevice(m_mainWindow->GetWindow());
    return hr;
}

INT Game::Run()
{
    LARGE_INTEGER startTime;
    LARGE_INTEGER endTime;
    LARGE_INTEGER frequency;
    LARGE_INTEGER elapsedMsc;

    QueryPerformanceCounter(&startTime);
    QueryPerformanceFrequency(&frequency);

    // Main message loop
    MSG msg = { 0 };
    while (WM_QUIT != msg.message)
    {
        if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
        else
        {
            QueryPerformanceCounter(&endTime);
            elapsedMsc.QuadPart = endTime.QuadPart - startTime.QuadPart;
            elapsedMsc.QuadPart *= 1000000;
            elapsedMsc.QuadPart /= frequency.QuadPart;

            QueryPerformanceFrequency(&frequency);
            QueryPerformanceCounter(&startTime);

            FLOAT deltaTime = static_cast<FLOAT>(elapsedMsc.QuadPart) / 1000000.0f;

            m_renderer->HandleInput(m_mainWindow->GetDirections(), m_mainWindow->GetMouseRelativeMovement(), m_mainWindow->GetMouseRightClick(), deltaTime);
            m_mainWindow->ResetMouseMovement();
            m_renderer->Update(deltaTime);
            m_renderer->Render();
        }
    }

    return static_cast<INT>(msg.wParam);
}

PCWSTR Game::GetGameName() const
{
    return m_pszGameName;
}

std::unique_ptr<MainWindow>& Game::GetWindow()
{
    return m_mainWindow;
}

std::unique_ptr<Renderer>& Game::GetRenderer()
{
    return m_renderer;
}

bool Game::Addmodel(LPCWSTR modelname, LPCWSTR vs, LPCWSTR ps, LPCWSTR path)
{
    if (!ModelPtrContainer.contains(modelname)) {
        ModelPtrContainer[modelname] = std::make_shared<Model>(path);

        if (FAILED(m_renderer->AddModel(modelname, ModelPtrContainer[modelname])))
            return 0;
        if (FAILED(m_renderer->SetVertexShaderOfModel(modelname, vs)))
            return 0;
        if (FAILED(m_renderer->SetPixelShaderOfModel(modelname, ps)))
            return 0;
    }
    return 1;
}

bool Game::AddCharacter(LPCWSTR charname, LPCWSTR path)
{
    if (!ModelPtrContainer.contains(charname))
    {
        ModelPtrContainer[charname] = std::make_shared<Character>(path);

        if (FAILED(m_renderer->AddModel(charname, ModelPtrContainer[charname])))
            return 0;
        if (FAILED(m_renderer->SetVertexShaderOfModel(charname, L"SkinningVertexShader")))
            return 0;
        if (FAILED(m_renderer->SetPixelShaderOfModel(charname, L"SkinningPixelShader")))
            return 0;
    }
    return true;
}

bool Game::AddLight(LPCWSTR LightType, LPCWSTR lightname, XMFLOAT4 location, XMFLOAT4 color)
{
    if (!LightPtrContainer.contains(lightname))
    {
        if (LightType == L"PointLight")
        {
            LightPtrContainer[lightname] = std::make_shared<PointLight>(location, color);         
        }
        else if (LightType == L"RotatingPointLight")
        {
           LightPtrContainer[lightname] = std::make_shared<RotatingPointLight>(location, color);
        }
    }

    if (FAILED(m_renderer->AddPointLight(LightCount, LightPtrContainer[lightname])))
        return 0;
    LightCount++;
}

std::shared_ptr<Model> Game::GetModelPtr(LPCWSTR name)
{
    return ModelPtrContainer[name];
}


std::shared_ptr<Material> Game::MaterialPtr(LPCWSTR name)
{
    return MaterialPtrContainer[name];
}

std::shared_ptr<Light> Game::LightPtr(LPCWSTR name)
{
    return LightPtrContainer[name];
}

