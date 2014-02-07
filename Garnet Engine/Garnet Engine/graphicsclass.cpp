#include "graphicsclass.h"

GraphicsClass::GraphicsClass()
{
	m_D3D = 0;
	m_Camera = 0;
	m_Model = 0;
	m_LightShader = 0;
	m_Light = 0;
}

GraphicsClass::GraphicsClass(const GraphicsClass& other)
{
}

GraphicsClass::~GraphicsClass()
{
}

bool GraphicsClass::Initialize(int screenWidth, int screenHeight, HWND hwnd)
{
	bool result;

	m_D3D = new D3DClass;
	if (!m_D3D)
	{
		return false;
	}

	result = m_D3D->Initialize(screenWidth, screenHeight, VSYNC_ENABLED, hwnd, FULL_SCREEN, SCREEN_DEPTH, SCREEN_NEAR);
	if (!result)
	{
		MessageBox(hwnd, L"Could not initialize Direct3D", L"Error", MB_OK);
		return false;
	}

	m_Camera = new CameraClass;
	if (!m_Camera)
	{
		return false;
	}

	m_Camera->SetPosition(0.0F, 0.0F, -10.0F);

	m_Model = new ModelClass;
	if (!m_Model)
	{
		return false;
	}

	result = m_Model->Initialize(m_D3D->GetDevice(), "../Garnet Engine/data/cube.txt", L"../Garnet Engine/data/rock.dds");
	if (!result)
	{
		MessageBox(hwnd, L"Could not initialize the model object", L"Error", MB_OK);
		return false;
	}

	m_LightShader = new LightShaderClass;
	if (!m_LightShader)
	{
		return false;
	}

	result = m_LightShader->Initialize(m_D3D->GetDevice(), hwnd);
	if (!result)
	{
		MessageBox(hwnd, L"Could not initialize the light shader object.", L"Error", MB_OK);
		return false;
	}

	m_Light = new LightClass;
	if(!m_Light)
	{
		return false;
	}

	m_Light->SetAmbientColor(0.15F, 0.15F, 0.15F, 1.0F);
	m_Light->SetDiffuseColor(1.0F, 1.0F, 1.0F, 1.0F);
	m_Light->SetDirection(1.0F, 0.0F, 0.0F);

	return true;
}

void GraphicsClass::Shutdown()
{

	if(m_Light)
	{
		delete m_Light;
		m_Light = 0;
	}

	if (m_LightShader)
	{
		m_LightShader->Shutdown();
		delete m_LightShader;
		m_LightShader = 0;
	}

	if (m_Model)
	{
		m_Model->Shutdown();
		delete m_Model;
		m_Model = 0;
	}

	if (m_Camera)
	{
		delete m_Camera;
		m_Camera = 0;
	}

	if (m_D3D)
	{
		m_D3D->Shutdown();
		delete m_D3D;
		m_D3D = 0;
	}

	return;
}

bool GraphicsClass::Frame()
{
	bool result;
	static float rotation = 0.0F;

	rotation += (float)D3DX_PI * 0.005F;
	if(rotation > 360.0F)
	{
		rotation -= 360.0F;
	}

	result = Render(rotation);
	if (!result)
	{
		return false;
	}

	return true;
}

bool GraphicsClass::Render(float rotation)
{
	D3DXMATRIX viewMatrix, projectionMatrix, worldMatrix;

	m_D3D->BeginScene(0.0F, 0.0F, 0.0F, 1.0F);

	m_Camera->Render();

	m_Camera->GetViewMatrix(viewMatrix);
	m_D3D->GetWorldMatrix(worldMatrix);
	m_D3D->GetProjectionMatrix(projectionMatrix);

	D3DXMatrixRotationY(&worldMatrix, rotation);

	m_Model->Render(m_D3D->GetDevice());

	m_LightShader->Render(m_D3D->GetDevice(), m_Model->GetIndexCount(), worldMatrix, viewMatrix, projectionMatrix, m_Model->GetTexture(), m_Light->GetDirection(), m_Light->GetAmbientColor(), m_Light->GetDiffuseColor());

	m_D3D->EndScene();

	return true;
}