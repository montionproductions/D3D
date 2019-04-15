#pragma once

#include "VertexBuffer.h"
#include "Texture.h"

class Sprite {

public:
  Sprite(float size);

  ~Sprite();

  void Initialize(ID3D11Device* device, Shader* shader, LPCSTR textureFileName);
  void Update() {}
  void Render(ID3D11DeviceContext* deviceContext, 
              D3DXMATRIX worldMatrix,
              D3DXMATRIX viewMatrix,
              D3DXMATRIX projectionsMatrix);

private:
  VertexBuffer* m_vertexBuffer;
  Texture* m_texture;
  Shader* m_shader;
  float m_size;             

};