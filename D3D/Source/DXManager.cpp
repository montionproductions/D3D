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
  unsigned int numModes, numerator = 0, denominator = 0, stringLength;
  DXGI_MODE_DESC* displayModeList;
  DXGI_ADAPTER_DESC adapterDesc;
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
  result = adapter->GetDesc(&adapterDesc);
  if(FAILED(result)) {
    return false;
  }

  //Store the video card memory in mbs
  m_videoCardMemory = (int)(adapterDesc.DedicatedVideoMemory / 1024 / 1024);
  
  //Convert the name of the video card to a charater
  error = wcstombs_s(&stringLength, m_videoCardDescription,128, adapterDesc.Description, 128);
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

  if(!InitilizeDepthStencilBuffer()) {
    return false;
  }

  if (!InitilizeStencilView()) {
    return false;
  }

  //bind the render target vire and depth stencil buffer to the output render pipelene
  m_deviceContext->OMSetRenderTargets(1, &m_renderTargetView, m_depthStencilView);

  if(!InitilizeRasterizerState()) {
    return false;
  }

  InitilizeViewport(screenWigth, screenHeight);

  if(!InitilizeAlphaBlending()) {
    return false;
  }

  if (!InitilaizeZBuffer()) {
    return false;
  }
  
  return true;
}

void DXManager::BeginScene(float r, float g, float b, float a)
{
  float color[4];
  
  color[0] = r;
  color[1] = g;
  color[2] = b;
  color[3] = a;

  //clear the backbuffer
  m_deviceContext->ClearRenderTargetView(m_renderTargetView, color);
  
  //clear tje depth buffer
  m_deviceContext->ClearDepthStencilView(m_depthStencilView, D3D11_CLEAR_DEPTH, 1.0f, 0);
  
}

void DXManager::EndScene()
{
  if(m_vsync_enabled) {
    //lock to screen refresh rate
    m_swapChain->Present(1,0);
  } else {
    //present as fast as possible
    m_swapChain->Present(0, 0);
  }
}

void DXManager::EnableAlphaBlending(bool enable)
{
  float blendFactor[4];
  blendFactor[0] = 0.0f;
  blendFactor[0] = 0.0f;
  blendFactor[0] = 0.0f;
  blendFactor[0] = 0.0f;
  
  if(enable) {
    m_deviceContext->OMSetBlendState(m_alphaEnableBlendingState, blendFactor, 0xffffffff);
  } else {
    m_deviceContext->OMSetBlendState(m_alphaDisableBlendingState, blendFactor, 0xffffffff);
  } 
}

void DXManager::EnableZBuffer(bool enable)
{
  if(enable) {
   m_deviceContext->OMSetDepthStencilState(m_depthStencilState, 1);
  } else {
    m_deviceContext->OMSetDepthStencilState(m_depthDisableStencilState, 1);
  }
}

ID3D11Device * DXManager::GetDevide()
{
  return m_device;
}

ID3D11DeviceContext * DXManager::GetDeviceContext()
{
  return m_deviceContext;
}

bool DXManager::InitializeSwapChain(HWND hwnd, bool fullscreen, int screenWidth, int screenHeight, unsigned int numerator, unsigned int denominator)
{
  DXGI_SWAP_CHAIN_DESC swapChainDesc;
  D3D_FEATURE_LEVEL featureLevel;
  HRESULT result;
  
  //Initialize swap chain
  ZeroMemory(&swapChainDesc, sizeof(DXGI_SWAP_CHAIN_DESC));
  
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

  return true;
}

bool DXManager::InitilizaDepthBuffer(int screenWidth, int screenHeight)
{
  D3D11_TEXTURE2D_DESC depthBufferDesc;
  HRESULT result;

  ZeroMemory(&depthBufferDesc, sizeof(D3D11_TEXTURE2D_DESC));

  //Set up depth buffer desc
  depthBufferDesc.Width = screenWidth;
  depthBufferDesc.Height = screenHeight;
  depthBufferDesc.MipLevels = 1;
  depthBufferDesc.ArraySize = 1;
  depthBufferDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
  depthBufferDesc.SampleDesc.Count = 1;
  depthBufferDesc.SampleDesc.Quality = 0;
  depthBufferDesc.Usage = D3D11_USAGE_DEFAULT;
  depthBufferDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
  depthBufferDesc.CPUAccessFlags = 0;
  depthBufferDesc.MiscFlags = 0;

  //create texture for the deph buffer
  result = m_device->CreateTexture2D(&depthBufferDesc, NULL, &m_depthStencilBuffer);
  if(FAILED(result)) {
    return false;
  }


  return true;
}

bool DXManager::InitilizeDepthStencilBuffer()
{

  D3D11_DEPTH_STENCIL_DESC depthStencilDesc;
  HRESULT result;

  //Initilize depth stencil desc
  ZeroMemory(&depthStencilDesc, sizeof(D3D11_DEPTH_STENCIL_DESC));
  
  //Set up dscription
  depthStencilDesc.DepthEnable = TRUE;
  depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
  depthStencilDesc.DepthFunc = D3D11_COMPARISON_LESS;
  depthStencilDesc.StencilEnable  = TRUE;
  depthStencilDesc.StencilReadMask = 0xFF;  
  depthStencilDesc.StencilWriteMask = 0xFF;
   
  //stenil operations id puxel front-facing
  depthStencilDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
  depthStencilDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
  depthStencilDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
  depthStencilDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
  
  //STENCIL operatoon f pixels is back-facig
  depthStencilDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
  depthStencilDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
  depthStencilDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
  depthStencilDesc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
  
  //createdephth stencil state
  result = m_device->CreateDepthStencilState(&depthStencilDesc, &m_depthStencilState);
  if(FAILED(result)) {
    return false;
  }

  //set depth stencil state
  m_deviceContext->OMSetDepthStencilState(m_depthStencilState, 1);
  
  return true;
}

bool DXManager::InitilizeStencilView()
{

  D3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc;
  HRESULT result;
  
  //Imiyia;ie description 
  ZeroMemory(&depthStencilViewDesc, sizeof(D3D11_DEPTH_STENCIL_VIEW_DESC));

  //Set up description
  depthStencilViewDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
  depthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
  depthStencilViewDesc.Texture2D.MipSlice = 0;
  
  //crete depth stencil view
  result = m_device->CreateDepthStencilView(m_depthStencilBuffer, &depthStencilViewDesc, &m_depthStencilView);
  if(FAILED(result)) {
    return false;
  }

  return true;
}

bool DXManager::InitilizeRasterizerState()
{
  D3D11_RASTERIZER_DESC rasterDesc;
  HRESULT result;
  
  //Set ip descripcion
  rasterDesc.AntialiasedLineEnable = FALSE;
  rasterDesc.CullMode = D3D11_CULL_BACK;
  rasterDesc.DepthBias = 0;
  rasterDesc.DepthBiasClamp = 0.0f;
  rasterDesc.DepthClipEnable = TRUE;
  rasterDesc.FillMode = D3D11_FILL_SOLID;
  rasterDesc.FrontCounterClockwise = FALSE;
  rasterDesc.MultisampleEnable = FALSE;
  rasterDesc.ScissorEnable = FALSE;
  rasterDesc.SlopeScaledDepthBias = 0.0f;
  
  //Create rasterizer state
  result = m_device->CreateRasterizerState(&rasterDesc, &m_rasterState);
  if(FAILED(result)) {
    return false;
  }

  //Set the rasterazer
  m_deviceContext->RSSetState(m_rasterState);
  
  return true;
}

void DXManager::InitilizeViewport(int screenWidth, int screenHeight)
{
  D3D11_VIEWPORT viewport;
  
  //setup
  viewport.Width = (float)screenWidth;
  viewport.Height = (float)screenHeight;
  viewport.MinDepth = 0.0f;
  viewport.MaxDepth = 1.0f;
  viewport.TopLeftX = 0.0f;
  viewport.TopLeftY = 0.0f;

  //Create viewport
  m_deviceContext->RSSetViewports(1, &viewport);
  
}

bool DXManager::InitilizeAlphaBlending()
{
  D3D11_BLEND_DESC blendStateDesc;
  HRESULT result;

  ZeroMemory(&blendStateDesc, sizeof(D3D11_BLEND_DESC));

  //Initilize/cleat description
  blendStateDesc.RenderTarget[0].BlendEnable = TRUE;
  blendStateDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
  blendStateDesc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
  blendStateDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
  blendStateDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
  blendStateDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
  blendStateDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
  blendStateDesc.RenderTarget[0].RenderTargetWriteMask = 0x0f;
  
  //create the blend sttate
  result = m_device->CreateBlendState(&blendStateDesc, &m_alphaEnableBlendingState);

  if(FAILED(result) ) {
    return false;
  }

  //Modify to create disable alpha blend state
  blendStateDesc.RenderTarget[0].BlendEnable = false;
  
  //Create the disabled blind state
  result = m_device->CreateBlendState(&blendStateDesc, &m_alphaDisableBlendingState);

  if (FAILED(result)) {
    return false;
  }

  return true;
}

bool DXManager::InitilaizeZBuffer()
{
  D3D11_DEPTH_STENCIL_DESC depthStencilDesc;
  HRESULT result;

  //clear the descrip[tcion
  ZeroMemory(&depthStencilDesc, sizeof(depthStencilDesc));

  //Set up dscription
  depthStencilDesc.DepthEnable = FALSE;
  depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
  depthStencilDesc.DepthFunc = D3D11_COMPARISON_LESS;
  depthStencilDesc.StencilEnable = FALSE;
  depthStencilDesc.StencilReadMask = 0xFF;
  depthStencilDesc.StencilWriteMask = 0xFF;

  //stenil operations id puxel front-facing
  depthStencilDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
  depthStencilDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
  depthStencilDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
  depthStencilDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

  //STENCIL operatoon f pixels is back-facig
  depthStencilDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
  depthStencilDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
  depthStencilDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
  depthStencilDesc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
  
   //create
  result = m_device->CreateDepthStencilState(&depthStencilDesc, &m_depthDisableStencilState);
  if(FAILED(result)) {
    return false;
  }
  
  return true;
}
