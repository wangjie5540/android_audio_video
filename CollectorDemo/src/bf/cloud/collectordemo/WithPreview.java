package bf.cloud.collectordemo;

import bf.cloud.datasource.BFRecorder;
import bf.cloud.datasource.CameraPreview;
import android.app.Activity;
import android.content.Context;
import android.os.Bundle;
import android.util.Log;
import android.view.View;
import android.view.View.OnClickListener;
import android.widget.EditText;
import android.widget.ImageButton;
import android.widget.Toast;

public class WithPreview extends Activity {
	private final String TAG = WithPreview.class.getSimpleName();
	private BFRecorder mBfRecorder = null;
	private ImageButton ibStartOrStop = null;
	private EditText etChannelId = null;
	private Context mContext = null;

	@Override
	protected void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		Log.d(TAG, "WithPreview onCreate");
		setContentView(R.layout.activity_with_preview);
		init();
	}

	private void init() {
		mContext = this;
		mBfRecorder = BFRecorder.getInstance();
		CameraPreview cp = (CameraPreview) findViewById(R.id.cp_preview);
		mBfRecorder.setPreview(cp);
		etChannelId = (EditText) findViewById(R.id.channelIdEditText);
		etChannelId.setText("F0CE16B468674A11A74966C85AE1B226");
		ibStartOrStop = (ImageButton) findViewById(R.id.ib_start_stop);
		ibStartOrStop.setOnClickListener(new OnClickListener() {

			@Override
			public void onClick(View v) {
				Log.d(TAG, "ibStartOrStop onClick etChannelId");
				String channelId = etChannelId.getText().toString();
				if (channelId == null || channelId.length() == 0) {
					Toast.makeText(mContext, "频道不能为空",
							Toast.LENGTH_SHORT).show();
				}
				Log.d(TAG, "ibStartOrStop onClick channelId = " + channelId);
				if (channelId.length() == 0)
					return;

				mBfRecorder.start(channelId);
			}
		});
	}

	@Override
	protected void onStop() {
		Log.d(TAG, "WithPreview onStop");
		mBfRecorder.stop();
		super.onStop();
	}

	@Override
	protected void onDestroy() {
		Log.d(TAG, "WithPreview onDestroy");
		mBfRecorder.release();
		super.onDestroy();
	}
}
