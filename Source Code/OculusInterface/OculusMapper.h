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
	ovrSensorData		cachedSensorData;
	long long			cachedFrameIndex;

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

	// Read the tracker state.
	ovrTrackingState ReadTrackingState ( void ) {
		// This somehow predicts the time when the frame will be shown which is then
		// used to return a predicted tracking state. I really don't know how the 
		// prediction is done or how to choose the frame index. Based on OculusTinyRoom,
		// I am using the index for the next frame to be shown.
        double ftiming = ovr_GetPredictedDisplayTime( session, cachedFrameIndex );
		// We keep track of the state that is returned. This is used when sending the frame to the Compositor.
		// See BlastIt() below.
		// It also allows us to get the hand poses that correspond to the most recent fetch of the HMD pose.
		// See ReadCachedHandPose() below.
		cachedHmdState = ovr_GetTrackingStateWithSensorData( session, ftiming, ovrTrue, &cachedSensorData );
		return cachedHmdState;
	}

	// Read the tracker state compute the poses for each of the eyes.
	ovrSensorData ReadSensorData ( void ) {
        double ftiming = ovr_GetPredictedDisplayTime( session, cachedFrameIndex );
		cachedHmdState = ovr_GetTrackingStateWithSensorData( session, ftiming, ovrTrue, &cachedSensorData );
		return cachedSensorData;
	}

	// Read the HMD tracker pose.
	ovrPoseStatef ReadHeadPose ( unsigned int *flags = nullptr ) {
		ovrTrackingState hmdState = ReadTrackingState();
		if ( flags ) *flags = cachedHmdState.StatusFlags;
		return hmdState.HeadPose;
	}

	// Read a hand tracker pose as sampled with the previous HMD tracker pose.
	ovrPoseStatef ReadCachedHandPose ( ovrHandType hand, unsigned int *flags = nullptr ) {
		if ( flags ) *flags = cachedHmdState.HandStatusFlags[hand];
		return cachedHmdState.HandPoses[hand];
	}

	// Read the hand tracker now. Does not depend on a previous read of the HMD tracker.
	// Nor does it cache the HMD state for other calls.
	ovrPoseStatef ReadHandPose ( ovrHandType hand,unsigned int *flags = nullptr ) {
		ovrPoseStatef handPose;
        double ftiming = ovr_GetPredictedDisplayTime( session, cachedFrameIndex );
 		ovrTrackingState hmdState = ovr_GetTrackingState( session, ftiming, ovrTrue );
		handPose = hmdState.HandPoses[hand];
		if ( flags ) *flags = hmdState.HandStatusFlags[hand];
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

		// Do distortion rendering, Present and flush/sync

		/////////////////////////////////////////////////////////////////////////////////////////////////////////////

		// Here is some magic that I do not fully understand.
		// EyeRenderPose[] must contain the orientation as sensed by the HMD,
		// even if we have overridden the viewing orientation with another tracker. I believe
		// that this is so that asynchronous time warping can be performed based on the 
		// movement since the head pose was computed and used to render the scene.
		
		// If the HMD orientation has been read 'recently', I assume that the viewpoints are already set.
		// But if the delay is long, then perhaps we are not using ReadHeadPose() at all
		//  (i.e. we are using another tracker), so I perform a ReadHeadPose() here to be 
		//  sure that the Viewpoints are prepared as needed. This will mean that the asyncronous time warping
		//  will be less effective if using a tracker that does not query the Oculus, because the rendering will
		//  have been done earlier in time than what is used by the Compositor to compute the warp. But I don't 
		//  see a way out. To make the warping a effective as possible, call ReadHeadPose() in your program when
		//  you sample your other tracker just to give the Oculus a refence time and pose to be used to calculate
		//  the delta for the warp.
		if ( ovr_GetTimeInSeconds() - cachedHmdState.HeadPose.TimeInSeconds > PRESUMED_SAME_FRAME_THRESHOLD ) {
			ReadHeadPose();
		}

		// The compositor needs to know where each eye is looking based on where it *thinks* the head is looking.
		// The cached head pose may not be where the subject is actually looking, e.g. because the Oculus
		// tracker does not work in 0g. But we don't care here where the subject is looking in the virtual world
		// as the scene is already rendered from whatever viewpoint we computed based on another tracker. What the
		// compositor cares about is where it *thought* that the eyes were looking based on the Oculus tracker at 
		// the time that the rendering viewpoint was computed so that it can compute the delta compared with where the 
		// Oculus tracker *thinks* that the subject is looking now , even if what the Oculus tracker *thinks* 
		// is actually wrong.

		ovrPosef	eyePose[2];
        ovr_CalcEyePoses( cachedHmdState.HeadPose.ThePose, hmdToEyePose, eyePose );

		/////////////////////////////////////////////////////////////////////////////////////////////////////////////

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
		ovrResult result = ovr_SubmitFrame( session, cachedFrameIndex, nullptr, &layers, 1 );

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

