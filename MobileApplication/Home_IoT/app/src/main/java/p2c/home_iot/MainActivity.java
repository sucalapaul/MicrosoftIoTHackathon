package p2c.home_iot;

import android.content.Intent;
import android.os.Bundle;
import android.support.v7.app.AppCompatActivity;
import android.support.v7.widget.Toolbar;
import android.view.View;
import android.view.Menu;
import android.view.MenuItem;
import android.widget.EditText;
import android.widget.Toast;

import com.google.zxing.integration.android.*;


public class MainActivity extends AppCompatActivity {

    private String scanQR = null; // temporary !

    private Server server = null;

    private String ssidString, passString;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);
        Toolbar toolbar = (Toolbar) findViewById(R.id.toolbar);
        setSupportActionBar(toolbar);
    }

    /**
     * Open QR code scanning Activity
     */
    public void onClickQRScan(View view) {
        IntentIntegrator scanIntegrator = new IntentIntegrator(this);
        scanIntegrator.initiateScan();
    }

    /**
     * Retrieve the QR scan result
     */
    public void onActivityResult(int requestCode, int resultCode, Intent intent) {
        IntentResult scanningResult = IntentIntegrator.parseActivityResult(requestCode, resultCode, intent);

        /* check if the obtained code is valid */
        if (null != scanningResult) {
            scanQR = scanningResult.getContents();
        } else {
            scanQR = null;
            Toast.makeText(this, "Scanned code not valid!", Toast.LENGTH_SHORT).show();
        }
    }

    /**
     * Open WiFi Hotspot and wait for IoT device to connect
     */
    public void onClickDeviceScan(View view) {
        EditText ssid = (EditText) findViewById(R.id.textSSID);
        EditText pass = (EditText) findViewById(R.id.textPassword);

        /* if a valid QR code was already scanned */
        if (null != scanQR) {

            ssidString = ssid.getText().toString();
            passString = pass.getText().toString();

            /* if the SSID and password inputs are valid (fields contain information) */
            if (0 < ssidString.length() && 0 < passString.length()) {

                /* if the server hasn't been started yet */
                if (null == server) {

                    /* reset the text fields */
                    ssid.setText("");
                    pass.setText("");

                    /* Create new server */
                    server = new Server(this, ssidString, passString, scanQR);

                    /* wait for ESP8266 to connect */
                    /* start execution of the asynchronous server thread */
                    server.execute();
                }
            } else {
                /* if the text inputs are not valid (non existent) */
                ssid.setText("");
                pass.setText("");
                Toast.makeText(this, "Invalid input!", Toast.LENGTH_SHORT).show();
            }
        } else {
            /* if no QR code was previously scanned */
            Toast.makeText(this, "No QR code scanned", Toast.LENGTH_SHORT).show();
        }
    }


    @Override
    public boolean onCreateOptionsMenu(Menu menu) {
        // Inflate the menu; this adds items to the action bar if it is present.
        getMenuInflater().inflate(R.menu.menu_main, menu);
        return true;
    }

    @Override
    public boolean onOptionsItemSelected(MenuItem item) {
        // Handle action bar item clicks here. The action bar will
        // automatically handle clicks on the Home/Up button, so long
        // as you specify a parent activity in AndroidManifest.xml.
        int id = item.getItemId();

        //noinspection SimplifiableIfStatement
        if (id == R.id.action_settings) {
            return true;
        }

        return super.onOptionsItemSelected(item);
    }
}
