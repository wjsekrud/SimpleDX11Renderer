#include "PotatoRenderer.h"
#include "Game/Game.h"

using namespace System;
using namespace System::Windows::Forms;


void main(array<String^>^ args) {
    /*
    std::unique_ptr<Game> game = std::make_unique<Game>(L"Potest Engine 0.0.1");

    std::shared_ptr<SkinningVertexShader> skinningVertexShader = std::make_shared<SkinningVertexShader>(L"Shaders/SkinningShader.fx", "VS", "vs_5_0");
    if (FAILED(game->GetRenderer()->AddVertexShader(L"SkinningVertexShader", skinningVertexShader)))
        return 0;
    std::shared_ptr<PixelShader> skinningPixelShader = std::make_shared<PixelShader>(L"Shaders/SkinningShader.fx", "PS", "ps_5_0");
    if (FAILED(game->GetRenderer()->AddPixelShader(L"SkinningPixelShader", skinningPixelShader)))
        return 0;
    std::shared_ptr<VertexShader> vertexShader = std::make_shared<VertexShader>(L"Shaders/Shader.fx", "VS", "vs_5_0");
    if (FAILED(game->GetRenderer()->AddVertexShader(L"VS", vertexShader)))
        return 0;
    std::shared_ptr<PixelShader> pixelShader = std::make_shared<PixelShader>(L"Shaders/Shader.fx", "PS", "ps_5_0");
    if (FAILED(game->GetRenderer()->AddPixelShader(L"PS", pixelShader)))
        return 0;
    std::shared_ptr<PixelShader> pixelSolidShader = std::make_shared<PixelShader>(L"Shaders/Shader.fx", "PSSolid", "ps_5_0");
    if (FAILED(game->GetRenderer()->AddPixelShader(L"PSSolid", pixelSolidShader)))
        return 0;
    // Color
    XMFLOAT4 color;
    XMStoreFloat4(&color, Colors::AntiqueWhite);

    game->AddLight(L"PointLight", L"PointLight1", XMFLOAT4(5.0f, 10.0f, -5.0f, 0.0f), color);
    //game->AddLight(L"RotatingPointLight", L"PointLight2", XMFLOAT4(0.0f, 0.0f, -5.0f, 0.0f), color);

    if (!game->Addmodel(L"Stone", L"VS", L"PS", L"../../Data/Stone/Stone.obj"))
    {
        return 0;
    }

    if (!game->Addmodel(L"room", L"VS", L"PS", L"../../Data/untitled.obj"))
    {
        return 0;
    }

    if (FAILED(game->Initialize(hInstance, nCmdShow)))
        return 0;

    return game->Run();
    */
	Application::EnableVisualStyles();
	Application::SetCompatibleTextRenderingDefault(false);
	Frontend::MyForm form;
	Application::Run(% form);

	


}