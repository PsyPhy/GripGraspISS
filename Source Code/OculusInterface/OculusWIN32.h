/************************************************************************************

								OculusWIN32.h

*************************************************************************************/

// Based on Win32_GLAppUtil.h from Oculus.
// I have separated the Win32 OpenGL functionality here from the scene drawing elsewhere.

// The following includes comes from the Oculus OVR source files.
// The path is set via the user macro $(OVRSDKROOT) and via the property pages
//  in the VS2010 project files. I was able to modify $(OVRSDKROOT) by editing 
//  OVRRootPath.props. I could not figure out how to do it within VS2010.
#include "Kernel/OVR_System.h"
#include "OVR_CAPI_GL.h"
#include "GL/CAPI_GLE.h"
#include "Extras/OVR_Math.h"

// Oculus had this in the include file, but that is bad practice because it gets included everywhere.
// I comment it out here and qualify the names with OVR:: as needed.
//using namespace OVR;

// Here I define indices into the Button map of OculusDisplayOGL defined below.
// It is a little dangerous because the names are so generic. But I don't see any
// warnings about redefinitions as of yet.
#define MOUSE_LEFT	1
#define MOUSE_MIDDLE 2
#define MOUSE_RIGHT 3


#ifndef VALIDATE
    #define VALIDATE(x, msg) if (!(x)) { MessageBoxA(NULL, (msg), "OculusWIN32", MB_ICONERROR | MB_OK); exit(-1); }
#endif

//---------------------------------------------------------------------------------------
struct DepthBuffer
{
    GLuint        texId;

    DepthBuffer(OVR::Sizei size, int sampleCount)
    {
        OVR_ASSERT(sampleCount <= 1); // The code doesn't currently handle MSAA textures.

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
    ovrHmd              hmd;
    ovrSwapTextureSet*  TextureSet;
    GLuint              texId;
    GLuint              fboId;
    OVR::Sizei               texSize;

    TextureBuffer(ovrHmd hmd, bool rendertarget, bool displayableOnHmd, OVR::Sizei size, int mipLevels, unsigned char * data, int sampleCount) :
        hmd(hmd),
        TextureSet(nullptr),
        texId(0),
        fboId(0),
        texSize(0, 0)
    {
        OVR_ASSERT(sampleCount <= 1); // The code doesn't currently handle MSAA textures.

        texSize = size;

        if (displayableOnHmd)
        {
            // This texture isn't necessarily going to be a rendertarget, but it usually is.
            OVR_ASSERT(hmd); // No HMD? A little odd.
            OVR_ASSERT(sampleCount == 1); // ovr_CreateSwapTextureSetD3D11 doesn't support MSAA. JMc: Did they mean ovr_CreateSwapTextureSetGL()?

            ovrResult result = ovr_CreateSwapTextureSetGL(hmd, GL_SRGB8_ALPHA8, size.w, size.h, &TextureSet);

            if(OVR_SUCCESS(result))
            {
                for (int i = 0; i < TextureSet->TextureCount; ++i)
                {
                    ovrGLTexture* tex = (ovrGLTexture*)&TextureSet->Textures[i];
                    glBindTexture(GL_TEXTURE_2D, tex->OGL.TexId);

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
        if (TextureSet)
        {
            ovr_DestroySwapTextureSet(hmd, TextureSet);
            TextureSet = nullptr;
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
        auto tex = reinterpret_cast<ovrGLTexture*>(&TextureSet->Textures[TextureSet->CurrentIndex]);

        glBindFramebuffer(GL_FRAMEBUFFER, fboId);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, tex->OGL.TexId, 0);
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
};

//-------------------------------------------------------------------------------------------
struct OculusDisplayOGL
{
    static const bool       UseDebugContext = false;

    HWND                    Window;
    HDC                     hDC;
    HGLRC                   WglContext;
    OVR::GLEContext         GLEContext;
    bool                    Running;
    bool                    Key[256];
	bool					Button[256];	
    int                     WinSizeW;
    int                     WinSizeH;
    GLuint                  fboId;
    HINSTANCE               hInstance;

    static LRESULT CALLBACK WindowProc(_In_ HWND hWnd, _In_ UINT Msg, _In_ WPARAM wParam, _In_ LPARAM lParam)
    {
        OculusDisplayOGL *p = reinterpret_cast<OculusDisplayOGL *>(GetWindowLongPtr(hWnd, 0));
        switch (Msg)
        {

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
            break;
        case WM_KEYUP:
            p->Key[wParam] = false;
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

    OculusDisplayOGL() :
        Window(nullptr),
        hDC(nullptr),
        WglContext(nullptr),
        GLEContext(),
        Running(false),
        WinSizeW(0),
        WinSizeH(0),
        fboId(0),
        hInstance(nullptr)
    {
		// Clear input
		for (int i = 0; i < sizeof(Key) / sizeof(Key[0]); ++i) Key[i] = false;
    }

    ~OculusDisplayOGL()
    {
        ReleaseDevice();
        CloseWindow();
    }

    bool InitWindow(HINSTANCE hInst, LPCWSTR title)
    {
        hInstance = hInst;
        Running = true;

        WNDCLASSW wc;
        memset(&wc, 0, sizeof(wc));
        wc.style = CS_CLASSDC;
        wc.lpfnWndProc = WindowProc;
        wc.cbWndExtra = sizeof(struct OGL *);
        wc.hInstance = GetModuleHandleW(NULL);
        wc.lpszClassName = L"ORT";
        RegisterClassW(&wc);

        // adjust the window size and show at InitDevice time
        Window = CreateWindowW(wc.lpszClassName, title, WS_OVERLAPPEDWINDOW, 0, 0, 0, 0, 0, 0, hInstance, 0);
        if (!Window) return false;

        SetWindowLongPtr(Window, 0, LONG_PTR(this));

        hDC = GetDC(Window);

        return true;
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
    bool InitDevice(int vpW, int vpH, const LUID* /*pLuid*/, bool windowed = true)
    {
        WinSizeW = vpW;
        WinSizeH = vpH;

        RECT size = { 0, 0, vpW, vpH };
        AdjustWindowRect(&size, WS_OVERLAPPEDWINDOW, false);
        const UINT flags = SWP_NOMOVE | SWP_NOZORDER | SWP_SHOWWINDOW;
        if (!SetWindowPos(Window, nullptr, 0, 0, size.right - size.left, size.bottom - size.top, flags))
            return false;

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
            OVR_ASSERT(wglChoosePixelFormatARBFunc && wglCreateContextAttribsARBFunc);

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
                OVR_DEBUG_LOG(("glDebugMessageCallbackARB failed."));
            }

            glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS_ARB);

            // Explicitly disable notification severity output.
            glDebugMessageControlARB(GL_DEBUG_SOURCE_API, GL_DONT_CARE, GL_DEBUG_SEVERITY_NOTIFICATION, 0, NULL, GL_FALSE);
        }

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
        OVR_DEBUG_LOG(("Message from OpenGL: %s\n", message));
    }
};
