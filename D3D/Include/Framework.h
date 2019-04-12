#pragma once

#include <Windows.h>

class Framework
{
public:
  Framework();
  ~Framework(); 

  bool Initilize();
  void Run();

private:  

  bool CreateDXWindow(char* windowTile,
                      int x,
                      int y,
                      int width,
                      int height);

  const char* m_applicationName;
  HINSTANCE m_hInstace;
  
};

