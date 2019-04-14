#include "TextureShader.h"

TextureShader::TextureShader(ID3D11Device * device, HWND hwnd, LPCSTR shaderFileName, LPCSTR vertexFuncName, LPCSTR pixelFuncName)
  : Shader(device, hwnd, shaderFileName, vertexFuncName, pixelFuncName)
{
  m_initialized = InitializeSamplerState(device);
}

TextureShader::~TextureShader()
{
  if(m_samplerState) {
    m_samplerState->Release();
    m_samplerState = NULL;
  }
}

void TextureShader::Begin(ID3D11DeviceContext * deviceContext, int indexCount)
{
  deviceContext->PSSetSamplers(0,1,&m_samplerState);
  Shader::Begin(deviceContext, indexCount);
}

void TextureShader::End(ID3D11DeviceContext * deviceContext)
{
  deviceContext->PSSetSamplers(0, 0, NULL);
  Shader::End(deviceContext);
}

bool TextureShader::Initialize(ID3D11Device * device, HWND hwnd, LPCSTR shaderFileName, LPCSTR vertexFuncName, LPCSTR pixelFuncName)
{
  if(!Shader::Initialize(device, hwnd, shaderFileName, vertexFuncName, pixelFuncName)) {
    return false;
  }

  
  
  return true;
}

bool TextureShader::InitializeSamplerState(ID3D11Device * device)
{
  D3D11_SAMPLER_DESC samplerDesc;
  HRESULT result;

  //Create texture sampler desc
  samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
  samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
  samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
  samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
  samplerDesc.MipLODBias = 0.0f;
  samplerDesc.MaxAnisotropy = 1;
  samplerDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
  samplerDesc.BorderColor[0] = 1;
  samplerDesc.BorderColor[1] = 1;
  samplerDesc.BorderColor[2] = 1;
  samplerDesc.BorderColor[3] = 1;
  samplerDesc.MinLOD = 0;
  samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;

  //Create sampler state
  result = device->CreateSamplerState(&samplerDesc, &m_samplerState);
  if (FAILED(result)) {
    return false;
  }
  return true;
}
