#include "Model.h"
#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>


XMMATRIX ConvertMatrix(_In_ const aiMatrix4x4& matrix)
{
	return XMMATRIX(
		matrix.a1,
		matrix.b1,
		matrix.c1,
		matrix.d1,
		matrix.a2,
		matrix.b2,
		matrix.c2,
		matrix.d2,
		matrix.a3,
		matrix.b3,
		matrix.c3,
		matrix.d3,
		matrix.a4,
		matrix.b4,
		matrix.c4,
		matrix.d4
	);
}
XMFLOAT3 ConvertVector3dToFloat3(_In_ const aiVector3D& vector)
{
	return XMFLOAT3(vector.x, vector.y, vector.z);
}
XMVECTOR ConvertQuaternionToVector(_In_ const aiQuaternion& quaternion)
{
	XMFLOAT4 float4 = XMFLOAT4(quaternion.x, quaternion.y, quaternion.z, quaternion.w);
	return XMLoadFloat4(&float4);
}

std::unique_ptr<Assimp::Importer> Model::sm_pImporter = std::make_unique<Assimp::Importer>();


Model::Model(_In_ const std::filesystem::path& filePath)
	: Renderable(XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f))
	, m_filePath(filePath)
	, m_aVertices()
	, m_aIndices()
	, m_aBoneData()
	, m_aBoneInfo()
	, m_boneNameToIndexMap()
	, m_pScene(nullptr)
	, m_timeSinceLoaded(0.0f)
	, m_globalInverseTransform()
	, m_animationBuffer()
	, m_skinningConstantBuffer()
	, m_aAnimationData()
	, m_aTransforms() 
{
}
/*
HRESULT Model::Initialize(_In_ ID3D11Device* pDevice, _In_ ID3D11DeviceContext* pImmediateContext)
{
	HRESULT hr = S_OK;
	Assimp::Importer importer;
	const aiScene* pScene = importer.ReadFile(
		m_filePath.string().c_str(),
		ASSIMP_LOAD_FLAGS | aiProcess_ConvertToLeftHanded
	);
	if (pScene)
	{
		hr = initFromScene(pDevice, pImmediateContext, pScene, m_filePath);
	}
	else
	{
		hr = E_FAIL;
		OutputDebugString(L"Error parsing ");
		OutputDebugString(m_filePath.c_str());
		OutputDebugString(L": ");
		OutputDebugStringA(importer.GetErrorString());
		OutputDebugString(L"\n");
	}
	return hr;
}*/

HRESULT Model::Initialize(_In_ ID3D11Device* pDevice, _In_ ID3D11DeviceContext* pImmediateContext)
{
	HRESULT hr = S_OK;

	// Create the buffers for the vertices attributes

	m_pScene = sm_pImporter->ReadFile(m_filePath.string().c_str(),(aiProcess_Triangulate | aiProcess_GenSmoothNormals | aiProcess_FlipUVs | aiProcess_JoinIdenticalVertices | aiProcess_ConvertToLeftHanded | aiProcess_CalcTangentSpace));

	if (m_pScene)
	{
		// global Inverse Transform 계산
		m_globalInverseTransform = ConvertMatrix(m_pScene->mRootNode->mTransformation);
		m_globalInverseTransform = XMMatrixInverse(nullptr, m_globalInverseTransform);
		hr = initFromScene(pDevice, pImmediateContext, m_pScene, m_filePath);
	}
	else
	{
		hr = E_FAIL;
		OutputDebugString(L"Error parsing ");
		OutputDebugString(m_filePath.c_str());
		OutputDebugString(L": ");
		OutputDebugStringA(sm_pImporter->GetErrorString());
		OutputDebugString(L"\n");
	}

	/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-
	TODO: Animation buffer (Animation Data) 추가
	hint: .ByteWidth = static_cast<UINT>(sizeof(AnimationData) * m_aAnimationData.size()) 사용
	-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
	D3D11_BUFFER_DESC bd = { 
		.ByteWidth = static_cast<UINT>(sizeof(AnimationData) * m_aAnimationData.size()),
		.Usage = D3D11_USAGE_DEFAULT,
		.BindFlags = D3D11_BIND_VERTEX_BUFFER,
		.CPUAccessFlags = 0
	};
	D3D11_SUBRESOURCE_DATA initData = {};
	initData.pSysMem = m_aAnimationData.data();
	
	hr = pDevice->CreateBuffer(&bd, &initData, m_animationBuffer.GetAddressOf());
	if (FAILED(hr))
		return hr;

	// Create the animation buffer

	/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-
	TODO: CBSkinning Constant buffer 생성
	-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
	bd = { 
		.ByteWidth = sizeof(CBSkinning),
		.Usage = D3D11_USAGE_DEFAULT,
		.BindFlags = D3D11_BIND_CONSTANT_BUFFER,
		.CPUAccessFlags = 0
	};
	hr = pDevice->CreateBuffer(&bd, nullptr, m_skinningConstantBuffer.GetAddressOf());
	if (FAILED(hr))
		return hr;

	return hr;
}


HRESULT Model::initFromScene(_In_ ID3D11Device* pDevice,_In_ ID3D11DeviceContext* pImmediateContext,_In_ const aiScene* pScene,_In_ const std::filesystem::path& filePath)
{
	static CHAR szDebugMessage[256];
	HRESULT hr = S_OK;
	m_aMeshes.resize(pScene-> mNumMeshes);
	m_aMaterials.resize(pScene->mNumMaterials);

	UINT uNumVertices = 0u;
	UINT uNumIndices = 0u;
	countVerticesAndIndices(uNumVertices, uNumIndices, pScene);
	reserveSpace(uNumVertices, uNumIndices);

	initAllMeshes(pScene);
	hr = initMaterials(pDevice, pImmediateContext, pScene, filePath);
	if(FAILED(hr))
		return hr;

	for (size_t i = 0; i < m_aVertices.size(); ++i)
	{
		sprintf_s(szDebugMessage, "\t\t aBoneIds %d\n", m_aBoneData[i].aBoneIds[0]);
		OutputDebugStringA(szDebugMessage);

		/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-
		TODO: m_aAnimationData에 AnimationData 자료형으로 데이터 추가.

		hint: m_aBoneData.at(i)로 접근.
		-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
		m_aAnimationData.at(i).aBoneIndices.x = m_aBoneData.at(i).aBoneIds[0];
		m_aAnimationData.at(i).aBoneIndices.y = m_aBoneData.at(i).aBoneIds[1];
		m_aAnimationData.at(i).aBoneIndices.z = m_aBoneData.at(i).aBoneIds[2];
		m_aAnimationData.at(i).aBoneIndices.w = m_aBoneData.at(i).aBoneIds[3];
		m_aAnimationData.at(i).aBoneWeights.x = m_aBoneData.at(i).aWeights[0];
		m_aAnimationData.at(i).aBoneWeights.y = m_aBoneData.at(i).aWeights[1];
		m_aAnimationData.at(i).aBoneWeights.z = m_aBoneData.at(i).aWeights[2];
		m_aAnimationData.at(i).aBoneWeights.w = m_aBoneData.at(i).aWeights[3];


		static CHAR szMessage[256];
		sprintf_s(
			szMessage,
			"[%4zu]"
			"\tbone indices: %u, %u, %u, %u\n"
			"\tbone weights: %f, %f, %f, %f\n",
			i,
			m_aAnimationData.at(i).aBoneIndices.x, m_aAnimationData.at(i).aBoneIndices.y, m_aAnimationData.at(i).aBoneIndices.z, m_aAnimationData.at(i).aBoneIndices.w,
			m_aAnimationData.at(i).aBoneWeights.x, m_aAnimationData.at(i).aBoneWeights.y, m_aAnimationData.at(i).aBoneWeights.z, m_aAnimationData.at(i).aBoneWeights.w
		);
		OutputDebugStringA(szMessage);
	}

	hr = initialize(pDevice, pImmediateContext);
	if(FAILED(hr))
		return hr;
	return hr;
}

void Model::countVerticesAndIndices(_Inout_ UINT& uOutNumVertices, _Inout_ UINT&uOutNumIndices, _In_ const aiScene* pScene)
{
	for(UINT i = 0u; i < m_aMeshes.size(); ++i){
		m_aMeshes[i].uMaterialIndex = pScene-> mMeshes[i]-> mMaterialIndex;
		m_aMeshes[i].uNumIndices = pScene-> mMeshes[i]-> mNumFaces * 3u;
		m_aMeshes[i].uBaseVertex = uOutNumVertices;
		m_aMeshes[i].uBaseIndex = uOutNumIndices;
		uOutNumVertices += pScene-> mMeshes[i]-> mNumVertices;
		uOutNumIndices += m_aMeshes[i].uNumIndices;
	}
}

void Model::reserveSpace(_In_ UINT uNumVertices, _In_ UINT uNumIndices)
{
	m_aVertices.reserve(uNumVertices);
	m_aIndices.reserve(uNumIndices);
}

void Model::initAllMeshes(_In_ const aiScene* pScene){
	for(UINT i = 0u; i < m_aMeshes.size(); ++i){
		const aiMesh* pMesh = pScene-> mMeshes[i];
		initSingleMesh(i, pMesh);
	}
}

void Model::initSingleMesh(_In_ UINT uMeshIndex, _In_ const aiMesh* pMesh)
{
	const aiVector3D zero3d(0.0f, 0.0f, 0.0f);
	// Populate the vertex attribute vectors
	for (UINT i = 0u; i < pMesh->mNumVertices; ++i){
		const aiVector3D& position = pMesh-> mVertices[i];
		const aiVector3D& normal = pMesh-> mNormals[i];
		const aiVector3D& texCoord = pMesh-> HasTextureCoords(0u) ? pMesh-> mTextureCoords[0][i] : zero3d;
		m_aVertices.push_back(
			SimpleVertex{
			.Position = XMFLOAT3(position.x, position.y, position.z), 
			.TexCoord = XMFLOAT2(texCoord.x, texCoord.y),
			.Normal = XMFLOAT3(normal.x, normal.y, normal.z
			)
			}
		);
	}

	if (pMesh->mNumBones > 0) {
		CHAR szDebugMessage[256];
		sprintf_s(
			szDebugMessage,
			"\tMesh %u '%s': vertices %u indices %u bones %u\n",
			uMeshIndex,
			pMesh->mName.C_Str(),
			pMesh->mNumVertices,
			pMesh->mNumFaces * 3,
			pMesh->mNumBones
		);
		OutputDebugStringA(szDebugMessage);
	}

	m_aBoneData.resize(m_aVertices.size());
	m_aAnimationData.resize(m_aVertices.size());
	initMeshBones(uMeshIndex, pMesh);
	

	// Populate the index buffer
	for (UINT i = 0u; i < pMesh->mNumFaces; ++i){
		const aiFace& face = pMesh-> mFaces[i];
		assert(face.mNumIndices == 3u);
		m_aIndices.push_back(static_cast<WORD>(face.mIndices[0]));
		m_aIndices.push_back(static_cast<WORD>(face.mIndices[1]));
		m_aIndices.push_back(static_cast<WORD>(face.mIndices[2]));
	}
}

HRESULT Model::initMaterials(_In_ ID3D11Device* pDevice, _In_ ID3D11DeviceContext*
	pImmediateContext, _In_ const aiScene* pScene, _In_ const std::filesystem::path& filePath)
{
	HRESULT hr = S_OK;

	// Extract the directory part from the file name
	std::filesystem::path parentDirectory = filePath.parent_path();
	// Initialize the materials
	CHAR szDebugMessage[256];
	for(UINT i = 0u; i < pScene->mNumMaterials; ++i){
		m_aMaterials[i] = std::make_shared<Material>();
		const aiMaterial* pMaterial = pScene->mMaterials[i];
		loadTextures(pDevice, pImmediateContext, parentDirectory, pMaterial, i);
		loadColors(pMaterial, i);
		sprintf_s(szDebugMessage, "\t %u \n", i);
		OutputDebugStringA(szDebugMessage);
	}
	return hr;
}

HRESULT Model::loadTextures(_In_ ID3D11Device* pDevice, _In_ ID3D11DeviceContext*
	pImmediateContext, _In_ const std::filesystem::path& parentDirectory, _In_ const aiMaterial*
	pMaterial, _In_ UINT uIndex){
	HRESULT hr = loadDiffuseTexture(pDevice, pImmediateContext, parentDirectory, pMaterial,uIndex);
	if(FAILED(hr))
		return hr;
	hr = loadSpecularTexture(pDevice, pImmediateContext, parentDirectory, pMaterial, uIndex);
	if(FAILED(hr))
		return hr;
	return hr;
}

HRESULT Model::loadDiffuseTexture(_In_ ID3D11Device* pDevice, _In_ ID3D11DeviceContext*
	pImmediateContext, _In_ const std::filesystem::path& parentDirectory, _In_ const
	aiMaterial* pMaterial, _In_ UINT uIndex)
{
	HRESULT hr = S_OK;
	m_aMaterials[uIndex]->pDiffuse = nullptr;
	if (pMaterial->GetTextureCount(aiTextureType_DIFFUSE) > 0)
	{
		aiString aiPath;
		if (pMaterial->GetTexture(aiTextureType_DIFFUSE, 0u, &aiPath, nullptr, nullptr,
			nullptr, nullptr, nullptr) == AI_SUCCESS)
		{
			std::string szPath(aiPath.data);
			if (szPath.substr(0ull, 2ull) == ".\\")
			{
				szPath = szPath.substr(2ull, szPath.size() - 2ull);
			}
			std::filesystem::path fullPath = parentDirectory / szPath;
			m_aMaterials[uIndex]->pDiffuse = std::make_shared<Texture>(fullPath);
			hr = m_aMaterials[uIndex]->pDiffuse->Initialize(pDevice, pImmediateContext);
			if (FAILED(hr))
			{
				OutputDebugString(L"Error loading diffuse texture \"");
				OutputDebugString(fullPath.c_str());
				OutputDebugString(L"\"\n");
				return hr;
			}
			OutputDebugString(L"Loaded diffuse texture \"");
			OutputDebugString(fullPath.c_str());
			OutputDebugString(L"\"\n");
		}
	}
	return hr;
}

HRESULT Model::loadSpecularTexture(_In_ ID3D11Device* pDevice, _In_
	ID3D11DeviceContext* pImmediateContext, _In_ const std::filesystem::path&
	parentDirectory, _In_ const aiMaterial* pMaterial, _In_ UINT uIndex)
{
	HRESULT hr = S_OK;
	m_aMaterials[uIndex]->pSpecular = nullptr;
	if (pMaterial->GetTextureCount(aiTextureType_SHININESS) > 0)
	{
		aiString aiPath;
		if (pMaterial->GetTexture(aiTextureType_SHININESS, 0u, &aiPath, nullptr,
			nullptr, nullptr, nullptr, nullptr) == AI_SUCCESS)
		{
			std::string szPath(aiPath.data);
			if (szPath.substr(0ull, 2ull) == ".\\")
			{
				szPath = szPath.substr(2ull, szPath.size() - 2ull);
			}
			std::filesystem::path fullPath = parentDirectory / szPath;
			m_aMaterials[uIndex]->pSpecular = std::make_shared<Texture>(fullPath);
			hr = m_aMaterials[uIndex]->pSpecular->Initialize(pDevice, pImmediateContext);
			if (FAILED(hr))
			{
				OutputDebugString(L"Error loading specular texture \"");
				OutputDebugString(fullPath.c_str());
				OutputDebugString(L"\"\n");
				return hr;
			}
			OutputDebugString(L"Loaded specular texture \"");
			OutputDebugString(fullPath.c_str());
			OutputDebugString(L"\"\n");
		}
	}
	return hr;
}

void Model::loadColors(_In_ const aiMaterial* pMaterial, _In_ UINT uIndex)
{
	aiColor3D ambientColor(0.0f, 0.0f, 0.0f);
	if (pMaterial-> Get(AI_MATKEY_COLOR_AMBIENT, ambientColor) == AI_SUCCESS){
		m_aMaterials[uIndex]->AmbientColor.x = ambientColor.r;
		m_aMaterials[uIndex]->AmbientColor.y = ambientColor.g;
		m_aMaterials[uIndex]->AmbientColor.z = ambientColor.b;
	}
	aiColor3D diffuseColor(0.0f, 0.0f, 0.0f);
	if (pMaterial->Get(AI_MATKEY_COLOR_DIFFUSE, diffuseColor) == AI_SUCCESS){
		m_aMaterials[uIndex]->DiffuseColor.x = diffuseColor.r;
		m_aMaterials[uIndex]->DiffuseColor.y = diffuseColor.g;
		m_aMaterials[uIndex]->DiffuseColor.z = diffuseColor.b;
	}
	aiColor3D specularColor(0.0f, 0.0f, 0.0f);
	if (pMaterial->Get(AI_MATKEY_COLOR_SPECULAR, specularColor) == AI_SUCCESS){
		m_aMaterials[uIndex]->SpecularColor.x = specularColor.r;
		m_aMaterials[uIndex]->SpecularColor.y = specularColor.g;
		m_aMaterials[uIndex]->SpecularColor.z = specularColor.b;
	}
}

const SimpleVertex* Model::getVertices() const{
	return m_aVertices.data();
}

const WORD* Model::getIndices() const {
	return m_aIndices.data();
}

UINT Model::GetNumVertices() const {
	return m_aVertices.size();
}

UINT Model::GetNumIndices() const {
	return m_aIndices.size();
}

void Model::Update(FLOAT deltaTime) {
	m_timeSinceLoaded += deltaTime;
	
	
	if (this->m_pScene->HasAnimations())
	{
		//OutputDebugStringA("HasAnimation");
		for (BoneInfo& boneInfo : m_aBoneInfo)
		{
			boneInfo.FinalTransformation = XMMatrixIdentity();
		}

		// 추가 
		XMMATRIX identity = XMMatrixIdentity();
		FLOAT ticksPerSecond = static_cast<FLOAT>(m_pScene->mAnimations[0]->mTicksPerSecond != 0.0 ? m_pScene->mAnimations[0]->mTicksPerSecond : 25.0f);

		FLOAT timeInTicks = m_timeSinceLoaded * ticksPerSecond;
		FLOAT animationTimeTicks = fmod(timeInTicks, static_cast<FLOAT>(m_pScene->mAnimations[0]->mDuration));
		if (m_pScene->mRootNode)
		{

			// 추가
			readNodeHierarchy(animationTimeTicks, m_pScene->mRootNode,identity);

			m_aTransforms.resize(m_aBoneInfo.size());

			for (UINT i = 0u; i < m_aBoneInfo.size(); ++i)
			{
				m_aTransforms[i] = m_aBoneInfo[i].FinalTransformation;
			}
		}
	}
}

/*----------------------------------------------------------------*/

void Model::initMeshBones(_In_ UINT uMeshIndex, _In_ const aiMesh* pMesh)
{
	for (UINT i = 0u; i < pMesh->mNumBones; ++i)
	{
		initMeshSingleBone(uMeshIndex, pMesh->mBones[i]);
	}
}

void Model::initMeshSingleBone(_In_ UINT uMeshIndex, _In_ const aiBone* pBone)
{
	CHAR szDebugMessage[256];
	sprintf_s(szDebugMessage, "\tBone '%s': num vertices affected by this bone: %u\n", pBone->mName.C_Str(), pBone->mNumWeights);
	OutputDebugStringA(szDebugMessage);

	UINT uBoneId = getBoneId(pBone);
	sprintf_s(szDebugMessage, "\t\tbone id %u\n", uBoneId);
	OutputDebugStringA(szDebugMessage);

	if (uBoneId == m_aBoneInfo.size())
	{
		BoneInfo boneInfo(ConvertMatrix(pBone->mOffsetMatrix));
		m_aBoneInfo.push_back(boneInfo);
	}

	for (UINT i = 0u; i < pBone->mNumWeights; ++i)
	{
		const aiVertexWeight& vertexWeight = pBone->mWeights[i];
		UINT uGlobalVertexId = m_aMeshes[uMeshIndex].uBaseVertex + vertexWeight.mVertexId;

		sprintf_s(szDebugMessage, "\t\tvertex id %u, weight %.2f\n", uGlobalVertexId, vertexWeight.mWeight);
		OutputDebugStringA(szDebugMessage);
		m_aBoneData[uGlobalVertexId].AddBoneData(uBoneId, vertexWeight.mWeight);
		
	}
}




void Model::interpolatePosition(_Inout_ XMFLOAT3& outTranslate, _In_ FLOAT animationTimeTicks, _In_ const aiNodeAnim* pNodeAnim)
{
	if (pNodeAnim->mNumPositionKeys == 1)
	{
		outTranslate = ConvertVector3dToFloat3(pNodeAnim->mPositionKeys[0].mValue);
		return;
	}

	UINT uPositionIndex = findPosition(animationTimeTicks, pNodeAnim);
	UINT uNextPositionIndex = uPositionIndex + 1u;
	assert(uNextPositionIndex < pNodeAnim->mNumPositionKeys);

	FLOAT t1 = static_cast<FLOAT>(pNodeAnim->mPositionKeys[uPositionIndex].mTime);
	FLOAT t2 = static_cast<FLOAT>(pNodeAnim->mPositionKeys[uNextPositionIndex].mTime);
	FLOAT deltaTime = t2 - t1;
	FLOAT factor = (animationTimeTicks - t1) / deltaTime;

	const aiVector3D& start = pNodeAnim->mPositionKeys[uPositionIndex].mValue;
	const aiVector3D& end = pNodeAnim->mPositionKeys[uNextPositionIndex].mValue;
	aiVector3D delta = end - start;
	outTranslate = ConvertVector3dToFloat3(start + factor * delta);
}


/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-
TODO: Model::interpolateRotation() 구현

hint: interpolatePosition() 함수 참고하여, rotation으로 바꾸어 구현.
ConvertQuaternionToVector(), findRoataion() 등 사용.
Quatrenion으로 구현.
-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
void Model::interpolateRotation(_Inout_ XMVECTOR& outQuaternion, _In_ FLOAT animationTimeTicks, _In_ const aiNodeAnim* pNodeAnim)
{

	// 추가
	if (pNodeAnim->mNumPositionKeys == 1)
	{
		outQuaternion = ConvertQuaternionToVector(pNodeAnim->mRotationKeys[0].mValue);
		return;
	}
	UINT uRotationIndex = findRotation(animationTimeTicks, pNodeAnim);
	UINT uNextRotationIndex = uRotationIndex + 1u;
	assert(uNextRotationIndex < pNodeAnim->mNumRotationKeys);

	FLOAT t1 = static_cast<FLOAT>(pNodeAnim->mRotationKeys[uRotationIndex].mTime);
	FLOAT t2 = static_cast<FLOAT>(pNodeAnim->mRotationKeys[uNextRotationIndex].mTime);
	FLOAT deltaTime = t2 - t1;
	FLOAT factor = (animationTimeTicks - t1) / deltaTime;
	
	const aiQuaternion& start = pNodeAnim->mRotationKeys[uRotationIndex].mValue;
	const aiQuaternion& end = pNodeAnim->mRotationKeys[uNextRotationIndex].mValue;
	//
	aiQuaternion quaternion;
	aiQuaternion::Interpolate(quaternion, start, end, factor);
	quaternion = start;

	outQuaternion = ConvertQuaternionToVector(quaternion.Normalize());
}

/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-
TODO: Model::interpolateScaling() 구현
-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
void Model::interpolateScaling(_Inout_ XMFLOAT3& outScale, _In_ FLOAT animationTimeTicks, _In_ const aiNodeAnim* pNodeAnim)
{
	if (pNodeAnim->mNumScalingKeys == 1)
	{
		outScale = ConvertVector3dToFloat3(pNodeAnim->mScalingKeys[0].mValue);
		return;
	}

	UINT uScaleIndex = findScaling(animationTimeTicks, pNodeAnim);
	UINT uNextScaleIndex = uScaleIndex + 1u;
	assert(uScaleIndex < pNodeAnim->mNumScalingKeys);

	FLOAT t1 = static_cast<FLOAT>(pNodeAnim->mScalingKeys[uScaleIndex].mTime);
	FLOAT t2 = static_cast<FLOAT>(pNodeAnim->mScalingKeys[uNextScaleIndex].mTime);
	FLOAT deltaTime = t2 - t1;
	FLOAT factor = (animationTimeTicks - t1) / deltaTime;

	const aiVector3D& start = pNodeAnim->mScalingKeys[uScaleIndex].mValue;
	const aiVector3D& end = pNodeAnim->mScalingKeys[uNextScaleIndex].mValue;
	aiVector3D delta = end - start;
	outScale= ConvertVector3dToFloat3(start + factor * delta);
}

/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-
TODO: Model::readNodeHierarchy() 구현
-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
void Model::readNodeHierarchy(_In_ FLOAT animationTimeTicks, _In_ const aiNode* pNode, _In_ const XMMATRIX& parentTransform)
{
	PCSTR pszNodeName = pNode-> mName.data;
	const aiAnimation* pAnimation = m_pScene-> mAnimations[0];
	XMMATRIX nodeTransformation = ConvertMatrix(pNode-> mTransformation);
	const aiNodeAnim* pNodeAnim = findNodeAnimOrNull(pAnimation,pszNodeName);
	if(pNodeAnim)
	{
		// interpolation 함수 호출 추가
		XMFLOAT3 outTranslation;
		XMVECTOR outQuarternion;
		XMFLOAT3 outScale;
		interpolatePosition(outTranslation, animationTimeTicks, pNodeAnim);
		interpolateRotation(outQuarternion, animationTimeTicks, pNodeAnim);
		interpolateScaling(outScale, animationTimeTicks, pNodeAnim);
		XMMATRIX scalingMatrix = XMMatrixScaling(outScale.x, outScale.y, outScale.z);
		XMMATRIX rotationMatrix = XMMatrixRotationQuaternion(outQuarternion);
		XMMATRIX translationMatrix = XMMatrixTranslation(outTranslation.x, outTranslation.y, outTranslation.z);
		
		nodeTransformation = scalingMatrix * rotationMatrix * translationMatrix;
	}
	XMMATRIX globalTransformation = nodeTransformation * parentTransform;
	if(m_boneNameToIndexMap.contains(pszNodeName))
	{
		UINT uBoneIndex = m_boneNameToIndexMap[pszNodeName];
		m_aBoneInfo[uBoneIndex].FinalTransformation = m_aBoneInfo[uBoneIndex].OffsetMatrix * globalTransformation * m_globalInverseTransform;
	}

	for (UINT i = 0u; i < pNode->mNumChildren; ++i)
	{
		readNodeHierarchy(animationTimeTicks, pNode->mChildren[i], globalTransformation);
	}
}

const aiNodeAnim* Model::findNodeAnimOrNull(_In_ const aiAnimation* pAnimation, _In_ PCSTR pszNodeName)
{
	for (UINT i = 0u; i < pAnimation->mNumChannels; ++i)
	{
		const aiNodeAnim* pNodeAnim = pAnimation->mChannels[i];

		if (strncmp(pNodeAnim->mNodeName.data, pszNodeName, pNodeAnim->mNodeName.length) == 0)
		{
			return pNodeAnim;
		}
	}

	return nullptr;
}

UINT Model::findPosition(_In_ FLOAT animationTimeTicks, _In_ const aiNodeAnim* pNodeAnim)
{
	for (UINT i = 0; i < pNodeAnim->mNumPositionKeys - 1; ++i)
	{
		FLOAT t = static_cast<FLOAT>(pNodeAnim->mPositionKeys[i + 1].mTime);

		if (animationTimeTicks < t)
		{
			return i;
		}
	}
}

UINT Model::findRotation(_In_ FLOAT animationTimeTicks, _In_ const aiNodeAnim* pNodeAnim)
{
	assert(pNodeAnim->mNumRotationKeys > 0);

	for (UINT i = 0u; i < pNodeAnim->mNumRotationKeys - 1; ++i)
	{
		FLOAT t = static_cast<FLOAT>(pNodeAnim->mRotationKeys[i + 1].mTime);

		if (animationTimeTicks < t)
		{
			return i;
		}
	}

	return 0u;
}

UINT Model::findScaling(_In_ FLOAT animationTimeTicks, _In_ const aiNodeAnim* pNodeAnim)
{
	assert(pNodeAnim->mNumScalingKeys > 0);

	for (UINT i = 0u; i < pNodeAnim->mNumScalingKeys - 1; ++i)
	{
		FLOAT t = static_cast<FLOAT>(pNodeAnim->mScalingKeys[i + 1].mTime);

		if (animationTimeTicks < t)
		{
			return i;
		}
	}

	return 0u;
}

UINT Model::getBoneId(_In_ const aiBone* pBone)
{
	UINT uBoneIndex = 0u;
	PCSTR pszBoneName = pBone->mName.C_Str();
	if (!m_boneNameToIndexMap.contains(pszBoneName))
	{
		uBoneIndex = static_cast<UINT>(m_boneNameToIndexMap.size());
		m_boneNameToIndexMap[pszBoneName] = uBoneIndex;
	}
	else
	{
		uBoneIndex = m_boneNameToIndexMap[pszBoneName];
	}

	return uBoneIndex;
}

ComPtr<ID3D11Buffer>& Model::GetAnimationBuffer()
{
	return m_animationBuffer;
}

ComPtr<ID3D11Buffer>& Model::GetSkinningConstantBuffer()
{
	return m_skinningConstantBuffer;
}

std::vector<XMMATRIX>& Model::GetBoneTransforms()
{
	return m_aTransforms;
}

const std::unordered_map<std::string, UINT>& Model::GetBoneNameToIndexMap() const
{
	return m_boneNameToIndexMap;
}