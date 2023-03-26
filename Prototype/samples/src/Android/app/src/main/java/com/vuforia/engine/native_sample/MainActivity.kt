/*===============================================================================
Copyright (c) 2020 PTC Inc. All Rights Reserved.

Vuforia is a trademark of PTC Inc., registered in the United States and other
countries.
===============================================================================*/

package com.vuforia.engine.native_sample

import android.content.Intent
import android.os.Bundle
import android.view.View
import androidx.appcompat.app.AppCompatActivity
import kotlinx.android.synthetic.main.activity_main.*


/**
 * The MainActivity presents a simple choice for the user to select Image Targets or Model Targets.
 */
class MainActivity : AppCompatActivity() {

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContentView(R.layout.activity_main)
    }


    fun goToActivity(view: View) {
        if (view.id == btn_image_target.id || view.id == btn_model_target.id) {

            val intent = Intent(
                this@MainActivity,
                VuforiaActivity::class.java
            )
            if (view.id == btn_image_target.id) {
                intent.putExtra("Target", VuforiaActivity.getImageTargetId())
            } else {
                intent.putExtra("Target", VuforiaActivity.getModelTargetId())
            }

            startActivity(intent)
        }
    }


    companion object {

        // Used to load the 'VuforiaSample' library on application startup.
        init {
            System.loadLibrary("VuforiaSample")
        }
    }
}
