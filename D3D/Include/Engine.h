#pragma once

#include "Graphics.h"

class Engine
{
public:
  ~Engine();
  
  bool InitializeGraphics(HWND hwnd);
  bool Initialize(HINSTANCE hinstance, HWND hwnd);
  void Run();

  void Release();
  
  Graphics* GetGraphics();
  static Engine* GetEngine();
private:
  Engine();
  void Update();
  void Render();

  Graphics* m_graphics;
  static Engine* m_instance;
};

