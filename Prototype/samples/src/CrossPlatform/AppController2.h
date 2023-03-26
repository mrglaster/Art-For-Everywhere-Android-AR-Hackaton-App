/*===============================================================================
Copyright (c) 2020 PTC Inc. All rights reserved.

Vuforia is a trademark of PTC Inc., registered in the United States and other
countries.
===============================================================================*/

#ifndef __APPCONTROLLER_H__
#define __APPCONTROLLER_H__

#include <VuforiaEngine/VuforiaEngine.h>

#include <chrono>
#include <cstdio>
#include <functional>
#include <memory>
#include <string>


/// The AppController provides a platform-independent encapsulation of the Vuforia lifecycle
/// and Observer operation.
class AppController
{

public:
    // Constants
    static constexpr int IMAGE_TARGET_ID = 0;
    static constexpr int MODEL_TARGET_ID = 1;

    // Type definitions
    using ErrorCallback = std::function<void(const char* errorString)>;
    using InitDoneCallback = std::function<void()>;

    /// Struct to group initialization parameters passed to initAR
    class InitConfig
    {
    public:
        VuRenderVBBackendType vbRenderBackend{ VU_RENDER_VB_BACKEND_DEFAULT };
        void* appData{ nullptr };
        ErrorCallback showErrorCallback{};
        InitDoneCallback initDoneCallback{};
    };


    /// Initialize Vuforia. When the initialization is completed successfully the callback method initDoneCallback will be invoked.
    /// If initialization fails the error callback showErrorCallback will be invoked.
    /// On Android the appData pointer should be a pointer to the Activity object.
    void initAR(const InitConfig& initConfig, int target);

    /// Start the AR session
    /// Call this method when the app resumes from paused.
    bool startAR();

    /// Stop the AR session
    /// Call this method when the app is paused.
    bool stopAR();

    /// Clean up and deinitialize Vuforia.
    void deinitAR();

    /// Request that the camera refocuses in the current position
    void cameraPerformAutoFocus();

    /// Restore the camera to continuous autofocus mode
    void cameraRestoreAutoFocus();

    /// Configure Vuforia rendering.
    /// This method must be called after initAR and startAR are complete.
    /// This should be called from the Rendering thread.
    /// The orientation is specified as the platform-specific descriptor, hence the typeless parameter.
    bool configureRendering(int width, int height, void* orientation);

    /// Get the dimensions you should use to create the video background texture
    /// This method can only be called after initAR and startAR are complete.
    bool getVideoBackgroundTextureSize(VuVector2I& textureSize);

    /// Query whether the camera is currently started
    bool isARStarted() { return mARStarted; }

    /// Call this method at the start of Vuforia rendering.
    /// Gets the latest video background texture from Vuforia.
    /// Whatever the result of this call finishRender must be called before rendering completes.
    bool prepareToRender(double* viewport, VuRenderVideoBackgroundData* renderData);

    /// Call this method when Vuforia rendering is complete, this should be near the end of the
    /// platform render callback.
    void finishRender();

    /// Get the current RenderState
    /// The returned object is only valid after prepareToRender has been called
    const VuRenderState& getRenderState() { return mCurrentRenderState; }

    /// Get rendering information for the world origin position.
    /// Returns false if the world origin position is not currently available.
    bool getOrigin(VuMatrix44F& projectionMatrix, VuMatrix44F& modelViewMatrix);

    /// Get rendering information for the Image Target.
    /// Returns false if Vuforia isn't currently tracking the Image Target.
    bool getImageTargetResult(VuMatrix44F& projectionMatrix, VuMatrix44F& modelViewMatrix, VuMatrix44F& scaledModelViewMatrix);

    /// Get rendering information for the Model Target.
    /// Returns false if Vuforia isn't currently tracking the Model Target.
    bool getModelTargetResult(VuMatrix44F& projectionMatrix, VuMatrix44F& modelViewMatrix, VuMatrix44F& scaledModelViewMatrix);

    /// Get rendering information for the Model Target Guide View.
    /// Returns false if Guide View rendering isn't required for the current frame.
    bool getModelTargetGuideView(VuMatrix44F& projectionMatrix, VuMatrix44F& modelViewMatrix, VuImageInfo& guideViewImageInfo,
                                 VuBool& guideViewImageHasChanged);

    /// Get the PlatformController handle.
    /// The result is only valid after initAR is called and before deinitAR is called.
    VuController* getPlatformController() { return mPlatformController; }


private: // methods
    /// Used by initAR to prepare and invoke Vuforia initialization.
    bool initVuforiaInternal(void* appData);

    /// Convert a Vuforia initialization error to a string message
    static std::string initErrorToString(VuErrorCode error);

    /// Create the set of Vuforia Observers needed in the application
    bool createObservers();

    /// Clean up Observers created by createObservers
    void destroyObservers();

    /// Called in prepareToRender to update the cached device pose information
    void updateDevicePose();

private: // data members
    /// Callback to inform the user of an error
    ErrorCallback mShowErrorCallback;
    /// Callback to inform the user that initialization is complete
    InitDoneCallback mInitDoneCallback;

    /// Vuforia Engine instance
    VuEngine* mEngine{ nullptr };

    /// Vuforia render controller object
    VuController* mRenderController{ nullptr };

    /// Vuforia platform controller object
    VuController* mPlatformController{ nullptr };

    /// THe rendering backend to use for the Video Background
    VuRenderVBBackendType mVbRenderBackend = VuRenderVBBackendType::VU_RENDER_VB_BACKEND_DEFAULT;
    /// The target to use, either IMAGE_TARGET_ID or MODEL_TARGET_ID
    int mTarget = IMAGE_TARGET_ID;

    /// The Vuforia camera video mode to use, either DEFAULT, SPEED or QUALITY.
    VuCameraVideoModePreset mCameraVideoMode = VuCameraVideoModePreset::VU_CAMERA_VIDEO_MODE_PRESET_DEFAULT;

    /// Flag that is true when Vuforia is running
    bool mARStarted = false;

    /// Local copy of current RenderState
    VuRenderState mCurrentRenderState;
    /// Remember the display aspect ratio for later configuration of Guide View rendering
    float mDisplayAspectRatio;

    /// The observer for device poses
    VuObserver* mDevicePoseObserver = nullptr;

    /// Data structure with information about the last known device pose
    struct DevicePoseData
    {
        /// Device pose
        VuMatrix44F pose{};

        /// Device pose status
        VuObservationPoseStatus poseStatus{ VU_OBSERVATION_POSE_STATUS_NO_POSE };

        /// Device pose status info
        VuDevicePoseObservationStatusInfo poseStatusInfo{ VU_DEVICE_POSE_OBSERVATION_STATUS_INFO_UNKNOWN };
    };
    DevicePoseData mLatestDevicePoseData{};

    /// Flag set when the tracker is relocalizing
    bool mTimingRelocalizingState{ false };
    /// The time when the tracker entered the relocalizing state
    std::chrono::steady_clock::time_point mEnteredRelocalizingState;
    /// The maximum length of time in the RELOCALIZING state before tracking is reset
    static constexpr int MAX_RELOCALIZING_SECONDS{ 15 };

    /// The observer for either the Image or Model target depending on which target was specified
    VuObserver* mObjectObserver = nullptr;

    /// Between calls to prepareToRender and finishRender this holds a copy of the Vuforia state.
    VuState* mVuforiaState = nullptr;

    /// If a Model Target Guide View should be displayed this points to the object providing
    /// details of what the App should render.
    VuGuideView* mGuideViewModelTarget = nullptr;
};

#endif /* __APPCONTROLLER_H__ */
