package com.example.audiorecod;

import java.io.BufferedOutputStream;
import java.io.DataOutputStream;
import java.io.File;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.OutputStream;

import com.example.audiorecod.AudioRecorder.OnCollectingAudioDataCallback;

import android.app.Activity;
import android.media.AudioFormat;
import android.media.AudioRecord;
import android.media.MediaRecorder;
import android.os.Bundle;
import android.os.Environment;
import android.util.Log;
import android.view.Menu;
import android.view.MenuItem;
import android.view.View;
import android.view.View.OnClickListener;
import android.widget.Button;


public class MainActivity extends Activity {
	private String TAG = MainActivity.class.getSimpleName();
	private AudioRecorder mAudioRecorder = null;

	@Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);
        mAudioRecorder = new AudioRecorder(0, 0, 0, 0);
        mAudioRecorder.setOnCollectingAudioDataCallback(new OnCollectingAudioDataCallback() {
			
			@Override
			public void onData(byte[] data, long timeStamp) {
				Log.d(TAG, "onData " + timeStamp);
			}
		});
        Button start = (Button)findViewById(R.id.start_bt) ;
        start.setOnClickListener(new OnClickListener() {
			
			@Override
			public void onClick(View v) {
				mAudioRecorder.startAudioRecording();
			}
		});
        
        Button stop = (Button)findViewById(R.id.end_bt);
        stop.setOnClickListener(new OnClickListener() {
			
			@Override
			public void onClick(View v) {
				mAudioRecorder.stopAudioRecording();
			}
		});
        
//        start.setOnClickListener(new OnClickListener(){
// 
//            @Override
//            public void onClick(View arg0) {
//                // TODO Auto-generated method stub
//                Thread thread = new Thread(new Runnable() {
//                    public void run() {
//                      record();
//                    }    
//                  });
//                  thread.start();
//                  findViewById(R.id.start_bt).setEnabled(false) ;
//                  findViewById(R.id.end_bt).setEnabled(true) ;
//            }
//         
//        }) ;

    }
	
	@Override
	protected void onStop() {
		mAudioRecorder.releaseAudioRecorder();
		super.onStop();
	}
    
//    public void record() {
//        int frequency = 11025;
//        int channelConfiguration = AudioFormat.CHANNEL_CONFIGURATION_MONO;
//        int audioEncoding = AudioFormat.ENCODING_PCM_16BIT;
//        File file = new File(Environment.getExternalStorageDirectory().getAbsolutePath() + "/reverseme.pcm");
//        
//        // Delete any previous recording.
//        if (file.exists())
//          file.delete();
//   
//   
//        // Create the new file.
//        try {
//          file.createNewFile();
//        } catch (IOException e) {
//          throw new IllegalStateException("Failed to create " + file.toString());
//        }
//        
//        try {
//          // Create a DataOuputStream to write the audio data into the saved file.
//          OutputStream os = new FileOutputStream(file);
//          BufferedOutputStream bos = new BufferedOutputStream(os);
//          DataOutputStream dos = new DataOutputStream(bos);
//          
//          // Create a new AudioRecord object to record the audio.
//          int bufferSize = AudioRecord.getMinBufferSize(frequency, channelConfiguration,  audioEncoding);
//          AudioRecord audioRecord = new AudioRecord(MediaRecorder.AudioSource.MIC,
//                                                    frequency, channelConfiguration,
//                                                    audioEncoding, bufferSize);
//        
//          short[] buffer = new short[bufferSize];  
//          audioRecord.startRecording();
//   
//          isRecording  = true ;
//          while (isRecording) {
//            int bufferReadResult = audioRecord.read(buffer, 0, bufferSize);
//            for (int i = 0; i < bufferReadResult; i++)
//              dos.writeShort(buffer[i]);
//          }
//   
//   
//          audioRecord.stop();
//          dos.close();
//        
//        } catch (Throwable t) {
//          Log.e("AudioRecord","Recording Failed");
//        }
//      }

}
