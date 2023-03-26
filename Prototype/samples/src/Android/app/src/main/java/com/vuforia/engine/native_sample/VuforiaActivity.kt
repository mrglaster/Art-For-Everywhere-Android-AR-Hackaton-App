/*===============================================================================
Copyright (c) 2022 PTC Inc. All Rights Reserved.

Vuforia is a trademark of PTC Inc., registered in the United States and other
countries.
===============================================================================*/

package com.vuforia.engine.native_sample

import android.Manifest
import android.app.Activity
import android.content.pm.PackageManager
import android.content.res.AssetManager
import android.opengl.GLSurfaceView
import android.os.Build
import android.os.Bundle
import android.util.Log
import android.view.*
import android.view.GestureDetector.SimpleOnGestureListener
import android.widget.RelativeLayout
import android.widget.Toast
import androidx.appcompat.app.AlertDialog
import androidx.appcompat.app.AppCompatActivity
import androidx.core.app.ActivityCompat
import androidx.core.app.NavUtils
import androidx.core.content.ContextCompat
import androidx.core.view.GestureDetectorCompat
import kotlinx.coroutines.Dispatchers
import kotlinx.coroutines.GlobalScope
import kotlinx.coroutines.launch
import kotlinx.coroutines.withContext
import java.nio.ByteBuffer
import java.util.*
import javax.microedition.khronos.egl.EGLConfig
import javax.microedition.khronos.opengles.GL10
import kotlin.concurrent.schedule

/**
 * Activity to demonstrate how to use Vuforia Image Target and Model Target features,
 * Video Background rendering and Vuforia lifecycle.
 */
class VuforiaActivity : AppCompatActivity(), GLSurfaceView.Renderer, SurfaceHolder.Callback {

    /**
     * The permissions that we need to run the Vuforia Activity
     * To use the SessionRecorder with audio add Manifest.permission.RECORD_AUDIO to this array
     * and to the App manifest file
     */
    val REQUIRED_PERMISSIONS = arrayOf(Manifest.permission.CAMERA)

    private var mPermissionsRequested = false;

    private lateinit var mGLView : GLSurfaceView

    private var mTarget = 0
    private var mProgressIndicatorLayout: RelativeLayout? = null

    private var mWidth = 0
    private var mHeight = 0

    private var mVuforiaStarted = false
    private var mSurfaceChanged = false

    private var mWindowDisplayRotation = Surface.ROTATION_0

    private var mGestureDetector : GestureDetectorCompat? = null

    // Native methods
    private external fun initAR(activity: Activity, assetManager: AssetManager, target: Int)
    private external fun deinitAR()

    private external fun startAR() : Boolean
    private external fun stopAR()

    external fun cameraPerformAutoFocus()
    external fun cameraRestoreAutoFocus()

    private external fun initRendering()
    private external fun setTextures(astronautWidth: Int, astronautHeight: Int, astronautBytes: ByteBuffer,
                                     planeWidth: Int, planeHeight: Int, planeBytes: ByteBuffer,
                                     landerWidth: Int, landerHeight: Int, landerBytes: ByteBuffer)
    private external fun deinitRendering()
    private external fun configureRendering(width: Int, height: Int, orientation: Int, rotation: Int) : Boolean
    private external fun renderFrame() : Boolean


    // Activity methods
    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)

        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.P) {
            window.attributes.layoutInDisplayCutoutMode = WindowManager.LayoutParams.LAYOUT_IN_DISPLAY_CUTOUT_MODE_SHORT_EDGES
        }

        mTarget = intent.getIntExtra("Target", 0)
        mVuforiaStarted = false
        mSurfaceChanged = true

        // Create an OpenGL ES 3.0 context (also works for 3.1, 3.2)
        mGLView = GLSurfaceView(this)
        mGLView.holder.addCallback(this)
        mGLView.setEGLContextClientVersion(3)
        mGLView.setRenderer(this)
        addContentView(mGLView, ViewGroup.LayoutParams(
            ViewGroup.LayoutParams.MATCH_PARENT,
            ViewGroup.LayoutParams.MATCH_PARENT)
        )
        // Hide the GLView until we are ready
        mGLView.visibility = View.GONE

        // Prevent screen from dimming
        window.addFlags(WindowManager.LayoutParams.FLAG_KEEP_SCREEN_ON)

        makeFullScreen()

        // Setup and show a progress indicator
        mProgressIndicatorLayout = View.inflate(
            applicationContext,
            R.layout.progress_indicator, null
        ) as RelativeLayout

        addContentView(
            mProgressIndicatorLayout, ViewGroup.LayoutParams(
                ViewGroup.LayoutParams.MATCH_PARENT,
                ViewGroup.LayoutParams.MATCH_PARENT
            )
        )

        // If we have the needed runtime permissions we can start Vuforia initialization here
        // If we need to ask the user for runtime permissions we need to wait until we have them
        if (runtimePermissionsGranted()) {
            // Start Vuforia initialization in a coroutine
            GlobalScope.launch(Dispatchers.Unconfined) {
                initializeVuforia()
            }
        }

        mGestureDetector = GestureDetectorCompat(this, GestureListener())
    }


    override fun onPause() {
        stopAR()
        super.onPause()
    }


    override fun onResume() {
        super.onResume()

        makeFullScreen()

        if (runtimePermissionsGranted()) {
            if (!mPermissionsRequested && mVuforiaStarted) {
                GlobalScope.launch(Dispatchers.Unconfined) {
                    startAR()
                }
            }
        } else {
            ActivityCompat.requestPermissions(this, REQUIRED_PERMISSIONS, 0)
            mPermissionsRequested = true
        }

    }


    override fun onBackPressed() {
        // Hide the GLView while we clean up
        mGLView.visibility = View.INVISIBLE
        // Stop Vuforia Engine and call parent to navigate back
        stopAR()
        mVuforiaStarted = false
        deinitAR()
        super.onBackPressed()
    }


    override fun onOptionsItemSelected(item: MenuItem): Boolean {
        val id = item.itemId
        if (id == android.R.id.home) {
            // This ID represents the Home or Up button.
            NavUtils.navigateUpFromSameTask(this)
            return true
        }
        return super.onOptionsItemSelected(item)
    }


    override fun onRequestPermissionsResult(requestCode: Int, permissions: Array<out String>, grantResults: IntArray) {
        super.onRequestPermissionsResult(requestCode, permissions, grantResults)
        mPermissionsRequested = false

        if (permissions.isEmpty()) {
            // Permissions request was cancelled
            Toast.makeText(this, "The permission request was cancelled. You must grant Camera permission to access AR features of this application.", Toast.LENGTH_LONG).show()
            finish()
            return
        }

        if (runtimePermissionsGranted()) {
            // Start Vuforia initialization in a coroutine
            GlobalScope.launch(Dispatchers.Unconfined) {
                initializeVuforia()
            }
        } else {
            Toast.makeText(this, "You must grant Camera permission to access AR features of this application.", Toast.LENGTH_LONG).show()
            finish()
        }
    }


    // Overrider onTouchEvent to connect it to our GestureListener
    override fun onTouchEvent(event: MotionEvent?): Boolean {
        mGestureDetector?.onTouchEvent(event)
        return super.onTouchEvent(event)
    }


    /// Custom GestureListener to capture single and double tap
    inner class GestureListener : SimpleOnGestureListener() {
        override fun onSingleTapUp(e: MotionEvent): Boolean {
            // Calls the Autofocus Native Method
            cameraPerformAutoFocus()

            // After triggering a focus event wait 2 seconds
            // before restoring continuous autofocus
            Timer("RestoreAutoFocus", false).schedule(2000) {
                cameraRestoreAutoFocus()
            }

            return true
        }

        override fun onDoubleTap(e: MotionEvent): Boolean {
            onBackPressed()
            return true
        }
    }


    private fun makeFullScreen() {
        // Make the Activity completely full screen
        (View.SYSTEM_UI_FLAG_LOW_PROFILE or
                View.SYSTEM_UI_FLAG_FULLSCREEN or
                View.SYSTEM_UI_FLAG_LAYOUT_STABLE or
                View.SYSTEM_UI_FLAG_IMMERSIVE_STICKY or
                View.SYSTEM_UI_FLAG_LAYOUT_HIDE_NAVIGATION or
                View.SYSTEM_UI_FLAG_HIDE_NAVIGATION).also { mGLView.systemUiVisibility = it }
        supportActionBar?.setDisplayHomeAsUpEnabled(true)
    }


    private suspend fun initializeVuforia() {
        return withContext(Dispatchers.Default) {
            initAR(this@VuforiaActivity, this@VuforiaActivity.assets, mTarget)
        }
    }


    private fun runtimePermissionsGranted(): Boolean {
        var result = true
        for (permission in REQUIRED_PERMISSIONS) {
            result = result && ContextCompat.checkSelfPermission(this, permission) == PackageManager.PERMISSION_GRANTED
        }
        return result
    }


    @Suppress("unused")
    private fun presentError(message: String) {
        val builder: AlertDialog.Builder = this.let {
            AlertDialog.Builder(it)
        }

        builder.setMessage(message)
        builder.setTitle(R.string.error_dialog_title)
        builder.setPositiveButton(R.string.ok
        ) { _, _ ->
            stopAR()
            deinitAR()
            this@VuforiaActivity.finish()
        }

        // This is called from another coroutine not on the Main thread
        // Showing the UI needs to be on the main thread
        GlobalScope.launch(Dispatchers.Main) {
            val dialog: AlertDialog = builder.create()
            dialog.show()
        }
    }


    @Suppress("unused")
    private fun initDone() {
        mVuforiaStarted = startAR()
        if (!mVuforiaStarted) {
            Log.e("VuforiaSample", "Failed to start AR")
        }
        // Show the GLView
        GlobalScope.launch(Dispatchers.Main) {
            mGLView.visibility = View.VISIBLE
        }
    }


    // GLSurfaceView.Renderer methods
    override fun onSurfaceCreated(unused: GL10, config: EGLConfig) {
        initRendering()
    }


    override fun onDrawFrame(unused: GL10) {
        if (mVuforiaStarted) {

            if (mSurfaceChanged || mWindowDisplayRotation != windowManager.defaultDisplay.rotation) {
                mSurfaceChanged = false
                mWindowDisplayRotation = windowManager.defaultDisplay.rotation

                // Pass rendering parameters to Vuforia Engine
                configureRendering(mWidth, mHeight, resources.configuration.orientation, mWindowDisplayRotation)
            }

            // OpenGL rendering of Video Background and augmentations is implemented in native code
            val didRender = renderFrame()
            if (didRender && mProgressIndicatorLayout?.visibility != View.GONE) {
                GlobalScope.launch(Dispatchers.Main) {
                    mProgressIndicatorLayout?.visibility = View.GONE
                }
            }
        }
    }


    override fun onSurfaceChanged(unused: GL10, width: Int, height: Int) {
        // Store values for later use
        mWidth = width
        mHeight = height

        // Re-load textures in case they got destroyed
        val astronautTexture = Texture.loadTextureFromApk("plane.jpg", assets)
        val planeTexture = Texture.loadTextureFromApk("plane.jpg", assets)
        val landerTexture = Texture.loadTextureFromApk("VikingLander.jpg", assets)
        if (astronautTexture != null && planeTexture != null && landerTexture != null) {
            setTextures(
                astronautTexture.width, astronautTexture.height, astronautTexture.data!!,
                planeTexture.width, planeTexture.height, planeTexture.data!!,
                landerTexture.width, landerTexture.height, landerTexture.data!!
            )
        } else {
            Log.e("VuforiaSample", "Failed to load astronaut or lander texture")
        }

        // Update flag to tell us we need to update Vuforia configuration
        mSurfaceChanged = true
    }


    // SurfaceHolder.Callback
    override fun surfaceCreated(var1: SurfaceHolder) {}


    override fun surfaceChanged(var1: SurfaceHolder, var2: Int, var3: Int, var4: Int) {}


    override fun surfaceDestroyed(var1: SurfaceHolder) {
        deinitRendering()
    }


    companion object {
        external fun getImageTargetId() : Int
        external fun getModelTargetId() : Int
    }
}
