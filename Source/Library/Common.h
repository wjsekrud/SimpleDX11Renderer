/*+===================================================================
  File:      COMMON.H
  Summary:   Common header file that contains common header files and
			 macros used for the Library project of Game Graphics
			 Programming course.
  Functions:
  © 2022 Kyung Hee University
===================================================================+*/
#pragma once

#ifndef  UNICODE
#define UNICODE
#endif // ! UNICODE

#include <windows.h>
#include <wrl.h>
#include <d3dcompiler.h>

#include <d3d11_4.h>
#include <DirectXColors.h>

#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "d3dcompiler.lib")
#pragma comment(lib, "dxguid.lib")

#include <iostream>
#include <string>
#include <filesystem>
#include <unordered_map>

#define ASSIMP_LOAD_FLAGS (aiProcess_Triangulate | aiProcess_GenSmoothNormals | aiProcess_FlipUVs | aiProcess_JoinIdenticalVertices | aiProcess_ConvertToLeftHanded)

using namespace Microsoft::WRL;
using namespace DirectX;