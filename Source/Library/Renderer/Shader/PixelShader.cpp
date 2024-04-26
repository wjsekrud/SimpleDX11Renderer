#include "PixelShader.h"

PixelShader::PixelShader(_In_ PCWSTR pszFileName, _In_ PCSTR pszEntryPoint, _In_ PCSTR  pszShaderModel)
	:Shader(pszFileName, pszEntryPoint, pszShaderModel)
	, m_pixelShader()
{
}

HRESULT PixelShader::Initialize(_In_ ID3D11Device * pDevice)
{
    ComPtr<ID3DBlob> psBlob;

    HRESULT hr = compile(psBlob.GetAddressOf());

    if (FAILED(hr))
    {
        WCHAR szMessage[256];
        swprintf_s(
            szMessage,
            L"The FX file %s cannot be compiled. Please run this executable from the directory that contains the FX file.",
            m_pszFileName, m_pszEntryPoint
        );
        MessageBox(
            nullptr,
            L"The PSFX file cannot be compiled. Please run this executable from the directory that contains the FX file.",
            L"Error",
            MB_OK
        );
        return hr;
    }

    hr = pDevice->CreatePixelShader(psBlob->GetBufferPointer(), psBlob->GetBufferSize(), nullptr, m_pixelShader.GetAddressOf());

    return hr;
}

ComPtr<ID3D11PixelShader>& PixelShader::GetPixelShader()
{
    return m_pixelShader;
}