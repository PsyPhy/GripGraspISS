#define PRESUMED_SAME_FRAME_THRESHOLD 0.0025

// Sets how many rendered pixels per actual screen pixel.
// Set this to 1 for normal operation.
// Set it to 0.5 if you are having trouble keeping up the frame rate.

static const float pixelsPerDisplayPixel = 1.0f;

class OculusMapper
{

private:

    static const bool   UseDebugContext = false;


	TextureBuffer		*eyeRenderTexture[2];
    DepthBuffer			*eyeDepthBuffer[2];
    ovrMirrorTexture	mirrorTexture;
	GLuint				mirrorWidth;
	GLuint				mirrorHeight;
    GLuint				mirrorFBO;

	ovrGraphicsLuid		luid;

public:

	OculusDisplayOGL	*display;
	ovrSession			session;

	ovrHmdDesc			hmdDesc;
	ovrPosef			hmdToEyePose[2];
	ovrSizei			idealTextureSize;

	// Keeps track of when the HMD tracker was last sampled and what was the pose of
	// each eye when it was sampled. This is used by the compositor to do asynchronous
	// time warping of the visual field.
	ovrTrackingState	cachedHmdState;			
	long long			cachedFrameIndex;
	unsigned int		cachedHandStatusFlags[2];

	bool	isVisible;
	bool	mirrorOn;

	OculusMapper () : mirrorFBO( 0 ) , session( nullptr ) , display( nullptr ) , isVisible( true ), mirrorOn( false ), cachedFrameIndex(0)
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

		ovrEyeRenderDesc	eye_render_desc;
		eye_render_desc = ovr_GetRenderDesc(session, ovrEye_Left, hmdDesc.DefaultEyeFov[0]);
        hmdToEyePose[0] = eye_render_desc.HmdToEyePose;
		eye_render_desc = ovr_GetRenderDesc(session, ovrEye_Right, hmdDesc.DefaultEyeFov[1]);
		hmdToEyePose[1] = eye_render_desc.HmdToEyePose;

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

	// Read the tracker state compute the poses for each of the eyes.
	ovrTrackingState ReadTrackingState ( ovrSensorData *sensorData = nullptr ) {
        double ftiming = ovr_GetPredictedDisplayTime( session, cachedFrameIndex );
		if ( sensorData ) cachedHmdState = ovr_GetTrackingStateWithSensorData( session, ftiming, ovrTrue, sensorData );
		else cachedHmdState = ovr_GetTrackingState( session, ftiming, ovrTrue );
		cachedHandStatusFlags[0] = cachedHmdState.HandStatusFlags[0];
		cachedHandStatusFlags[1] = cachedHmdState.HandStatusFlags[1];
		return cachedHmdState;
	}
	// Read the HMD tracker pose.
	ovrPoseStatef ReadHeadPose () {
		ovrTrackingState hmdState = ReadTrackingState();
		return hmdState.HeadPose;
	}

	// Read a hand tracker pose as sampled with the previous HMD tracker pose.
	ovrPoseStatef ReadCachedHandPose ( ovrHandType hand, unsigned int *flags = nullptr ) {
		if ( flags ) *flags = cachedHmdState.HandStatusFlags[hand];
		return cachedHmdState.HandPoses[hand];
	}
	// Read the hand tracker now. Does not depend on a previous read of the HMD tracker.
	ovrPoseStatef ReadHandPose ( ovrHandType hand,unsigned int *flags = nullptr ) {
		ovrPoseStatef handPose;
        double ftiming = ovr_GetPredictedDisplayTime( session, cachedFrameIndex );
 		ovrTrackingState hmdState = ovr_GetTrackingState( session, ftiming, ovrTrue );
		handPose = hmdState.HandPoses[hand];
		if ( flags ) *flags = hmdState.HandStatusFlags[hand];
		cachedHandStatusFlags[0] = hmdState.HandStatusFlags[0];
		cachedHandStatusFlags[1] = hmdState.HandStatusFlags[1];
		return handPose;
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
	// The mapper takes care of transforming from player to head pose to eye pose,
	// taking into account the current pose of the HMD tracker.
	// Outputs are the view and projection matrices for the specified eye.
	void GetEyeProjections ( int eye, OVR::Vector3f position, OVR::Matrix4f orientation, OVR::Matrix4f *view, OVR::Matrix4f *projection ) {

		ovrPoseStatef hmdPoseState = ReadHeadPose();
		ovrPosef eyePose[2];
		ovr_CalcEyePoses( hmdPoseState.ThePose, hmdToEyePose, eyePose );

		OVR::Vector3f shiftedEyePosition = position + orientation.Transform( eyePose[eye].Position );
		OVR::Matrix4f eyeOrientation = orientation * OVR::Matrix4f( eyePose[eye].Orientation );
        OVR::Vector3f up = eyeOrientation.Transform( OVR::Vector3f( 0, 1, 0 ) );
        OVR::Vector3f forward = eyeOrientation.Transform( OVR::Vector3f( 0, 0, -1 ) );

        *view = OVR::Matrix4f::LookAtRH( shiftedEyePosition, shiftedEyePosition + forward, up );
        *projection = ovrMatrix4f_Projection( hmdDesc.DefaultEyeFov[eye], 0.2f, 1000.0f, ovrProjection_None );

	}

	ovrResult BlastIt () {
		
		 
		ovrResult result;

		// Do distortion rendering, Present and flush/sync

		// Here is some magic that I do not fully understand.
		// EyeRenderPose[] must contain the orientation as sensed by the HMD,
		//  even if we have overridden the viewing orientation with another tracker. If the HMD 
		//  orientation has been read 'recently', I assume that the viewpoints are already set.
		// But if the delay is long, then perhaps we are not using ReadHeadPose() at all
		//  (i.e. we are using another tracker), so I perform a ReadHeadPose() here to be 
		//  sure that the Viewpoints are prepared as needed.
		if ( ovr_GetTimeInSeconds() - cachedHmdState.HeadPose.TimeInSeconds > PRESUMED_SAME_FRAME_THRESHOLD ) {
			ReadHeadPose();
		}

		ovrPosef	eyePose[2];
        ovr_CalcEyePoses( cachedHmdState.HeadPose.ThePose, hmdToEyePose, eyePose );

		ovrLayerEyeFov ld;
        ld.Header.Type  = ovrLayerType_EyeFov;
		ld.Header.Flags = ovrLayerFlag_TextureOriginAtBottomLeft;   // Because OpenGL.
        for (int eye = 0; eye < 2; ++eye)
        {
            ld.ColorTexture[eye] = eyeRenderTexture[eye]->TextureChain;
            ld.Viewport[eye]     = OVR::Recti(eyeRenderTexture[eye]->GetSize());
            ld.Fov[eye]          = hmdDesc.DefaultEyeFov[eye];
            ld.RenderPose[eye]   = eyePose[eye];
            ld.SensorSampleTime  = cachedHmdState.HeadPose.TimeInSeconds;
        } 	
        ovrLayerHeader *layers = &ld.Header;
		result = ovr_SubmitFrame( session, cachedFrameIndex, nullptr, &layers, 1 );

        // exit the rendering loop if submit returns an error
        if ( !OVR_SUCCESS(result) ) return result;
        isVisible = (result == ovrSuccess);
		cachedFrameIndex++;

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

