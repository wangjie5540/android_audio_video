package bf.cloud.datasource;

import java.io.File;
import java.io.FileOutputStream;
import java.io.InputStream;

import android.content.Context;
import android.os.Environment;

public class Utils {

	public static String getAppDataPath(Context context) {
		return context.getFilesDir() + File.separator;
	}

	public static String getSdCardPath() {
		return Environment.getExternalStorageDirectory().getAbsolutePath() + File.separator;
	}

	public static boolean isSdCardAvailable() {
		return Environment.getExternalStorageState().equals(Environment.MEDIA_MOUNTED);
	}

	public static boolean fileExists(String pathName) {
		return (new File(pathName)).exists();
	}

	public static boolean deleteFile(String pathName) {
		return (new File(pathName)).delete();
	}

	public static int dip2px(Context context, float dpValue) {
		final float scale = context.getResources().getDisplayMetrics().density;
		return (int) (dpValue * scale + 0.5f);
	}

	public static int px2dip(Context context, float pxValue) {
		final float scale = context.getResources().getDisplayMetrics().density;
		return (int) (pxValue / scale + 0.5f);
	}

	public static String pathWithSlash(String path) {
		final char SLASH = File.separatorChar;
		String result = path;

		if (result.isEmpty()) {
			result = String.valueOf(SLASH);
		} else {
			if (result.charAt(result.length() - 1) != SLASH) {
				result += SLASH;
			}
		}

		return result;
	}

	public static String pathWithoutSlash(String path) {
		final char SLASH = File.separatorChar;
		String result = path;

		if (!result.isEmpty()) {
			if (result.charAt(result.length() - 1) == SLASH) {
				result = result.substring(0, result.length() - 1);
			}
		}

		return result;
	}

	public static void copyAssetsToSdCard(Context context, String assetsSubPath, String sdCardPath) {
		try {
			assetsSubPath = pathWithoutSlash(assetsSubPath);
			sdCardPath = pathWithoutSlash(sdCardPath);

	        String fileNames[] = context.getAssets().list(assetsSubPath);
	        if (fileNames.length > 0) { // if is directory
	            File file = new File(sdCardPath);
	            file.mkdirs();
	            for (String fileName : fileNames) {
	            	copyAssetsToSdCard(context, assetsSubPath + "/" + fileName, sdCardPath + "/" + fileName);
	            }
	        } else { // if is file
	            InputStream is = context.getAssets().open(assetsSubPath);
	            FileOutputStream os = new FileOutputStream(new File(sdCardPath));

	            byte[] buffer = new byte[1024];
	            int byteCount = 0;

	            while ((byteCount = is.read(buffer)) != -1) {
	                os.write(buffer, 0, byteCount);
	            }

	            os.flush();
	            is.close();
	            os.close();
	        }
	    } catch (Exception e) {
	        e.printStackTrace();
	    }
	}

}
