package edu.hood.aom.ms;

import edu.mit.media.hlt.sensorgraph.*;
import android.app.Activity;
import android.content.Intent;
import android.os.Bundle;
import android.os.Handler;
import android.view.Window;

public class splashscreen extends Activity {
	Handler handler;
    Runnable runnable;
    Long delay_time;
    Long time = 1500L;
    
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        requestWindowFeature(Window.FEATURE_NO_TITLE);
        setContentView(R.layout.splash);
        handler = new Handler(); 
        
        runnable = new Runnable() { 
             public void run() { 
                 Intent intent = new Intent(splashscreen.this, home.class);
                    startActivity(intent);
                    finish();
             } 
        };
    }
    
    public void onResume() {
        super.onResume();
        delay_time = time;
        handler.postDelayed(runnable, delay_time);
        time = System.currentTimeMillis();
    }
    
    public void onPause() {
        super.onPause();
        handler.removeCallbacks(runnable);
        time = delay_time - (System.currentTimeMillis() - time);
    }
}