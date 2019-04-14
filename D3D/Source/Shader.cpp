#include "Shader.h"

Shader::Shader(ID3D11Device * device, HWND hwnd, LPCSTR shaderFileName, LPCSTR vertexFuncName, LPCSTR pixelFuncName)
{
  m_vertexShader = 0;
  m_pixelShader = 0;
  m_layout = 0;
  m_matrixBuffer = 0;
  
  m_initialized = Initialize(device, hwnd, shaderFileName, vertexFuncName, pixelFuncName);

}

Shader::~Shader()
{
  //Release the matrix constan buffer
  if(m_matrixBuffer) {
    m_matrixBuffer->Release();
    m_matrixBuffer = 0;
  }

  //Relese layout
  if(m_layout) {
    m_layout->Release();
    m_layout = 0;
  }

  //Release pixel shader
  if(m_pixelShader) {
    m_pixelShader->Release();
    m_pixelShader = 0;
  }

  //Release vertex shader
  if(m_vertexShader) {
    m_vertexShader->Release();
    m_vertexShader = 0;

  }

  if(m_name) {
    delete [] m_name;
    m_name = 0;
  }
}

void Shader::Begin(ID3D11DeviceContext * deviceContext, int indexCount)
{
  //Set the vertex input layout
  deviceContext->IASetInputLayout(m_layout);
  
  //Set tje vertex and pixel shaders that will used to render
  deviceContext->VSSetShader(m_vertexShader, NULL, 0);
  deviceContext->PSSetShader(m_pixelShader, NULL, 0);

  //Render
  deviceContext->DrawIndexed(indexCount, 0, 0);
}

void Shader::End(ID3D11DeviceContext * deviceContext)
{
  deviceContext->IASetInputLayout(NULL);
  deviceContext->VSSetShader(NULL, NULL, 0);
  deviceContext->PSSetShader(NULL, NULL, 0);
}

bool Shader::SetShaderParameters(ID3D11DeviceContext * deviceContext, ID3D11ShaderResourceView * texture)
{
  deviceContext->PSSetShaderResources(0, 1, &texture);
  return true;
}

bool Shader::SetShaderParameters(ID3D11DeviceContext * deviceContext, D3DXMATRIX worldMatrix, D3DXMATRIX viewMatrix, D3DXMATRIX projectionMatrix)
{
  HRESULT result;

  D3D11_MAPPED_SUBRESOURCE mappedResource;
  MatrixBufferType* dataPtr;
  
  //transpose all matrix
  D3DXMatrixTranspose(&worldMatrix, &worldMatrix);
  D3DXMatrixTranspose(&viewMatrix, &viewMatrix);
  D3DXMatrixTranspose(&projectionMatrix, &projectionMatrix);

  //Lock buffer
  result = deviceContext->Map(m_matrixBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
  if(FAILED(result)) {
    return false;
  }

  dataPtr = (MatrixBufferType*)mappedResource.pData;
  dataPtr->worldMatrix = worldMatrix;
  dataPtr->viewMatrix = viewMatrix;
  dataPtr->projectionMatrix = projectionMatrix;

  //Unlock buffer
  deviceContext->Unmap(m_matrixBuffer, 0);


  //Update values en shaders device context
  deviceContext->VSSetConstantBuffers(0, 1, &m_matrixBuffer);

  return true;
}

char * Shader::GetName()
{
  return m_name;
}

bool Shader::IsInitialize()
{
  return m_initialized;
}

bool Shader::Initialize(ID3D11Device * device, HWND hwnd, LPCSTR shaderFileName, LPCSTR vertexFuncName, LPCSTR pixelFuncName)
{

  bool result;
  
  m_name = new char[strlen(shaderFileName) + 1];
  memcpy(m_name, shaderFileName, strlen(shaderFileName) + 1);
  
  char vsFileName[100];
  strcpy_s(vsFileName, shaderFileName);
  strcat_s(vsFileName, ".vs");
  vsFileName[strlen(shaderFileName) + 4] = '\0';
  
  char psFileName[100];
  strcpy_s(psFileName, shaderFileName);
  strcat_s(psFileName, ".ps");
  psFileName[strlen(shaderFileName) + 4] = '\0';

  //Initalize the vertex and pixel shaders
  result = InitializeShader(device, hwnd, vsFileName, psFileName, vertexFuncName, pixelFuncName);
  return true;
}

bool Shader::InitializeShader(ID3D11Device * device, HWND hwnd, LPCSTR vsFileName, LPCSTR psFileName, LPCSTR vertexFuncName, LPCSTR pixelFuncName)
{
  HRESULT result;
  ID3D10Blob* errorMessage = 0;
  ID3D10Blob* vertexShaderBuffer = 0;
  ID3D10Blob* pixelShaderBuffer = 0;
  D3D11_INPUT_ELEMENT_DESC polygonLayout[2];
  unsigned int numElements;
  D3D11_BUFFER_DESC matrixBufferDesc;

  //Compile vertex shader
  result = D3DX11CompileFromFile(vsFileName, 
                                 NULL, 
                                 NULL, 
                                 vertexFuncName, 
                                 "vs_5_0", 
                                 D3D10_SHADER_ENABLE_STRICTNESS, 
                                 0, NULL, &vertexShaderBuffer, &errorMessage, NULL);

  if(FAILED(result)) {
    //If we have error message
    if(errorMessage) {
      OutputShaderErrorMessage(errorMessage, hwnd, vsFileName);
    } else {
      MessageBox(hwnd, vsFileName, "Error in vs shader File", MB_OK);
    }

    return false;
  }

  //Compile pixel shader
  result = D3DX11CompileFromFile(psFileName,
                                 NULL,
                                 NULL,
                                 pixelFuncName,
                                 "ps_5_0",
                                 D3D10_SHADER_ENABLE_STRICTNESS,
                                 0, NULL, &pixelShaderBuffer, &errorMessage, NULL);

  if (FAILED(result)) {
    //If we have error message
    if (errorMessage) {
      OutputShaderErrorMessage(errorMessage, hwnd, psFileName);
    }
    else {
      MessageBox(hwnd, vsFileName, "Error in ps shader File", MB_OK);
    }

    return false;
  }

  //Create vertex shader buffer
  result = device->CreateVertexShader(vertexShaderBuffer->GetBufferPointer(), vertexShaderBuffer->GetBufferSize(), NULL, &m_vertexShader);
  if(FAILED(result)) {
    return false;
  }

  //Create pixel shade buffer
  result = device->CreatePixelShader(pixelShaderBuffer->GetBufferPointer(), pixelShaderBuffer->GetBufferSize(), NULL, &m_pixelShader);
  if (FAILED(result)) {
    return false;
  }

  //Setup the layout of the data that goes into the shader
  polygonLayout[0].SemanticName = "POSITION";
  polygonLayout[0].SemanticIndex = 0;
  polygonLayout[0].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
  polygonLayout[0].InputSlot = 0;
  polygonLayout[0].AlignedByteOffset = 0;
  polygonLayout[0].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
  polygonLayout[0].InstanceDataStepRate = 0;

  polygonLayout[1].SemanticName = "TEXCOORD";
  polygonLayout[1].SemanticIndex = 0;
  polygonLayout[1].Format = DXGI_FORMAT_R32G32_FLOAT;
  polygonLayout[1].InputSlot = 0;
  polygonLayout[1].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
  polygonLayout[1].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
  polygonLayout[1].InstanceDataStepRate = 0;

  numElements = sizeof(polygonLayout) / sizeof(polygonLayout[0]);

  //Crete the vertes input layout
  result = device->CreateInputLayout(polygonLayout, 
                                     numElements, 
                                     vertexShaderBuffer->GetBufferPointer(),
                                     vertexShaderBuffer->GetBufferSize(), 
                                     &m_layout);
  if(FAILED(result)) {
   return false;
  }

  //Release shader buffers
  vertexShaderBuffer->Release();
  vertexShaderBuffer = 0;
  
  pixelShaderBuffer->Release();
  pixelShaderBuffer = 0;

  //Setup the matrix buffer description
  matrixBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
  matrixBufferDesc.ByteWidth = sizeof(MatrixBufferType);
  matrixBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
  matrixBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
  matrixBufferDesc.MiscFlags = 0;
  matrixBufferDesc.StructureByteStride = 0;
  
  //Create the costant buffer pointer
  result = device->CreateBuffer(&matrixBufferDesc, NULL, &m_matrixBuffer);
  if(FAILED(result)) {
    return false;
  }
  
  return true;
}

void Shader::OutputShaderErrorMessage(ID3D10Blob * errorMessage, HWND hwnd, LPCSTR shaderFileName)
{
  char* compileErrors = (char*)errorMessage->GetBufferPointer();
  unsigned int bufferSize = errorMessage->GetBufferSize();
  
  ofstream fout;
  //Open file to write
  fout.open("shader-error.txt");
  
  for(int i = 0; i < bufferSize; i++) {
    fout << compileErrors[i];
  }

  //close file
  fout.close();
  
  //release
  errorMessage->Release();
  errorMessage = 0;

  MessageBox(hwnd, "Error compiling shader. Check shader-error.txt for message", shaderFileName, MB_OK);
 
}
