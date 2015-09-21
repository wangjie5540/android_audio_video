package bf.cloud.datasource;

import android.util.Log;
import bf.cloud.datasource.AudioRecorder.OnCollectingAudioDataCallback;
import bf.cloud.datasource.CameraRecorder;
import bf.cloud.datasource.CameraRecorder.OnCollectingVideoDataCallback;
import bf.cloud.datasource.UploadCenter.Listener;

public class BFRecorder {
	private final String TAG = BFRecorder.class.getSimpleName();
	private static BFRecorder mBfRecorder = null;
	private static CameraRecorder mCameraRecorder = null;
	private static AudioRecorder mAudioRecorder = null;
	private CameraPreview mCameraPreview = null;
	// private AudioRecorder mAudioRecorder = null;
	private String mChannelId = null;
	private OnCollectingVideoDataCallback mVideoDataCallback = new OnCollectingVideoDataCallback() {

		@Override
		public void onData(byte[] data, long timeStamp) {
			mUploadCenter.nativeUploadVideoSample(data, data.length, timeStamp);
		}
	};
	private OnCollectingAudioDataCallback mAudioDataCallback = new OnCollectingAudioDataCallback(){

		@Override
		public void onAudioData(byte[] data, int size, long timestamp) {
			mUploadCenter.nativeUploadAudioSample(data, size, timestamp);
		}
	};
	private UploadCenter mUploadCenter = null;

	private BFRecorder() {
		mCameraRecorder = new CameraRecorder(0);
		mCameraRecorder.setDataCallback(mVideoDataCallback);
		mAudioRecorder = new AudioRecorder();
		mAudioRecorder.setListener(mAudioDataCallback);
		
		mUploadCenter = UploadCenter.getInstance();
		String logDir = Utils.getSdCardPath() + "collector/log";
		mUploadCenter.nativeInit(logDir, new Listener() {
			
			@Override
			public void onError(int errorCode) {
				Log.d(TAG, "onError");
			}
		});
	}

	public static BFRecorder getInstance() {
		if (mBfRecorder == null || mCameraRecorder == null || mAudioRecorder == null) {
			mBfRecorder = new BFRecorder();
		}
		return mBfRecorder;
	}

	public void start(String channelId) {
		mChannelId = channelId;
		int videoWidth = 640;
		int videoHeight = 480;
		int videoFrameRate = 16;
		int videoDataRate = 1024*300;
		int audioSampleRate = AudioRecorder.SMAPLE_RATE;
		int audioDataRate = AudioRecorder.DATA_RATE;
		int audioChannelCount = mAudioRecorder.getChannelCount();
		int audioSampleSize = mAudioRecorder.getSampleSize();
		mUploadCenter.nativeOpen(channelId, videoWidth, videoHeight,
				videoFrameRate, videoDataRate, audioSampleRate, audioDataRate,
				audioChannelCount, audioSampleSize);
		mCameraRecorder.open();
		mAudioRecorder.open();
	}

	public void stop() {
		mCameraRecorder.close();
		mAudioRecorder.close();
	}

	public void release() {
		mCameraRecorder.release();
		mCameraRecorder = null;
		mAudioRecorder.release();
		mAudioRecorder = null;
		mUploadCenter.nativeUninit();
	}

	public void setPreview(CameraPreview cp) {
		mCameraPreview = cp;
		mCameraPreview.setCameraRecorder(mCameraRecorder);
	}
}
