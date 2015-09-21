package bf.cloud.datasource;

import android.media.AudioFormat;
import android.media.AudioRecord;
import android.media.MediaRecorder;
import android.util.Log;


public class AudioRecorder{
	private final String TAG = AudioRecorder.class.getSimpleName();
	public final static int SMAPLE_RATE = 44100;
	public final static int DATA_RATE = 64;
	public final static int CHANNEL_CONFIG = AudioFormat.CHANNEL_IN_MONO;
	public final static int AUDIO_FORMAT = AudioFormat.ENCODING_PCM_16BIT;
	private long mBaseTimeStamp = -1;
	
	private AudioRecord mAudioRecord;
	private int mBufferSize;

	private boolean mStarted;
	private OnCollectingAudioDataCallback mListener;
	
	public interface OnCollectingAudioDataCallback {
		void onAudioData(byte[] data, int size, long timeStamp);
	}

	public AudioRecorder() {
		Log.d(TAG, "new AudioRecorder");
		mBufferSize = AudioRecord.getMinBufferSize(SMAPLE_RATE, CHANNEL_CONFIG, AUDIO_FORMAT);
		mAudioRecord = new AudioRecord(
				MediaRecorder.AudioSource.MIC,
				SMAPLE_RATE, CHANNEL_CONFIG, AUDIO_FORMAT,
				mBufferSize);
	}
	
	public void open() {
		if (!mStarted) {
			new Thread(new RecordTask()).start();
		}
	}
	
	public void close() {
		if (mStarted) {
			mStarted = false;
			if (mAudioRecord != null) {
				mAudioRecord.stop();
			}
		}
	}
	
	public int getSampleSize() {
		Log.d(TAG, "getSampleSize AUDIO_FORMAT = " + AUDIO_FORMAT);
		switch (AUDIO_FORMAT) {
		case AudioFormat.ENCODING_PCM_16BIT:
			return 16;
		case AudioFormat.ENCODING_PCM_8BIT:
			return 8;
		default:
			return 8;
		}
	}
	
	public int getChannelCount() {
		Log.d(TAG, "getChannelCount CHANNEL_CONFIG = " + CHANNEL_CONFIG);
		switch (CHANNEL_CONFIG) {
		case AudioFormat.CHANNEL_IN_MONO:
			return 1;
		case AudioFormat.CHANNEL_IN_STEREO:
			return 2;
		default:
			return 1;
		}
	}
	
	public void setListener(OnCollectingAudioDataCallback l) {
		mListener = l;
	}

	private class RecordTask implements Runnable {
		private final int MAX_BYTES_PER_FRAME = 1024*2;
		private long mTimeStamp;
		
		@Override
		public void run() {
			mAudioRecord.startRecording();
			mStarted = true;
			Log.d(TAG, "RecordTask start");
			
			while (mStarted) {
				byte[] buffer = new byte[MAX_BYTES_PER_FRAME];
				int ret = mAudioRecord.read(buffer, 0, MAX_BYTES_PER_FRAME);
				if (mBaseTimeStamp <= 0){
					mBaseTimeStamp = (long) (System.nanoTime() / 1000);
				}
				mTimeStamp = (System.nanoTime() / 1000) - mBaseTimeStamp;
				if (ret != AudioRecord.ERROR_BAD_VALUE && ret != AudioRecord.ERROR_INVALID_OPERATION) {
					if (mListener != null) {
						mListener.onAudioData(buffer, ret, mTimeStamp / 1000);
					}
				}
			}
		}
	}
	
	/**
	 * 销毁recorder，避免占用硬件资源
	 * 
	 * @author wangtonggui
	 */
	public void release() {
		mStarted = false;
		if (mAudioRecord.getState() == AudioRecord.RECORDSTATE_RECORDING)
			mAudioRecord.stop();
		mAudioRecord.release();
	}
}
