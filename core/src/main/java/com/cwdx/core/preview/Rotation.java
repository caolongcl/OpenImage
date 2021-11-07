package com.cwdx.core.preview;

import androidx.annotation.IntDef;

import java.lang.annotation.Retention;
import java.lang.annotation.RetentionPolicy;

public class Rotation {
  public static final int ROTATION_0 = 0;
  public static final int ROTATION_90 = 90;
  public static final int ROTATION_180 = 180;
  public static final int ROTATION_270 = 270;

  private Rotation() {
  }

  @IntDef({ROTATION_0, ROTATION_90, ROTATION_180, ROTATION_270})
  @Retention(RetentionPolicy.SOURCE)
  public @interface Value {
  }
}
