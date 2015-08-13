package com.example.testvideocollect;

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
import android.view.TextureView;

public class CameraRecorder {
	private String TAG = CameraRecorder.class.getSimpleName();
	private Camera mCamera = null;
	private SurfaceTexture mSurfaceTexture = null;
	private Parameters parameters;
	private int bufferSize;
	private byte[] gBuffer;
	private OnCollectingVideoDataCallback mCallback = null;
	private boolean mIsCollecting = false;
	private TextureView mPreview = null;

	public CameraRecorder() {
		mSurfaceTexture = new SurfaceTexture(10);
	}
	
	public void setSurfaceTexture(SurfaceTexture st) {
		mSurfaceTexture = st;
	}

	private void init() {
		if (mCamera == null)
			mCamera = Camera.open();
		mCamera.setDisplayOrientation(90);
		try {
			mCamera.setPreviewTexture(mSurfaceTexture);
		} catch (IOException e) {
			Log.d(TAG, e.getMessage());
		}

		parameters = mCamera.getParameters();
		List<Size> preSize = parameters.getSupportedPreviewSizes();
		int previewWidth = preSize.get(0).width;
		int previewHeight = preSize.get(0).height;
		Log.d(TAG, "preSize = " + preSize.size() + "/preview = " + previewWidth
				+ "..." + previewHeight);
		parameters.setPreviewSize(previewWidth, previewHeight);
		mCamera.setParameters(parameters);
		bufferSize = previewWidth * previewHeight;
		bufferSize = 2 * bufferSize
				* ImageFormat.getBitsPerPixel(parameters.getPreviewFormat())
				/ 8;
		gBuffer = new byte[bufferSize];
		mCamera.setPreviewCallbackWithBuffer(new PreviewCallback() {

			@Override
			public void onPreviewFrame(byte[] data, Camera camera) {
				camera.addCallbackBuffer(gBuffer);
				try {
					mSurfaceTexture.updateTexImage();
					// Log.d(TAG, mSurfaceTexture.getTimestamp() + "");
				} catch (Exception e) {
					Log.d(TAG, e.getMessage());
				}

				// 回调拿原始视频数据
				if (mCallback != null) {
					mCallback.onData(data,
							mSurfaceTexture.getTimestamp() / 1000000);
				}

				// 在preview上进行展示
				if (mPreview != null) {

				}
			}
		});
		mCamera.setErrorCallback(new ErrorCallback() {

			@Override
			public void onError(int error, Camera camera) {
				Log.d(TAG, "onerror");
			}
		});
	}

	/**
	 * 通过测试，在stop的时候，如果仅仅调用stopPreview，并不能用startPreview重新开启。
	 * 所以这里不得已而选择先release，再启动
	 */
	public void startCollecting() {
		if (mIsCollecting)
			return;
		init();
		Log.d(TAG, "startCollecting");
		mCamera.stopPreview();
		Log.d(TAG, gBuffer.length + "");
		mCamera.addCallbackBuffer(gBuffer);
		mCamera.startPreview();
		mIsCollecting = true;
	}

	public void stopCollecting() {
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

	public void release() {
		if (mCamera != null)
			mCamera.release();
	}

	public void setOnCollectingVideoDataCallback(
			OnCollectingVideoDataCallback cb) {
		mCallback = cb;
	}

	public static interface OnCollectingVideoDataCallback {
		void onData(byte[] data, long timeStamp);
	}

	public void setPreview(TextureView tv) {
		mPreview = tv;
	}
}
