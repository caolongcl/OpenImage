package com.cwdx.opensrc.common;

import android.util.Log;

public final class Utils {
    // Log
    public static void d(String subTag, String log) {
        Log.d(Common.TAG, "[" + subTag + "] " + log);
    }

    public static void w(String subTag, String log) {
        Log.w(Common.TAG, "[" + subTag + "] " + log);
    }

    public static void e(String subTag, String log) {
        Log.e(Common.TAG, "[" + subTag + "] " + log);
    }

    public static void v(String subTag, String log) {
        Log.v(Common.TAG, "[" + subTag + "] " + log);
    }
}

