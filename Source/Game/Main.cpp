#include "Common.h"
#include "Model.h"
#include "Cube/Cube.h"
#include "Cube/RotatingCube.h"

#include "Game/Game.h"

INT WINAPI wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPWSTR lpCmdLine, _In_ INT nCmdShow)
{

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
    
    // add location of target obj/fbx model
    if (!game->Addmodel(L"Stone", L"VS", L"PS", L"../../Data/Stone/Stone.obj"))
    {
        return 0;
    }
    
    game->GetModelPtr(L"Stone")->Scale(0.5f, 0.5f, 0.5f);
    game->GetModelPtr(L"Stone")->Translate(XMVectorSet(10.0f, -2.0f, 5.0f, 0.0f));



  
    ///*
    //rotatingPointLight
    std::shared_ptr<RotatingPointLight> rotatingPointLight = std::make_shared<RotatingPointLight>(
        XMFLOAT4(0.0f, 10.0f, -5.0f, 0.0f), color);
    if (FAILED(game->GetRenderer()->AddPointLight(0u, rotatingPointLight)))
        return 0;
    
    
    //Cube default Textures
    std::shared_ptr<Material> cubeMaterial = std::make_shared<Material>(L"CubeMaterial");
    cubeMaterial->pDiffuse = std::make_shared<Texture>(L"../../Data/Cube/diffuse.png");
    cubeMaterial->pNormal = std::make_shared<Texture>(L"../../Data/Cube/normal.png");
    
    /*
    // Rotating Point Light Cube
    std::shared_ptr<RotatingCube> rotatingLightCube = std::make_shared<RotatingCube>(color);
    rotatingLightCube->Translate(XMVectorSet(0.0f, 10.0f, -5.0f, 0.0f));
    if (FAILED(game->GetRenderer()->AddRenderable(L"RotatingLightCube", rotatingLightCube)))
        return 0;
    if (FAILED(game->GetRenderer()->SetVertexShaderOfRenderable(L"RotatingLightCube", L"VS")))
        return 0;
    if (FAILED(game->GetRenderer()->SetPixelShaderOfRenderable(L"RotatingLightCube", L"PSSolid")))
        return 0;
    //rotatingLightCube->AddMaterial(cubeMaterial);

    // Point Light
    XMStoreFloat4(&color, Colors::AntiqueWhite);
    std::shared_ptr<PointLight> pointLight = std::make_shared<PointLight>(XMFLOAT4(-5.0f, 15.0f, -5.0f, 1.0f), color);
    if (FAILED(game->GetRenderer()->AddPointLight(1u, pointLight)))
        return 0;

    // Point Light Cube
    std::shared_ptr<Cube> cube = std::make_shared<Cube>(color);
    cube->Translate(XMVectorSet(-5.0f, 15.0f, -5.0f, 0.0f));
    if (FAILED(game->GetRenderer()->AddRenderable(L"Cube", cube)))
        return 0;
    if (FAILED(game->GetRenderer()->SetVertexShaderOfRenderable(L"Cube", L"VS")))
        return 0;
    if (FAILED(game->GetRenderer()->SetPixelShaderOfRenderable(L"Cube", L"PSSolid")))
        return 0;
    
    
    //base cube
    std::shared_ptr<Cube> mcube = std::make_shared<Cube>(color);
    mcube->Translate(XMVectorSet(-5.0f, 3.0f, 0.0f, 0.0f));
    if (FAILED(game->GetRenderer()->AddRenderable(L"MCube", mcube)))
        return 0;
    if (FAILED(game->GetRenderer()->SetVertexShaderOfRenderable(L"MCube", L"VS")))
        return 0;
    if (FAILED(game->GetRenderer()->SetPixelShaderOfRenderable(L"MCube", L"PS")))
        return 0;
    mcube->AddMaterial(cubeMaterial);
    */
    
    // Skinning Model
    std::shared_ptr<Character> Char = std::make_shared<Character>(L"../../Data/BobLampClean/boblampclean.md5mesh");
    Char->Scale(0.1f, 0.1f, 0.1f);
    Char->RotateX(XM_PIDIV2);
    if (FAILED(game->GetRenderer()->AddModel(L"Char", Char)))
        return 0;
    if (FAILED(game->GetRenderer()->SetVertexShaderOfModel(L"Char", L"SkinningVertexShader")))
        return 0;
    if (FAILED(game->GetRenderer()->SetPixelShaderOfModel(L"Char", L"SkinningPixelShader")))
        return 0;

    game->GetRenderer()->GetCamera()->SetCharacter(Char);
    
    /*
    std::shared_ptr<Model> Stone = std::make_shared<Model>(L"../../Data/Stone/Stone.obj");
    //Stone->Scale(0.5f, 0.5f, 0.5f);
    Stone->Translate(XMVectorSet(10.0f, -2.0f, 5.0f, 0.0f));
    if (FAILED(game->GetRenderer()->AddRenderable(L"Stone", Stone)))
        return 0;
    if (FAILED(game->GetRenderer()->SetVertexShaderOfRenderable(L"Stone", L"VS")))
        return 0;
    if (FAILED(game->GetRenderer()->SetPixelShaderOfRenderable(L"Stone", L"PS")))
        return 0;
     */
    
    
    if (FAILED(game->Initialize(hInstance, nCmdShow)))
        return 0;
   
    return game->Run();
}