package com.cwdx.core.preview;

@SuppressWarnings("JavaJniMissingFunction")
public class FFmpegUtils {

  public void SetFFmpegDebug(boolean debug) {
    NativeSetFFmpegDebug(debug);
  }

  // 是否开启 ffmpeg 库的调试
  private native void NativeSetFFmpegDebug(boolean debug);
}
