/*===============================================================================
Copyright (c) 2020 PTC Inc. All Rights Reserved.

Vuforia is a trademark of PTC Inc., registered in the United States and other
countries.
===============================================================================*/

package com.vuforia.engine.native_sample

import android.content.res.AssetManager
import android.graphics.BitmapFactory
import android.util.Log
import java.io.BufferedInputStream
import java.io.IOException
import java.io.InputStream
import java.nio.ByteBuffer
import java.nio.ByteOrder


/**
 * Utility class to read image data to be loaded into a texture.
 */
class Texture {

    /// The width of the texture.
    var width = 0
    /// The height of the texture.
    var height = 0
    /// The number of channels e.g. 4 for RGBA
    var channels = 4
    /// The pixel data.
    var data: ByteBuffer? = null


    companion object {

        @JvmStatic
        fun loadTextureFromApk(
            fileName: String,
            assets: AssetManager
        ): Texture? {
            val inputStream: InputStream
            return try {
                inputStream = assets.open(fileName, AssetManager.ACCESS_BUFFER)
                val bufferedStream = BufferedInputStream(
                    inputStream
                )
                val bitMap = BitmapFactory.decodeStream(bufferedStream)
                val data = IntArray(bitMap.width * bitMap.height)
                bitMap.getPixels(
                    data, 0, bitMap.width, 0, 0,
                    bitMap.width, bitMap.height
                )
                loadTextureFromIntBuffer(
                    data, bitMap.width,
                    bitMap.height
                );


            } catch (e: IOException) {
                Log.i("Texture", "Failed to load texture '$fileName' from APK")
                Log.i("Texture", e.message.toString())
                null
            }
        }


        @JvmStatic
        private fun loadTextureFromIntBuffer(
            data: IntArray, width: Int,
            height: Int
        ): Texture? {
            // Convert:
            val numPixels = width * height
            val dataBytes = ByteArray(numPixels * 4)
            for (p in 0 until numPixels) {
                val colour = data[p]
                dataBytes[p * 4] = (colour ushr 16).toByte() // R
                dataBytes[p * 4 + 1] = (colour ushr 8).toByte() // G
                dataBytes[p * 4 + 2] = colour.toByte() // B
                dataBytes[p * 4 + 3] = (colour ushr 24).toByte() // A
            }
            val texture: Texture = Texture()
            texture.width = width
            texture.height = height
            texture.data = ByteBuffer.allocateDirect(dataBytes.size).order(
                ByteOrder.nativeOrder()
            )
            val rowSize: Int = texture.width * texture.channels
            for (r in 0 until texture.height) texture.data?.put(
                dataBytes, rowSize * (texture.height - 1 - r),
                rowSize
            )
            texture.data?.rewind()
            return texture
        }
    }
}
