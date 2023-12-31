package net.bastibl.fmrx

import android.Manifest
import android.annotation.SuppressLint
import android.app.PendingIntent
import android.content.BroadcastReceiver
import android.content.Context
import androidx.appcompat.app.AppCompatActivity
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
import android.widget.Button
import android.widget.TextView
import androidx.core.app.NotificationManagerCompat

import java.util.HashMap;

private const val ACTION_USB_PERMISSION = "com.android.example.USB_PERMISSION"

class MainActivity : AppCompatActivity() {

    var fileDescriptor: Int = -1
    var usbfsPath = ""

    @SuppressLint("SetTextI18n")
    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContentView(R.layout.activity_main)

        checkStoragePermission()

        var errorMessage = ""

        val initButton = findViewById<Button>(R.id.initButton)
        initButton.setOnClickListener {
            try {
                val textView = findViewById<TextView>(R.id.fgStatus)
//                textView.text =
//                    "Status:\tFlowgraph Initialising..."
                errorMessage = fgInit(fileDescriptor, usbfsPath)
                displayNotification("Flowgraph initalised\nError:${errorMessage}")
                textView.text =
                    "Status:\tFlowgraph Initialised\n\t\tError:${errorMessage}"
            } catch (e: Exception) {
                displayNotification("Error running app")
            }
        }

        val startButton = findViewById<Button>(R.id.startButton)
        startButton.setOnClickListener {
            try {
                val textView = findViewById<TextView>(R.id.fgStatus)
//                textView.text =
//                    "Status:\tFlowgraph Running..."
                errorMessage = fgStart(cacheDir.absolutePath)
                displayNotification("Flowgraph running\nError:${errorMessage}")
                textView.text =
                    "Status:\tFlowgraph Ended\n\t\tError:${errorMessage}"
            } catch (e: Exception) {
                displayNotification("Error running app")
            }
        }

        val stopButton = findViewById<Button>(R.id.stopButton)
        stopButton.setOnClickListener {
            try {
                val textView = findViewById<TextView>(R.id.fgStatus)
//                textView.text =
//                    "Status:\tFlowgraph Stopping"
                fgStop()
                displayNotification("Flowgraph stopped\nError:${errorMessage}")
                textView.text =
                    "Status:\tFlowgraph Stopped\n\t\tError:${errorMessage}"
            } catch (e: Exception) {
                displayNotification("Error running app")
            }
        }

        val usbButton1 = findViewById<Button>(R.id.usbButton1)
        usbButton1.setOnClickListener {
            try {
                val usbList = checkUSB()
                displayNotification("USB Connections:\n${usbList}")
            } catch (e: Exception) {
                displayNotification("Error running JNI code to check USB connections")
            }
        }

        val usbButton2 = findViewById<Button>(R.id.usbButton2)
        usbButton2.setOnClickListener {
            try {
                if(fileDescriptor == -1){
                    displayNotification("Invalid Filedescriptor\nDevice is not opened")
                } else {
                    val usbList = checkUSBTwo(fileDescriptor)
                    displayNotification("USB Connections:\n${usbList}")
                }
            } catch (e: Exception) {
                displayNotification("Error running JNI code to check USB connections")
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

    private fun checkStoragePermission() {
        if (ContextCompat.checkSelfPermission(this,
                Manifest.permission.WRITE_EXTERNAL_STORAGE)
            != PackageManager.PERMISSION_GRANTED) {

            ActivityCompat.requestPermissions(this,
                arrayOf(Manifest.permission.WRITE_EXTERNAL_STORAGE),
                123)
        } else {
            checkHWPermission()
        }
    }

    override fun onRequestPermissionsResult(requestCode: Int,
                                            permissions: Array<String>, grantResults: IntArray) {
        when (requestCode) {
            123 -> {
               checkHWPermission()
            }
        }
    }
    
    private fun checkHWPermission() {
        //Getting USB service
        val manager = getSystemService(Context.USB_SERVICE) as UsbManager
        //Getting list of connected USB device
        val deviceList: HashMap<String, UsbDevice> = manager.deviceList
            
        // Iterating through each connected USB device
        deviceList.values.forEach { device ->
            if(true) {
//            if(device.vendorId == 0x2500) { //USRP
//            if(device.vendorId == 0x1d50) { //LimeSDR
                // Preparing the permission request broadcast
                val permissionIntent = PendingIntent.getBroadcast(this, 0, Intent(ACTION_USB_PERMISSION), 0)
                // Creating an intent filter for USB permission
                val filter = IntentFilter(ACTION_USB_PERMISSION)
                // Registering a broadcast receiver for USB permission
                registerReceiver(usbReceiver, filter)

                // Extracting information about the USB device
                val deviceName = device.deviceName
                val vendorId = device.vendorId
                val productId = device.productId
                Log.d("USBDevice", "Name: $deviceName, Vendor ID: $vendorId, Product ID: $productId")
                displayNotification("USBDeviceName: $deviceName, Vendor ID: $vendorId, Product ID: $productId")

                manager.requestPermission(device, permissionIntent)
            }
        }
    }

    private val usbReceiver = object : BroadcastReceiver() {

        @SuppressLint("SetTextI18n")
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
                        val textView = findViewById<TextView>(R.id.sampleText)
                        textView.text = "permission denied for device $device"
                        displayNotification("Permission denied for device $device")
                    }
                }
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

        val textView = findViewById<TextView>(R.id.sampleText)
        textView.text =
            "Found fd: $fd  \nusbfsPath: $usbfsPath \nvid: $vid  pid: $pid"

        val usbDeviceConnection: UsbDeviceConnection = manager.openDevice(usbDevice)
        fileDescriptor = usbDeviceConnection.fileDescriptor

//        var errorMessage = fgInit(fd, usbfsPath)
//        displayNotification("Flowgraph initalised\nError:${errorMessage}")
//        errorMessage = fgStart(cacheDir.absolutePath)
//        displayNotification("Flowgraph running\nError:${errorMessage}")
    }

    override fun onStop() {
        fgStop()
        super.onStop()
    }

    private external fun fgInit(fd: Int, usbfsPath: String): String
    private external fun fgStart(tmpName: String): String
    private external fun fgStop(): String
    private external fun checkUSB(): String
    private external fun checkUSBTwo(fd: Int): String

    companion object {
        init {
            System.loadLibrary("native-lib")
        }
    }
}
