package com.example.testvideocollect;


import android.content.Context;
import android.graphics.SurfaceTexture;
import android.util.AttributeSet;
import android.util.Log;
import android.view.TextureView;

public class CameraPreview extends TextureView implements TextureView.SurfaceTextureListener{
	private final String TAG = CameraPreview.class.getSimpleName();
	private CameraRecorder mCameraRecorder = null;
	public CameraPreview(Context context, CameraRecorder cr) {
		super(context);
		mCameraRecorder = cr;
		setSurfaceTextureListener(this);
		// TODO Auto-generated constructor stub
	}
	
	public CameraPreview(Context context, AttributeSet attrs) {
		super(context, attrs);
		setSurfaceTextureListener(this);
	}
	
	public CameraPreview(Context context, AttributeSet attrs,
			int defStyleAttr) {
		super(context, attrs, defStyleAttr);
	}
	
	public void setCameraRecorder(CameraRecorder cr){
		mCameraRecorder = cr;
	}
	
	public void startPreview(){
		if (mCameraRecorder != null){
			mCameraRecorder.startCollecting();
		}else{
			Log.d(TAG, "mCameraRecorder is null");
		}
	}
	
	@Override
	public void onSurfaceTextureAvailable(SurfaceTexture surface, int width,
			int height) {
		Log.d(TAG, "onSurfaceTextureAvailable");
		startPreview();
	}

	@Override
	public void onSurfaceTextureSizeChanged(SurfaceTexture surface, int width,
			int height) {
		// TODO Auto-generated method stub
		
	}

	@Override
	public boolean onSurfaceTextureDestroyed(SurfaceTexture surface) {
		// TODO Auto-generated method stub
		return false;
	}

	@Override
	public void onSurfaceTextureUpdated(SurfaceTexture surface) {
		// TODO Auto-generated method stub
	}
}
