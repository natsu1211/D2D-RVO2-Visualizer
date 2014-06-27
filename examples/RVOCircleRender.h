//Created by long
//2014.05

#ifndef _RVO_CIRCLE_RENDER_H_
#define _RVO_CIRCLE_RENDER_H_

// Windows Header Files:
#include <windows.h>

// C RunTime Header Files:
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <wchar.h>
#include <math.h>

#include <d2d1.h>
#include <d2d1helper.h>
#include <dwrite.h>
#include <wincodec.h>

const float wndWidth = 640.f;
const float wndHeight = 480.f;

template<class Interface>
inline void SafeRelease(
	Interface **ppInterfaceToRelease
	)
{
	if (*ppInterfaceToRelease != NULL)
	{
		(*ppInterfaceToRelease)->Release();

		(*ppInterfaceToRelease) = NULL;
	}
}


#ifndef Assert
#if defined( DEBUG ) || defined( _DEBUG )
#define Assert(b) do {if (!(b)) {OutputDebugStringA("Assert: " #b "\n");}} while(0)
#else
#define Assert(b)
#endif //DEBUG || _DEBUG
#endif



#ifndef HINST_THISCOMPONENT
EXTERN_C IMAGE_DOS_HEADER __ImageBase;
#define HINST_THISCOMPONENT ((HINSTANCE)&__ImageBase)
#endif


//
// DemoApp class declaration
//
class RVORender
{
public:
	RVORender();
	~RVORender();

	// Register the window class and call methods for instantiating drawing resources
	HRESULT Initialize();


public:
	// Initialize device-independent resources.
	HRESULT CreateDeviceIndependentResources();

	// Initialize device-dependent resources.
	HRESULT CreateDeviceResources();

	// Release device-dependent resource.
	void DiscardDeviceResources();

	// Draw content.
	HRESULT OnRender();

	// Resize the render target.
	void OnResize(
		UINT width,
		UINT height
		);

	// The windows procedure.
	static LRESULT CALLBACK WndProc(
		HWND hWnd,
		UINT message,
		WPARAM wParam,
		LPARAM lParam
		);
public:
	HWND m_hwnd;
	ID2D1Factory *m_pDirect2dFactory;
	ID2D1HwndRenderTarget *m_pRenderTarget;
	ID2D1BitmapRenderTarget *m_pCompatibleRenderTarget;// back buffer
	ID2D1Bitmap *m_pBitmap;
	ID2D1SolidColorBrush *m_pLightSlateGrayBrush;
	ID2D1SolidColorBrush *m_pCornflowerBlueBrush;
	ID2D1SolidColorBrush *m_pBrush;
	IDWriteFactory *m_pDWriteFactory;
	IDWriteTextFormat *m_pTextFormat;
};

#endif // _RVO_CIRCLE_RENDER_H_