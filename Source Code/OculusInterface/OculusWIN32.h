/************************************************************************************

OculusWIN32.h

*************************************************************************************/

// Based on Win32_GLAppUtil.h from Oculus.
// I have separated the Win32 OpenGL functionality here from the scene drawing elsewhere.

// The following includes comes from the Oculus OVR source files.
// The path is set via the user macro $(OVRSDKROOT) and via the property pages
//  in the VS2010 project files. I was able to modify $(OVRSDKROOT) by editing 
//  OVRRootPath.props. I could not figure out how to do it within VS2010.
#include "GL/CAPI_GLE.h"
#include "Extras/OVR_Math.h"
#include "OVR_CAPI_GL.h"
#include <assert.h>

#include "../Useful/fOutputDebugString.h"
#include "../Useful/fMessageBox.h"

// Oculus had this in the include file, but that is bad practice because it gets included everywhere.
// I comment it out here and qualify the names with OVR:: as needed.
//using namespace OVR;

// Here I define indices into the Button map of OculusDisplayOGL defined below.
// It is a little dangerous because the names are so generic. But I don't see any
// warnings about redefinitions as of yet.
#define MOUSE_LEFT	1
#define MOUSE_MIDDLE 2
#define MOUSE_RIGHT 3

#ifndef UNDEFINED
#define UNDEFINED -1
#endif  
#define BACKGROUND_MESSAGE_LINES 5

#ifndef VALIDATE
#define VALIDATE(x, msg) if (!(x)) { MessageBoxA(NULL, (msg), "OculusWIN32", MB_ICONERROR | MB_OK); exit(-1); }
#endif

//---------------------------------------------------------------------------------------
struct DepthBuffer
{
	GLuint        texId;

	DepthBuffer(OVR::Sizei size, int sampleCount)
	{
		assert( sampleCount <= 1 ); // The code doesn't currently handle MSAA textures.

		glGenTextures(1, &texId);
		glBindTexture(GL_TEXTURE_2D, texId);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

		GLenum internalFormat = GL_DEPTH_COMPONENT24;
		GLenum type = GL_UNSIGNED_INT;
		if (GLE_ARB_depth_buffer_float)
		{
			internalFormat = GL_DEPTH_COMPONENT32F;
			type = GL_FLOAT;
		}

		glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, size.w, size.h, 0, GL_DEPTH_COMPONENT, type, NULL);
	}
	~DepthBuffer()
	{
		if (texId)
		{
			glDeleteTextures(1, &texId);
			texId = 0;
		}
	}
};

//--------------------------------------------------------------------------
struct TextureBuffer
{
	ovrSession          Session;
	ovrTextureSwapChain  TextureChain;
	GLuint              texId;
	GLuint              fboId;
	OVR::Sizei               texSize;

	TextureBuffer(ovrSession session, bool rendertarget, bool displayableOnHmd, OVR::Sizei size, int mipLevels, unsigned char * data, int sampleCount) :
	Session(session),
		TextureChain(nullptr),
		texId(0),
		fboId(0),
		texSize(0, 0)
	{
		assert(sampleCount <= 1); // The code doesn't currently handle MSAA textures.

		texSize = size;

		if (displayableOnHmd)
		{
			// This texture isn't necessarily going to be a rendertarget, but it usually is.
			assert( session ); // No HMD? A little odd.
			assert( sampleCount <= 1 ); // ovr_CreateSwapTextureSetD3D11 doesn't support MSAA. JMc: Did they mean ovr_CreateSwapTextureSetGL()?

			ovrTextureSwapChainDesc desc = {};
			desc.Type = ovrTexture_2D;
			desc.ArraySize = 1;
			desc.Width = size.w;
			desc.Height = size.h;
			desc.MipLevels = 1;
			desc.Format = OVR_FORMAT_R8G8B8A8_UNORM_SRGB;
			desc.SampleCount = 1;
			desc.StaticImage = ovrFalse;

			ovrResult result = ovr_CreateTextureSwapChainGL(Session, &desc, &TextureChain);

			int length = 0;
			ovr_GetTextureSwapChainLength(session, TextureChain, &length);

			if(OVR_SUCCESS(result))
			{
				for (int i = 0; i < length; ++i)
				{
					GLuint chainTexId;
					ovr_GetTextureSwapChainBufferGL(Session, TextureChain, i, &chainTexId);
					glBindTexture(GL_TEXTURE_2D, chainTexId);

					if (rendertarget)
					{
						glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
						glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
						glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
						glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
					}
					else
					{
						glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
						glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
						glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
						glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
					}
				}
			}
		}
		else
		{
			glGenTextures(1, &texId);
			glBindTexture(GL_TEXTURE_2D, texId);

			if (rendertarget)
			{
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
			}
			else
			{
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
			}

			glTexImage2D(GL_TEXTURE_2D, 0, GL_SRGB8_ALPHA8, texSize.w, texSize.h, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
		}

		if (mipLevels > 1)
		{
			glGenerateMipmap(GL_TEXTURE_2D);
		}

		glGenFramebuffers(1, &fboId);
	}

	~TextureBuffer()
	{
		if (TextureChain)
		{
			ovr_DestroyTextureSwapChain(Session, TextureChain);
			TextureChain = nullptr;
		}
		if (texId)
		{
			glDeleteTextures(1, &texId);
			texId = 0;
		}
		if (fboId)
		{
			glDeleteFramebuffers(1, &fboId);
			fboId = 0;
		}
	}

	OVR::Sizei GetSize() const
	{
		return texSize;
	}
	void SetAndClearRenderSurface(DepthBuffer* dbuffer)
	{
		GLuint curTexId;
		if (TextureChain)
		{
			int curIndex;
			ovr_GetTextureSwapChainCurrentIndex(Session, TextureChain, &curIndex);
			ovr_GetTextureSwapChainBufferGL(Session, TextureChain, curIndex, &curTexId);
		}
		else
		{
			curTexId = texId;
		}

		glBindFramebuffer(GL_FRAMEBUFFER, fboId);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, curTexId, 0);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, dbuffer->texId, 0);

		glViewport(0, 0, texSize.w, texSize.h);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glEnable(GL_FRAMEBUFFER_SRGB);
	}

	void UnsetRenderSurface()
	{
		glBindFramebuffer(GL_FRAMEBUFFER, fboId);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, 0, 0);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, 0, 0);
	}

	void Commit()
	{
		if (TextureChain)
		{
			ovr_CommitTextureSwapChain(Session, TextureChain);
		}
	}
};

//-------------------------------------------------------------------------------------------
struct OculusDisplayOGL
{
	static const bool       UseDebugContext = false;

	HWND                    Window;
	UINT					WindowStyle;
	bool					fullscreen;
	HDC                     hDC;
	HGLRC                   WglContext;
	OVR::GLEContext         GLEContext;
	bool                    Running;
	bool                    Key[256];
	bool					Button[256];
	unsigned int			KeyDownEvents[256];
	unsigned int			ButtonDownEvents[256];
	int                     WinSizeW;
	int                     WinSizeH;
	GLuint                  fboId;
	HINSTANCE               hInstance;
	HWND					parentWindow;

	char *backgroundMessage[BACKGROUND_MESSAGE_LINES];

	static const int MARGIN = 100;
	int       pointerLastX;
	int       pointerLastY;
	int       mouseDeltaX;
	int       mouseDeltaY;
	int       mouseCumulativeX;
	int       mouseCumulativeY;


	static LRESULT CALLBACK WindowProc(_In_ HWND hWnd, _In_ UINT Msg, _In_ WPARAM wParam, _In_ LPARAM lParam) {
		
		// Point to the instance that invoked this callback.
		OculusDisplayOGL *p = reinterpret_cast<OculusDisplayOGL *>(GetWindowLongPtr(hWnd, 0));

		// Variables used to handle mouse movements.
		union {
			unsigned long   param;
			unsigned short  word[2];
		} unpack;
		int mouse_x, mouse_y;
			
		HDC hDC = GetWindowDC( hWnd );

		switch (Msg)
		{

		case WM_PAINT:

			// Place a text message on the desktop screen when Oculus is running but mirroring is off.
			// This should only get called once when the program starts up. It is called even when
			// mirroring is on, but it is quickly overwritten by the mirroring bitblt.

			// Since it should only be called once at the onset, I create and destroy the font and brush objects here.

			HFONT hFont, hOldFont; 
			HBRUSH brush;
			PAINTSTRUCT ps;
			RECT rect;
			int font_size;

			BeginPaint( hWnd, &ps );

			// Clear the screen.
			brush = CreateSolidBrush( RGB( 0, 0, 25 ) );
			GetClientRect( hWnd, &rect );
			// Hacks to fill the whole window.
			// I don't know why the client rectangle is not correct.
			rect.right += 10;
			rect.bottom += 40;
			FillRect( hDC, &rect, brush );
			DeleteObject( brush );

			// Set the text drawing color and mode.
			SetTextColor( hDC,RGB( 0, 255, 255 ) );
			SetBkMode( hDC,TRANSPARENT);

			// A large message saying what is going on just above the centerline.
			// Scale the text to the size of the window.
			GetClientRect( hWnd, &rect );
			font_size = ( rect.right - rect.left ) / 20;
			hFont = CreateFont( font_size, 0, 0, 0, FW_BOLD, FALSE,
				FALSE, FALSE, ANSI_CHARSET, OUT_DEFAULT_PRECIS,
				CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH,
				"Broadway"); 
			hOldFont = (HFONT)SelectObject( hDC, hFont);
			GetClientRect( hWnd, &rect );
			rect.bottom = ( rect.top + rect.bottom ) / 2;
			DrawText(hDC, "Application is running in VR Helmet", -1, &rect, DT_SINGLELINE | DT_NOCLIP | DT_CENTER | DT_BOTTOM ) ;
			SelectObject( hDC, hOldFont);
			DeleteObject( hFont );

			// A smaller message saying how to escape out of the program.
			hFont = CreateFont( font_size / 2, 0, 0, 0, FW_BOLD, FALSE,
				FALSE, FALSE, ANSI_CHARSET, OUT_DEFAULT_PRECIS,
				CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH,
				"Broadway"); 
			hOldFont = (HFONT)SelectObject( hDC, hFont);
			GetClientRect( hWnd, &rect );
			rect.top = ( rect.top + rect.bottom ) / 2 + 50;
			for (int i = 0; i < BACKGROUND_MESSAGE_LINES; i++) {
				DrawText(hDC, p->backgroundMessage[i], -1, &rect, DT_SINGLELINE | DT_NOCLIP | DT_CENTER | DT_TOP ) ;
				rect.top += (int)((double)font_size * 0.6);
			}
			SelectObject( hDC, hOldFont); 
			DeleteObject( hFont );

			EndPaint( hWnd, &ps );

			break;

		case WM_LBUTTONDOWN:
			p->Button[MOUSE_LEFT] = true;
			break;
		case WM_LBUTTONUP:
			p->Button[MOUSE_LEFT] = false;
			break;
		case WM_MBUTTONDOWN:
			p->Button[MOUSE_MIDDLE] = true;
			break;
		case WM_MBUTTONUP:
			p->Button[MOUSE_MIDDLE] = false;
			break;
		case WM_RBUTTONDOWN:
			p->Button[MOUSE_RIGHT] = true;
			break;
		case WM_RBUTTONUP:
			p->Button[MOUSE_RIGHT] = false;
			break;

		case WM_KEYDOWN:
			p->Key[wParam] = true;
			p->KeyDownEvents[wParam]++;
			break;

		case WM_KEYUP:
			p->Key[wParam] = false;
			break;

		case WM_MOUSEMOVE:

			// Here we handle movements of the mouse. We want to keep focus in the window so when the pointer gets close
			// to the edge of the window, we warp it to the other edge. And we keep track of how much the mouse has moved
			// in each direction. This can be used, for instance, to adjust values up and down. What I don't remember is 
			// how the delta movements are reset to 0, if ever.

			unpack.param = lParam;
			mouse_x = unpack.word[0];
			mouse_y = unpack.word[1];

			// In my previous version the user had to press the Left button to affect the tracked delta movement of the mouse.
			// Here I am experimenting between that and always tracking the movement in a way that depends if we requested
			// fullscreen mode or not.
			if ( wParam == MK_LBUTTON || p->fullscreen ) {

				// If this is the fist event we need to establish the reference point for mouseDeltaX and mouseDeltaY.
				if ( p->pointerLastX == UNDEFINED ) {
					p->pointerLastX = mouse_x;
					p->pointerLastY = mouse_y;
				}

				p->mouseDeltaX = mouse_x - p->pointerLastX;
				p->mouseDeltaY = mouse_y - p->pointerLastY;
				p->mouseCumulativeX += p->mouseDeltaX;
				p->mouseCumulativeY += p->mouseDeltaY;

				if ( mouse_x < MARGIN ) {
					POINT point;
					RECT rect;
					GetCursorPos( &point );
					GetWindowRect( hWnd, &rect );
					int delta = rect.right - 2 * MARGIN - point.x;
					p->pointerLastX += delta;
					SetCursorPos( rect.right - 2 * MARGIN, point.y );
					return 0;
				}
				if ( mouse_x > p->WinSizeW -  MARGIN ) {
					POINT point;
					RECT rect;
					GetCursorPos( &point );
					GetWindowRect( hWnd, &rect );
					int delta = rect.left + 2 * MARGIN - point.x;
					p->pointerLastX += delta;
					SetCursorPos( rect.left + 2 * MARGIN, point.y );
					return 0;
				}

				if ( mouse_y < MARGIN ) {
					POINT point;
					RECT rect;
					GetCursorPos( &point );
					GetWindowRect( hWnd, &rect );
					int delta = rect.bottom - 2 * MARGIN - point.y;
					p->pointerLastY += delta;
					SetCursorPos( point.x, rect.bottom - 2 * MARGIN );
					return 0;
				}
				if ( mouse_y > p->WinSizeH -  MARGIN ) {
					POINT point;
					RECT rect;
					GetCursorPos( &point );
					GetWindowRect( hWnd, &rect );
					int delta = rect.top + 2 * MARGIN - point.y;
					p->pointerLastY += delta;
					SetCursorPos( point.x, rect.top + 2 * MARGIN );
					return 0;
				}

				p->pointerLastX = mouse_x;
				p->pointerLastY = mouse_y;

			}
			else {
				p->pointerLastX = mouse_x;
				p->pointerLastY = mouse_y;
			}
			return 0;
			break;          

		case WM_DESTROY:
			p->Running = false;
			break;
		default:
			return DefWindowProcW(hWnd, Msg, wParam, lParam);
		}
		if ((p->Key['Q'] && p->Key[VK_CONTROL]) || p->Key[VK_ESCAPE])
		{
			p->Running = false;
		}
		return 0;
	}

	void ClearKeyDownEvents( int key = -1 ) {
		if ( key < 0 ) 	for (int i = 0; i < sizeof(KeyDownEvents) / sizeof(KeyDownEvents[0]); ++i) KeyDownEvents[i] = 0;
		else KeyDownEvents[key] = 0;
	}

	OculusDisplayOGL() :

	hInstance(nullptr),
		Window(nullptr),
		WindowStyle( WS_OVERLAPPEDWINDOW ),
		fullscreen(false),
		hDC(nullptr),
		WglContext(nullptr),
		GLEContext(),
		Running(false),
		WinSizeW(0),
		WinSizeH(0),
		fboId(0),
		mouseDeltaX(0),
		mouseDeltaY(0),
		mouseCumulativeX(0),
		mouseCumulativeY(0),
		pointerLastX( UNDEFINED ),
		pointerLastY( UNDEFINED )

	{
		// Clear input
		for (int i = 0; i < sizeof(Key) / sizeof(Key[0]); ++i) Key[i] = false;
		for (int i = 0; i < sizeof(Button) / sizeof(Button[0]); ++i) Button[i] = false;
		ClearKeyDownEvents();

		backgroundMessage[0] = "Press 'Escape' on keyboard to force exit.";
		for (int i = 1; i < BACKGROUND_MESSAGE_LINES; i++) backgroundMessage[i] = "";
	}

	~OculusDisplayOGL()
	{
		ReleaseDevice();
		CloseWindow();
	}

	void SetBackgroundMessage( int line, char *text ) {
		if ( line < BACKGROUND_MESSAGE_LINES ) backgroundMessage[line] = text;
	}

	bool InitWindow( HINSTANCE hInst, LPCWSTR title, bool fullscreen = true, HWND parent = 0 )
	{

		parentWindow = parent;
		hInstance = hInst;
		Running = true;

		this->fullscreen = fullscreen;
		if ( fullscreen ) WindowStyle = WS_POPUP;
		else if ( parent ) WindowStyle = WS_OVERLAPPED | WS_CHILD;
		else WindowStyle = WS_OVERLAPPEDWINDOW;

		WNDCLASSW wc;
		memset(&wc, 0, sizeof(wc));
		wc.style = CS_CLASSDC;
		wc.lpfnWndProc = WindowProc;
		wc.cbWndExtra = sizeof(struct OGL *);
		wc.hInstance = GetModuleHandleW(NULL);
		wc.lpszClassName = L"ORT";
		RegisterClassW(&wc);

		RECT win_rect;
		if ( parentWindow ) GetWindowRect( parent, &win_rect );
		else {
			// Get a handle to the desktop window
			HWND hDesktop = GetDesktopWindow();
			// Get the size of screen to the variable desktop
			GetWindowRect (hDesktop, &win_rect );
		}
		WinSizeW = win_rect.right - win_rect.left;
		WinSizeH = win_rect.bottom - win_rect.top;
		int x = 0;
		int y = 0;
		if ( !parentWindow && !fullscreen ) {
			WinSizeW /= 2;
			WinSizeH /= 2;
			x = WinSizeW / 2;
			y = WinSizeH / 2;
		}

		// Create a window.
		Window = CreateWindowW(wc.lpszClassName, title, WindowStyle, x, y, WinSizeW, WinSizeH, parentWindow, 0, hInstance, 0);
		if (!Window) return false;

		SetWindowLongPtr(Window, 0, LONG_PTR(this));

		hDC = GetDC(Window);

		return true;
	}

	void ResizeWindow( int width, int height ) {
		if ( fullscreen || parentWindow ) return;
		RECT win_rect;
		// Get a handle to the desktop window
		HWND hDesktop = GetDesktopWindow();
		// Get the size of screen to the variable desktop
		GetWindowRect (hDesktop, &win_rect );
		WinSizeW = width;
		WinSizeH = height;
		int x = ( win_rect.left + win_rect.right ) / 2 - width / 2;
		int y = ( win_rect.bottom + win_rect.top ) / 2 - height / 2 + 40;
		SetWindowPos( Window, NULL, x, y, width, height, SWP_SHOWWINDOW );
	}

	void CloseWindow()
	{
		if (Window)
		{
			if (hDC)
			{
				ReleaseDC(Window, hDC);
				hDC = nullptr;
			}
			DestroyWindow(Window);
			Window = nullptr;
			UnregisterClassW(L"OGL", hInstance);
		}
	}

	// Note: currently there is no way to get GL to use the passed pLuid
	bool InitDevice( const LUID* pLuid = nullptr )
	{

		const UINT flags = SWP_NOMOVE | SWP_NOZORDER | SWP_SHOWWINDOW;
		// if ( !SetWindowPos( Window, nullptr, 0, 0, size.right - size.left, size.bottom - size.top, flags)) return false;

		PFNWGLCHOOSEPIXELFORMATARBPROC wglChoosePixelFormatARBFunc = nullptr;
		PFNWGLCREATECONTEXTATTRIBSARBPROC wglCreateContextAttribsARBFunc = nullptr;
		{
			// First create a context for the purpose of getting access to wglChoosePixelFormatARB / wglCreateContextAttribsARB.
			PIXELFORMATDESCRIPTOR pfd;
			memset(&pfd, 0, sizeof(pfd));
			pfd.nSize = sizeof(pfd);
			pfd.nVersion = 1;
			pfd.iPixelType = PFD_TYPE_RGBA;
			pfd.dwFlags = PFD_SUPPORT_OPENGL | PFD_DRAW_TO_WINDOW | PFD_DOUBLEBUFFER;
			pfd.cColorBits = 32;
			pfd.cDepthBits = 16;
			int pf = ChoosePixelFormat(hDC, &pfd);
			VALIDATE(pf, "Failed to choose pixel format.");

			VALIDATE(SetPixelFormat(hDC, pf, &pfd), "Failed to set pixel format.");

			HGLRC context = wglCreateContext(hDC);
			VALIDATE(context, "wglCreateContextfailed.");
			VALIDATE(wglMakeCurrent(hDC, context), "wglMakeCurrent failed.");

			wglChoosePixelFormatARBFunc = (PFNWGLCHOOSEPIXELFORMATARBPROC)wglGetProcAddress("wglChoosePixelFormatARB");
			wglCreateContextAttribsARBFunc = (PFNWGLCREATECONTEXTATTRIBSARBPROC)wglGetProcAddress("wglCreateContextAttribsARB");
			assert( wglChoosePixelFormatARBFunc && wglCreateContextAttribsARBFunc );

			wglDeleteContext(context);
		}

		// Now create the real context that we will be using.
		int iAttributes[] =
		{
			// WGL_DRAW_TO_WINDOW_ARB, GL_TRUE,
			WGL_SUPPORT_OPENGL_ARB, GL_TRUE,
			WGL_COLOR_BITS_ARB, 32,
			WGL_DEPTH_BITS_ARB, 16,
			WGL_DOUBLE_BUFFER_ARB, GL_TRUE,
			WGL_FRAMEBUFFER_SRGB_CAPABLE_ARB, GL_TRUE,
			0, 0
		};

		float fAttributes[] = { 0, 0 };
		int   pf = 0;
		UINT  numFormats = 0;

		VALIDATE(wglChoosePixelFormatARBFunc(hDC, iAttributes, fAttributes, 1, &pf, &numFormats), "wglChoosePixelFormatARBFunc failed.");

		PIXELFORMATDESCRIPTOR pfd;
		memset(&pfd, 0, sizeof(pfd));
		VALIDATE(SetPixelFormat(hDC, pf, &pfd), "SetPixelFormat failed.");

		GLint attribs[16];
		int   attribCount = 0;
		if (UseDebugContext)
		{
			attribs[attribCount++] = WGL_CONTEXT_FLAGS_ARB;
			attribs[attribCount++] = WGL_CONTEXT_DEBUG_BIT_ARB;
		}

		attribs[attribCount] = 0;

		WglContext = wglCreateContextAttribsARBFunc(hDC, 0, attribs);
		VALIDATE(wglMakeCurrent(hDC, WglContext), "wglMakeCurrent failed.");

		OVR::GLEContext::SetCurrentContext(&GLEContext);
		GLEContext.Init();

		glGenFramebuffers(1, &fboId);

		glEnable(GL_DEPTH_TEST);
		glFrontFace(GL_CW);
		glEnable(GL_CULL_FACE);

		if (UseDebugContext && GLE_ARB_debug_output)
		{
			glDebugMessageCallbackARB(DebugGLCallback, NULL);
			if (glGetError())
			{
				VALIDATE( false, "glDebugMessageCallbackARB failed." );
			}

			glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS_ARB);

			// Explicitly disable notification severity output.
			glDebugMessageControlARB(GL_DEBUG_SOURCE_API, GL_DONT_CARE, GL_DEBUG_SEVERITY_NOTIFICATION, 0, NULL, GL_FALSE);
		}

		ShowWindow( Window, SW_SHOW );
		UpdateWindow( Window );
		return true;
	}

	bool HandleMessages(void)
	{
		MSG msg;
		while (PeekMessage(&msg, NULL, 0U, 0U, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		return Running;
	}

	void ReleaseDevice()
	{
		if (fboId)
		{
			glDeleteFramebuffers(1, &fboId);
			fboId = 0;
		}
		if (WglContext)
		{
			wglMakeCurrent(NULL, NULL);
			wglDeleteContext(WglContext);
			WglContext = nullptr;
		}
		GLEContext.Shutdown();
	}


	static void GLAPIENTRY DebugGLCallback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, const void* userParam)
	{
		fOutputDebugString("Message from OpenGL: %s\n", message );
	}
};
