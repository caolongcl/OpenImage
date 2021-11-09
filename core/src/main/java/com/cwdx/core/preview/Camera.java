package com.cwdx.core.preview;

import android.app.Activity;
import android.content.Context;
import android.graphics.Point;
import android.graphics.SurfaceTexture;
import android.hardware.camera2.CameraAccessException;
import android.hardware.camera2.CameraCaptureSession;
import android.hardware.camera2.CameraCharacteristics;
import android.hardware.camera2.CameraDevice;
import android.hardware.camera2.CameraManager;
import android.hardware.camera2.CaptureRequest;
import android.hardware.camera2.CaptureResult;
import android.hardware.camera2.TotalCaptureResult;
import android.hardware.camera2.params.StreamConfigurationMap;
import android.os.Handler;
import android.os.HandlerThread;
import android.util.Size;
import android.view.Surface;

import androidx.annotation.NonNull;
import androidx.annotation.Nullable;

import com.cwdx.utils.CLog;
import com.google.gson.Gson;
import com.google.gson.GsonBuilder;
import com.google.gson.JsonArray;
import com.google.gson.JsonObject;

import java.util.ArrayList;
import java.util.Arrays;
import java.util.Collections;
import java.util.Comparator;
import java.util.List;
import java.util.concurrent.Semaphore;
import java.util.concurrent.TimeUnit;

/**
 * 管理Camera的接口，提供预览、拍照、录制等操作
 */
public class Camera implements ICamera {
  private static final String TAG = "Camera";
  private static final Comparator<Size> comparator = (left, right) ->
    Long.signum((long) left.getWidth() * left.getHeight() -
      (long) right.getWidth() * right.getHeight());
  private static Camera instance;
  // 监控摄像头是否可用
  private CameraManager.AvailabilityCallback mAvailabilityCallback = new CameraManager.AvailabilityCallback() {
    @Override
    public void onCameraAvailable(@NonNull String cameraId) {
      CLog.v(TAG, cameraId + " available");
    }

    @Override
    public void onCameraUnavailable(@NonNull String cameraId) {
      CLog.v(TAG, cameraId + " unavailable");
    }

    @Override
    public void onCameraAccessPrioritiesChanged() {
      CLog.v(TAG, "accessPrioritiesChanged");
    }
  };

  // 监控闪光灯模式
  private CameraManager.TorchCallback mTorchCallback = new CameraManager.TorchCallback() {
    @Override
    public void onTorchModeUnavailable(@NonNull String cameraId) {
      CLog.v(TAG, cameraId + " TorchModeUnavailable");
    }

    @Override
    public void onTorchModeChanged(@NonNull String cameraId, boolean enabled) {
      CLog.v(TAG, cameraId + " TorchMode " + enabled);
    }
  };
  /**
   * 以下为内部实现
   */
  private List<Surface> mSurfaces;
  private CameraManager mCameraManager;
  private CaptureRequest.Builder mPreviewRequestBuilder;
  private CaptureRequest mPreviewRequest;
  private CameraCaptureSession mCaptureSession;
  private CameraDevice mCameraDevice;
  private Semaphore mCameraOpenCloseLock = new Semaphore(1);
  private ICamera.onError mErrorHandler = null;
  private HandlerThread mWorkerThread;
  private Handler mWorkerHandler;
  // 相机会话阶段回调
  private CameraCaptureSession.CaptureCallback mCaptureCallback
    = new CameraCaptureSession.CaptureCallback() {
    @Override
    public void onCaptureProgressed(@NonNull CameraCaptureSession session,
                                    @NonNull CaptureRequest request,
                                    @NonNull CaptureResult partialResult) {
    }

    @Override
    public void onCaptureCompleted(@NonNull CameraCaptureSession session,
                                   @NonNull CaptureRequest request,
                                   @NonNull TotalCaptureResult result) {
    }

  };
  // 相机的状态改变回调
  private final CameraDevice.StateCallback mStateCallback = new CameraDevice.StateCallback() {

    @Override
    public void onOpened(@NonNull CameraDevice cameraDevice) {
      mCameraOpenCloseLock.release();
      mCameraDevice = cameraDevice;
      createCameraPreviewSession();
    }

    @Override
    public void onDisconnected(@NonNull CameraDevice cameraDevice) {
      mCameraOpenCloseLock.release();
      cameraDevice.close();
      mCameraDevice = null;
    }

    @Override
    public void onError(@NonNull CameraDevice cameraDevice, int error) {
      mCameraOpenCloseLock.release();
      cameraDevice.close();
      mCameraDevice = null;
      switch (error) {
        case ERROR_CAMERA_IN_USE:
        case ERROR_MAX_CAMERAS_IN_USE:
        case ERROR_CAMERA_DISABLED:
        case ERROR_CAMERA_DEVICE:
        case ERROR_CAMERA_SERVICE:
        default:
          if (mErrorHandler != null) {
            mErrorHandler.handleError(error);
          }
          break;
      }
    }
  };

  public static Camera Get() {
    if (instance != null) return instance;

    synchronized (Camera.class) {
      if (instance == null) {
        instance = new Camera();
      }
    }
    return instance;
  }

  /**
   * 以下为提供外部的接口
   */
  @Override
  public void Create(@NonNull Context context) {
    mCameraManager = (CameraManager) context.getApplicationContext()
      .getSystemService(Context.CAMERA_SERVICE);

    queryDeviceParam(context);
    queryCamera(mCameraManager);

    // 创建执行线程
//    mWorkerThread = new HandlerThread("camera_state_cb");
//    mWorkerThread.start();
//    mWorkerHandler = new Handler(mWorkerThread.getLooper());

    mWorkerHandler = new Handler();

    mCameraManager.registerAvailabilityCallback(mAvailabilityCallback, mWorkerHandler);
    mCameraManager.registerTorchCallback(mTorchCallback, mWorkerHandler);
  }

  @Override
  public void Destroy() {
//    mWorkerThread.quitSafely();
//    try {
//      mWorkerThread.join();
//      mWorkerThread = null;
//      mWorkerHandler = null;
//    } catch (InterruptedException e) {
//      e.printStackTrace();
//    }

    mCameraManager.unregisterAvailabilityCallback(mAvailabilityCallback);
    mCameraManager.unregisterTorchCallback(mTorchCallback);
    mCameraManager = null;
  }

  @Override
  public void Open(String id, @NonNull List<Surface> surfaces) {
    mSurfaces = surfaces;
    open(id);
  }

  @Override
  public void Close() {
    close();
  }

  @Override
  public void SetErrorHandler(onError handler) {
    mErrorHandler = handler;
  }

  // 创建相机预览会话
  private void createCameraPreviewSession() {
    try {
      // 创建预览请求，绑定渲染目标
      mPreviewRequestBuilder = mCameraDevice.createCaptureRequest(CameraDevice.TEMPLATE_PREVIEW);
      for (Surface surface : mSurfaces) {
        mPreviewRequestBuilder.addTarget(surface);
      }

      // 创建预览会话
      mCameraDevice.createCaptureSession(mSurfaces, new CameraCaptureSession.StateCallback() {
        @Override
        public void onConfigured(@NonNull CameraCaptureSession cameraCaptureSession) {
          if (mCameraDevice == null) {
            return;
          }

          // 开启预览
          mCaptureSession = cameraCaptureSession;
          try {
            mPreviewRequestBuilder.set(CaptureRequest.FLASH_MODE, CaptureRequest.FLASH_MODE_OFF);
            // 自动对焦
            mPreviewRequestBuilder.set(CaptureRequest.CONTROL_AF_MODE,
              CaptureRequest.CONTROL_AF_MODE_CONTINUOUS_PICTURE);

            mPreviewRequest = mPreviewRequestBuilder.build();
            mCaptureSession.setRepeatingRequest(mPreviewRequest, mCaptureCallback, mWorkerHandler);
          } catch (CameraAccessException e) {
            e.printStackTrace();
          }
        }

        @Override
        public void onConfigureFailed(
          @NonNull CameraCaptureSession cameraCaptureSession) {
          CLog.e(TAG, "创建预览失败");
        }
      }, mWorkerHandler);
    } catch (CameraAccessException e) {
      e.printStackTrace();
    }
  }

  // 打开相机
  private void open(String id) {
    try {
      if (!mCameraOpenCloseLock.tryAcquire(2500, TimeUnit.MILLISECONDS)) {
        throw new RuntimeException("Time out waiting to lock camera opening.");
      }
      mCameraManager.openCamera(id, mStateCallback, mWorkerHandler);
    } catch (CameraAccessException e) {
      e.printStackTrace();
    } catch (InterruptedException e) {
      throw new RuntimeException("Interrupted while trying to lock camera opening.", e);
    } catch (SecurityException e) {
      throw new RuntimeException("No permission while trying to lock camera opening.", e);
    }
  }

  // 关闭相机
  private void close() {
    try {
      mCameraOpenCloseLock.acquire();

      if (mCaptureSession != null) {
        mCaptureSession.close();
        mCaptureSession = null;
      }
      if (mCameraDevice != null) {
        mCameraDevice.close();
        mCameraDevice = null;
      }
    } catch (InterruptedException e) {
      throw new RuntimeException("Interrupted while trying to lock camera closing.", e);
    } finally {
      mCameraOpenCloseLock.release();
    }
  }

  // 获取设备默认参数
  private void queryDeviceParam(@NonNull Context context) {
    Point point = new Point();
    ((Activity) context).getWindowManager().getDefaultDisplay().getSize(point);
    Param.displaySize = new Size(point.x, point.y);
    Param.displayOrientation = ((Activity) context).getWindowManager().getDefaultDisplay().getRotation();
    Param.surfaceOrientation = parseSurfaceRotation(Param.displayOrientation);
  }

  private void queryCamera(CameraManager manager) {
    Param.CAMERA_PARAMS.clear();
    try {
      int count = 0;
      Param param;
      for (String cameraId : manager.getCameraIdList()) {
        CameraCharacteristics characteristics
          = manager.getCameraCharacteristics(cameraId);

        Integer facing = characteristics.get(CameraCharacteristics.LENS_FACING);
        if (facing == null) {
          continue;
        }

        StreamConfigurationMap map = characteristics.get(
          CameraCharacteristics.SCALER_STREAM_CONFIGURATION_MAP);
        if (map == null) {
          continue;
        }

        // 判断是否使前置摄像头或者后置摄像头
        if (facing == CameraCharacteristics.LENS_FACING_FRONT) {
          param = Param.FRONT;
          param.cameraId = cameraId;
          param.facing = "front";
          Param.FRONT_ID = count;
        } else if (facing == CameraCharacteristics.LENS_FACING_BACK) {
          param = Param.BACK;
          param.cameraId = cameraId;
          param.facing = "back";
          Param.BACK_ID = count;
        } else {
          param = new Param();
          param.cameraId = "other_camera" + count;
          param.facing = "other";
        }

        param.id = count++;

        // 获取摄像头拍照方向（相机图像顺时针旋转到设备自然方向一致时的角度）
        param.sensorOrientation = characteristics.get(CameraCharacteristics.SENSOR_ORIENTATION);

        // 获取支持的硬件level
        param.deviceLevel = characteristics.get(CameraCharacteristics.INFO_SUPPORTED_HARDWARE_LEVEL);

        // 支持的分辨率
        param.outputSizes = map.getOutputSizes(SurfaceTexture.class);

        List<Size> allSizes = Arrays.asList(param.outputSizes);
        Collections.sort(allSizes, comparator);

        for (Size s : allSizes) {
          if (AspectRatio.aspectRatio(AspectRatio.RATIO_4_3).equals(AspectRatio.aspectRatio(s.getWidth(), s.getHeight()))) {
            param.size4x3.add(s);
          } else if (AspectRatio.aspectRatio(AspectRatio.RATIO_16_9).equals(AspectRatio.aspectRatio(s.getWidth(), s.getHeight()))) {
            param.size16x9.add(s);
          } else {
            param.sizeOther.add(s);
          }
        }

        // 支持的输出格式
        param.outputFormats = map.getOutputFormats();

        Param.CAMERA_PARAMS.add(param);
      }
    } catch (CameraAccessException e) {
      e.printStackTrace();
    } catch (NullPointerException e) {
      e.printStackTrace();
    }
  }

  private int parseSurfaceRotation(int displayOrientation) {
    int surfaceRotation = 0;
    switch (displayOrientation) {
      case Surface.ROTATION_0:
        surfaceRotation = 0;
        break;
      case Surface.ROTATION_90:
        surfaceRotation = 90;
        break;
      case Surface.ROTATION_180:
        surfaceRotation = 180;
        break;
      case Surface.ROTATION_270:
        surfaceRotation = 270;
        break;
      default:
        CLog.w(TAG, "invalid surfaceRotation " + displayOrientation);
        throw new IllegalStateException("Unknown Rotation");
    }
    return surfaceRotation;
  }


  // 获取各个相机的特性
  private void dumpCamera(@NonNull CameraManager manager, @NonNull String id) {
    CLog.d(TAG, "camera_" + id);
    try {
      CameraCharacteristics characteristics = manager.getCameraCharacteristics(id);
      int size = characteristics.getKeys().size();

      StringBuilder sb = new StringBuilder();
      sb.append(CameraCharacteristics.COLOR_CORRECTION_AVAILABLE_ABERRATION_MODES.getName()).append(":");

      int[] colorCorrect = characteristics.get(CameraCharacteristics.COLOR_CORRECTION_AVAILABLE_ABERRATION_MODES);
      if (colorCorrect != null) {
        for (int c : colorCorrect) {
          sb.append(c).append(" ");
        }
        sb.append("\n");
        CLog.d(TAG, sb.toString());
      }

      sb = new StringBuilder();
      sb.append(CameraCharacteristics.CONTROL_AVAILABLE_SCENE_MODES.getName()).append(":");
      int[] sceneModes = characteristics.get(CameraCharacteristics.CONTROL_AVAILABLE_SCENE_MODES);
      if (sceneModes != null) {
        for (int c : sceneModes) {
          sb.append(c).append(" ");
        }
        sb.append("\n");
        CLog.d(TAG, sb.toString());
      }

    } catch (CameraAccessException e) {
      e.printStackTrace();
    }
  }

  /**
   *
   */
  public static final class Param {
    // 预定义前后摄像头
    public static final int INVALID_ID = -1;
    public static final Param FRONT = new Param();
    public static final Param BACK = new Param();
    public static final List<Param> CAMERA_PARAMS = new ArrayList<>();
    // 设备方向及大小
    public static int displayOrientation = Surface.ROTATION_0;
    // 设备显示角度
    public static int surfaceOrientation;
    public static Size displaySize = new Size(360, 640);
    public static int BACK_ID = 0;
    public static int FRONT_ID = 1;
    public static final int DEFAULT_ID = FRONT_ID;
    public int id;
    public String cameraId;
    public String facing;
    // 摄像头设备角度
    public int sensorOrientation;
    public int deviceLevel;
    public Size[] outputSizes;
    public int[] outputFormats;
    public List<Size> size4x3 = new ArrayList<>();
    public List<Size> size16x9 = new ArrayList<>();
    public List<Size> sizeOther = new ArrayList<>();

    @NonNull
    public String dumpParam() {
      Gson gson = new GsonBuilder()
        .setPrettyPrinting()
        .create();

      JsonObject root = new JsonObject();
      root.addProperty("id", cameraId);
      root.addProperty("facing", facing);
      root.addProperty("surface_rotation", surfaceOrientation);
      root.addProperty("sensor_ori", sensorOrientation);
      root.addProperty("display_size", displaySize.toString());
      JsonArray jsonOutputSizes = new JsonArray();
      // 支持的预览尺寸
      for (Size s : outputSizes) {
        jsonOutputSizes.add(s.toString() + " " + AspectRatio.aspectRatio(s.getWidth(), s.getHeight()).toString());
      }
      root.add("outputSize", jsonOutputSizes);

      return gson.toJson(root);
    }

    @Override
    public String toString() {
      return dumpParam();
    }

    @Override
    public boolean equals(@Nullable Object obj) {
      return obj instanceof Param && id == ((Param) obj).id;
    }


  }

}
