#include "lightshaderclass.h"

LightShaderClass::LightShaderClass()
{
	m_effect = 0;
	m_technique = 0;
	m_layout = 0;

	m_worldMatrixPtr = 0;
	m_viewMatrixPtr = 0;
	m_projectionMatrixPtr = 0;
	m_texturePtr = 0;

	m_ambientColorPtr = 0;
	m_lightDirectionPtr = 0;
	m_diffuseColorPtr = 0;

	m_cameraPositionPtr = 0;
	m_specularColorPtr = 0;
	m_specularPowerPtr = 0;
}

LightShaderClass::LightShaderClass(const LightShaderClass& other)
{
}

LightShaderClass::~LightShaderClass()
{
}

bool LightShaderClass::Initialize(ID3D10Device *device, HWND hwnd)
{
	bool result;

	result = InitializeShader(device, hwnd, L"../Garnet Engine/light.fx");
	if(!result)
	{
		return false;
	}

	return true;
}

void LightShaderClass::Shutdown()
{
	ShutdownShader();

	return;
}

void LightShaderClass::Render(ID3D10Device *device, int indexCount, D3DXMATRIX worldMatrix, D3DXMATRIX viewMatrix, D3DXMATRIX projectionMatrix, ID3D10ShaderResourceView *texture, D3DXVECTOR3 lightDirection, D3DXVECTOR4 ambientColor, D3DXVECTOR4 diffuseColor, D3DXVECTOR3 cameraPosition, D3DXVECTOR4 specularColor, float specularPower)
{
	SetShaderParameters(worldMatrix, viewMatrix, projectionMatrix, texture, lightDirection, ambientColor, diffuseColor, cameraPosition, specularColor, specularPower);

	RenderShader(device, indexCount);

	return;
}

bool LightShaderClass::InitializeShader(ID3D10Device *device, HWND hwnd, WCHAR* filename)
{
	HRESULT result;
	ID3D10Blob *errorMessage;
	D3D10_INPUT_ELEMENT_DESC polygonLayout[3];
	unsigned int numElements;
	D3D10_PASS_DESC passDesc;

	errorMessage = 0;

	result = D3DX10CreateEffectFromFile(filename, NULL, NULL, "fx_4_0", D3D10_SHADER_ENABLE_STRICTNESS, 0, device, NULL, NULL, &m_effect, &errorMessage, NULL);
	if(FAILED(result))
	{
		if(errorMessage)
		{
			OutputShaderErrorMessage(errorMessage, hwnd, filename);
		}
		else
		{
			MessageBox(hwnd, filename, L"Missing shader file", MB_OK);
		}

		return false;
	}

	m_technique = m_effect->GetTechniqueByName("LightTechnique");
	if(!m_technique)
	{
		return false;
	}

	polygonLayout[0].SemanticName = "POSITION";
	polygonLayout[0].SemanticIndex = 0;
	polygonLayout[0].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	polygonLayout[0].InputSlot = 0;
	polygonLayout[0].AlignedByteOffset = 0;
	polygonLayout[0].InputSlotClass = D3D10_INPUT_PER_VERTEX_DATA;
	polygonLayout[0].InstanceDataStepRate = 0;

	polygonLayout[1].SemanticName = "TEXCOORD";
	polygonLayout[1].SemanticIndex = 0;
	polygonLayout[1].Format = DXGI_FORMAT_R32G32_FLOAT;
	polygonLayout[1].InputSlot = 0;
	polygonLayout[1].AlignedByteOffset = D3D10_APPEND_ALIGNED_ELEMENT;
	polygonLayout[1].InputSlotClass = D3D10_INPUT_PER_VERTEX_DATA;
	polygonLayout[1].InstanceDataStepRate = 0;

	polygonLayout[2].SemanticName = "NORMAL";
	polygonLayout[2].SemanticIndex = 0;
	polygonLayout[2].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	polygonLayout[2].InputSlot = 0;
	polygonLayout[2].AlignedByteOffset = D3D10_APPEND_ALIGNED_ELEMENT;
	polygonLayout[2].InputSlotClass = D3D10_INPUT_PER_VERTEX_DATA;
	polygonLayout[2].InstanceDataStepRate = 0;

	numElements = sizeof(polygonLayout) / sizeof(polygonLayout[0]);

	m_technique->GetPassByIndex(0)->GetDesc(&passDesc);

	result = device->CreateInputLayout(polygonLayout, numElements, passDesc.pIAInputSignature, passDesc.IAInputSignatureSize, &m_layout);
	if(FAILED(result))
	{
		return false;
	}

	m_worldMatrixPtr = m_effect->GetVariableByName("worldMatrix")->AsMatrix();
	m_viewMatrixPtr = m_effect->GetVariableByName("viewMatrix")->AsMatrix();
	m_projectionMatrixPtr = m_effect->GetVariableByName("projectionMatrix")->AsMatrix();

	m_texturePtr = m_effect->GetVariableByName("shaderTexture")->AsShaderResource();

	m_ambientColorPtr = m_effect->GetVariableByName("ambientColor")->AsVector();
	m_lightDirectionPtr = m_effect->GetVariableByName("lightDirection")->AsVector();
	m_diffuseColorPtr = m_effect->GetVariableByName("diffuseColor")->AsVector();

	m_cameraPositionPtr = m_effect->GetVariableByName("cameraPosition")->AsVector();
	m_specularColorPtr = m_effect->GetVariableByName("specularColor")->AsVector();
	m_specularPowerPtr = m_effect->GetVariableByName("specularPower")->AsScalar();

	return true;
}

void LightShaderClass::ShutdownShader()
{
	m_cameraPositionPtr = 0;
	m_specularColorPtr = 0;
	m_specularPowerPtr = 0;

	m_ambientColorPtr = 0;
	m_lightDirectionPtr = 0;
	m_diffuseColorPtr = 0;

	m_texturePtr = 0;

	m_worldMatrixPtr = 0;
	m_viewMatrixPtr = 0;
	m_projectionMatrixPtr = 0;

	if(m_layout)
	{
		m_layout->Release();
		m_layout = 0;
	}

	m_technique = 0;

	if(m_effect)
	{
		m_effect->Release();
	    m_effect = 0;
	}

	return;
}

void LightShaderClass::OutputShaderErrorMessage(ID3D10Blob *errorMessage, HWND hwnd, WCHAR *shaderFilename)
{
	char *compileErrors;
	unsigned long bufferSize, i;
	ofstream fout;

	compileErrors = (char*)(errorMessage->GetBufferPointer());

	bufferSize = errorMessage->GetBufferSize();

	fout.open("shader-error.txt");

	for(i = 0; i < bufferSize; i++)
	{
		fout << compileErrors[i];
	}

	fout.close();

	errorMessage->Release();
	errorMessage = 0;

	MessageBox(hwnd, L"Error compiling shader. Check shader-error.txt for message.", shaderFilename, MB_OK);

	return;
}

void LightShaderClass::SetShaderParameters(D3DXMATRIX worldMatrix, D3DXMATRIX viewMatrix, D3DXMATRIX projectionMatrix, ID3D10ShaderResourceView *texture, D3DXVECTOR3 lightDirection, D3DXVECTOR4 ambientColor, D3DXVECTOR4 diffuseColor, D3DXVECTOR3 cameraPosition, D3DXVECTOR4 specularColor, float specularPower)
{
	m_worldMatrixPtr->SetMatrix((float*)&worldMatrix);

	m_viewMatrixPtr->SetMatrix((float*)&viewMatrix);

	m_projectionMatrixPtr->SetMatrix((float*)&projectionMatrix);

	m_texturePtr->SetResource(texture);

	m_ambientColorPtr->SetFloatVector((float*)&ambientColor);

	m_lightDirectionPtr->SetFloatVector((float*)&lightDirection);

	m_diffuseColorPtr->SetFloatVector((float*)&diffuseColor);

	m_cameraPositionPtr->SetFloatVector((float*)&cameraPosition);

	m_specularColorPtr->SetFloatVector((float*)&specularColor);

	m_specularPowerPtr->SetFloat(specularPower);

	return;
}

void LightShaderClass::RenderShader(ID3D10Device *device, int indexCount)
{
	D3D10_TECHNIQUE_DESC techniqueDesc;
	unsigned int i;

	device->IASetInputLayout(m_layout);

	m_technique->GetDesc(&techniqueDesc);

	for(i = 0; i < techniqueDesc.Passes; ++i)
	{
		m_technique->GetPassByIndex(i)->Apply(0);
		device->DrawIndexed(indexCount, 0, 0);
	}

	return;
}