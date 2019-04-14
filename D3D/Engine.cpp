#include "Engine.h"

Engine* Engine::m_instance = NULL;

Engine::Engine()
{
  m_graphics = NULL;
}

void Engine::Update()
{

  
}

void Engine::Render()
{
  m_graphics->BeginScene(1.0f, 0.f, 0.f, 0.f);

  //render stuff goes here

  m_graphics->EndScene();
}


Engine::~Engine()
{
  if (m_graphics) {
    delete m_graphics;
    m_graphics = NULL;
  }
}

bool Engine::InitializeGraphics(HWND hwnd)
{
  m_graphics = new Graphics();

  return m_graphics->InitializeDX(hwnd);
}

bool Engine::Initialize(HINSTANCE hinstance, HWND hwnd)
{ 
  m_graphics->Initialize();
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
