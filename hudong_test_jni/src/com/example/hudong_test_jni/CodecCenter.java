package com.example.hudong_test_jni;

import android.util.Log;

public class CodecCenter {
	private final static String libName = "native_codec_center";

	static {
		System.loadLibrary(libName);
	}

	// native methods declared below
	private native String stringFromJNI(int num);

	private native long nativeCreateAudioEncoder(AUDIO_CODEC_TYPE type,
			int sample_rate, int channel, int sample_size, int bitrate);

	private native int nativeDestroyAudioEncoder(long handle);

	private native long nativeAudioEncoderExtradataSize(long handle);

	private native long nativeAudioEncoderExtradata(long hanlde,
			short[] extradata, long size);

	private native long nativeAudioEncoderEncode(long handle, short[] in,
			long in_size, short[] out, long out_size);

	private native long nativeCreateAudioDecoder(AUDIO_CODEC_TYPE type,
			int sample_rate, int channel, int sample_size);

	private native int nativeDestroyAudioDecoder(long handle);

	private native long nativeAudioDecoderDecode(long handle, short[] in,
			long in_size, short[] out, long out_size);
	
	
	
	
	
	private native long nativeCreateVideoEncoder(VIDEO_CODEC_TYPE type, int width, int height, 
						int bitrate, int framerate);
	
	private native int nativeDestroyVideoEncoder(long handle);
	
	private native long nativeVideoEncoderExtradataSize(long handle);
	
	private native long nativeVideoEncoderExtradata(long handle, short[] extradata, long extradata_size);
	
	private native long nativeVideoEncoderEncode(long handle, short[] in, long in_size, short[] out, long out_size, VIDEO_FRAME_TYPE type);
	
	private native long nativeCreateVideoDecoder(long codec_type, int width, int height);
	
	private native int nativeDestroyVideoDecoder(long handle);
	
	private native long nativeVideoDecoderDecode(long handle, short[] in, long in_size, short[] out, long out_size);
	
	
	

	
	
	// java methods below
	public String getStringFromNative() {
		nativeCreateAudioEncoder(AUDIO_CODEC_TYPE.AUDIO_CODEC_MP3, 0, 0, 0, 0);
		nativeDestroyAudioEncoder(10000);
		nativeAudioEncoderExtradataSize(2000);
		short[] array_short = new short[20];
		array_short[0] = 111;
		nativeAudioEncoderExtradata(3000, array_short, array_short.length);
		Log.d("wangjie", "array_short[0] = " + array_short[0]);
		short[] array_short1 = new short[30];
		nativeAudioEncoderEncode(40000, array_short, array_short.length, array_short1, array_short1.length);
		Log.d("wangjie", "array_short[0] = " + array_short1[0]);
		nativeCreateAudioDecoder(AUDIO_CODEC_TYPE.AUDIO_CODEC_MP3, 0, 0, 0);
		nativeDestroyAudioDecoder(6000);
		nativeAudioDecoderDecode(40000, array_short, array_short.length, array_short1, array_short1.length);
		
		
		VIDEO_FRAME_TYPE videoType = VIDEO_FRAME_TYPE.VIDEO_FRAME_UNKNOWN;
		videoType = VIDEO_FRAME_TYPE.VIDEO_FRAME_UNKNOWN;
		nativeVideoEncoderEncode(40000, array_short, array_short.length, array_short1, array_short1.length, videoType);
		
		
		return stringFromJNI(10);
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
		VIDEO_FRAME_UNKNOWN(0), VIDEO_FRAME_I(1), VIDEO_FRAME_P(2), VIDEO_FRAME_B(3);
		private VIDEO_FRAME_TYPE(int type) {
			this.type = type;
		}

		private int type;

		public int type() {
			return this.type;
		}
		public void setType(int type){
			this.type = type;
		}
		
		@Override
		public String toString() {
			return String.valueOf(this.type);
		}
	}
}
