#define PRESUMED_SAME_FRAME_THRESHOLD 0.0025

class OculusMapper
{

public:

    static const bool   UseDebugContext = false;

	OculusDisplayOGL	*display;

	TextureBuffer		*eyeRenderTexture[2];
    DepthBuffer			*eyeDepthBuffer[2];
    ovrGLTexture		*mirrorTexture;
    GLuint				 mirrorFBO;

	ovrHmd				HMD;
	ovrGraphicsLuid		luid;
	ovrHmdDesc			hmdDesc;
	ovrEyeRenderDesc	EyeRenderDesc[2];
    ovrPosef            EyeRenderPose[2];
	ovrVector3f         ViewOffset[2];

	double				fTiming;
	double				sensorSampleTime;

	bool	isVisible;

	OculusMapper () : mirrorFBO( 0 ) , HMD( nullptr ) , display( nullptr ) , isVisible( true )
	{
		eyeRenderTexture[0] = eyeRenderTexture[1] = nullptr;
		eyeDepthBuffer[0] = eyeDepthBuffer[1] = nullptr;
		mirrorTexture = nullptr;
	}
	~OculusMapper ()
	{}

	ovrResult Initialize ( OculusDisplayOGL *display ) {

		this->display = display;
		this->HMD = HMD;

		ovrResult result = ovr_Create( &HMD, &luid );
		if ( !OVR_SUCCESS( result) ) return result;

		hmdDesc = ovr_GetHmdDesc(HMD);

		// Setup Window and Graphics
		// Note: the mirror window can be any size, for this sample we use 1/2 the HMD resolution
		ovrSizei windowSize = { hmdDesc.Resolution.w / 2, hmdDesc.Resolution.h / 2 };
		if ( !display->InitDevice( windowSize.w, windowSize.h, reinterpret_cast<LUID*>(&luid)) ) return ovrError_Initialize;

		// Make eye render buffers
		for (int eye = 0; eye < 2; ++eye)
		{
			ovrSizei idealTextureSize = ovr_GetFovTextureSize(HMD, ovrEyeType(eye), hmdDesc.DefaultEyeFov[eye], 1);
			eyeRenderTexture[eye] = new TextureBuffer(HMD, true, true, idealTextureSize, 1, NULL, 1);
			eyeDepthBuffer[eye]   = new DepthBuffer(eyeRenderTexture[eye]->GetSize(), 0);

			if ( !eyeRenderTexture[eye]->TextureSet ) return ovrError_MemoryAllocationFailure;
		}

		// Create mirror texture and an FBO used to copy mirror texture to back buffer
		result = ovr_CreateMirrorTextureGL( HMD, GL_SRGB8_ALPHA8, windowSize.w, windowSize.h, reinterpret_cast<ovrTexture**>(&mirrorTexture) );
		if ( !OVR_SUCCESS( result ) ) return ovrError_MemoryAllocationFailure;

		// Configure the mirror read buffer
		glGenFramebuffers(1, &mirrorFBO);
		glBindFramebuffer(GL_READ_FRAMEBUFFER, mirrorFBO);
		glFramebufferTexture2D(GL_READ_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, mirrorTexture->OGL.TexId, 0);
		glFramebufferRenderbuffer(GL_READ_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, 0);
		glBindFramebuffer(GL_READ_FRAMEBUFFER, 0);

		ovrEyeRenderDesc EyeRenderDesc[2]; 
		EyeRenderDesc[0] = ovr_GetRenderDesc(HMD, ovrEye_Left, hmdDesc.DefaultEyeFov[0]);
		EyeRenderDesc[1] = ovr_GetRenderDesc(HMD, ovrEye_Right, hmdDesc.DefaultEyeFov[1]);

		// Turn off vsync to let the compositor do its magic
		wglSwapIntervalEXT(0);
		return ovrSuccess;
	}

	void Release () {

		if ( mirrorFBO ) glDeleteFramebuffers(1, &mirrorFBO);
		if ( mirrorTexture ) ovr_DestroyMirrorTexture(HMD, reinterpret_cast<ovrTexture*>(mirrorTexture));
		for (int eye = 0; eye < 2; ++eye)
		{
			delete eyeRenderTexture[eye];
			delete eyeDepthBuffer[eye];
		}
		if ( display ) display->ReleaseDevice();
		if (HMD) ovr_Destroy( HMD );
	}

	// Read the tracker and compute the poses for each of the eyes.
	ovrPosef ReadHeadPose ( void ) {

        ViewOffset[0] = EyeRenderDesc[0].HmdToEyeViewOffset;
		ViewOffset[1] = EyeRenderDesc[1].HmdToEyeViewOffset;
 
		ovrPosef headPose;
        double ftiming = ovr_GetPredictedDisplayTime( HMD, 0 );
         // Keeping sensorSampleTime as close to ovr_GetTrackingState as possible - fed into the layer
        sensorSampleTime = ovr_GetTimeInSeconds();
		ovrTrackingState hmdState = ovr_GetTrackingState( HMD, ftiming, ovrTrue );
		headPose = hmdState.HeadPose.ThePose;
        ovr_CalcEyePoses( headPose, ViewOffset, EyeRenderPose );

		return headPose;
	}

	// Prepare for rendering to one or the other eye.
	void SelectEye ( int eye ) {

		// Increment to use next texture, just before writing
        eyeRenderTexture[eye]->TextureSet->CurrentIndex = (eyeRenderTexture[eye]->TextureSet->CurrentIndex + 1) % eyeRenderTexture[eye]->TextureSet->TextureCount;
        // Switch to eye render target
        eyeRenderTexture[eye]->SetAndClearRenderSurface( eyeDepthBuffer[eye] );

	}

	void DeselectEye ( int eye ) {
		// Avoids an error when calling SetAndClearRenderSurface during next iteration.
        // Without this, during the next while loop iteration SetAndClearRenderSurface
        // would bind a framebuffer with an invalid COLOR_ATTACHMENT0 because the texture ID
        // associated with COLOR_ATTACHMENT0 had been unlocked by calling wglDXUnlockObjectsNV.
        eyeRenderTexture[eye]->UnsetRenderSurface();
	}
	
	// Get the transforms that will map from world space to the space of each eye.
	// Inputs are which eye, the position and the orientation of the player.
	// The mapper takes care of transforming from player to head pose to eye pose.
	// Outputs are the view and projection matrices for the specified eye.
	void GetEyeProjections ( int eye, OVR::Vector3f position, OVR::Matrix4f orientation, OVR::Matrix4f *view, OVR::Matrix4f *projection ) {

		OVR::Vector3f shiftedEyePosition = position + orientation.Transform( EyeRenderPose[eye].Position );
		OVR::Matrix4f eyeOrientation = orientation * OVR::Matrix4f( EyeRenderPose[eye].Orientation );
        OVR::Vector3f up = eyeOrientation.Transform( OVR::Vector3f( 0, 1, 0 ) );
        OVR::Vector3f forward = eyeOrientation.Transform( OVR::Vector3f( 0, 0, -1 ) );

        *view = OVR::Matrix4f::LookAtRH( shiftedEyePosition, shiftedEyePosition + forward, up );
        *projection = ovrMatrix4f_Projection( hmdDesc.DefaultEyeFov[eye], 0.2f, 1000.0f, ovrProjection_RightHanded );

	}

	ovrResult BlastIt () {
		
		// Do distortion rendering, Present and flush/sync

		// Here is some magic that I do not fully understand.
		// EyeRenderPose[] must contain the orientation as sensed by the HMD,
		//  even if we have overridden the viewing orientation with another tracker. If the HMD 
		//  orientation has been read 'recently', I assume that the viewpoints are already set.
		// But if the delay is long, then perhaps we are not using ReadHeadPose() at all
		//  (i.e. we are using another tracker), so I perform a ReadHeadPose() here to be 
		//  sure that the Viewpoints are prepared as needed.
		if ( ovr_GetTimeInSeconds() - sensorSampleTime > PRESUMED_SAME_FRAME_THRESHOLD ) {
			ReadHeadPose();
		}

        // Set up positional data.
        ovrViewScaleDesc viewScaleDesc;
        viewScaleDesc.HmdSpaceToWorldScaleInMeters = 1.0f;
        viewScaleDesc.HmdToEyeViewOffset[0] = ViewOffset[0];
        viewScaleDesc.HmdToEyeViewOffset[1] = ViewOffset[1];

        ovrLayerEyeFov ld;
        ld.Header.Type  = ovrLayerType_EyeFov;
        ld.Header.Flags = ovrLayerFlag_TextureOriginAtBottomLeft;   // Because OpenGL.

        for (int eye = 0; eye < 2; ++eye)
        {
            ld.ColorTexture[eye] = eyeRenderTexture[eye]->TextureSet;
            ld.Viewport[eye]     = OVR::Recti(eyeRenderTexture[eye]->GetSize());
            ld.Fov[eye]          = hmdDesc.DefaultEyeFov[eye];
            ld.RenderPose[eye]   = EyeRenderPose[eye];
            ld.SensorSampleTime  = sensorSampleTime;
        }
		
        ovrLayerHeader *layers = &ld.Header;
        ovrResult result = ovr_SubmitFrame( HMD, 0, &viewScaleDesc, &layers, 1 );
        // exit the rendering loop if submit returns an error
        if ( !OVR_SUCCESS(result) ) return result;

        isVisible = (result == ovrSuccess);

        // Blit mirror texture to back buffer
        glBindFramebuffer( GL_READ_FRAMEBUFFER, mirrorFBO );
        glBindFramebuffer( GL_DRAW_FRAMEBUFFER, 0 );
        GLint w = mirrorTexture->OGL.Header.TextureSize.w;
        GLint h = mirrorTexture->OGL.Header.TextureSize.h;
        glBlitFramebuffer(0, h, w, 0,
                          0, 0, w, h,
                          GL_COLOR_BUFFER_BIT, GL_NEAREST);
        glBindFramebuffer(GL_READ_FRAMEBUFFER, 0);

        SwapBuffers( display->hDC );

		return ovrSuccess;
	}

};

