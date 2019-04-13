#include "DXManager.h"



DXManager::DXManager()
{
  m_swapChain = 0;
  m_device = 0;
  m_deviceContext = 0;
  m_renderTargetView = 0;
  m_depthStencilBuffer = 0;
  m_depthStencilState = 0;
  m_depthStencilView = 0;
  m_rasterState = 0;
  m_alphaEnableBlendingState = 0;
  m_alphaDisableBlendingState = 0;
  m_depthDisableStencilState = 0;

}


DXManager::~DXManager()
{
  if(m_swapChain) {
    m_swapChain->SetFullscreenState(false, NULL);
  }
  
  if(m_device) {
    m_device->Release();
    m_device = 0;
  }

  if (m_deviceContext) {
    m_deviceContext->Release();
    m_deviceContext = 0;
  }

  if (m_renderTargetView) {
    m_renderTargetView->Release();
    m_renderTargetView = 0;
  }

  if (m_depthStencilBuffer) {
    m_depthStencilBuffer->Release();
    m_depthStencilBuffer = 0;
  }

  if (m_depthStencilState) {
    m_depthStencilState->Release();
    m_depthStencilState = 0;
  }

  if (m_depthStencilView) {
    m_depthStencilView->Release();
    m_depthStencilView = 0;
  }

  if (m_rasterState) {
    m_rasterState->Release();
    m_rasterState = 0;
  }

  if (m_alphaEnableBlendingState) {
    m_alphaEnableBlendingState->Release();
    m_alphaEnableBlendingState = 0;
  }

  if (m_alphaDisableBlendingState) {
    m_alphaDisableBlendingState->Release();
    m_alphaDisableBlendingState = 0;
  }

  if (m_depthDisableStencilState) {
    m_depthDisableStencilState->Release();
    m_depthDisableStencilState = 0;
  }
}

bool DXManager::Initialize(int screenWigth, int screenHeight, bool vsync, HWND hwnd, bool fullscreen)
{
  HRESULT result;
  IDXGIFactory* factory;
  IDXGIAdapter* adapter;
  IDXGIOutput* adapterOutput;
  unsigned int numModes, i, numerator, denominator, stringLength;
  DXGI_MODE_DESC* displayModeList;
  DXGI_ADAPTER_DESC* adapterDesc = 0;
  int error;
  ID3D11Texture2D* backBufferPtr;
  
  //store vsync settings
  m_vsync_enabled = vsync;
  
  //Create a Directx graphic interface factory
  result = CreateDXGIFactory(__uuidof(IDXGIFactory), (void**)&factory);
  if(FAILED(result)) {
    return false;
  }

  //use the factory to create an adapter for the primary graphic interface
  result = factory->EnumAdapters(0, &adapter);
  if(FAILED(result)) {
    return false;
  }

  //Enumarate the primary adapter  output
  result = adapter->EnumOutputs(0, &adapterOutput);
  if(FAILED(result)) {
    return false;
  }

  //Get the numver of modes that fit the DXGI_FORMAT_R8G8B8A8_UNORM display format for the adpater output
  result = adapterOutput->GetDisplayModeList(DXGI_FORMAT_B8G8R8A8_UNORM, DXGI_ENUM_MODES_INTERLACED, &numModes, NULL);
  if(FAILED(result)) {
    return false;
  }

  //Create a list to hold all the posible modes for this monitor/video card combination
    displayModeList = new DXGI_MODE_DESC[numModes];
  // Fill the display mode list
  result = adapterOutput->GetDisplayModeList(DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_ENUM_MODES_INTERLACED, &numModes, displayModeList);
  if(FAILED(result)) {
    return false;
  }

  for(int i = 0; i < numModes; i++) {
    if(displayModeList[i].Width == (unsigned int)screenWigth) {
      if(displayModeList[i].Height == (unsigned int)screenHeight) {
        numerator = displayModeList[i].RefreshRate.Numerator;
        denominator = displayModeList[i].RefreshRate.Denominator;
      }
    }
  }

  if(numerator == 0 && denominator == 0) {
    return false;
  }

  //Get the adapter description
  result = adapter->GetDesc(adapterDesc);
  if(FAILED(result)) {
    return false;
  }

  //Store the video card memory in mbs
  m_videoCardMemory = (int)(adapterDesc->DedicatedVideoMemory / 1024 / 1024);
  
  //Convert the name of the video card to a charater
  error = wcstombs_s(&stringLength, m_videoCardDescription,128, adapterDesc->Description, 128);
  if(error != 0) {
    return false;
  }  

  //release memory
  delete [] displayModeList;
  displayModeList = 0;
  
  adapterOutput->Release();
  adapterOutput = 0;

  adapter->Release();
  adapter = 0;  

  factory->Release();
  factory = 0;

  if(!InitializeSwapChain(hwnd, fullscreen, screenWigth, screenHeight, numerator, denominator)) {
    return false;
  }

  //Get the pinter to the back buffer
  result = m_swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&backBufferPtr);

  if(FAILED(result) ) {
   return false;
  }

  //Create the render target view
  result = m_device->CreateRenderTargetView(backBufferPtr, NULL, &m_renderTargetView);
  if (FAILED(result)) {
    return false;
  }

  //release back buffer
  backBufferPtr->Release();
  backBufferPtr = 0;

  if(!InitilizaDepthBuffer(screenWigth, screenHeight)) {
    return false;
  }

  return true;
}

bool DXManager::InitializeSwapChain(HWND hwnd, bool fullscreen, int screenWidth, int screenHeight, unsigned int numerator, unsigned int denominator)
{
  DXGI_SWAP_CHAIN_DESC swapChainDesc;
  D3D_FEATURE_LEVEL featureLevel;
  HRESULT result;
  
  //Initialize swap chain
  ZeroMemory(&swapChainDesc, sizeof(swapChainDesc));
  
  //set to a single back buffer
  swapChainDesc.BufferCount = 1;

  //set the width and height of the back buffer
  swapChainDesc.BufferDesc.Width = screenWidth;
  swapChainDesc.BufferDesc.Height = screenHeight;
  
  //set regular 32-bit back buffer
  swapChainDesc.BufferDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
  
  //SET the refresh rate of the back buffer
  if(m_vsync_enabled) {
    swapChainDesc.BufferDesc.RefreshRate.Numerator = numerator;
    swapChainDesc.BufferDesc.RefreshRate.Denominator = denominator;  
  } else {
    swapChainDesc.BufferDesc.RefreshRate.Numerator = 0;
    swapChainDesc.BufferDesc.RefreshRate.Denominator = 1;
  }

  //Set the usage of the back buffer
  swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;

  //Set the handle for the window
  swapChainDesc.OutputWindow = hwnd;

  //turn multisapling off
  swapChainDesc.SampleDesc.Count = 1;
  swapChainDesc.SampleDesc.Quality = 0;

  //Set to full screen or windowed mode
  swapChainDesc.Windowed = fullscreen;
  
  //Set the scan line ordering an scaling to unpecified
  swapChainDesc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
  swapChainDesc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;

  //Discrd the back buffet content adter presenting
  swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

  swapChainDesc.Flags = 0;
  featureLevel = D3D_FEATURE_LEVEL_11_0;

  result = D3D11CreateDeviceAndSwapChain(NULL, D3D_DRIVER_TYPE_HARDWARE, NULL, 0, &featureLevel, 1, D3D11_SDK_VERSION, &swapChainDesc, &m_swapChain, &m_device, NULL, &m_deviceContext);

  if(FAILED(result)) {
    return false;
  }

  return false;
}

bool DXManager::InitilizaDepthBuffer(int screenWidth, int screenHeight)
{
  D3D11_TEXTURE2D_DESC depthBufferDesc;
  HRESULT result;

  //

  return false;
}
