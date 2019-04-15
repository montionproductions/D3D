#include "Sprite.h"

Sprite::Sprite(float size)
{
  m_vertexBuffer = NULL;
  m_texture = NULL;

  m_size = size;
  
}

Sprite::~Sprite()
{
  if(m_vertexBuffer) {
    delete m_vertexBuffer;
    m_vertexBuffer = NULL;
  }

  delete m_texture;
  m_texture = NULL;
}

void Sprite::Initialize(ID3D11Device * device, Shader * shader, LPCSTR textureFileName)
{
  m_shader = shader;

  m_vertexBuffer = new VertexBuffer();
  m_vertexBuffer->Initialize(device, shader, m_size);

  if(textureFileName != NULL) {
    m_texture = new Texture();
    m_texture->Initialized(device, textureFileName);
  }
}

void Sprite::Render(ID3D11DeviceContext * deviceContext, D3DXMATRIX worldMatrix, D3DXMATRIX viewMatrix, D3DXMATRIX projectionsMatrix)
{
  if(m_texture) {
    m_shader->SetShaderParameters(deviceContext, m_texture->GetTexture());
    m_shader->SetShaderParameters(deviceContext, worldMatrix, viewMatrix, projectionsMatrix);

    m_vertexBuffer->Render(deviceContext);
  }
}
