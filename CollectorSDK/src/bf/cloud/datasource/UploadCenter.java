package bf.cloud.datasource;

import android.util.Log;

public class UploadCenter{
	private final String TAG = UploadCenter.class.getSimpleName();
	private static UploadCenter mCodecCenter = null;

	static {
		try {
			System.loadLibrary("ffmpeg");
			System.loadLibrary("streamuploader");
		} catch (UnsatisfiedLinkError e) {
			
		}
	}

	private UploadCenter() {
	}

	public static UploadCenter getInstance() {
		if (mCodecCenter == null) {
			mCodecCenter = new UploadCenter();
		}
		return mCodecCenter;
	}

	public enum AUDIO_CODEC_TYPE {
		AUDIO_CODEC_AAC(0), AUDIO_CODEC_MP3(1);
		private AUDIO_CODEC_TYPE(int type) {
			this.type = type;
		}

		private int type;

		public int type() {
			return this.type;
		}
	}

	public enum VIDEO_CODEC_TYPE {
		VIDEO_CODEC_H264(1);
		private VIDEO_CODEC_TYPE(int type) {
			this.type = type;
		}

		private int type;

		public int type() {
			return this.type;
		}
	}

	public enum VIDEO_FRAME_TYPE {
		VIDEO_FRAME_UNKNOWN(0), VIDEO_FRAME_I(1), VIDEO_FRAME_P(2), VIDEO_FRAME_B(
				3);
		private VIDEO_FRAME_TYPE(int type) {
			this.type = type;
		}

		private int type;

		public int type() {
			return this.type;
		}

		public void setType(int type) {
			this.type = type;
		}

		@Override
		public String toString() {
			return String.valueOf(this.type);
		}
	}
	
	public static interface Listener{
		void onError(int errorCode);
	}

	// native methods declared below
	public native void nativeInit(String logDir, Listener listen);

	public native void nativeOpen(String channelId, int videoWidth,
			int videoHeight, int videoFrameRate, int videoDataRate,
			int audioSampleRate, int audioDataRate, int audioChannelCount,
			int audioSampleSize);

	public native void nativeUploadVideoSample(byte[] data, int size,
			long timestamp);

	public native void nativeUploadAudioSample(byte[] data, int size,
			long timestamp);

	public native void nativeClose();

	public native void nativeUninit();
}
