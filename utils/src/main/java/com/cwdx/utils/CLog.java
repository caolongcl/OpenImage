package com.cwdx.utils;

import android.util.Log;

public final class CLog {
  private static final String spaces = "             ";

  // Log
  public static void d(String subTag, String log) {
    Log.d(Common.TAG, tagAlign(subTag) + log);
  }

  public static void w(String subTag, String log) {
    Log.w(Common.TAG, tagAlign(subTag) + log);
  }

  public static void e(String subTag, String log) {
    Log.e(Common.TAG, tagAlign(subTag) + log);
  }

  public static void v(String subTag, String log) {
    Log.v(Common.TAG, tagAlign(subTag) + log);
  }

  private static String tagAlign(String tag) {
    if (tag.length() >= 13) {
      return "[" + tag.substring(0, 13) + "] ";
    } else {
      return "[" + tag + spaces.substring(0, 13 - tag.length()) + "] ";
    }
  }
}
