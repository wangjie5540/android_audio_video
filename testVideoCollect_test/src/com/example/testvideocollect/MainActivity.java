package com.example.testvideocollect;

import android.app.Activity;
import android.content.pm.ActivityInfo;
import android.graphics.Bitmap;
import android.graphics.Canvas;
import android.graphics.RectF;
import android.os.Bundle;
import android.view.SurfaceHolder;
import android.view.SurfaceView;
import android.view.View;
import android.view.View.OnClickListener;


public class MainActivity extends Activity {

    private CameraRecorder cv;
	private SurfaceView sf;
	private SurfaceHolder gHolder;
	private int width = 1920;
	private int height = 1080;
	private CameraPreview cp;
	
	@Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);
        findViewById(R.id.startCollecting).setOnClickListener(new OnClickListener() {
			
			@Override
			public void onClick(View v) {
				setRequestedOrientation(ActivityInfo.SCREEN_ORIENTATION_LANDSCAPE);
				cv.startCollecting();
			}
		});
        findViewById(R.id.stopCollecting).setOnClickListener(new OnClickListener() {
			
			@Override
			public void onClick(View v) {
				setRequestedOrientation(ActivityInfo.SCREEN_ORIENTATION_PORTRAIT);
				cv.stopCollecting();
			}
		});
        sf = (SurfaceView)findViewById(R.id.sf);
        gHolder = sf.getHolder();
    }


    @Override
    protected void onStart() {
    	cv = new CameraRecorder();
    	cv.setOnCollectingVideoDataCallback(new CameraRecorder.OnCollectingVideoDataCallback() {
			private Bitmap gBitmap = Bitmap.createBitmap(width, height, Bitmap.Config.ARGB_8888);

			@Override
			public void onData(byte[] data, long timeStamp) {
				
				int[] textureBuffer = new int[width*height];
				for(int i=0;i<textureBuffer.length;i++)
				{
					int R=data[i]<<16;
					int G=data[i]<<8;
					int B=data[i];
					textureBuffer[i]=0xff000000|(R+G+B);
				}
				gBitmap.setPixels(textureBuffer, 0, width, 0, 0, width, height);
				synchronized (gHolder){		
					try {
						Canvas canvas = gHolder.lockCanvas();
						RectF gRect = new RectF(0, 0, width, height);
						canvas.drawBitmap(gBitmap, null,gRect , null);
						gHolder.unlockCanvasAndPost(canvas);
					} catch (Exception e) {
						
					}					
					
				}
			}
		});
    	
    	cp = (CameraPreview) findViewById(R.id.cp);
    	cp.setCameraRecorder(cv);
    	super.onStart();
    }
    
    @Override
    protected void onStop() {
    	try {
			cv.stopCollecting();
		} catch (Throwable e) {
			e.printStackTrace();
		}
    	cv = null;
    	super.onStop();
    }
}
