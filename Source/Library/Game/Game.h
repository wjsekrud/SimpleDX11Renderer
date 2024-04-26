#pragma once

#include "Common.h"

#include "Window/MainWindow.h"
#include "Renderer/Renderer.h"

class Game final
{
public:
	Game(_In_ PCWSTR pszGameName);
	~Game() = default;

	HRESULT Initialize(_In_ HINSTANCE hInstance, _In_ INT nCmdShow);
	INT Run();

	PCWSTR GetGameName() const;
	std::unique_ptr<MainWindow>& GetWindow();
	std::unique_ptr<Renderer>& GetRenderer();

	bool Addmodel(LPCWSTR modelname, LPCWSTR vs, LPCWSTR ps, LPCWSTR path);
	bool AddCharacter(LPCWSTR charname, LPCWSTR path);
	bool AddLight(LPCWSTR LightType, LPCWSTR lightname, XMFLOAT4 location, XMFLOAT4 color);

	std::shared_ptr<Model> GetModelPtr(LPCWSTR name);
	std::shared_ptr<Character> GetCharPtr(LPCWSTR name);
	std::shared_ptr<Material> MaterialPtr(LPCWSTR name);
	std::shared_ptr<Light> LightPtr(LPCWSTR name);

	void ChangeBackgroundColor();

	void SetFreeCamera();

private:
	unsigned int LightCount;
	PCWSTR m_pszGameName;
	std::unique_ptr<MainWindow> m_mainWindow;
	std::unique_ptr<Renderer> m_renderer;

	std::unordered_map<PCWSTR, std::shared_ptr<Model>> ModelPtrContainer;
	std::unordered_map<PCWSTR, std::shared_ptr<Material>> MaterialPtrContainer;

	std::unordered_map<PCWSTR, std::shared_ptr<Light>> LightPtrContainer;
};