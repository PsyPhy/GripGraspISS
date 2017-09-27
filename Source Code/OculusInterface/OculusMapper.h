#define PRESUMED_SAME_FRAME_THRESHOLD 0.0025

// Sets how many rendered pixels per actual screen pixel.
// Set this to 1 for normal operation.
// Set it to 0.5 if you are having trouble keeping up the frame rate.

static const float pixelsPerDisplayPixel = 1.0f;

class OculusMapper
{

public:

    static const bool   UseDebugContext = false;

	OculusDisplayOGL	*display;

	TextureBuffer		*eyeRenderTexture[2];
    DepthBuffer			*eyeDepthBuffer[2];
    ovrMirrorTexture	mirrorTexture;
	GLuint				mirrorWidth;
	GLuint				mirrorHeight;
    GLuint				mirrorFBO;
	ovrSizei			idealTextureSize;

	ovrSession			session;
	ovrGraphicsLuid		luid;
	ovrHmdDesc			hmdDesc;
	ovrEyeRenderDesc	EyeRenderDesc[2];
    ovrPosef            EyeRenderPose[2];
	ovrVector3f         ViewOffset[2];
	unsigned int		handStatusFlags[2];

	double				sensorSampleTime;
	long long			frameIndex;


	bool	isVisible;
	bool	mirrorOn;

	OculusMapper () : mirrorFBO( 0 ) , session( nullptr ) , display( nullptr ) , isVisible( true ), mirrorOn( false ), frameIndex(0)
	{
		eyeRenderTexture[0] = eyeRenderTexture[1] = nullptr;
		eyeDepthBuffer[0] = eyeDepthBuffer[1] = nullptr;
		mirrorTexture = nullptr;
	}
	~OculusMapper ()
	{}

	ovrResult Initialize ( OculusDisplayOGL *display, bool mirrorOn ) {

		this->display = display;
		this->mirrorOn = mirrorOn;

		ovrResult result = ovr_Create( &session, &luid );
		if ( !OVR_SUCCESS( result) ) return result;
		hmdDesc = ovr_GetHmdDesc(session);

		// Resize the mirror window to match the dimensions of the HMD.
		// Note: this will have no effect if the window is in full screen mode or 
		//  if it is encrusted inside another window.
		display->ResizeWindow( hmdDesc.Resolution.w / 2, hmdDesc.Resolution.h / 2 );
		if ( !display->InitDevice( reinterpret_cast<LUID*>(&luid) ) ) return ovrError_Initialize;
		mirrorWidth = display->WinSizeW;
		mirrorHeight = display->WinSizeH;

		// Make eye render buffers
		for (int eye = 0; eye < 2; ++eye) {
			idealTextureSize = ovr_GetFovTextureSize(session, ovrEyeType(eye), hmdDesc.DefaultEyeFov[eye], pixelsPerDisplayPixel );
			eyeRenderTexture[eye] = new TextureBuffer( session, true, true, idealTextureSize, 1, NULL, 1 );
			eyeDepthBuffer[eye]   = new DepthBuffer(eyeRenderTexture[eye]->GetSize(), 0);
			if ( !eyeRenderTexture[eye]->TextureChain ) VALIDATE( false, "Failed to create texture." );
		}

		// Create mirror texture and an FBO used to copy mirror texture to back buffer
		ovrMirrorTextureDesc desc;
		memset(&desc, 0, sizeof(desc));
		desc.Width = mirrorWidth;
		desc.Height =mirrorHeight;
		desc.Format = OVR_FORMAT_R8G8B8A8_UNORM_SRGB;

		result = ovr_CreateMirrorTextureGL( session, &desc, &mirrorTexture );
		if ( !OVR_SUCCESS( result ) ) return ovrError_MemoryAllocationFailure;

		// Configure the mirror read buffer
		GLuint texId;
		ovr_GetMirrorTextureBufferGL(session, mirrorTexture, &texId);

		glGenFramebuffers(1, &mirrorFBO);
		glBindFramebuffer(GL_READ_FRAMEBUFFER, mirrorFBO);
		glFramebufferTexture2D(GL_READ_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texId, 0);
		glFramebufferRenderbuffer(GL_READ_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, 0);
		glBindFramebuffer(GL_READ_FRAMEBUFFER, 0);

		ovrEyeRenderDesc EyeRenderDesc[2]; 
		EyeRenderDesc[0] = ovr_GetRenderDesc(session, ovrEye_Left, hmdDesc.DefaultEyeFov[0]);
		EyeRenderDesc[1] = ovr_GetRenderDesc(session, ovrEye_Right, hmdDesc.DefaultEyeFov[1]);

		// Turn off vsync to let the compositor do its magic
		wglSwapIntervalEXT(0);
		return ovrSuccess;
	}

	void Release () {

		// Release the buffers.
		if ( mirrorFBO ) glDeleteFramebuffers(1, &mirrorFBO);
		if ( mirrorTexture ) ovr_DestroyMirrorTexture( session, mirrorTexture );
		for (int eye = 0; eye < 2; ++eye)
		{
			delete eyeRenderTexture[eye];
			delete eyeDepthBuffer[eye];
		}
		if ( display ) display->ReleaseDevice();
		if ( session ) ovr_Destroy( session );
	}

	ovrPosef ReadHandPose ( ovrHandType hand ) {

		ovrPosef handPose;
        double ftiming = ovr_GetPredictedDisplayTime( session, frameIndex );
         // Keeping sensorSampleTime as close to ovr_GetTrackingState as possible - fed into the layer
        //sensorSampleTime = ovr_GetTimeInSeconds();
		ovrTrackingState hmdState = ovr_GetTrackingState( session, ftiming, ovrTrue );
		handPose = hmdState.HandPoses[hand].ThePose;
		handStatusFlags[0] = hmdState.HandStatusFlags[0];
		handStatusFlags[1] = hmdState.HandStatusFlags[1];

		return handPose;
	}
	// Read the tracker and compute the poses for each of the eyes.
	ovrPosef ReadHeadPose ( void ) {

        ViewOffset[0] = EyeRenderDesc[0].HmdToEyeOffset;
		ViewOffset[1] = EyeRenderDesc[1].HmdToEyeOffset;
 
		ovrPosef headPose;
        double ftiming = ovr_GetPredictedDisplayTime( session, frameIndex );
         // Keeping sensorSampleTime as close to ovr_GetTrackingState as possible - fed into the layer
        sensorSampleTime = ovr_GetTimeInSeconds();
		ovrTrackingState hmdState = ovr_GetTrackingState( session, ftiming, ovrTrue );
		headPose = hmdState.HeadPose.ThePose;
        ovr_CalcEyePoses( headPose, ViewOffset, EyeRenderPose );

		return headPose;
	}
	// Read the tracker state compute the poses for each of the eyes.
	ovrTrackingState ReadTrackingState ( ovrSensorData *sensorData = nullptr ) {

        ViewOffset[0] = EyeRenderDesc[0].HmdToEyeOffset;
		ViewOffset[1] = EyeRenderDesc[1].HmdToEyeOffset;
 
		ovrPosef headPose;
		ovrTrackingState hmdState;
		frameIndex++;
        double ftiming = ovr_GetPredictedDisplayTime( session, frameIndex );
         // Keeping sensorSampleTime as close to ovr_GetTrackingState as possible - fed into the layer
        sensorSampleTime = ovr_GetTimeInSeconds();
		/* if ( sensorData ) hmdState = ovr_GetTrackingStateWithSensorData( session, ftiming, ovrTrue, sensorData );
		else */ hmdState = ovr_GetTrackingState( session, ftiming, ovrTrue );
		headPose = hmdState.HeadPose.ThePose;
        ovr_CalcEyePoses( headPose, ViewOffset, EyeRenderPose );

		return hmdState;
	}

	// Prepare for rendering to one or the other eye.
	void SelectEye ( int eye ) {

		// Increment to use next texture, just before writing
        // eyeRenderTexture[eye]->TextureSet->CurrentIndex = (eyeRenderTexture[eye]->TextureSet->CurrentIndex + 1) % eyeRenderTexture[eye]->TextureSet->TextureCount;
        // Switch to eye render target
        eyeRenderTexture[eye]->SetAndClearRenderSurface( eyeDepthBuffer[eye] );

	}

	void DeselectEye ( int eye ) {
		// Avoids an error when calling SetAndClearRenderSurface during next iteration.
        // Without this, during the next while loop iteration SetAndClearRenderSurface
        // would bind a framebuffer with an invalid COLOR_ATTACHMENT0 because the texture ID
        // associated with COLOR_ATTACHMENT0 had been unlocked by calling wglDXUnlockObjectsNV.
        eyeRenderTexture[eye]->UnsetRenderSurface();
         // Commit changes to the textures so they get picked up frame
         eyeRenderTexture[eye]->Commit();
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
       *projection = ovrMatrix4f_Projection( hmdDesc.DefaultEyeFov[eye], 0.2f, 1000.0f, ovrProjection_None );

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
        viewScaleDesc.HmdToEyeOffset[0] = ViewOffset[0];
        viewScaleDesc.HmdToEyeOffset[1] = ViewOffset[1];

        ovrLayerEyeFov ld;
		// Here I am playing with the layer type to see how we might increase the frame rate.
		// Pick EyeFov for distortion-cancelling and asyncronous time warp.
       ld.Header.Type  = ovrLayerType_EyeFov;
		// Direct will instead map directly without distortion correction.
	    // I am not sure that it exists in 1.3.
 		// ld.Header.Type  = ovrLayerType_Direct; 
 
		ld.Header.Flags = ovrLayerFlag_TextureOriginAtBottomLeft;   // Because OpenGL.
        for (int eye = 0; eye < 2; ++eye)
        {
            ld.ColorTexture[eye] = eyeRenderTexture[eye]->TextureChain;
            ld.Viewport[eye]     = OVR::Recti(eyeRenderTexture[eye]->GetSize());
            ld.Fov[eye]          = hmdDesc.DefaultEyeFov[eye];
            ld.RenderPose[eye]   = EyeRenderPose[eye];
            ld.SensorSampleTime  = sensorSampleTime;
       } 	
        ovrLayerHeader *layers = &ld.Header;
        ovrResult result = ovr_SubmitFrame( session, frameIndex, &viewScaleDesc, &layers, 1 );
        // exit the rendering loop if submit returns an error
        if ( !OVR_SUCCESS(result) ) return result;

        isVisible = (result == ovrSuccess);

		if ( mirrorOn ) {
			// Blit mirror texture to back buffer
			glBindFramebuffer( GL_READ_FRAMEBUFFER, mirrorFBO );
			glBindFramebuffer( GL_DRAW_FRAMEBUFFER, 0 );
			GLint w = mirrorWidth;
			GLint h = mirrorHeight;
			glBlitFramebuffer(0, h, w, 0,
							  0, 0, w, h,
							  GL_COLOR_BUFFER_BIT, GL_NEAREST);
			glBindFramebuffer(GL_READ_FRAMEBUFFER, 0);

			SwapBuffers( display->hDC );
		}
		return result;
	}

};

