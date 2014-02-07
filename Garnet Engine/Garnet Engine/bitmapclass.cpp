#include "bitmapclass.h"

BitmapClass::BitmapClass()
{
	m_vertexBuffer = 0;
	m_indexBuffer = 0;
	m_Texture = 0;
}

BitmapClass::BitmapClass(const BitmapClass& other)
{
}

BitmapClass::~BitmapClass()
{
}

bool BitmapClass::Initialize(ID3D10Device *device, int screenWidth, int screenHeight, WCHAR *textureFilename, int bitmapWidth, int bitmapHeight)
{
	bool result;

	m_screenWidth = screenWidth;
	m_screenHeight = screenHeight;

	m_bitmapWidth = bitmapWidth;
	m_bitmapHeight = bitmapHeight;

	m_prevPosX = -1;
	m_prevPosY = -1;

	result = InitializeBuffers(device);
	if(!result)
	{
		return false;
	}

	result = LoadTexture(device, textureFilename);
	if(!result)
	{
		return false;
	}

	return true;
}

void BitmapClass::Shutdown()
{
	ReleaseTexture();

	ShutdownBuffers();

	return;
}

bool BitmapClass::Render(ID3D10Device *device, int posX, int posY)
{
	bool result;

	result = UpdateBuffers(posX, posY);
	if(!result)
	{
		return false;
	}

	RenderBuffers(device);

	return true;
}

int BitmapClass::GetIndexCount()
{
	return m_indexCount;
}

ID3D10ShaderResourceView *BitmapClass::GetTexture()
{
	return m_Texture->GetTexture();
}

bool BitmapClass::InitializeBuffers(ID3D10Device *device)
{
	VertexType *vertices;
	unsigned long *indices;
	D3D10_BUFFER_DESC vertexBufferDesc, indexBufferDesc;
	D3D10_SUBRESOURCE_DATA vertexData, indexData;
	HRESULT result;
	int i;

	m_vertexCount = 6;

	m_indexCount = m_vertexCount;

	vertices = new VertexType[m_vertexCount];
	if(!vertices)
	{
		return false;
	}

	indices = new unsigned long[m_indexCount];
	if(!indices)
	{
		return false;
	}

	memset(vertices, 0, (sizeof(VertexType) *m_vertexCount));

	for(i = 0; i < m_indexCount; i++)
	{
		indices[i] = i;
	}

	vertexBufferDesc.Usage = D3D10_USAGE_DYNAMIC;
	vertexBufferDesc.ByteWidth = sizeof(VertexType) * m_vertexCount;
	vertexBufferDesc.BindFlags = D3D10_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags = D3D10_CPU_ACCESS_WRITE;
	vertexBufferDesc.MiscFlags = 0;

	vertexData.pSysMem = vertices;

	result = device->CreateBuffer(&vertexBufferDesc, &vertexData, &m_vertexBuffer);
	if(FAILED(result))
	{
		return false;
	}

	indexBufferDesc.Usage = D3D10_USAGE_DEFAULT;
	indexBufferDesc.ByteWidth = sizeof(unsigned long) * m_indexCount;
	indexBufferDesc.BindFlags = D3D10_BIND_INDEX_BUFFER;
	indexBufferDesc.CPUAccessFlags = 0;
	indexBufferDesc.MiscFlags = 0;

	indexData.pSysMem = indices;

	result = device->CreateBuffer(&indexBufferDesc, &indexData, &m_indexBuffer);
	if(FAILED(result))
	{
		return false;
	}

	delete [] vertices;
	vertices = 0;

	delete [] indices;
	indices = 0;

	return true;
}

void BitmapClass::ShutdownBuffers()
{
	if(m_indexBuffer)
	{
		m_indexBuffer->Release();
		m_indexBuffer = 0;
	}

	if(m_vertexBuffer)
	{
		m_vertexBuffer->Release();
		m_vertexBuffer = 0;
	}

	return;
}

bool BitmapClass::UpdateBuffers(int posX, int posY)
{
	float left, right, top, bottom;
	VertexType *vertices;
	void *verticesPtr;
	HRESULT result;

	if((posX == m_prevPosX) && (posY = m_prevPosY))
	{
		return true;
	}

	m_prevPosX = posX;
	m_prevPosY = posY;

	left = (float)((m_screenWidth / 2) * -1) + (float)posX;

	right = left + (float)m_bitmapWidth;

	top = (float)(m_screenHeight / 2) - (float)posY;

	bottom = top - (float)m_bitmapHeight;

	vertices = new VertexType[m_vertexCount];
	if(!vertices)
	{
		return false;
	}

	vertices[0].position = D3DXVECTOR3(left, top, 0.0F);
	vertices[0].texture = D3DXVECTOR2(0.0F, 0.0F);

	vertices[1].position = D3DXVECTOR3(right, bottom, 0.0F);
	vertices[1].texture = D3DXVECTOR2(1.0F, 1.0F);

	vertices[2].position = D3DXVECTOR3(left, bottom, 0.0F);
	vertices[2].texture = D3DXVECTOR2(0.0F, 1.0F);


	vertices[3].position = D3DXVECTOR3(left, top, 0.0F);
	vertices[3].texture = D3DXVECTOR2(0.0F, 0.0F);

	vertices[4].position = D3DXVECTOR3(right, top, 0.0F);
	vertices[4].texture = D3DXVECTOR2(1.0F, 0.0F);
	
	vertices[5].position = D3DXVECTOR3(right, bottom, 0.0F);
	vertices[5].texture = D3DXVECTOR2(1.0F, 1.0F);

	verticesPtr = 0;

	result = m_vertexBuffer->Map(D3D10_MAP_WRITE_DISCARD, 0, (void**)&verticesPtr);
	if(FAILED(result))
	{
		return false;
	}

	memcpy(verticesPtr, (void*)vertices, (sizeof(VertexType) * m_vertexCount));

	m_vertexBuffer->Unmap();

	delete [] vertices;
	vertices = 0;

	return true;
}

void BitmapClass::RenderBuffers(ID3D10Device *device)
{
	unsigned int stride;
	unsigned int offset;

	stride = sizeof(VertexType);
	offset = 0;

	device->IASetVertexBuffers(0, 1, &m_vertexBuffer, &stride, &offset);

	device->IASetIndexBuffer(m_indexBuffer, DXGI_FORMAT_R32_UINT, 0);

	device->IASetPrimitiveTopology(D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	return;
}

bool BitmapClass::LoadTexture(ID3D10Device *device, WCHAR *filename)
{
	bool result;

	m_Texture = new TextureClass;
	if(!m_Texture)
	{
		return false;
	}

	result = m_Texture->Initialize(device, filename);
	if(!result)
	{
		return false;
	}

	return true;
}

void BitmapClass::ReleaseTexture()
{
	if(m_Texture)
	{
		m_Texture->Shutdown();
		delete m_Texture;
		m_Texture = 0;
	}

	return;
}