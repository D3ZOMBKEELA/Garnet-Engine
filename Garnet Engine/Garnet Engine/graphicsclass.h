#ifndef _GRAPHICSCLASS_H_
#define _GRAPHICSCLASS_H_

#include "d3dclass.h"
#include "cameraclass.h"
#include "modelclass.h"
#include "textureshaderclass.h"

const bool FULL_SCREEN = false;
const bool VSYNC_ENABLED = true;
const float SCREEN_DEPTH = 1000.0F;
const float SCREEN_NEAR = 0.1F;

class GraphicsClass
{
public:
	GraphicsClass();
	GraphicsClass(const GraphicsClass&);
	~GraphicsClass();

	bool Initialize(int, int, HWND);
	void Shutdown();
	bool Frame();

private:
	bool Render();

private:
	D3DClass *m_D3D;
	CameraClass *m_Camera;
	ModelClass *m_Model;
	TextureShaderClass *m_TextureShader;
};

#endif