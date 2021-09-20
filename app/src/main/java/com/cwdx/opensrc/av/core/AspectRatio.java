package com.cwdx.opensrc.av.core;

import android.util.Size;

import androidx.annotation.IntDef;

import java.lang.annotation.Retention;
import java.lang.annotation.RetentionPolicy;
import java.security.InvalidParameterException;

public class AspectRatio {
  // 定义预览比例
  /**
   * 4:3 standard aspect ratio.
   */
  public static final int RATIO_4_3 = 0;
  /**
   * 16:9 standard aspect ratio.
   */
  public static final int RATIO_16_9 = 1;

  /**
   * 根据显示区域大小决定
   */
  public static final int RATIO_OTHER = 2;

  private static final Size SIZE_RATIO_4_3 = new Size(4, 3);
  private static final Size SIZE_RATIO_16_9 = new Size(16, 9);
  private static final Size SIZE_RATIO_OTHER = new Size(Integer.MAX_VALUE, 1);

  private AspectRatio() {
  }

  public static float of(@Ratio int ratio) {
    if (ratio == RATIO_4_3) {
      return 4.0f / 3.0f;
    } else if (ratio == RATIO_16_9) {
      return 16.0f / 9.0f;
    } else {
      return Float.POSITIVE_INFINITY;
    }
  }

  public static String from(@Ratio int ratio) {
    if (ratio == RATIO_4_3) {
      return "4:3";
    } else if (ratio == RATIO_16_9) {
      return "16:9";
    } else {
      return "Other";
    }
  }

  public static Size aspectRatio(@Ratio int ratio) {
    if (ratio == RATIO_4_3) {
      return SIZE_RATIO_4_3;
    } else if (ratio == RATIO_16_9) {
      return SIZE_RATIO_16_9;
    } else {
      return SIZE_RATIO_OTHER;
    }
  }

  public static Size aspectRatio(int width, int height) throws InvalidParameterException {
    if (width == 0 || height == 0) {
      throw new InvalidParameterException("width=" + width + "height=" + height);
    }

    Size ratio;
    int c = gcd(Math.max(width, height), Math.min(width, height));
    return new Size(width / c, height / c);
  }

  // 辗转相除法求最大公约数 a > b
  private static int gcd(int a, int b) {
    while (b != 0) {
      int c = b;
      b = a % b;
      a = c;
    }
    return a;
  }

  @IntDef({RATIO_4_3, RATIO_16_9, RATIO_OTHER})
  @Retention(RetentionPolicy.SOURCE)
  public @interface Ratio {
  }

}
