
package edu.hood.aom.ms;

import edu.mit.media.hlt.sensorgraph.*;
import android.R.color;
import android.app.Activity;
import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.content.IntentFilter;
import android.graphics.Color;
import android.os.Bundle;
import android.widget.TextView;
import at.abraxas.amarino.Amarino;
import at.abraxas.amarino.AmarinoIntent;
import java.util.Date;
import android.view.*;


/**
* <h3>Application that receives sensor readings from Arduino displaying it graphically.</h3>
* 
* This example demonstrates how to catch data sent from Arduino forwarded by Amarino 2.0.
* SensorGraph registers a BroadcastReceiver to catch Intents with action string: <b>AmarinoIntent.ACTION_RECEIVED</b>
* 
* @author Bonifaz Kaufmann - April 2010
*
*/
public class HeartRate extends Activity {
	
	private static final String TAG = "SensorGraph";
	
	// change this to your Bluetooth device address 
	private static final String DEVICE_ADDRESS =  "00:06:66:0A:AA:97"; //"00:06:66:03:73:7B";
	
	//private GraphView mGraph; 
	//private TextView mValueTV;
	private TextView TVTempRoom;
	private TextView TVHumid;
	private GraphView GPMEAS;
	private TextView TVMEAS;
	private GraphView GPBEAT;
	private TextView TVBMP;
	private TextView TVBEAT;
	private ArduinoReceiver arduinoReceiver = new ArduinoReceiver();

	long TimeStart;
	int BEAT = 0;
	boolean HaveAtLeastTwoBeats = false;
	long temp_BPM;
  /** Called when the activity is first created. */
  @Override
  public void onCreate(Bundle savedInstanceState) {
      super.onCreate(savedInstanceState);
       
      setContentView(R.layout.heartrate);
      TVBMP = (TextView) findViewById(R.id.TVBPM);
      TVTempRoom = (TextView) findViewById(R.id.TVTempRoom);
      TVHumid = (TextView) findViewById(R.id.TVHumid);  
      GPMEAS = (GraphView) findViewById(R.id.GPmmHg);
      GPMEAS.setMaxValue(200);
      GPMEAS.setSpeed(4);
      
      TVMEAS = (TextView) findViewById(R.id.TVMEAS);   
      GPBEAT = (GraphView) findViewById(R.id.GPBEAT);
      GPBEAT.setMaxValue(2);
      GPBEAT.setSpeed(4);
      TVBEAT = (TextView) findViewById(R.id.TVBEAT);
      //mGraph.setMaxValue(1024);
  }
  
	@Override
	protected void onStart() {
		super.onStart();
		// in order to receive broadcasted intents we need to register our receiver
		registerReceiver(arduinoReceiver, new IntentFilter(AmarinoIntent.ACTION_RECEIVED));
		
		// this is how you tell Amarino to connect to a specific BT device from within your own code
		Amarino.connect(this, DEVICE_ADDRESS);
	}


	@Override
	protected void onStop() {
		super.onStop();
		
		// if you connect in onStart() you must not forget to disconnect when your app is closed
		Amarino.disconnect(this, DEVICE_ADDRESS);
		
		// do never forget to unregister a registered receiver
		unregisterReceiver(arduinoReceiver);
	}
	

	/**
	 * ArduinoReceiver is responsible for catching broadcasted Amarino
	 * events.
	 * 
	 * It extracts data from the intent and updates the graph accordingly.
	 */
	public class ArduinoReceiver extends BroadcastReceiver {

		@Override
		public void onReceive(Context context, Intent intent) {
			String data = null;
			
			// the device address from which the data was sent, we don't need it here but to demonstrate how you retrieve it
			final String address = intent.getStringExtra(AmarinoIntent.EXTRA_DEVICE_ADDRESS);
			
			// the type of data which is added to the intent
			final int dataType = intent.getIntExtra(AmarinoIntent.EXTRA_DATA_TYPE, -1);
			
			
			// we only expect String data though, but it is better to check if really string was sent
			// later Amarino will support differnt data types, so far data comes always as string and
			// you have to parse the data to the type you have sent from Arduino, like it is shown below
			if (dataType == AmarinoIntent.STRING_EXTRA){
				data = intent.getStringExtra(AmarinoIntent.EXTRA_DATA);
				
				if (data != null){
					String Values[] = data.split(",");
					for(String _Case : Values)
					{
						String Condition[] = _Case.split(":");
						int Cond = Integer.parseInt(Condition[0]);
						switch(Cond){
						case 1 : TVTempRoom.setText(Condition[1]);
						break;
						case 2 : TVHumid.setText(Condition[1]);
						break;
						case 3 :  
							BEAT = Integer.parseInt(Condition[1]);
							if(BEAT == 1) 
							{
								if(HaveAtLeastTwoBeats)
								{
									long BPM = (long) ((1.0 / (System.currentTimeMillis() - TimeStart)) * 60000.0);
									TVBMP.setText(Long.toString(BPM));
									TimeStart = System.currentTimeMillis();
									temp_BPM = BPM;
									
								}
								else
								{
									TimeStart = System.currentTimeMillis();
									HaveAtLeastTwoBeats = true;
								}
							}
							GPBEAT.addDataPoint(Integer.parseInt(Condition[1]));
							TVBEAT.setText(Condition[1]);
							
						break;
						}
						GPMEAS.addDataPoint(temp_BPM);
						TVMEAS.setText(Long.toString(temp_BPM));
					}

				}
			}
		}
	}

}

