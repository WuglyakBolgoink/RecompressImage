#include <jni.h>
#include <android/log.h>
#include <android/bitmap.h>

#include <stdio.h>
#include <stdlib.h>

#define  LOG_TAG    "native_rotate"
#define  LOGI(...)  __android_log_print(ANDROID_LOG_INFO,LOG_TAG,__VA_ARGS__)
#define  LOGE(...)  __android_log_print(ANDROID_LOG_ERROR,LOG_TAG,__VA_ARGS__)
#define  LOGD(...)  __android_log_print(ANDROID_LOG_DEBUG,LOG_TAG,__VA_ARGS__)

extern "C" {
	JNIEXPORT jobject JNICALL Java_de_cyberkatze_phonegap_plugin_recompressimage_RecompressImage_rotateBitmap(JNIEnv * env, jobject obj, jobject bitmap, jint rotation)
	{
		 //
		  //getting bitmap info:
		  //

		  LOGD("reading bitmap info...");

		  AndroidBitmapInfo info;

		  int ret;

		  if ((ret = AndroidBitmap_getInfo(env, bitmap, &info)) < 0)
			{
			LOGE("AndroidBitmap_getInfo() failed ! error=%d", ret);
			return NULL;
			}

		  LOGD("width:%d height:%d stride:%d", info.width, info.height, info.stride);

		  if (info.format != ANDROID_BITMAP_FORMAT_RGBA_8888)
			{
			LOGE("Bitmap format is not RGBA_8888! Format = %d", info.format);
			return NULL;
			}

		  //
		  //read pixels of bitmap into native memory :
		  //

		  LOGD("reading bitmap pixels...");

		  void* bitmapPixels;

		  if ((ret = AndroidBitmap_lockPixels(env, bitmap, &bitmapPixels)) < 0)
			{
			LOGE("AndroidBitmap_lockPixels() failed ! error=%d", ret);
			return NULL;
			}

		  uint32_t* src = (uint32_t*) bitmapPixels;
		  uint32_t* tempPixels = new uint32_t[info.height * info.width];

		  int stride = info.stride;
		  int pixelsCount = info.height * info.width;

		  memcpy(tempPixels, src, sizeof(uint32_t) * pixelsCount);
		  AndroidBitmap_unlockPixels(env, bitmap);

		  //
		  //recycle bitmap - using bitmap.recycle()
		  //

		  LOGD("recycling bitmap...");

		  jclass bitmapCls = env->GetObjectClass(bitmap);
		  jmethodID recycleFunction = env->GetMethodID(bitmapCls, "recycle", "()V");

		  if (recycleFunction == 0)
			{
			LOGE("error recycling!");
			return NULL;
			}

		  env->CallVoidMethod(bitmap, recycleFunction);

		  //
		  //creating a new bitmap to put the pixels into it - using Bitmap Bitmap.createBitmap (int width, int height, Bitmap.Config config) :
		  //

		  LOGD("creating new bitmap...");

		  jmethodID createBitmapFunction = env->GetStaticMethodID(bitmapCls, "createBitmap", "(IILandroid/graphics/Bitmap$Config;)Landroid/graphics/Bitmap;");
		  jstring configName = env->NewStringUTF("ARGB_8888");
		  jclass bitmapConfigClass = env->FindClass("android/graphics/Bitmap$Config");
		  jmethodID valueOfBitmapConfigFunction = env->GetStaticMethodID(bitmapConfigClass, "valueOf", "(Ljava/lang/String;)Landroid/graphics/Bitmap$Config;");
		  jobject bitmapConfig = env->CallStaticObjectMethod(bitmapConfigClass, valueOfBitmapConfigFunction, configName);

		  jobject newBitmap;

		  if (rotation == 180)
		    {
			  newBitmap = env->CallStaticObjectMethod(bitmapCls, createBitmapFunction, info.width, info.height, bitmapConfig);
		    } else {
			  newBitmap = env->CallStaticObjectMethod(bitmapCls, createBitmapFunction, info.height, info.width, bitmapConfig);
		    }

		  //
		  // putting the pixels into the new bitmap:
		  //

		  if ((ret = AndroidBitmap_lockPixels(env, newBitmap, &bitmapPixels)) < 0)
			{
			LOGE("AndroidBitmap_lockPixels() failed ! error=%d", ret);
			return NULL;
			}

		  uint32_t* newBitmapPixels = (uint32_t*) bitmapPixels;
		  int whereToPut = 0;

		  if (rotation == 270){
			  for (int x = info.width - 1; x >= 0; --x)
				  for (int y = 0; y < info.height; ++y)
				  {
					  uint32_t pixel = tempPixels[info.width * y + x];
					  newBitmapPixels[whereToPut++] = pixel;
				  }
		  } else if (rotation == 90){
			  for (int x = 0; x < info.width; ++x)
				  for (int y = info.height -1; y >= 0; --y)
				  {
					  uint32_t pixel = tempPixels[info.width * y + x];
					  newBitmapPixels[whereToPut++] = pixel;
				  }
		  } else if (rotation == 180){
			  for (int x = info.width * info.height - 1; x >= 0; --x)
				  {
					  uint32_t pixel = tempPixels[x];
					  newBitmapPixels[whereToPut++] = pixel;
				  }
		  }

		  AndroidBitmap_unlockPixels(env, newBitmap);

		  //
		  // freeing the native memory used to store the pixels
		  //

		  delete[] tempPixels;
		  return newBitmap;
	}
};
