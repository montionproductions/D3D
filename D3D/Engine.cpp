#include "Engine.h"

Engine* Engine::m_instance = NULL;

Engine::Engine()
{
  m_graphics = NULL;
  vertexBuffer = NULL;
  textureShader = NULL;
  texture = NULL;
}

void Engine::Update()
{

  
}

void Engine::Render()
{
  m_graphics->BeginScene(0.0f, 0.f, 0.f, 0.f);

  //render stuff goes here
  D3DXMATRIX viewMatrix;
  D3DXMATRIX projMatrix;
  D3DXMATRIX worldMatrix;

  D3DXVECTOR3 position = D3DXVECTOR3(0,0,-100.f);
  D3DXVECTOR3 up = D3DXVECTOR3(0,1,0);
  D3DXVECTOR3 lookAt = D3DXVECTOR3(0,0,1);
  
  D3DXMatrixLookAtLH(&viewMatrix, &position, &lookAt, &up);
  D3DXMatrixOrthoLH(&projMatrix, (float)SCREEN_WIGTH, (float)SCREEN_HEIGTH, 0.1f, 1000.f);
  D3DXMatrixIdentity(&worldMatrix);

  m_graphics->EnableAlphaBlending(true);
  
  textureShader->SetShaderParameters(m_graphics->GetDeviceContex(),
                                     texture->GetTexture());
  textureShader->SetShaderParameters(m_graphics->GetDeviceContex(),
                                     worldMatrix, viewMatrix, projMatrix);

  vertexBuffer->Render(m_graphics->GetDeviceContex());

  m_graphics->EndScene();
}


Engine::~Engine()
{
  if (m_graphics) {
    delete m_graphics;
    m_graphics = NULL;
  }

  delete vertexBuffer;
  delete texture;
  delete textureShader;
}

bool Engine::InitializeGraphics(HWND hwnd)
{
  m_graphics = new Graphics();

  return m_graphics->InitializeDX(hwnd);
}

bool Engine::Initialize(HINSTANCE hinstance, HWND hwnd)
{ 
  m_graphics->Initialize();

  textureShader = new TextureShader(m_graphics->GetDevice(),
                                    hwnd,
                                    "texture",
                                    "TextureVertexShader",
                                    "TexturePixelShader");

  texture = new Texture();
  texture->Initialized(m_graphics->GetDevice(),
                       "test.png");

  vertexBuffer = new VertexBuffer();
  vertexBuffer->Initialize(m_graphics->GetDevice(),
                           textureShader,
                           100.f);
  return true;
}

void Engine::Run()
{
  Update();
  Render();
}

void Engine::Release()
{
  if(m_instance) {
    delete m_instance;
    m_instance = NULL;
  }
}

Engine * Engine::GetEngine()
{
  if(m_instance == NULL) {
    m_instance = new Engine();
  }
  return m_instance;
}

Graphics * Engine::GetGraphics()
{
  return m_graphics;
}
