package bf.cloud.datasource;

import java.io.IOException;
import java.util.List;

import android.graphics.ImageFormat;
import android.graphics.SurfaceTexture;
import android.hardware.Camera;
import android.hardware.Camera.ErrorCallback;
import android.hardware.Camera.Parameters;
import android.hardware.Camera.PreviewCallback;
import android.hardware.Camera.Size;
import android.util.Log;

/**
 * 通过摄像头录制视频
 * 
 * @author wangtonggui
 * 
 */
public class CameraRecorder {
	private String TAG = CameraRecorder.class.getSimpleName();
	private Camera mCamera = null;
	private SurfaceTexture mSurfaceTexture = null;
	private Parameters parameters;
	private int bufferSize;
	private byte[] gBuffer;
	private OnCollectingVideoDataCallback mCallback = null;
	private boolean mIsCollecting = false;
	private int mCameraIndex = -1;
	private long mTimeStamp = 0;
	private long mBaseTimeStamp = -1;

	/**
	 * 
	 * @param cameraIndex
	 *            相机索引
	 */
	public CameraRecorder(int cameraIndex) {
		mSurfaceTexture = new SurfaceTexture(10);
		if (cameraIndex >= 0 && cameraIndex <= 1)
			mCameraIndex = cameraIndex;
		else
			Log.d(TAG, "cameraIndex is error");
	}

	/**
	 * 设置需要预览的表面
	 * 
	 * @param st
	 * 需要需要的表面
	 */
	public void setSurfaceTexture(SurfaceTexture st) {
		mSurfaceTexture = st;
	}

	/**
	 * 设置码率
	 * 
	 * @param bitrate
	 */
	public void setBitRate(int bitrate) {

	}

	/**
	 * 设置帧率
	 * 
	 * @param framerate
	 */
	public void setFrameRate(int framerate) {

	}

	/**
	 * 设置关键帧间隔
	 * 
	 * @param interval 单位为秒
	 */
	public void setIFrameInterval(int interval) {

	}

	/**
	 * 对摄像头参数、回调进行设置
	 */
	private void init() {
		mCamera.setDisplayOrientation(90);
		try {
			mCamera.setPreviewTexture(mSurfaceTexture);
		} catch (IOException e) {
			Log.d(TAG, e.getMessage());
		}

		parameters = mCamera.getParameters();
		List<Size> preSize = parameters.getSupportedPreviewSizes();
		int previewWidth = 640;// preSize.get(0).width;
		int previewHeight = 480;// preSize.get(0).height;

		Log.d(TAG, "preSize = " + preSize.size() + "/preview = " + previewWidth
				+ "..." + previewHeight);
		parameters.setPreviewSize(previewWidth, previewHeight);
		parameters.setPreviewFormat(ImageFormat.NV21);
		mCamera.setParameters(parameters);
		bufferSize = previewWidth * previewHeight;
		bufferSize = bufferSize
				* ImageFormat.getBitsPerPixel(parameters.getPreviewFormat())
				/ 8;
		gBuffer = new byte[bufferSize];
		mCamera.setPreviewCallbackWithBuffer(new PreviewCallback() {

			@Override
			public void onPreviewFrame(byte[] data, Camera camera) {
				camera.addCallbackBuffer(gBuffer);
				try {
					mSurfaceTexture.updateTexImage();
				} catch (Exception e) {
					Log.d(TAG, e.getMessage());
				}
				if (mBaseTimeStamp < 0) {
					mBaseTimeStamp = mSurfaceTexture.getTimestamp() / 1000;
				}
				mTimeStamp = mSurfaceTexture.getTimestamp() / 1000
						- mBaseTimeStamp;
				// 回调拿原始视频数据
				if (mCallback != null) {
					mCallback.onData(data, mTimeStamp / 1000);
				}
			}
		});
		mCamera.setErrorCallback(new ErrorCallback() {

			@Override
			public void onError(int error, Camera camera) {
				Log.d(TAG, "onerror");
			}
		});

		int[] frameRate = new int[2];
		parameters.getPreviewFpsRange(frameRate);
		int averageFrameRate = (frameRate[0] + frameRate[1]) / 2;
		Log.d(TAG, "Camera getPreviewFpsRange = " + averageFrameRate);
	}

	/**
	 * 通过测试，在stop的时候，如果仅仅调用stopPreview，并不能用startPreview重新开启。
	 * 所以这里不得已而选择先release，再启动
	 */
	public void open() {
		if (mIsCollecting)
			return;
		if (mCamera == null)
			mCamera = Camera.open(mCameraIndex);
		init();
		Log.d(TAG, "startCollecting");
		mCamera.stopPreview();
		Log.d(TAG, gBuffer.length + "");
		mCamera.addCallbackBuffer(gBuffer);
		mCamera.startPreview();
		mIsCollecting = true;
	}

	/**
	 * 停止预览，并释放摄像头
	 */
	public void close() {
		if (!mIsCollecting)
			return;
		try {
			mCamera.stopPreview();
			mCamera.release();
			mCamera = null;
			mIsCollecting = false;
		} catch (Exception e) {
			Log.d(TAG, e.getMessage());
		}
	}

	/**
	 * 释放摄像头，只有调用release，其他程序才可以使用
	 */
	public void release() {
		if (mCamera != null)
			mCamera.release();
	}

	/**
	 * 设置获取摄像头源数据回调，暂时先不公开，所以设置成为private
	 * 
	 * @param cb 回调
	 */
	public void setDataCallback(OnCollectingVideoDataCallback cb) {
		mCallback = cb;
	}

	public static interface OnCollectingVideoDataCallback {
		void onData(byte[] data, long timeStamp);
	}
}
