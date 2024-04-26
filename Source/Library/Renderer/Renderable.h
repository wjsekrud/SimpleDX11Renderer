#pragma once

#include "Common.h"

#include "Shader/PixelShader.h"
#include "Shader/VertexShader.h"

#include "Renderer/DataTypes.h"


class Texture {
private:
	std::filesystem::path m_filePath;
	ComPtr<ID3D11ShaderResourceView> m_textureRV;
	ComPtr<ID3D11SamplerState> m_samplerLinear;

public:
	Texture();
	Texture(const std::filesystem::path&);
	~Texture() = default;
	HRESULT Initialize(ID3D11Device*, ID3D11DeviceContext*);


	ComPtr<ID3D11ShaderResourceView>& GetTextureResourceView();
	ComPtr<ID3D11SamplerState>& GetSamplerState();


};

class Material {
public:
	std::shared_ptr<Texture> pDiffuse;
	std::shared_ptr<Texture> pSpecular;
	std::shared_ptr<Texture> pNormal;
	XMFLOAT3 DiffuseColor;
	XMFLOAT3 AmbientColor;
	XMFLOAT3 SpecularColor;

	Material();
	Material(_In_ std::wstring szName);
	virtual ~Material() = default;
	
	virtual HRESULT Initialize(_In_ ID3D11Device* pDevice, _In_ ID3D11DeviceContext* pImmediateContext);

	std::wstring Getname() const;
private:
	std::wstring m_szName;
};


class Renderable
{
protected:
	const virtual SimpleVertex* getVertices() const = 0;
	virtual const WORD* getIndices() const = 0;

	HRESULT initialize(
		_In_ ID3D11Device* pDevice, 
		_In_ ID3D11DeviceContext* pImmediateContext
	);

	ComPtr<ID3D11Buffer> m_vertexBuffer;
	ComPtr<ID3D11Buffer> m_indexBuffer;
	ComPtr<ID3D11Buffer> m_constantBuffer;
	ComPtr<ID3D11Buffer> m_normalBuffer;
	 
	std::shared_ptr<VertexShader> m_vertexShader;
	std::shared_ptr<PixelShader> m_pixelShader;

	XMFLOAT4 m_outputColor;
	XMMATRIX m_world;
	BOOL m_bHasNormalMap;

	struct BasicMeshEntry {
	public:
		BasicMeshEntry() : uNumIndices(0u), uBaseVertex(0u), uBaseIndex(0u), uMaterialIndex(INVALID_MATERIAL) {}

		UINT uNumIndices;
		UINT uBaseVertex;
		UINT uBaseIndex;
		UINT uMaterialIndex;
	};

	std::vector<BasicMeshEntry> m_aMeshes;
	std::vector<std::shared_ptr<Material>> m_aMaterials;
	std::vector<NormalData> m_aNormalData;
public:
	Renderable(_In_ const std::filesystem::path& textureFilePath);
	Renderable(_In_ const XMFLOAT4& outputColor);
	Renderable(const Renderable& other) = delete;
	Renderable(Renderable&& other) = delete;
	Renderable() = default;
	virtual ~Renderable() = default;

	virtual HRESULT Initialize(_In_ ID3D11Device* pDevice, _In_ ID3D11DeviceContext* pImmediateContext) = 0;
	virtual void Update(_In_ FLOAT deltaTime) = 0;

	virtual UINT GetNumVertices() const = 0;
	virtual UINT GetNumIndices() const = 0;

	void SetVertexShader(_In_ const std::shared_ptr<VertexShader>& vertexShader);
	void SetPixelShader(_In_ const std::shared_ptr<PixelShader>& pixelShader);

	ComPtr<ID3D11VertexShader>& GetVertexShader();
	ComPtr<ID3D11PixelShader>& GetPixelShader();
	ComPtr<ID3D11InputLayout>& GetVertexLayout();

	ComPtr<ID3D11Buffer>& GetVertexBuffer();
	ComPtr<ID3D11Buffer>& GetIndexBuffer();
	ComPtr<ID3D11Buffer>& GetConstantBuffer();
	ComPtr<ID3D11Buffer>& GetNormalBuffer();

	const XMMATRIX& GetWorldMatrix() const;

	const XMFLOAT4& GetOutputColor() const;
	BOOL HasTexture() const;
	BOOL HasNormalMap() const;

	void AddMaterial(_In_ const std::shared_ptr<Material>& material);
	HRESULT SetMaterialOfMesh(_In_ const UINT uMeshIndex, _In_ const UINT uMaterialIndex);

	void RotateX(_In_ FLOAT angle);
	void RotateY(_In_ FLOAT angle);
	void RotateZ(_In_ FLOAT angle);
	void RotateRollPitchYaw(_In_ FLOAT roll, _In_ FLOAT pitch, _In_ FLOAT yaw);
	void Scale(_In_ FLOAT scaleX, _In_ FLOAT scaleY, _In_ FLOAT scaleZ);
	void Translate(_In_ const XMVECTOR& offset);
	void RotateYInObjectCoordinate(float angle, XMVECTOR Offset);
	void calculateNormalMapVectors();
	void calculateTBT(_In_ const SimpleVertex& v1, _In_ const SimpleVertex& v2, _In_ const SimpleVertex& v3, _Out_ XMFLOAT3& tangent, _Out_ XMFLOAT3& bitangent);

	static constexpr const UINT INVALID_MATERIAL = (0xFFFFFFFF);

	const std::shared_ptr<Material>& GetMaterial(UINT);
	const BasicMeshEntry& GetMesh(UINT);
	UINT GetNumMeshes() const;
	UINT GetNumMaterials() const;

};
