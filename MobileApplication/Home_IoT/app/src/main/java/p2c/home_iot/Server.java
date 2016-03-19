package p2c.home_iot;

import android.content.Context;
import android.os.AsyncTask;
import android.util.Log;
import android.widget.Toast;

import java.io.BufferedReader;
import java.io.BufferedWriter;
import java.io.IOException;
import java.io.InputStreamReader;
import java.io.OutputStreamWriter;
import java.net.ServerSocket;
import java.net.Socket;

/**
 * Created by Paul on 18/03/16.
 */
public class Server extends AsyncTask<Void, Void, String> {

    private Context context;
    private String ssid;
    private String pass;
    private String scanQR;

    private BufferedReader input = null;
    private BufferedWriter output = null;

    private ServerSocket socket = null;
    private Socket clientSocket = null;

    public Server(Context context, String ssid, String pass, String scanQR) {
        this.context = context;
        this.ssid = ssid;
        this.pass = pass;
        this.scanQR = scanQR;
    }

    /**
     * Background process waits for a device client to connect and serves it with corresponding data
     */
    protected String doInBackground(Void... str) {
        StringBuilder buff = new StringBuilder();
        try {
            /* open TCP port 8080 (port 80 not available without root permissions */
            socket = new ServerSocket(8080);
            /* wait for ESP8266 to connect */
            try {
                Log.println(Log.DEBUG, "WebServer", "Starting server, waiting for client");
                clientSocket = socket.accept();
            } catch (IOException e) {
                e.printStackTrace();
            }

            input = new BufferedReader(new InputStreamReader(clientSocket.getInputStream()));
            output = new BufferedWriter(new OutputStreamWriter(clientSocket.getOutputStream()));

            Log.println(Log.DEBUG, "WebServer", "IP: " + clientSocket.getInetAddress().toString());

            /* read the received request message */
            String s;
            while ((s = input.readLine()) != null) {
                buff.append(s);
                if (s.isEmpty()) {
                    break;
                }
            }


        } catch (Exception e) {
            System.out.println(e);
        }
        return buff.toString();
    }

    /**
     * Function called after the background asynchronous task has finished
     */
    protected void onPostExecute(String message) {
        /* sanity check */
        if (null != message) {
            Log.println(Log.DEBUG, "WebServer", "MSG: " + message);

            /* check if the request message is of configuration type */
            int i = strStr(message, "/config");
            if (i > 0) {
                Log.println(Log.DEBUG, "WebServer", "Config message received");

                /**
                 *  Build the response buffer;
                 *  First 16 bytes contain the SSID;
                 *  Next 16 bytes contain the password;
                 *  Last 4 bytes contain the device ID read from the QR code
                 */

                char[] buff = new char[36];
                int it;
                for (it = 0; it < ssid.length(); it++) {
                    buff[it] = ssid.charAt(it);
                }
                if (it < 15) {
                    while (it < 16) {
                        buff[it] = '\0';
                        it++;
                    }
                }
                for (it = 16; (it - 16) < pass.length(); it++) {
                    buff[it] = pass.charAt(it - 16);
                }
                if (it < 31) {
                    while (it < 32) {
                        buff[it] = '\0';
                        it++;
                    }
                }
                for (it = 32; (it - 32) < scanQR.length() && it < 36; it++) {
                    buff[it] = scanQR.charAt(it - 32);
                }
                if (it < 35) {
                    while (it < 36) {
                        buff[it] = '\0';
                        it++;
                    }
                }

                /* send the response message to the requesting device */
                try {
                    output.write(buff);
                    output.flush();
                    Log.println(Log.DEBUG, "WebServer", "Flushed: " + buff);

                    Toast.makeText(context, "Device configured", Toast.LENGTH_LONG).show();
                } catch (IOException e) {
                    e.printStackTrace();
                }
            }
        }

        /* close the opened connections */
        try {
            output.close();
            input.close();
            clientSocket.close();
            socket.close();
        } catch (IOException e) {
            e.printStackTrace();
        }
    }

    /**
     * Search for a string within a string (classic strstr() function in C :) )
     */
    public int strStr(String haystack, String needle) {
        if (haystack == null || needle == null)
            return 0;

        if (needle.length() == 0)
            return 0;

        for (int i = 0; i < haystack.length(); i++) {
            if (i + needle.length() > haystack.length())
                return -1;

            int m = i;
            for (int j = 0; j < needle.length(); j++) {
                if (needle.charAt(j) == haystack.charAt(m)) {
                    if (j == needle.length() - 1)
                        return i;
                    m++;
                } else {
                    break;
                }

            }
        }

        return -1;
    }
}
