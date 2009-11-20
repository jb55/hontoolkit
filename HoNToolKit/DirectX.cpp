#include <windows.h>
#include "DirectX.hpp"
#include "HoNClasses.hpp"
#include "HoNImports.hpp"

DirectX::DirectX(IDirect3DDevice9* pIDirect3DDevice) : m_pIDirect3DDevice(pIDirect3DDevice)
{
	
}

DirectX::~DirectX()
{

}

bool
DirectX::Initialize(void)
{
	if(SUCCEEDED(D3DXCreateFont(this->m_pIDirect3DDevice, 
							 40,			// Height
							 0,				// Width (0 default)
							 FW_NORMAL,		// Weight
							 1,				// MipLevels
							 FALSE,			// Italic
							 DEFAULT_CHARSET,
							 OUT_DEFAULT_PRECIS,
							 DEFAULT_QUALITY,
							 DEFAULT_PITCH,
							 "Arial",
							 &this->m_pDXFont)))
	{
		GetClientRect(((HoN::CSystem*)HoN::pK2System)->m_hWnd, &this->m_rWindow);
		return true;
	}
	
	return false;
}

void
DirectX::DrawText(LPSTR lpszText, int x, int y, DWORD dwColor)
{
	RECT Rect;
	SetRect(&Rect, x, y, this->m_rWindow.right, this->m_rWindow.bottom);

	this->m_pDXFont->DrawTextA(NULL, lpszText, strlen(lpszText), &Rect, 0, dwColor);
}

IDirect3DDevice9*
DirectX::GetDirect3DDevice(void)
{
	return this->m_pIDirect3DDevice;
}