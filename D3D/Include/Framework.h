#pragma once

#include "Engine.h"

class Framework
{
public:
  Framework();
  ~Framework(); 

  bool Initilize();
  void Run();

private:  

  bool CreateDXWindow(const char* windowTile,
                      int x,
                      int y,
                      int width,
                      int height);

  const char* m_applicationName;
  HINSTANCE m_hInstace;
  
};

