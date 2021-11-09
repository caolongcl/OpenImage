package com.cwdx.core.platform;

import android.app.ActivityManager;
import android.content.Context;
import android.content.pm.ConfigurationInfo;

public class GPUVideo {
  private Context mContext;

  /**
   * 创建 GPUVideo
   *
   * @param context the context
   */
  public GPUVideo(final Context context) {
    if (!supportsOpenGLES3(context)) {
      throw new IllegalStateException("Device is not support OpenGL ES 3.1");
    }

    mContext = context;
  }


  /**
   * 是否支持 OpenGL ES 2.0
   *
   * @param context
   * @return
   */
  private boolean supportsOpenGLES2(final Context context) {
    final ActivityManager activityManager = (ActivityManager)
      context.getSystemService(Context.ACTIVITY_SERVICE);
    final ConfigurationInfo configurationInfo =
      activityManager.getDeviceConfigurationInfo();

    return configurationInfo.reqGlEsVersion >= 0x20000;
  }

  private boolean supportsOpenGLES3(final Context context) {
    final ActivityManager activityManager = (ActivityManager)
      context.getSystemService(Context.ACTIVITY_SERVICE);
    final ConfigurationInfo configurationInfo =
      activityManager.getDeviceConfigurationInfo();
    return configurationInfo.reqGlEsVersion >= 0x30001;
  }
}
