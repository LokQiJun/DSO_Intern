package net.bastibl.fmrx

import android.Manifest
import android.annotation.SuppressLint
import android.app.PendingIntent
import android.content.BroadcastReceiver
import android.content.Context
import androidx.appcompat.app.AppCompatActivity
import kotlinx.android.synthetic.main.activity_main.*
import android.hardware.usb.UsbDevice
import android.hardware.usb.UsbManager
import android.content.IntentFilter
import android.content.Intent
import android.content.pm.PackageManager
import android.hardware.usb.UsbDeviceConnection
import android.os.*
import android.util.Log

import androidx.core.app.ActivityCompat
import androidx.core.content.ContextCompat
import androidx.core.app.NotificationCompat
import android.app.NotificationChannel
import android.app.NotificationManager
import android.net.ConnectivityManager
import android.net.NetworkCapabilities
import android.widget.Button
import android.widget.Toast
import androidx.core.app.NotificationManagerCompat

import java.util.HashMap;
import kotlin.concurrent.thread

private const val ACTION_USB_PERMISSION = "com.android.example.USB_PERMISSION"

class MainActivity : AppCompatActivity() {

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContentView(R.layout.activity_main)

        checkStoragePermission()

//Check IP connection
//        val ipAddressToCheck = "192.168.10.2" // Replace with the IP address you want to check
//        val checkConnectionTask = CheckConnectionTask(this)
//
//        checkConnectionTask.setCallback(object : CheckConnectionTask.ConnectionCallback {
//            override fun onConnectionResult(isConnected: Boolean) {
//                if (isConnected) {
//                    Toast.makeText(this@MainActivity, "Connected to ${ipAddressToCheck}", Toast.LENGTH_SHORT).show()
//                } else {
//                    Toast.makeText(this@MainActivity, "Not connected to ${ipAddressToCheck}", Toast.LENGTH_SHORT).show()
//                }
//            }
//        })
//
//        checkConnectionTask.execute(ipAddressToCheck)

        val startButton = findViewById<Button>(R.id.startButton)
        startButton.setOnClickListener {
            try {
                val errorMessage = fgInit(1, "")
                displayNotification("Flowgraph initalised, error:${errorMessage}")
//                if (isEthernetConnected()) {
//                    displayNotification("Connected to Ethernet, starting flowgraph")
//
//                    try {
//                        val errorMessage = fgInit(1, "")
//                        displayNotification("Flowgraph initalised, error:${errorMessage}")
//                    } catch (e: Exception) {
//                        displayNotification("Flowgraph initalised error \n${e.message}")
//                    }
//
//                    try {
//                        fgStart(cacheDir.absolutePath)
//                    } catch (e: Exception) {
//                        displayNotification("Flowgraph run error \n${e.message}")
//                    }
//                } else {
//                    displayNotification("Not connected to Ethernet")
//                }
            } catch (e: Exception) {
                displayNotification("Error running app")
            }
        }
    }

    private fun displayNotification(message: String) {

        val channelId = "statusNotif" // Use the same channel ID as defined in step 2
        val notificationBuilder = NotificationCompat.Builder(this, channelId)
            .setSmallIcon(R.drawable.gr_icon) // Replace with your notification icon
            .setContentTitle("App Status")
            .setContentText(message)
            .setStyle(NotificationCompat.BigTextStyle().bigText(message))
            .setPriority(NotificationCompat.PRIORITY_HIGH) // Adjust the priority level as needed
            .setAutoCancel(true) // Close the notification when the user taps on it

        val notificationManager = NotificationManagerCompat.from(this)


        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.O) {
            val channelName = "Current status"
            val channelDescription = "Current status"
            val importance =
                NotificationManager.IMPORTANCE_HIGH // Adjust the importance level as needed

            val channel = NotificationChannel(channelId, channelName, importance)
            channel.description = channelDescription
            channel.enableVibration(true)

            notificationManager.createNotificationChannel(channel)
        }

        notificationManager.notify(1, notificationBuilder.build())

    }

    private fun isEthernetConnected(): Boolean {
        var result = false
        val connectivityManager =
            getSystemService(Context.CONNECTIVITY_SERVICE) as ConnectivityManager
        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.M) {
            val networkCapabilities = connectivityManager.activeNetwork ?: return false
            val actNw =
                connectivityManager.getNetworkCapabilities(networkCapabilities) ?: return false
            result = when {
                actNw.hasTransport(NetworkCapabilities.TRANSPORT_ETHERNET) -> true
                else -> false
            }
        } else {
            connectivityManager.run {
                connectivityManager.activeNetworkInfo?.run {
                    result = when (type) {
                        ConnectivityManager.TYPE_ETHERNET -> true
                        else -> false
                    }

                }
            }
        }

        return result
    }


    private val usbReceiver = object : BroadcastReceiver() {

        @Suppress("IMPLICIT_CAST_TO_ANY")
        override fun onReceive(context: Context, intent: Intent) {
            if (ACTION_USB_PERMISSION == intent.action) {
                synchronized(this) {
                    val device: UsbDevice? = intent.getParcelableExtra(UsbManager.EXTRA_DEVICE)

                    if (intent.getBooleanExtra(UsbManager.EXTRA_PERMISSION_GRANTED, false)) {
                        device?.apply {
                            setupUSB(device)
                        }
                    } else {
                        Log.d("GR", "permission denied for device $device")
                    }
                }
            }
        }
    }

    @Volatile var connected = false

    private fun checkStoragePermission() {
        if (ContextCompat.checkSelfPermission(this,
                Manifest.permission.WRITE_EXTERNAL_STORAGE)
            != PackageManager.PERMISSION_GRANTED) {

            ActivityCompat.requestPermissions(this,
                arrayOf(Manifest.permission.WRITE_EXTERNAL_STORAGE),
                123)
        } else {
            sample_text.text = grConf()
            checkHWPermission()
        }
    }

    private fun checkHWPermission() {

        val manager = getSystemService(Context.USB_SERVICE) as UsbManager
        val deviceList: HashMap<String, UsbDevice> = manager.deviceList
        deviceList.values.forEach { device ->
//            if(device.vendorId == 0x0bda && device.productId == 0x2838) {
//            if(device.vendorId == 0x1d50) {
            if(device.vendorId == 0x2500) {
                val permissionIntent = PendingIntent.getBroadcast(this, 0, Intent(ACTION_USB_PERMISSION), 0)
                val filter = IntentFilter(ACTION_USB_PERMISSION)
                registerReceiver(usbReceiver, filter)

                manager.requestPermission(device, permissionIntent)
            }
        }
    }

    override fun onRequestPermissionsResult(requestCode: Int,
                                            permissions: Array<String>, grantResults: IntArray) {
        when (requestCode) {
            123 -> {
//                sample_text.text = grConf()
                checkHWPermission()
            }
        }
    }

    @SuppressLint("SetTextI18n")
    fun setupUSB(usbDevice: UsbDevice) {

        val manager = getSystemService(Context.USB_SERVICE) as UsbManager
        val connection: UsbDeviceConnection = manager.openDevice(usbDevice)

        val fd = connection.fileDescriptor

        val usbfsPath = usbDevice.deviceName

        val vid = usbDevice.vendorId
        val pid = usbDevice.productId

        Log.d("gnuradio", "#################### NEW RUN ###################")
        Log.d("gnuradio", "Found fd: $fd  usbfs_path: $usbfsPath")
        Log.d("gnuradio", "Found vid: $vid  pid: $pid")

        sample_text.text =
            "Found fd: $fd  usbfsPath: $usbfsPath vid: $vid  pid: $pid"

//        fgInit(fd, usbfsPath)
//        fgStart(cacheDir.absolutePath)

    }

    override fun onStop() {
        super.onStop()
        //fgStop()
    }

    private external fun fgInit(fd: Int, usbfsPath: String): String
    private external fun fgStart(tmpName: String): Void
    external fun fgStop(): Void
    external fun fgRep(): String
    external fun grConf(): String

    companion object {
        init {
            System.loadLibrary("native-lib")
        }
    }
}