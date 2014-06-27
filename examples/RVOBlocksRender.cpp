//Created by long
//2014.05

#include "RVOBlocksRender.h"

// RVORender constructor
RVORender::RVORender() :
m_hwnd(NULL),
m_pDirect2dFactory(NULL),
m_pRenderTarget(NULL),
m_pBitmap(NULL),
m_pCompatibleRenderTarget(NULL),
m_pLightSlateGrayBrush(NULL),
m_pCornflowerBlueBrush(NULL),
m_pDWriteFactory(NULL),
m_pTextFormat(NULL)
{
}

// RVORender destructor
// Releases the application's resources.
RVORender::~RVORender()
{
	SafeRelease(&m_pDirect2dFactory);
	SafeRelease(&m_pRenderTarget);
	SafeRelease(&m_pCompatibleRenderTarget);
	SafeRelease(&m_pBitmap);
	SafeRelease(&m_pLightSlateGrayBrush);
	SafeRelease(&m_pCornflowerBlueBrush);
	SafeRelease(&m_pDWriteFactory);
	SafeRelease(&m_pTextFormat);
}

// Creates the application window and device-independent
// resources.
HRESULT RVORender::Initialize()
{
	HRESULT hr;

	// Initialize device-indpendent resources, such
	// as the Direct2D factory.
	hr = CreateDeviceIndependentResources();

	if (SUCCEEDED(hr))
	{
		// Register the window class.
		WNDCLASSEX wcex = { sizeof(WNDCLASSEX) };
		wcex.style = CS_HREDRAW | CS_VREDRAW;
		wcex.lpfnWndProc = RVORender::WndProc;
		wcex.cbClsExtra = 0;
		wcex.cbWndExtra = sizeof(LONG_PTR);
		wcex.hInstance = HINST_THISCOMPONENT;
		wcex.hbrBackground = NULL;
		wcex.lpszMenuName = NULL;
		wcex.hCursor = LoadCursor(NULL, IDI_APPLICATION);
		wcex.lpszClassName = L"D2DRVOBlocksRender";

		RegisterClassEx(&wcex);


		// Because the CreateWindow function takes its size in pixels,
		// obtain the system DPI and use it to scale the window size.
		FLOAT dpiX, dpiY;

		// The factory returns the current system DPI. This is also the value it will use
		// to create its own windows.
		m_pDirect2dFactory->GetDesktopDpi(&dpiX, &dpiY);


		// Create the window.
		m_hwnd = CreateWindow(
			L"D2DRVOBlocksRender",
			L"D2DRVOBlocksRender",
			WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU,
			CW_USEDEFAULT,
			CW_USEDEFAULT,
			static_cast<UINT>(ceil(wndWidth * dpiX / 96.f)),
			static_cast<UINT>(ceil(wndHeight * dpiY / 96.f)),
			NULL,
			NULL,
			HINST_THISCOMPONENT,
			this
			);
		hr = m_hwnd ? S_OK : E_FAIL;
		if (SUCCEEDED(hr))
		{
			ShowWindow(m_hwnd, SW_SHOWNORMAL);
			UpdateWindow(m_hwnd);
		}
	}

	return hr;
}

// Creates resources that are not bound to a particular device.
// Their lifetime effectively extends for the duration of the
// application.
HRESULT RVORender::CreateDeviceIndependentResources()
{
	static const WCHAR msc_fontName[] = L"Calibri";
	static const FLOAT msc_fontSize = 14;
	ID2D1GeometrySink *pSink = NULL;
	HRESULT hr = S_OK;

	// Create a Direct2D factory.
	hr = D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, &m_pDirect2dFactory);
	if (SUCCEEDED(hr))
	{
		// Create a DirectWrite factory.
		hr = DWriteCreateFactory(
			DWRITE_FACTORY_TYPE_SHARED,
			__uuidof(m_pDWriteFactory),
			reinterpret_cast<IUnknown **>(&m_pDWriteFactory)
			);
	}
	if (SUCCEEDED(hr))
	{
		// Create a DirectWrite text format object.
		hr = m_pDWriteFactory->CreateTextFormat(
			msc_fontName,
			NULL,
			DWRITE_FONT_WEIGHT_NORMAL,
			DWRITE_FONT_STYLE_NORMAL,
			DWRITE_FONT_STRETCH_NORMAL,
			msc_fontSize,
			L"", //locale
			&m_pTextFormat
			);
	}
	if (SUCCEEDED(hr))
	{
		// left-upper the text horizontally and vertically.
		m_pTextFormat->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_LEADING);
		m_pTextFormat->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_NEAR);
	}

	SafeRelease(&pSink);

	return hr;
}


// Creates resources that are bound to a particular
// Direct3D device. These resources need to be recreated
// if the Direct3D device dissapears, such as when the display
// changes, the window is remoted, etc.
HRESULT RVORender::CreateDeviceResources()
{
	HRESULT hr = S_OK;

	if (!m_pRenderTarget)
	{
		RECT rc;
		GetClientRect(m_hwnd, &rc);

		D2D1_SIZE_U size = D2D1::SizeU(
			rc.right - rc.left,
			rc.bottom - rc.top
			);

		// Create a Direct2D render target.
		hr = m_pDirect2dFactory->CreateHwndRenderTarget(
			D2D1::RenderTargetProperties(),
			D2D1::HwndRenderTargetProperties(m_hwnd, size),
			&m_pRenderTarget
			);


		if (SUCCEEDED(hr))
		{
			// Create a gray brush.
			hr = m_pRenderTarget->CreateSolidColorBrush(
				D2D1::ColorF(D2D1::ColorF::LightSlateGray),
				&m_pLightSlateGrayBrush
				);
		}
		if (SUCCEEDED(hr))
		{
			// Create a blue brush.
			hr = m_pRenderTarget->CreateSolidColorBrush(
				D2D1::ColorF(D2D1::ColorF::CornflowerBlue),
				&m_pCornflowerBlueBrush
				);
		}

		//CompatibleRenderTarget for hwndRenderTarget
		if (SUCCEEDED(hr))
			hr = m_pRenderTarget->CreateCompatibleRenderTarget(&m_pCompatibleRenderTarget);

		if (SUCCEEDED(hr))
		{
			// Create a blue brush.
			hr = m_pCompatibleRenderTarget->CreateSolidColorBrush(
				D2D1::ColorF(D2D1::ColorF::CornflowerBlue),
				&m_pCornflowerBlueBrush
				);
		}

		if (SUCCEEDED(hr))
			hr = m_pRenderTarget->CreateBitmap(size, nullptr, 0, D2D1::BitmapProperties(
			D2D1::PixelFormat(
			DXGI_FORMAT_B8G8R8A8_UNORM,
			D2D1_ALPHA_MODE_PREMULTIPLIED)), &m_pBitmap);//todo


	}

	return hr;
}

// Discards device-dependent resources. These resources must be
// recreated when the Direct3D device is lost.
void RVORender::DiscardDeviceResources()
{
	SafeRelease(&m_pDirect2dFactory);
	SafeRelease(&m_pRenderTarget);
	SafeRelease(&m_pCompatibleRenderTarget);
	SafeRelease(&m_pBitmap);
	SafeRelease(&m_pLightSlateGrayBrush);
	SafeRelease(&m_pCornflowerBlueBrush);
	SafeRelease(&m_pDWriteFactory);
	SafeRelease(&m_pTextFormat);
}


// This method discards device-specific
// resources if the Direct3D device dissapears during execution and
// recreates the resources the next time it's invoked.
HRESULT RVORender::OnRender()
{
	HRESULT hr = S_OK;

	hr = CreateDeviceResources();

	if (SUCCEEDED(hr))
	{
		m_pRenderTarget->BeginDraw();

		m_pRenderTarget->SetTransform(D2D1::Matrix3x2F::Identity());

		m_pRenderTarget->Clear(D2D1::ColorF(D2D1::ColorF::White));

		D2D1_SIZE_F rtSize = m_pRenderTarget->GetSize();

		/*
		// Draw a grid background.
		int width = static_cast<int>(rtSize.width);
		int height = static_cast<int>(rtSize.height);

		for (int x = 0; x < width; x += 30)
		{
			m_pRenderTarget->DrawLine(
				D2D1::Point2F(static_cast<FLOAT>(x), 0.0f),
				D2D1::Point2F(static_cast<FLOAT>(x), rtSize.height),
				m_pLightSlateGrayBrush,
				1.0f
				);
		}

		for (int y = 0; y < height; y += 30)
		{
			m_pRenderTarget->DrawLine(
				D2D1::Point2F(0.0f, static_cast<FLOAT>(y)),
				D2D1::Point2F(rtSize.width, static_cast<FLOAT>(y)),
				m_pLightSlateGrayBrush,
				1.0f
				);
		}
		*/
		hr = m_pRenderTarget->EndDraw();
	}

	if (hr == D2DERR_RECREATE_TARGET)
	{
		hr = S_OK;
		DiscardDeviceResources();
	}

	//InvalidateRect(m_hwnd, NULL, FALSE);
	return hr;
}



//  If the application receives a WM_SIZE message, this method
//  resizes the render target appropriately.
void RVORender::OnResize(UINT width, UINT height)
{
	if (m_pRenderTarget)
	{
		// Note: This method can fail, but it's okay to ignore the
		// error here, because the error will be returned again
		// the next time EndDraw is called.
		m_pRenderTarget->Resize(D2D1::SizeU(width, height));
	}
}

// Handles window messages.
LRESULT CALLBACK RVORender::WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	LRESULT result = 0;

	if (message == WM_CREATE)
	{
		LPCREATESTRUCT pcs = (LPCREATESTRUCT)lParam;
		RVORender *pRVORender = (RVORender *)pcs->lpCreateParams;

		::SetWindowLongPtrW(
			hwnd,
			GWLP_USERDATA,
			PtrToUlong(pRVORender)
			);

		result = 1;
	}
	else
	{
		RVORender *pRVORender = reinterpret_cast<RVORender *>(static_cast<LONG_PTR>(
			::GetWindowLongPtrW(
			hwnd,
			GWLP_USERDATA
			)));

		bool wasHandled = false;

		if (pRVORender)
		{
			switch (message)
			{
			case WM_SIZE:
			{
				UINT width = LOWORD(lParam);
				UINT height = HIWORD(lParam);
				pRVORender->OnResize(width, height);
			}
				result = 0;
				wasHandled = true;
				break;

			case WM_DISPLAYCHANGE:
			{
				InvalidateRect(hwnd, NULL, FALSE);
			}
				result = 0;
				wasHandled = true;
				break;

			case WM_PAINT:
			{
				pRVORender->OnRender();
				ValidateRect(hwnd, NULL);
				//PAINTSTRUCT ps;
				//BeginPaint(hwnd, &ps);

				//pRVORender->OnRender();
				//EndPaint(hwnd, &ps);
			}
				result = 0;
				wasHandled = true;
				break;

			case WM_DESTROY:
			{
				PostQuitMessage(0);
			}
				result = 1;
				wasHandled = true;
				break;
			}
		}

		if (!wasHandled)
		{
			result = DefWindowProc(hwnd, message, wParam, lParam);
		}
	}

	return result;
}

