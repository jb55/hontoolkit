#pragma once

#include <D3D9.h>
#include <D3dx9core.h>

class DirectX
{
	private:
		IDirect3DDevice9*	m_pIDirect3DDevice;
		ID3DXFont*			m_pDXFont;
		RECT				m_rWindow;

	public:
		DirectX(IDirect3DDevice9* pIDirect3DDevice);
		~DirectX();

		bool				Initialize(void);
		void				DrawText(LPSTR lpszText, int x, int y, DWORD dwColor);
		IDirect3DDevice9*	GetDirect3DDevice(void);
};