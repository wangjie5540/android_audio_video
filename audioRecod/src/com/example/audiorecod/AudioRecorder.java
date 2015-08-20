package com.example.audiorecod;

import android.media.AudioFormat;
import android.media.AudioRecord;
import android.media.MediaRecorder;
import android.util.Log;

public class AudioRecorder {
	private static final String TAG = "AudioRecorder";
	private static final int SAMPLE_RATE = 44100; // 采样率(CD音质)
	private static final int CHANNEL_CONFIG = AudioFormat.CHANNEL_IN_MONO; // 音频通道(单声道)
	private static final int AUDIO_FORMAT = AudioFormat.ENCODING_PCM_16BIT; // 音频格式
	private static final int AUDIO_SOURCE = MediaRecorder.AudioSource.MIC; // 音频源（麦克风）
	private boolean is_recording = false;
	private RecorderTask recorderTask = new RecorderTask();

	private int mAudioSource = -1;
	private int mSampleRateInHz = -1;
	private int mChannelConfig = -1;
	private int mAudioFormat = -1;

	private OnCollectingAudioDataCallback mCallback = null;
	private AudioRecord mAudioRecord = null;
	private int bufferSizeInBytes;
	public short[] buffer;

	/**
	 * 构造函数，参数同 android.media.AudioRecord
	 * 
	 * @param audioSource
	 * @param sampleRateInHz
	 * @param channelConfig
	 * @param audioFormat
	 */
	public AudioRecorder(int audioSource, int sampleRateInHz,
			int channelConfig, int audioFormat) {
		// 先设置默认的参数
		mAudioSource = AUDIO_SOURCE;
		mSampleRateInHz = SAMPLE_RATE;
		mChannelConfig = CHANNEL_CONFIG;
		mAudioFormat = AUDIO_FORMAT;
		// 获取最小缓冲区大小
		bufferSizeInBytes = AudioRecord.getMinBufferSize(mSampleRateInHz,
				mChannelConfig, mAudioFormat);
		Log.d(TAG, "getMinBufferSize = " + bufferSizeInBytes);
		mAudioRecord = new AudioRecord(mAudioSource, // 音频源
				mSampleRateInHz, // 采样率
				mChannelConfig, // 音频通道
				mAudioFormat, // 音频格式
				bufferSizeInBytes // 缓冲区
		);
	}

	/*
	 * 开始录音
	 */
	public void startAudioRecording() {
		if (!is_recording)
			new Thread(recorderTask).start();
	}

	/*
	 * 停止录音
	 */
	public void stopAudioRecording() {
		if (is_recording){
			is_recording = false;
		}else
			return;
		if (mAudioRecord != null) {
			mAudioRecord.setRecordPositionUpdateListener(null);
			mAudioRecord.stop();
		}
	}

	/**
	 * 销毁recorder，避免占用硬件资源
	 * 
	 * @author wangtonggui
	 */
	public void releaseAudioRecorder() {
		is_recording = false;
		if (mAudioRecord.getState() == AudioRecord.RECORDSTATE_RECORDING)
			mAudioRecord.stop();
		mAudioRecord.release();
	}

	class RecorderTask implements Runnable {
		int bufferReadResult = 0;
		public int samples_per_frame = 4096;

		@Override
		public void run() {
			if (mAudioRecord.getState() == AudioRecord.STATE_UNINITIALIZED)
				mAudioRecord = new AudioRecord(mAudioSource, // 音频源
						mSampleRateInHz, // 采样率
						mChannelConfig, // 音频通道
						mAudioFormat, // 音频格式
						bufferSizeInBytes // 缓冲区
				);
			mAudioRecord.startRecording();
			is_recording = true;

			while (is_recording) {
				buffer = new short[samples_per_frame];
				// 从缓冲区中读取数据，存入到buffer字节数组数组中
				bufferReadResult = mAudioRecord.read(buffer, 0,
						samples_per_frame);
				Log.d(TAG, "" + bufferReadResult + "/" + samples_per_frame);
				// 判断是否读取成功
				if (bufferReadResult == AudioRecord.ERROR_BAD_VALUE
						|| bufferReadResult == AudioRecord.ERROR_INVALID_OPERATION)
					Log.e(TAG, "Read error");
				else{
					FileOperator1.append(buffer, bufferReadResult);
					if (mAudioRecord != null) {
						if (mCallback != null) {
//							mCallback.onData(buffer, System.nanoTime());
						}
					}
				}
			}
		}
	}

	public void setOnCollectingAudioDataCallback(
			OnCollectingAudioDataCallback cb) {
		mCallback = cb;
	}

	/**
	 * 使用这个接口来获取音频数据
	 * 
	 * @author wangtonggui
	 */
	public static interface OnCollectingAudioDataCallback {
		void onData(byte[] data, long timeStamp);
	}
}
