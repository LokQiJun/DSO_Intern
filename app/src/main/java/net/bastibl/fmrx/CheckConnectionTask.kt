package net.bastibl.fmrx

import android.content.Context
import android.os.AsyncTask
import java.io.IOException
import java.net.InetAddress

class CheckConnectionTask(private val context: Context) :
    AsyncTask<String, Void, Boolean>() {

    private var callback: ConnectionCallback? = null

    fun setCallback(callback: ConnectionCallback) {
        this.callback = callback
    }

    override fun doInBackground(vararg params: String?): Boolean {
        val ipAddress = params[0]
        return isReachable(ipAddress)
    }

    override fun onPostExecute(result: Boolean) {
        callback?.onConnectionResult(result)
    }

    private fun isReachable(ipAddress: String?): Boolean {
        try {
            val address = InetAddress.getByName(ipAddress)
            return address.isReachable(3000) // 3 seconds timeout
        } catch (e: IOException) {
            e.printStackTrace()
        }
        return false
    }

    interface ConnectionCallback {
        fun onConnectionResult(isConnected: Boolean)
    }
}