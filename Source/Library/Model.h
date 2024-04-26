#pragma once
#include "Renderer/Renderable.h"

struct aiScene;
struct aiMesh;
struct aiMaterial;
struct aiAnimation;
struct aiBone;
struct aiNode;
struct aiNodeAnim;

namespace Assimp
{
	class Importer;
}

class Model : public Renderable {
protected:
    struct VertexBoneData
    {
        VertexBoneData()
            : aBoneIds{ 0u, }
            , aWeights{ 0.0f, }
            , uNumBones(0u)
        {
            ZeroMemory(aBoneIds, ARRAYSIZE(aBoneIds) * sizeof(aBoneIds[0]));
            ZeroMemory(aWeights, ARRAYSIZE(aWeights) * sizeof(aWeights[0]));
        }

        void AddBoneData(_In_ UINT uBoneId, _In_ FLOAT weight)
        {
            assert(uNumBones < ARRAYSIZE(aBoneIds));

            aBoneIds[uNumBones] = uBoneId;
            aWeights[uNumBones] = weight;

            static CHAR szDebugMessage[256];
            sprintf_s(szDebugMessage, "\t\t\tBone %d, weight: %f, index %u\n", aBoneIds[uNumBones], weight, uNumBones);
            OutputDebugStringA(szDebugMessage);

            ++uNumBones;
        }

        UINT aBoneIds[MAX_NUM_BONES_PER_VERTEX];
        FLOAT aWeights[MAX_NUM_BONES_PER_VERTEX];
        UINT uNumBones;
    };

    struct BoneInfo
    {
        BoneInfo() = default;
        BoneInfo(const XMMATRIX& Offset)
            : OffsetMatrix(Offset)
            , FinalTransformation()
        {
        }

        XMMATRIX OffsetMatrix;
        XMMATRIX FinalTransformation;
    };

    ComPtr<ID3D11Buffer> m_animationBuffer;
    ComPtr<ID3D11Buffer> m_skinningConstantBuffer;
    std::vector<VertexBoneData> m_aBoneData;
    std::vector<AnimationData> m_aAnimationData;
    std::vector<BoneInfo> m_aBoneInfo;
    std::vector<XMMATRIX> m_aTransforms;
    std::unordered_map<std::string, UINT> m_boneNameToIndexMap;
    const aiScene* m_pScene;

    float m_timeSinceLoaded;

    XMMATRIX m_globalInverseTransform;


    const aiNodeAnim* findNodeAnimOrNull(_In_ const aiAnimation* pAnimation, _In_ PCSTR pszNodeName);
    UINT findPosition(_In_ FLOAT animationTimeTicks, _In_ const aiNodeAnim* pNodeAnim);
    UINT findRotation(_In_ FLOAT animationTimeTicks, _In_ const aiNodeAnim* pNodeAnim);
    UINT findScaling(_In_ FLOAT animationTimeTicks, _In_ const aiNodeAnim* pNodeAnim);
    UINT getBoneId(_In_ const aiBone* pBone);

    void initMeshBones(_In_ UINT uMeshIndex, _In_ const aiMesh* pMesh);
    void initMeshSingleBone(_In_ UINT uBoneIndex, _In_ const aiBone* pBone);
    void interpolatePosition(_Inout_ XMFLOAT3& outTranslate, _In_ FLOAT animationTimeTicks, _In_ const aiNodeAnim* pNodeAnim);
    void interpolateRotation(_Inout_ XMVECTOR& outQuaternion, _In_ FLOAT animationTimeTicks, _In_ const aiNodeAnim* pNodeAnim);
    void interpolateScaling(_Inout_ XMFLOAT3& outScale, _In_ FLOAT animationTimeTicks, _In_ const aiNodeAnim* pNodeAnim);

    void readNodeHierarchy(_In_ FLOAT animationTimeTicks, _In_ const aiNode* pNode, _In_ const XMMATRIX& parentTransform);

protected:

    static std::unique_ptr<Assimp::Importer> sm_pImporter;

	std::filesystem::path m_filePath;
	std::vector<SimpleVertex> m_aVertices;
	std::vector<WORD> m_aIndices;

	void countVerticesAndIndices(UINT&, UINT&, const aiScene*);
	const SimpleVertex* getVertices() const override;
	const WORD* getIndices() const override;

	void initAllMeshes(const aiScene*);
	HRESULT initFromScene(ID3D11Device*, ID3D11DeviceContext*, const aiScene*, const std::filesystem::path&);
	HRESULT initMaterials(ID3D11Device*, ID3D11DeviceContext*, const aiScene*, const std::filesystem::path&);
	void initSingleMesh(UINT, const aiMesh*);

	void loadColors(const aiMaterial*, UINT);
	HRESULT loadDiffuseTexture(ID3D11Device*, ID3D11DeviceContext* pImmediateContext, const std::filesystem::path&, const aiMaterial*, UINT);
	HRESULT loadSpecularTexture(ID3D11Device*, ID3D11DeviceContext* pImmediateContext, const std::filesystem::path&, const aiMaterial*, UINT);
	HRESULT loadTextures(ID3D11Device*, ID3D11DeviceContext*, const std::filesystem::path&, const aiMaterial*, UINT);

	void reserveSpace(UINT, UINT);

public:
	Model() = default;
	Model(const std::filesystem::path&);
	virtual ~Model() = default;
	HRESULT Initialize(ID3D11Device*, ID3D11DeviceContext*) override ;
	virtual void Update(FLOAT deltatime) override;
	virtual UINT GetNumVertices() const override;
	virtual UINT GetNumIndices() const override;

    ComPtr<ID3D11Buffer>& GetAnimationBuffer();
    ComPtr<ID3D11Buffer>& GetSkinningConstantBuffer();

    std::vector<XMMATRIX>& GetBoneTransforms();
    const std::unordered_map<std::string, UINT>& GetBoneNameToIndexMap() const;

};


