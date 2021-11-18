package com.cwdx.core.preview;

import android.util.Size;

import androidx.annotation.NonNull;

import com.cwdx.utils.CLog;

import java.util.ArrayList;
import java.util.Arrays;
import java.util.Collections;
import java.util.Comparator;
import java.util.List;

public class CameraSelector {
  public static final String TAG = "CameraSelector";

  public static final CameraSelector FrontCamera = new CameraSelector();
  public static final CameraSelector BackCamera = new CameraSelector();
  public static final CameraSelector DefaultCamera = FrontCamera;

  // 录制一般有标准的比例和大小，故简单起见，设置最小的预览大小
  public static final Size SMALLEST_PREVIEW_SIZE_16_9 = new Size(1920, 1080);
  public static final Size SMALLEST_PREVIEW_SIZE_4_3 = new Size(1440, 1080);
  private static final Comparator<Size> comparator = (left, right) ->
    Long.signum((long) left.getWidth() * left.getHeight() -
      (long) right.getWidth() * right.getHeight());

  static {
    FrontCamera.param = Camera.Param.FRONT;
    FrontCamera.ratio = AspectRatio.RATIO_4_3;
    FrontCamera.rotation = Rotation.ROTATION_0;
    FrontCamera.vFlip = false;
    FrontCamera.hFlip = true;
    FrontCamera.previewSize = new Size(SMALLEST_PREVIEW_SIZE_4_3.getWidth(),
      SMALLEST_PREVIEW_SIZE_4_3.getHeight());

    BackCamera.param = Camera.Param.BACK;
    BackCamera.ratio = AspectRatio.RATIO_4_3;
    BackCamera.rotation = Rotation.ROTATION_0;
    BackCamera.vFlip = false;
    BackCamera.hFlip = false;
    BackCamera.previewSize = new Size(SMALLEST_PREVIEW_SIZE_4_3.getWidth(),
      SMALLEST_PREVIEW_SIZE_4_3.getHeight());
  }

  // 相机固有属性
  public Camera.Param param;
  public @AspectRatio.Ratio
  int ratio;
  // 采集的图像恢复到设备自然方向所需顺时针旋转的角度
  public @Rotation.Value
  int rotation;
  public boolean vFlip;
  public boolean hFlip;
  public Size targetSize;
  public Size previewSize;
  public FilterState filterState;

  public CameraSelector() {
    filterState = new FilterState();
  }

  private static Size chooseOptimalSize(@NonNull List<Size> choices, int ratio, int targetWidth, int targetHeight) {
    Size smallSize;

    switch (ratio) {
      case AspectRatio.RATIO_4_3:
        smallSize = SMALLEST_PREVIEW_SIZE_4_3;
        break;
      case AspectRatio.RATIO_16_9:
        smallSize = SMALLEST_PREVIEW_SIZE_16_9;
        break;
      default:
        smallSize = new Size(targetWidth, targetHeight);
        break;
    }

    for (Size option : choices) {
      if (option.getWidth() >= targetWidth && option.getHeight() >= targetHeight
        && option.getWidth() >= smallSize.getWidth() && option.getHeight() >= smallSize.getHeight()) {
        return option;
      }
    }

    CLog.w(TAG, "Couldn't find any suitable preview size");
    return choices.get(choices.size() - 1);
  }

  private static Size choosePreviewSizeIgnoreTarget(Size[] previewSizes, int ratio) {
    // 根据面积由小到大排序
    List<Size> allSizes = Arrays.asList(previewSizes);
    Collections.sort(allSizes, comparator);

    Size smallSize;

    switch (ratio) {
      case AspectRatio.RATIO_4_3:
        smallSize = SMALLEST_PREVIEW_SIZE_4_3;
        break;
      case AspectRatio.RATIO_16_9:
        smallSize = SMALLEST_PREVIEW_SIZE_16_9;
        break;
      default:
        smallSize = SMALLEST_PREVIEW_SIZE_4_3;
        break;
    }

    for (Size option : allSizes) {
      if (option.getWidth() >= smallSize.getWidth() && option.getHeight() >= smallSize.getHeight()) {
        return option;
      }
    }

    CLog.w(TAG, "Couldn't find any suitable preview size");
    return allSizes.get(allSizes.size() - 1);
  }

  // 从正确aspectratio中选取
  private static Size chooseOptimalSizeByRatio(@NonNull List<Size> choices, int ratio, int targetWidth, int targetHeight, int maxWidth, int maxHeight) {
    List<Size> aspectLists = new ArrayList<>();
    for (Size s : choices) {
      if (AspectRatio.aspectRatio(ratio).equals(
        AspectRatio.aspectRatio(s.getWidth(), s.getHeight()))) {
        aspectLists.add(s);
      }
    }

    Collections.sort(aspectLists, comparator);

    Size target = null;
    for (Size t : aspectLists) {
      if (t.getWidth() >= targetWidth && t.getHeight() >= targetHeight
        && t.getWidth() <= maxWidth && t.getHeight() <= maxHeight) {
        target = t;
      }
    }
    return target;
  }

  // 从所有预览大小选取略大的
  private static Size chooseOptimalSizeBySize(@NonNull List<Size> choices, int targetWidth, int targetHeight, int maxWidth, int maxHeight) {

    // Collect the supported resolutions that are at least as big as the preview Surface
    List<Size> bigEnough = new ArrayList<>();
    // Collect the supported resolutions that are smaller than the preview Surface
    List<Size> notBigEnough = new ArrayList<>();

    for (Size option : choices) {
      if (option.getWidth() <= maxWidth && option.getHeight() <= maxHeight &&
        option.getHeight() == option.getWidth() * targetHeight / targetWidth) {
        if (option.getWidth() >= targetWidth && option.getHeight() >= targetHeight) {
          bigEnough.add(option);
        } else {
          notBigEnough.add(option);
        }
      }
    }

    // Pick the smallest of those big enough. If there is no one big enough, pick the
    // largest of those not big enough.
    if (bigEnough.size() > 0) {
      return Collections.min(bigEnough, comparator);
    } else if (notBigEnough.size() > 0) {
      return Collections.max(notBigEnough, comparator);
    } else {
      CLog.e(TAG, "Couldn't find any suitable preview size");
      return new Size(800, 600);
    }
  }

  public void Select() {
    this.rotation = computePreviewRotation(param.id,
      Camera.Param.surfaceOrientation, param.sensorOrientation);

    if (targetSize != null)
      previewSize = chooseBigPreviewSizeWithTarget(param, ratio, targetSize.getWidth(), targetSize.getHeight());
    else {
      previewSize = chooseBigPreviewSizeIgnoreTarget(param, ratio);
    }

    filterState.Reset();
  }

  public CameraSelector SetCamera(@NonNull Camera.Param param) {
    this.param = param;
    return this;
  }

  public CameraSelector SetRatio(@AspectRatio.Ratio int ratio) {
    this.ratio = ratio;
    return this;
  }

//  public CameraSelector SetPreviewSize(@NonNull Size size) {
//    this.previewSize = size;
//    return this;
//  }

//  param.largestSize = Collections.max(Arrays.asList(param.outputSizes), comparator);

  public CameraSelector SetVFlip(boolean vFlip) {
    this.vFlip = vFlip;
    return this;
  }

  public CameraSelector SetHFlip(boolean hFlip) {
    this.hFlip = hFlip;
    return this;
  }

  public CameraSelector SetSurfaceSize(@NonNull Size size) {
    this.targetSize = size;
    return this;
  }

  /**
   * 计算采集的图像恢复到设备自然方向（即人眼正常看）所需顺时针旋转的角度
   *
   * @param cameraId           相机ID
   * @param surfaceOrientation 设备方向
   * @param sensorOrientation  相机放置方向
   * @return 预览需旋转角度
   */
  private int computePreviewRotation(int cameraId, int surfaceOrientation, int sensorOrientation) {
    int previewRotation = 0;
    if (cameraId == Camera.Param.FRONT_ID) {
      previewRotation = (sensorOrientation + surfaceOrientation) % 360;
//      previewRotation = (360 - previewRotation) % 360;  // compensate the mirror
    } else if (cameraId == Camera.Param.BACK_ID) {
      previewRotation = (sensorOrientation - surfaceOrientation + 360) % 360;
    }

    return previewRotation;
  }

  // 选取宽高都略大于预览区域的预览帧大小
  private Size choosePreviewSize(Size[] previewSizes, int previewRotation, int ratio, int targetWidth, int targetHeight) {
    CLog.d(TAG, "expect_aspect:" + AspectRatio.from(ratio));
    CLog.d(TAG, "surface_size: " + targetWidth + "x" + targetHeight);

    boolean swappedDimensions = false;
    if (previewRotation == Rotation.ROTATION_0 || previewRotation == Rotation.ROTATION_180) {
      swappedDimensions = false;
    } else if (previewRotation == Rotation.ROTATION_90 || previewRotation == Rotation.ROTATION_270) {
      swappedDimensions = true;
    } else {
      CLog.e(TAG, "preview rotation is invalid: " + Camera.Param.displayOrientation);
    }

    int rotatedPreviewWidth = targetWidth;
    int rotatedPreviewHeight = targetHeight;

    if (swappedDimensions) {
      rotatedPreviewWidth = targetHeight;
      rotatedPreviewHeight = targetWidth;

      CLog.d(TAG, "rotate surface_size: " + rotatedPreviewWidth + "x" + rotatedPreviewHeight);
    }

    // 根据面积由小到大排序
    List<Size> allSizes = Arrays.asList(previewSizes);
    Collections.sort(allSizes, comparator);

    return chooseOptimalSize(allSizes, ratio, rotatedPreviewWidth, rotatedPreviewHeight);
  }

  // 选取相应比例最大的预览大小，不考虑target size
  private Size chooseBigPreviewSizeIgnoreTarget(Camera.Param param, int ratio) {
    Size preview;

    switch (ratio) {
      case AspectRatio.RATIO_4_3:
        if (!param.size4x3.isEmpty()) {
          preview = param.size4x3.get(param.size4x3.size() - 1);
        } else {
          preview = SMALLEST_PREVIEW_SIZE_4_3;
        }
        break;
      case AspectRatio.RATIO_16_9:
        if (!param.size16x9.isEmpty()) {
          preview = param.size16x9.get(param.size16x9.size() - 1);
        } else {
          preview = SMALLEST_PREVIEW_SIZE_16_9;
        }
        break;
      default:
        if (!param.sizeOther.isEmpty()) {
          preview = param.sizeOther.get(param.sizeOther.size() - 1);
        } else {
          preview = SMALLEST_PREVIEW_SIZE_4_3;
        }
        break;
    }

    CLog.d(TAG, "preview size " + preview.getWidth() + " " + preview.getHeight());

    return preview;
  }

  // 选取相应比例最大的预览大小，考虑target size
  private Size chooseBigPreviewSizeWithTarget(Camera.Param param, int ratio, int targetWidth, int targetHeight) {
    Size preview = null;

    switch (ratio) {
      case AspectRatio.RATIO_4_3:
        if (!param.size4x3.isEmpty()) {
          preview = param.size4x3.get(param.size4x3.size() - 1);
        } else {
          preview = SMALLEST_PREVIEW_SIZE_4_3;
        }
        break;
      case AspectRatio.RATIO_16_9:
        if (!param.size16x9.isEmpty()) {
          preview = param.size16x9.get(param.size16x9.size() - 1);
        } else {
          preview = SMALLEST_PREVIEW_SIZE_16_9;
        }
        break;
      default:
        if (param.outputSizes != null) {
          List<Size> sizes = Arrays.asList(param.outputSizes);
          sizes.sort(comparator);

          for (Size option : sizes) {
            if (option.getWidth() >= targetWidth && option.getHeight() >= targetHeight) {
              preview = option;
              break;
            }
          }

          if (preview == null)
            preview = SMALLEST_PREVIEW_SIZE_4_3;
        } else {
          throw new IllegalArgumentException("outputSizes is null");
        }
        break;
    }

    CLog.d(TAG, "preview size " + preview.getWidth() + " " + preview.getHeight());

    return preview;
  }

  @Override
  public String toString() {
    return "CameraSelector{" +
      "param=" + param +
      ", ratio=" + ratio +
      ", rotation=" + rotation +
      ", vFlip=" + vFlip +
      ", hFlip=" + hFlip +
      ", targetSize=" + targetSize +
      ", previewSize=" + previewSize +
      '}';
  }

  public static class FilterState {
    public boolean color_invert = false;
    public boolean grayscale = false;
    public boolean time_color = false;
    public boolean blur = false;
    public boolean mosaic = false;
    public boolean whirlpool = false;
    public boolean exposure = false;
    public boolean base = false;
    public boolean hue = false;
    public boolean face_detect = false;
    public boolean opencv_ar = false;
    public boolean yoloV5_detect = false;
    public float exposure_value = 0.0f;
    public float brightness_value = 0.0f;
    public float contrast_value = 1.0f;
    public float saturation_value = 1.0f;
    public float hue_value = 0.0f;

    public FilterState() {
      Reset();
    }

    public void Reset() {
      color_invert = false;
      grayscale = false;
      time_color = false;
      blur = false;
      mosaic = false;
      whirlpool = false;

      exposure = false;
      exposure_value = 0.0f;

      base = false;
      brightness_value = 0.0f;
      contrast_value = 1.0f;
      saturation_value = 1.0f;

      hue = false;
      hue_value = 0.0f;

      face_detect = false;
      opencv_ar = false;
      yoloV5_detect = false;
    }
  }
}
