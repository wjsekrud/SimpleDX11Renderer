#pragma once

#define NUM_LIGHTS (10)
#define MAX_NUM_BONES (256)
#define MAX_NUM_BONES_PER_VERTEX (16)

#include "Common.h"

struct SimpleVertex
{
	XMFLOAT3 Position;
	XMFLOAT2 TexCoord;
	XMFLOAT3 Normal;
};

struct CBChangeOnCameraMovement
{
	XMMATRIX View;
	XMFLOAT4 CameraPosition;
};
struct CBChangeOnResize
{
	XMMATRIX Projection;
};
struct CBChangeEveryFrame
{
	XMMATRIX World;
	XMFLOAT4 OutputColor;
	BOOL HasNormalMap;
};
struct CBLights
{
	XMFLOAT4 LightPositions[NUM_LIGHTS];
	XMFLOAT4 LightColors[NUM_LIGHTS];
};

struct InputDirections
{
	BOOL bFront;
	BOOL bBack;
	BOOL bLeft;
	BOOL bRight;
	BOOL bUp;
	BOOL bDown;
};

struct MouseRelativeMovement
{
	LONG X;
	LONG Y;
};

struct InstanceData
{
	XMMATRIX Transformation;
};

struct CBSkinning
{
	XMMATRIX BoneTransforms[MAX_NUM_BONES];
};

struct AnimationData
{
	XMUINT4 aBoneIndices;
	XMFLOAT4 aBoneWeights;
};

struct NormalData
{
	XMFLOAT3 Tangent;
	XMFLOAT3 Bitangent;
};