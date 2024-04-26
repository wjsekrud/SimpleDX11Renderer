#include "Renderer/Renderable.h"
#include "WICTextureLoader.h"
#include "Texture/DDSTextureLoader11.h"

Texture::Texture(const std::filesystem::path& m_filepath) : m_filePath(m_filepath)
	, m_textureRV()
	, m_samplerLinear()
{
}

Texture::Texture() : m_filePath("../seafloor.dds")
, m_textureRV()
, m_samplerLinear()
{
}



HRESULT Texture::Initialize(_In_ ID3D11Device* pDevice, _In_ ID3D11DeviceContext* pImmediateContext)
{
	HRESULT hr = CreateWICTextureFromFile(pDevice, pImmediateContext, m_filePath.c_str(), nullptr, m_textureRV.GetAddressOf());
	if (FAILED(hr))
	{
		OutputDebugString(L"Can't load texture from \"");
		OutputDebugString(m_filePath.c_str());
		OutputDebugString(L"\n");
		return hr;
	}
	// Create the sample state
	D3D11_SAMPLER_DESC sampDesc =
	{
	.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR,
	.AddressU = D3D11_TEXTURE_ADDRESS_WRAP,
	.AddressV = D3D11_TEXTURE_ADDRESS_WRAP,
	.AddressW = D3D11_TEXTURE_ADDRESS_WRAP,
	.ComparisonFunc = D3D11_COMPARISON_NEVER,
	.MinLOD = 0,
	.MaxLOD = D3D11_FLOAT32_MAX
	};
	hr = pDevice->CreateSamplerState(&sampDesc, m_samplerLinear.GetAddressOf());
	if (FAILED(hr))
		return hr;
	return hr;
}


ComPtr<ID3D11ShaderResourceView>& Texture::GetTextureResourceView()
{
	return m_textureRV;
}

ComPtr<ID3D11SamplerState>& Texture::GetSamplerState()
{
	return m_samplerLinear;
}

Material::Material(_In_ std::wstring szName)
	: DiffuseColor()
	, AmbientColor()
	, SpecularColor()
	, pDiffuse()
	, pSpecular()
	, pNormal()
	, m_szName(szName)
{
}

Material::Material()
	: DiffuseColor()
	, AmbientColor()
	, SpecularColor()
	, pDiffuse()
	, pSpecular()
	, pNormal()
	, m_szName(L"NULL")
{
	OutputDebugStringA("DefaultMaterialGeneratorCalled");
}



HRESULT Material::Initialize(ID3D11Device* pDevice, ID3D11DeviceContext* pImmediateContext)
{
	HRESULT hr = S_OK;

	if (pDiffuse)
	{
		hr = pDiffuse->Initialize(pDevice, pImmediateContext);
		if (FAILED(hr))
			return hr;
	}
	if (pSpecular)
	{
		hr = pSpecular->Initialize(pDevice, pImmediateContext);
		if (FAILED(hr))
			return hr;
	}

	if (pNormal)
	{
		hr = pNormal->Initialize(pDevice, pImmediateContext);
		if (FAILED(hr))
			return hr;
	}


	return hr;
}

Renderable::Renderable(_In_ const std::filesystem::path& textureFilePath)
	: m_vertexBuffer()
	, m_indexBuffer()
	, m_constantBuffer()
	, m_vertexShader()
	, m_pixelShader()
	, m_outputColor(XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f))
	, m_world(XMMatrixIdentity())
	, m_aMeshes()
	, m_aMaterials()
	, m_bHasNormalMap()
	, m_aNormalData()
	, m_normalBuffer()
{
}

Renderable::Renderable(_In_ const XMFLOAT4& outputColor)
	: m_vertexBuffer()
	, m_indexBuffer()
	, m_constantBuffer()
	, m_vertexShader()
	, m_pixelShader()
	, m_outputColor(outputColor)
	, m_world(XMMatrixIdentity())
	, m_aMeshes()
	, m_aMaterials()
	, m_bHasNormalMap()
	, m_aNormalData()
	, m_normalBuffer()
{
}

HRESULT Renderable::initialize(
	_In_ ID3D11Device* pDevice,
	_In_ ID3D11DeviceContext* pImmediateContext
)
{
	D3D11_BUFFER_DESC bd = {};
	{
		bd.Usage = D3D11_USAGE_DEFAULT;
		bd.ByteWidth = sizeof(SimpleVertex) * GetNumVertices();
		bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		bd.CPUAccessFlags = 0;
	}

	D3D11_SUBRESOURCE_DATA initData = {};
	initData.pSysMem = getVertices();

	HRESULT hr = pDevice->CreateBuffer(&bd, &initData, m_vertexBuffer.GetAddressOf());

	if (FAILED(hr))
		return hr;

	if (m_aNormalData.empty()) {
		calculateNormalMapVectors();
	}
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = sizeof(NormalData) * GetNumVertices();
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.CPUAccessFlags = 0;

	initData.pSysMem = m_aNormalData.data();
	hr = pDevice->CreateBuffer(&bd, &initData, m_normalBuffer.GetAddressOf());
	if (FAILED(hr))
		return hr;

	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = sizeof(WORD) * GetNumIndices();
	bd.BindFlags = D3D10_BIND_INDEX_BUFFER;
	bd.CPUAccessFlags = 0;

	initData.pSysMem = getIndices();

	hr = pDevice->CreateBuffer(&bd, &initData, m_indexBuffer.GetAddressOf());

	if (FAILED(hr))
		return hr;

	bd.ByteWidth = sizeof(CBChangeEveryFrame);
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	bd.CPUAccessFlags = 0;

	hr = pDevice->CreateBuffer(&bd, nullptr, m_constantBuffer.GetAddressOf());
	if (FAILED(hr))
		return hr;

	for (auto it = m_aMaterials.begin(); it != m_aMaterials.end(); ++it)
	{
		HRESULT hr = it->get()->Initialize(pDevice, pImmediateContext);
		if (FAILED(hr))
			return hr;
	}

	return S_OK;
}

void Renderable::calculateNormalMapVectors() {
	UINT uNumFaces = GetNumIndices() / 3;
	const SimpleVertex* aVertices = getVertices();
	const WORD* aIndices = getIndices(); 
	m_aNormalData.resize(GetNumVertices(), NormalData());
	XMFLOAT3 tangent, bitangent;
	for (int i = 0; i < uNumFaces; i++) {
		Renderable::calculateTBT(aVertices[aIndices[i * 3]], aVertices[aIndices[i * 3 + 1]],aVertices[aIndices[i * 3 + 2]], tangent, bitangent);
		m_aNormalData[aIndices[i * 3]].Tangent = tangent;
		m_aNormalData[aIndices[i * 3]].Bitangent = bitangent;
		m_aNormalData[aIndices[i * 3 + 1]].Tangent = tangent;
		m_aNormalData[aIndices[i * 3 + 1]].Bitangent = bitangent;
		m_aNormalData[aIndices[i * 3 + 2]].Tangent = tangent;
		m_aNormalData[aIndices[i * 3 + 2]].Bitangent = bitangent;
	}

}
void Renderable::calculateTBT(_In_ const SimpleVertex& v1, _In_ const SimpleVertex& v2, _In_ const SimpleVertex& v3, _Out_ XMFLOAT3& tangent, _Out_ XMFLOAT3& bitangent) {
	XMFLOAT3 vector1, vector2;
	XMFLOAT2 tuVector, tvVector;
	// Calculate the two vectors for this face.
	vector1.x = v2.Position.x - v1.Position.x; vector1.y = v2.Position.y - v1.Position.y; vector1.z = v2.Position.z - v1.Position.z;
	vector2.x = v3.Position.x - v1.Position.x; vector2.y = v3.Position.y - v1.Position.y; vector2.z = v3.Position.z - v1.Position.z;
	// Calculate the tu and tv texture space vectors.
	tuVector.x = v2.TexCoord.x - v1.TexCoord.x; tvVector.x = v2.TexCoord.y - v1.TexCoord.y;
	tuVector.y = v3.TexCoord.x - v1.TexCoord.x; tvVector.y = v3.TexCoord.y - v1.TexCoord.y;
	// Calculate the denominator of the tangent/binormal equation.
	float den = 1.0f / (tuVector.x * tvVector.y - tuVector.y * tvVector.x);
	// Calculate the cross products and multiply by the coefficient to get the tangent and binormal.
	tangent.x = (tvVector.y * vector1.x - tvVector.x * vector2.x) * den;
	tangent.y = (tvVector.y * vector1.y - tvVector.x * vector2.y) * den;
	tangent.z = (tvVector.y * vector1.z - tvVector.x * vector2.z) * den;
	bitangent.x = (tuVector.x * vector2.x - tuVector.y * vector1.x) * den;
	bitangent.y = (tuVector.x * vector2.y - tuVector.y * vector1.y) * den;
	bitangent.z = (tuVector.x * vector2.z - tuVector.y * vector1.z) * den;
	// Calculate the length of this normal.
	float length = sqrt((tangent.x * tangent.x) + (tangent.y * tangent.y) + (tangent.z * tangent.z));
	// Normalize the normal and then store it
	tangent.x = tangent.x / length; 
	tangent.y = tangent.y / length; 
	tangent.z = tangent.z / length;
	// Calculate the length of this normal.
	length = sqrt((bitangent.x * bitangent.x) + (bitangent.y * bitangent.y) + (bitangent.z * bitangent.z));
	// Normalize the normal and then store it
	bitangent.x = bitangent.x / length; 
	bitangent.y = bitangent.y / length; 
	bitangent.z = bitangent.z / length;
}

void Renderable::SetVertexShader(_In_ const std::shared_ptr<VertexShader>& vertexShader)
{
	m_vertexShader = vertexShader;
}

void Renderable::SetPixelShader(_In_ const std::shared_ptr<PixelShader>& pixelShader)
{
	m_pixelShader = pixelShader;
}

HRESULT Renderable::SetMaterialOfMesh(_In_ const UINT uMeshIndex, _In_ const UINT uMaterialIndex)
{
	if (uMeshIndex >= m_aMeshes.size() || uMaterialIndex >= m_aMaterials.size()) {
		return E_FAIL;
	}
	m_aMeshes[uMeshIndex].uMaterialIndex = uMaterialIndex;
	if (m_aMaterials[uMeshIndex]->pNormal)
	{
		m_bHasNormalMap = true;
	}

	return S_OK;

}

void Renderable::AddMaterial(_In_ const std::shared_ptr<Material>& material)
{
	m_aMaterials.push_back(material);
}

ComPtr<ID3D11VertexShader>& Renderable::GetVertexShader()
{
	return m_vertexShader->GetVertexShader();
}

ComPtr<ID3D11PixelShader>& Renderable::GetPixelShader()
{
	return m_pixelShader->GetPixelShader();
}

ComPtr<ID3D11InputLayout>& Renderable::GetVertexLayout()
{
	return m_vertexShader->GetVertexLayout();
}

ComPtr<ID3D11Buffer>& Renderable::GetVertexBuffer()
{
	return m_vertexBuffer;
}

ComPtr<ID3D11Buffer>& Renderable::GetIndexBuffer()
{
	return m_indexBuffer;
}

ComPtr<ID3D11Buffer>& Renderable::GetConstantBuffer()
{
	return m_constantBuffer;
}

ComPtr<ID3D11Buffer>& Renderable::GetNormalBuffer()
{
	return m_normalBuffer;
}


const XMMATRIX& Renderable::GetWorldMatrix() const
{
	return m_world;
}

const XMFLOAT4& Renderable::GetOutputColor() const
{
	return m_outputColor;
}

BOOL Renderable::HasTexture() const
{
	return m_aMaterials.size() > 0;
}

void Renderable::RotateX(_In_ FLOAT angle)
{
	m_world *= XMMatrixRotationX(angle);
}

void Renderable::RotateY(_In_ FLOAT angle)
{
	m_world *= XMMatrixRotationY(angle);
}

void Renderable::RotateZ(_In_ FLOAT angle)
{
	m_world *= XMMatrixRotationZ(angle);
}

void Renderable::RotateRollPitchYaw(_In_ FLOAT roll, _In_ FLOAT pitch, _In_ FLOAT yaw)
{
	m_world *= XMMatrixRotationRollPitchYaw(pitch, yaw, roll);
}

void Renderable::Scale(_In_ FLOAT scaleX, _In_ FLOAT scaleY, _In_ FLOAT scaleZ)
{
	m_world *= XMMatrixScaling(scaleX, scaleY, scaleZ);
}

void Renderable::Translate(_In_ const XMVECTOR& offset)
{
	m_world *= XMMatrixTranslationFromVector(offset);
}

void Renderable::RotateYInObjectCoordinate(_In_ float angle, _In_ XMVECTOR Offset) {
	m_world *= XMMatrixInverse(nullptr,XMMatrixTranslationFromVector(Offset));
	//m_world *= XMMatrixInverse(nullptr, XMMatrixRotationY(angle));
	m_world *= XMMatrixRotationY(angle);
	m_world *= XMMatrixTranslationFromVector(Offset);
}


const std::shared_ptr<Material>& Renderable::GetMaterial(UINT uIndex) {
	assert(uIndex < m_aMaterials.size());
	return m_aMaterials[uIndex];
}

const Renderable::BasicMeshEntry& Renderable::GetMesh(UINT uIndex) {
	assert(uIndex < m_aMeshes.size());
	return m_aMeshes[uIndex];
}

UINT Renderable::GetNumMeshes() const {
	return m_aMeshes.size();
}

UINT Renderable::GetNumMaterials() const {
	return m_aMaterials.size();
}

BOOL Renderable::HasNormalMap() const {
	return m_bHasNormalMap;
}