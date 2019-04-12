#include "Framework.h"
#include "SystemDefs.h"

#include <iostream>
using namespace std;

LRESULT CALLBACK WndProc(HWND hwnd,
                         UINT message,
                         WPARAM wParam,
                         LPARAM lParam);

Framework::Framework()
{
}


Framework::~Framework()
{
  if(FULL_SCREEN) {
    ChangeDisplaySettings(NULL, 0);
  }

  UnregisterClass(m_applicationName, m_hInstace);
  m_hInstace = NULL;
}

bool
Framework::Initilize() {

  if(!CreateDXWindow("Engine", WINDOW_POSX, WINDOW_POSY, SCREEN_WIGTH, SCREEN_HEIGTH)) {
    return false;
  }

  return true;
}

void 
Framework::Run() {
  MSG msg;
  ZeroMemory(&msg, sizeof(MSG));

  //update loop
  while(msg.message != WM_QUIT) {
    if(PeekMessage(&msg, NULL, 0U, 0U, PM_REMOVE)) {
      TranslateMessage(&msg);
      DispatchMessage(&msg);
    }
    else {
      //update & render fuctions
    }
  }
}

bool Framework::CreateDXWindow(const char* windowTile,
                               int x,
                               int y,
                               int width,
                               int height) {
  HWND hwnd;
  WNDCLASSEX wc;
  
  m_applicationName = windowTile;

  m_hInstace = GetModuleHandle(NULL);
  
  //Setup the win class with default settings

  wc.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
  wc.lpfnWndProc = WndProc;
  wc.cbClsExtra =0;
  wc.cbWndExtra =0;
  wc.hInstance = m_hInstace;
  wc.hIcon = LoadIcon(NULL, IDI_WINLOGO);
  wc.hIconSm = wc.hIcon;
  wc.hCursor = LoadCursor(NULL, IDC_ARROW);
  wc.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
  wc.lpszMenuName = NULL;
  wc.lpszClassName = m_applicationName;
  wc.cbSize = sizeof(WNDCLASSEX);  

  if(!RegisterClassEx(&wc)) {
    MessageBox(NULL, "RegisterClassEx() failed", "Error", MB_OK);

    return false;
  }
  
  int screenWigth = GetSystemMetrics(SM_CXSCREEN);
  int screenHeigth = GetSystemMetrics(SM_CYSCREEN);
  
  if(FULL_SCREEN){ 
    DEVMODE dmScreenSettings;
    memset(&dmScreenSettings, 0, sizeof(dmScreenSettings));
    dmScreenSettings.dmSize = sizeof(dmScreenSettings);
    dmScreenSettings.dmPelsWidth = (unsigned long) screenWigth;
    dmScreenSettings.dmPelsHeight = (unsigned long) screenHeigth;
    dmScreenSettings.dmBitsPerPel = 32;
    dmScreenSettings.dmFields = DM_BITSPERPEL | DM_PELSWIDTH | DM_PELSHEIGHT;

    ChangeDisplaySettings(&dmScreenSettings, CDS_FULLSCREEN);
  } else {
    screenWigth = width;
    screenHeigth = height;
  }
  
  //TODO: FULL SCREEN
  int nStyle = WS_OVERLAPPED | WS_SYSMENU | WS_VISIBLE | WS_CAPTION | WS_MINIMIZEBOX;
  hwnd = CreateWindowEx(WS_EX_APPWINDOW, windowTile, windowTile, nStyle, x, y, width, height, NULL, NULL, m_hInstace, NULL);
  if(hwnd == NULL) {
    MessageBox(NULL, "CreateWindowEx() failed", "Error", MB_OK);
    PostQuitMessage(0);

    return false;
  }

  ShowWindow(hwnd, SW_SHOW);
  SetForegroundWindow(hwnd);
  SetFocus(hwnd);

  return false;

}

LRESULT CALLBACK WndProc(HWND hwnd,
                         UINT message,
                         WPARAM wParam,
                         LPARAM lParam) {
  PAINTSTRUCT ps;
  HDC hdc;
  switch (message)
  {
  case WM_PAINT:
    {
      hdc = BeginPaint(hwnd, &ps);
      EndPaint(hwnd, &ps);
    } break;
  case WM_CLOSE: 
    {
      PostQuitMessage(0);
      DestroyWindow(hwnd);
    } break;

  default: 
    {
      return DefWindowProc(hwnd, message, wParam, lParam);
    }
  }
}
