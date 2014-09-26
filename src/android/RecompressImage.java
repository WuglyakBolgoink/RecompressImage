package de.cyberkatze.phonegap.plugin.recompressimage;

import java.io.File;
import java.io.FileOutputStream;
import java.io.IOException;
import java.net.URI;

import org.apache.cordova.CallbackContext;
import org.apache.cordova.CordovaPlugin;
import org.json.JSONArray;
import org.json.JSONObject;


import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.media.ExifInterface;
import android.os.Environment;
import android.util.Log;



public class RecompressImage extends CordovaPlugin {

    public static final String TAG = "PluginReconvertImage";
    public static final String ACTION_NDC_RECONVERT_IMAGE = "recompressImage";


    static {System.loadLibrary("nDCrotate");}

    private native Bitmap rotateBitmap(Bitmap bitmap, int rotation);


    @Override
    public boolean execute(String action, JSONArray args, CallbackContext callbackContext) {

        try {
            if (ACTION_NDC_RECONVERT_IMAGE.equals(action)) {
                JSONObject arg_object = args.getJSONObject(0);

                Log.i(TAG, "imagePath: " + arg_object.getString("imagePath"));
                Log.i(TAG, "imagePath: " + arg_object.getInt("compressionLvl"));

                if (processPicture(arg_object.getString("imagePath"),arg_object.getInt("compressionLvl"))) {
                    callbackContext.success();
                    return true;
                } else {
                    callbackContext.error("Error on processPicture");
                    return false;
                }

            }
            callbackContext.error("Invalid action");
            return false;
        } catch(Exception e) {
            System.err.println("Exception: " + e.getMessage());
            callbackContext.error(e.getMessage());
            return false;
        }
    }//execute

    /**
     * Change image rotation and compress the image with compression level.
     * @param picturePath String
     * @param compressionLevel integer
     * @return boolean
     */
    public boolean processPicture(String picturePath, int compressionLevel){
        boolean success = false;
        String path = picturePath;

        //path = picturePath.replace("file://", "");
        //imagePath: /Android/data/de.neobaum.ndc.android/ndcImages/foto20140921210249726_20140921210249726_1.jpg
        
        //@see: http://stackoverflow.com/a/7726224
        if (path.toLowerCase().startsWith("file://"))
        {
           // Selected file/directory path is below
           path = (new File(URI.create(path))).getAbsolutePath();
        }
        Log.d(TAG, "Environment.getExternalStorageDirectory(): " + Environment.getExternalStorageDirectory());
        Log.d(TAG, "path: " + path);

        File file = new File(path);
        if (file.exists()) {
        	Log.i(TAG, "file exist: " + file.getAbsolutePath());
        } else {
        	Log.i(TAG, "file not exist");
        	return false;
        }

        BitmapFactory.Options opts = new BitmapFactory.Options();
        Bitmap fotoBitmap = BitmapFactory.decodeFile(path, opts);

        if (fotoBitmap != null){
        	Log.d(TAG, "fotoBitmap exist!");
            File docPageFile = new File(path);
            FileOutputStream outputStream = null;

            try {
                int rotation = getRotation(path);
                outputStream = new FileOutputStream(docPageFile);

                if (rotation != 0) {	//Korrektur der Orientierung notwendig
                    fotoBitmap = rotateBitmap(fotoBitmap, 90);
                }

                fotoBitmap.compress(Bitmap.CompressFormat.JPEG, compressionLevel, outputStream);

                success = true;

            } catch (Exception e) {
                success = false;
                e.printStackTrace();
            } finally {
                try {
                    if (outputStream != null) {
                        outputStream.flush();
                        outputStream.close();
                    }
                } catch (IOException e) {
                    success = false;
                    e.printStackTrace();
                }
            }
            fotoBitmap.recycle();
            fotoBitmap = null;
            System.gc();
        } else {
        	Log.e(TAG, "fotoBitmap error!");
        }

        return success;
    }//processPicture

    /**
     * Get rotation.
     * @param pfad String
     * @return integer
     * @throws IOException
     */
    private int getRotation(String pfad)throws IOException{
        ExifInterface imgExif = new ExifInterface(pfad);
        int imageOrientation = Integer.valueOf(imgExif.getAttribute(ExifInterface.TAG_ORIENTATION));
        imgExif = null;

        int rotation = 0;

        switch(imageOrientation){
            case 1:
                rotation = 0;
                break;
            case 8:
                rotation = 270;
                break;
            case 3:
                rotation = 180;
                break;
            case 6:
                rotation = 90;
                break;
            default:
                rotation = 0;
            }

        return rotation;
    }//getRotation


}
