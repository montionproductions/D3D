#include "Texture.h"

Texture::Texture()
{
  m_texture = 0;
  m_width = 0;
  m_height = 0;
}

Texture::~Texture()
{
  if(m_texture) {
    m_texture->Release();
    m_texture = NULL;
  }
}

bool Texture::Initialized(ID3D11Device * device, LPCSTR fileName)
{
  HRESULT result;

  result = D3DX11CreateShaderResourceViewFromFile(device, fileName, NULL, NULL, &m_texture, NULL);
  if(FAILED(result)) {
    return false;
  }

  //Get width and height
  ID3D11Resource* resource = 0;
  m_texture->GetResource(&resource);

  ID3D11Texture2D* texture2D = 0;
  result = resource->QueryInterface(&texture2D);
  if(SUCCEEDED(result)) {
    D3D11_TEXTURE2D_DESC desc;
    texture2D->GetDesc(&desc);
    
    m_width = desc.Width;
    m_height = desc.Height;
  }

  texture2D->Release();
  resource->Release();
  
  return true;
}

ID3D11ShaderResourceView * Texture::GetTexture()
{
  return m_texture;
}

int Texture::GetWidth()
{
  return m_width;
}

int Texture::GetHeight()
{
  return m_height;
}
