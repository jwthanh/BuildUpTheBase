package com.tankorsmash.buildupthebase;

import org.cocos2dx.lib.Cocos2dxActivity;

import android.os.Vibrator;
import android.content.Context;
import android.app.ActivityManager;
import android.util.Log;
import android.os.Bundle;

public class AppActivity extends Cocos2dxActivity {

    private static volatile Cocos2dxActivity mainActivity;    
    public static void setMainActivity(Cocos2dxActivity activity)
    {
        mainActivity = activity;
    }

    private static native void initCricket(Context context);

    public static Cocos2dxActivity getMainActivity()
    {
        if(mainActivity == null)
        {
            Log.w("beatupface_javastuff", "Warning : null main Activity");
        }
        return mainActivity;
    }

	@Override
	protected void onCreate(Bundle savedInstanceState)
	{
		super.onCreate(savedInstanceState);
    	AppActivity.setMainActivity(this);
        AppActivity.initCricket(this.getApplicationContext());
	}

    public static void vibrate(int milliseconds)
    {
        Vibrator v = (Vibrator) getMainActivity().getSystemService(Context.VIBRATOR_SERVICE);
        if (getMainActivity().getSystemService(Context.VIBRATOR_SERVICE) != null) 
        {
            /* final long[] val = {0, 100, 25, 100, 25}; */
            /* v.vibrate(val, 0); */
            v.vibrate(milliseconds);
        } 
    }
}
