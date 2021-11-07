package com.cwdx.core.preview;

import android.content.Context;
import android.content.SharedPreferences;
import android.graphics.Bitmap;
import android.graphics.PixelFormat;
import android.graphics.SurfaceTexture;
import android.media.Image;
import android.media.ImageReader;
import android.os.Handler;
import android.os.HandlerThread;
import android.view.Surface;
import android.widget.Toast;

import androidx.annotation.IntDef;

import com.cwdx.core.platform.HWEncoder;
import com.cwdx.utils.ImageUtils;
import com.cwdx.utils.CLog;

import java.io.File;
import java.io.FileNotFoundException;
import java.io.FileOutputStream;
import java.io.IOException;
import java.lang.annotation.Retention;
import java.lang.annotation.RetentionPolicy;
import java.security.InvalidParameterException;
import java.util.ArrayList;
import java.util.List;

@SuppressWarnings("JavaJniMissingFunction")
public class PreviewController implements IPreviewController {
  public static final int MSG_ADD_PICTURE = 0;
  public static final int MSG_ADD_VIDEO = 1;
  public static final int MAIN_SURFACE = 0;
  public static final int CAPTURE_SURFACE = 1;
  public static final int RECORD_SURFACE = 2;
  private static final String TAG = "PreviewController";
  private final FFmpegUtils mFFmpegUtils = new FFmpegUtils();
  private SurfaceTexture mSurfaceTexture;
  private CameraSelector mSelector;
  private File[] mMediaDirs;
  private boolean mNormalCapture;
  private Handler mMainHandler;
  private ImageReader mImageReader;
  private Handler mWorkerHandler;
  private HandlerThread mWorkThread;
  private HWEncoder mHwEncoder;
  private @State.Value
  int mState = State.STATE_DEINIT;
  private boolean mHasInit = false;
  private boolean mHasSurface = true;
  private boolean mHasSelectCamera = false;
  private boolean mRecording = false;
  private Context mContext;
  private File mLastVideoFile;
  /**
   * 处理照片拍摄
   */
  private byte[] mData;

  @Override
  public void Create(Context context) {
    assert context != null;
    assert context.getExternalFilesDir(null) != null;

    mContext = context;
    // 初始化相机
    Camera.Get().Create(context);

    NativeCreate();
  }

  @Override
  public void Destroy() {
    NativeDestroy();

    // 释放相机
    Camera.Get().Destroy();

    mContext = null;
  }

  @Override
  public synchronized void Init() {
    if (!mHasInit) {
      NativeInit();
      mState = State.STATE_INIT;
      mHasInit = true;

      mWorkThread = new HandlerThread("worker1");
      mWorkThread.start();
      mWorkerHandler = new Handler(mWorkThread.getLooper());

      mHwEncoder = new HWEncoder();
    }
  }

  @Override
  public synchronized void DeInit() {
    if (mHasInit) {
//      Utils.d(TAG, "DeInit");
      NativeDeInit();
      mState = State.STATE_DEINIT;
      mHasInit = false;

      if (mImageReader != null) {
        mImageReader.setOnImageAvailableListener(null, null);
        mImageReader.close();
        mImageReader = null;
      }

      mWorkThread.quitSafely();
      try {
        mWorkThread.join();
      } catch (InterruptedException e) {
        e.printStackTrace();
      }
      mWorkThread = null;
      mWorkerHandler = null;
      mHwEncoder = null;
    }

    mMainHandler = null;
  }

  @Override
  public synchronized void SelectCamera(CameraSelector selector) {
    if (selector != null) {
      mSelector = selector;
    } else {
      mSelector = CameraSelector.DefaultCamera;
    }

    mSelector.Select();
    mHasSelectCamera = true;

    CLog.d(TAG, "SelectCamera " + mSelector);
  }

  @Override
  public synchronized void SwitchCamera(CameraSelector selector) {
    CLog.d(TAG, "Switch");

//    if (mSelector == null || !mSelector.equals(selector)) {
    SelectCamera(selector);
//    }

    Pause();
    Stop();
    UpdatePreviewMode();
    Start();
    Resume();
  }

//  @Override
//  public synchronized void SetSurfaceSize(int width, int height) {
//    //    Utils.d(TAG, "SetSurfaceSize (w h)=(" + width + " " + height + ")");
//    if (mHasSelectCamera) {
//      mSelector.SetSurfaceSize(new Size(width, height));
//    }
//
//    if (mHasSurface) {
//      NativeSetSurfaceSize(width, height);
//    }
//  }

  @Override
  public CameraSelector GetCameraSelector() {
    return mSelector;
  }

  @Override
  public void UpdatePreviewMode() {
    if (!mHasSelectCamera) return;

    if (mHasInit) {
      NativeSetPreviewMode(mSelector.rotation, mSelector.ratio,
          mSelector.vFlip, mSelector.hFlip,
          mSelector.previewSize.getWidth(), mSelector.previewSize.getHeight());
    }
  }

  @Override
  public synchronized void Start() {
    if (!mHasInit) {
      Init();
      SelectCamera(null);
      CLog.d(TAG, "Init from Start");
    }

    if (!mHasSelectCamera) return;

    if (mState == State.STATE_INIT || mState == State.STATE_STOP) {
//      Utils.d(TAG, "Start");
      NativeStart();
      mState = State.STATE_START;
    }
  }

  @Override
  public synchronized void Resume() {
    if (!mHasSelectCamera) return;

    if (mState == State.STATE_PAUSE || mState == State.STATE_START) {
//      Utils.d(TAG, "Resume");
      NativeResume();
      mState = State.STATE_RESUME;
    }
  }

  @Override
  public synchronized void Pause() {
    if (!mHasSelectCamera) return;

    if (mState == State.STATE_RESUME) {
//      Utils.d(TAG, "Pause");
      NativePause();
      mState = State.STATE_PAUSE;
    }
  }

  @Override
  public synchronized void Stop() {
    if (!mHasSelectCamera) return;

    if (mState == State.STATE_START || mState == State.STATE_PAUSE) {
//      Utils.d(TAG, "Stop");
      NativeStop();
      mState = State.STATE_STOP;
    }
  }

  @Override
  public void SetSurface(Surface surface) {
    if (mHasInit) {
      NativeSetSurface(MAIN_SURFACE, surface);
      mHasSurface = true;
    }
  }

  @Override
  public void SetSurfaceSize(int width, int height) {
    if (mHasInit) {
      NativeSetSurfaceSize(width, height);
      mHasSurface = true;
    }
  }

  @Override
  public synchronized int GetState() {
    return mState;
  }

  @Override
  public void Capture(boolean normal) {
    if (!mHasSelectCamera) return;

    if (mState == State.STATE_RESUME) {
      mNormalCapture = normal;
      NativeCapture();
    }
  }

  @Override
  public void SetMediaDir(File[] dir) {
    mMediaDirs = dir;
    NativeSetStringVar("basedir", mMediaDirs[0].getAbsolutePath());
    NativeSetStringVar("funcdir", mMediaDirs[1].getAbsolutePath());
  }

  @Override
  public void SetMainHandler(Handler handler) {
    mMainHandler = handler;
  }

  @Override
  public void Record() {
    if (mRecording) {
      StopRecord();
    } else {
      StartRecord();
    }
  }

  @Override
  public void StartRecord() {
    if (!mHasSelectCamera) return;
    if (mState == State.STATE_RESUME) {
      NativeRecord(true, 30, 0.2f);
      mRecording = true;
    }
  }

  @Override
  public void StopRecord() {
    if (!mHasSelectCamera) return;
    if (mState == State.STATE_RESUME) {
      NativeRecord(false, 0, 0);
      mRecording = false;
    }
  }

  @Override
  public boolean Recording() {
    return mRecording;
  }

  // 设置参数给native
  @Override
  public void SetFloatVar(String name, float[] var) {
    NativeSetFloatVar(name, var);
  }

  @Override
  public void SetBoolVar(String name, boolean var) {
    NativeSetBoolVar(name, var);
  }

  @Override
  public void SetIntVar(String name, int[] var) {
    NativeSetIntVar(name, var);
  }

  @Override
  public void SetStringVar(String name, String var) {
    NativeSetStringVar(name, var);
  }

  @Override
  public void EnableFilter(String name, boolean enable) {
    if (mHasInit) {
      NativeEnableFilter(name, enable);
    }
  }

  @Override
  public void EnableProcess(String name, boolean enable) {
    if (mHasInit) {
      NativeEnableProcess(name, enable);
    }
  }

  @Override
  public void UpdateTargetPos(int x, int y) {
    NativeUpdateTargetPos(x, y);
  }

  @Override
  public void SetFFmpegDebug(boolean debug) {
    mFFmpegUtils.SetFFmpegDebug(debug);
  }

  @Override
  public void SetCalibrateParams(int boardSizeWidth, int boardSizeHeight,
                                 float boardSquareSizeWidth, float boardSquareSizeHeight,
                                 float markerSizeWidth, float markerSizeHeight) {
    if (mHasInit) {
      NativeSetCalibrateParams(boardSizeWidth, boardSizeHeight,
          boardSquareSizeWidth, boardSquareSizeHeight,
          markerSizeWidth, markerSizeHeight);
    }
  }

  @Override
  public String GetParamsFromNative(String paramsGroupName) {
    if (mHasInit && paramsGroupName != null) {
      return NativeGetParams(paramsGroupName);
    }
    return "";
  }

  private void FromNativePostInfo(String info) {
    if (mContext != null && mMainHandler != null) {
      mMainHandler.post(() -> {
        Toast.makeText(mContext, info, Toast.LENGTH_LONG).show();
      });
    }
  }

  /**
   * 将assets的资源拷贝到APP目录下
   */
  private String FromNativeLoadAssets() {
    SharedPreferences preferences = mContext.getSharedPreferences("core", Context.MODE_PRIVATE);
    File dir = mContext.getExternalFilesDir(null);
    if (dir == null) throw new InvalidParameterException("no ExternalFiles");

    boolean hasCopyRes = preferences.getBoolean("res_copied", false);
//    boolean hasCopyRes = false;
    CLog.d(TAG, "res has copied " + hasCopyRes);
    if (!hasCopyRes) {
      CLog.d(TAG, "copy res... ");
      AssetCopier.CopyAllAssets(mContext.getApplicationContext(), dir.getAbsolutePath());
      preferences.edit().putBoolean("res_copied", true).apply();
    }

    return dir.getAbsolutePath();
  }

  private void FromNativeConfigEncoder(boolean start, int width, int height, int fps, int bitrate) {
    CLog.d(TAG, "start " + start + " wh(" + width + " " + height + " fps " + fps + " bitrate " + bitrate);
    if (start) {
      mLastVideoFile = ImageUtils.createFile(mMediaDirs[0], ImageUtils.FILENAME, ImageUtils.VIDEO_EXTENSION);
      mHwEncoder.PrepareVideoEncoder(width, height, fps, bitrate, mLastVideoFile.getAbsolutePath());

      NativeSetSurface(RECORD_SURFACE, mHwEncoder.GetInputSurface());
    } else {
      mHwEncoder.ReleaseVideoEncoder();
      if (mMainHandler != null && mLastVideoFile != null) {
        mMainHandler.obtainMessage(MSG_ADD_VIDEO, mLastVideoFile).sendToTarget();
      }
      mLastVideoFile = null;
    }
  }

  // 底层预览texture
  private void FromNativeSetPreviewTexture(int texId) {
    CLog.d(TAG, "FromNativeSetPreviewTexture id:" + texId);

    // 设置预览目标及大小
    mSurfaceTexture = new SurfaceTexture(texId);
    mSurfaceTexture.setDefaultBufferSize(mSelector.previewSize.getWidth(),
        mSelector.previewSize.getHeight());

    // 当Camera采集到一帧数据时，通知native渲染
    mSurfaceTexture.setOnFrameAvailableListener(surfaceTexture -> {
      NativeNotifyFrameAvailable();
    });
  }

  private void FromNativeStartPreview() {
    CLog.d(TAG, "FromNativeStartPreview");

    if (mSurfaceTexture != null) {
      List<Surface> surfaceList = new ArrayList<>();
      surfaceList.add(new Surface(mSurfaceTexture));

      Camera.Get().Open(mSelector.param.cameraId, surfaceList);
    }
  }

  private void FromNativeUpdateFrame() {
    if (mSurfaceTexture != null) {
//      Utils.d(TAG, "FromNativeUpdateFrame");
      mSurfaceTexture.updateTexImage();
    }
  }

  private void FromNativeStopPreview() {
    CLog.d(TAG, "FromNativeStopPreview");

    Camera.Get().Close();

    mSurfaceTexture.setOnFrameAvailableListener(null);
    mSurfaceTexture.release();
    mSurfaceTexture = null;
  }

  private void FromNativeCreateImageReader(int width, int height) {
    CLog.d(TAG, "FromNativeCreateImageReader " + width + " " + height);

    if (mImageReader != null) {
      mImageReader.setOnImageAvailableListener(null, null);
      mImageReader.close();
      mImageReader = null;
    }

    mData = new byte[width * height * 4];

    mImageReader = ImageReader.newInstance(width, height,
        PixelFormat.RGBA_8888, 2);
    mImageReader.setOnImageAvailableListener(reader -> {
      Image image = reader.acquireNextImage();
      // 处理像素及保存图片
      handleImage(image);

      image.close();
    }, mWorkerHandler);

    // 存储拍摄的照片
    Surface surface = mImageReader.getSurface();
    NativeSetSurface(CAPTURE_SURFACE, surface);
  }

  private void handleImage(Image image) {
    //获取所有平面，由于RGBA只有一个平面，所以只使用到平面0
    Image.Plane[] planes = image.getPlanes();
    int width = image.getWidth();
    int height = image.getHeight();
    int rowStride = planes[0].getRowStride();
    int pixelStride = planes[0].getPixelStride();
    //计算padding
    int rowPadding = rowStride - pixelStride * width;

    CLog.d(TAG, "image row padding:" + rowPadding);
    //把数据从buffer拷贝到data，取出用来对其的无效数据
    ImageUtils.copyToByteArray(planes[0].getBuffer(), width, height, rowPadding, mData);

    saveImage(mData, width, height);
  }

  private void saveImage(byte[] data, int width, int height) {
    if (data == null) return;

    int[] color = ImageUtils.convert(data, width, height);
    Bitmap bitmap = Bitmap.createBitmap(color, width, height, Bitmap.Config.ARGB_8888);

    File file;
    if (mNormalCapture) {
      file = ImageUtils.createFile(mMediaDirs[0], ImageUtils.FILENAME, ImageUtils.PHOTO_EXTENSION);
    } else {
      file = ImageUtils.createFile(mMediaDirs[1], ImageUtils.FILENAME, ImageUtils.CALIBRATE_EXTENSION);

//            float ratio = 640.0f / (float) bitmap.getWidth();
//            Matrix scale = new Matrix();
//            scale.postScale(ratio, ratio);
//            bitmap = Bitmap.createBitmap(bitmap, 0, 0, bitmap.getWidth(), bitmap.getHeight(), scale, true);
    }

    try (FileOutputStream output = new FileOutputStream(file)) {
      if (bitmap.compress(Bitmap.CompressFormat.JPEG, 100, output)) {
        output.flush();
      }
    } catch (FileNotFoundException e) {
      e.printStackTrace();
    } catch (IOException e) {
      e.printStackTrace();
    }
    bitmap.recycle();

    if (mMainHandler != null) {
      mMainHandler.obtainMessage(MSG_ADD_PICTURE, file).sendToTarget();
    }
  }

  private boolean validState() {
    return mState == State.STATE_INIT
        || mState == State.STATE_START
        || mState == State.STATE_RESUME
        || mState == State.STATE_PAUSE;
  }

  private native void NativeCreate();

  private native void NativeDestroy();

  /**
   * 创建、销毁EGL环境
   */
  private native void NativeInit();

  private native void NativeDeInit();

  /**
   * @param rotate 采集的图像恢复自然方向顺时针旋转的角度
   * @param ratio  采集的预览比例
   * @param width  预览宽
   * @param height 预览高
   */
  private native void NativeSetPreviewMode(int rotate, int ratio, boolean vFlip, boolean hFlip,
                                           int width, int height);

  /**
   * 启动、开始、暂停、停止预览
   */
  private native void NativeStart();

  private native void NativeResume();

  private native void NativePause();

  private native void NativeStop();

  /**
   * Camera采集到一帧，通知native渲染
   */
  private native void NativeNotifyFrameAvailable();

  private native void NativeSetRatio(int ratio);

  /**
   * 设置是否垂直翻转
   *
   * @param vFlip 标志
   */
  private native void NativeSetVFlip(boolean vFlip);

  /**
   * 设置是否水平翻转
   *
   * @param hFlip 标志
   */
  private native void NativeSetHFlip(boolean hFlip);

  /**
   * 拍照：将ImageReader的surface设置给底层
   * 录制：将MediaCodec的Surfaces设置给底层
   *
   * @param type    0：主界面 1：拍照 2：录制
   * @param surface surface
   */
  private native void NativeSetSurface(int type, Surface surface);

  /**
   * 设置预览显示大小
   */
  private native void NativeSetSurfaceSize(int width, int height);

  /**
   * 拍摄一帧
   */
  private native void NativeCapture();

  private native void NativeSetFloatVar(String name, float[] var);

  private native void NativeSetIntVar(String name, int[] var);

  private native void NativeSetStringVar(String name, String var);

  private native void NativeSetBoolVar(String name, boolean var);

  private native void NativeEnableFilter(String name, boolean enable);

  private native void NativeEnableProcess(String name, boolean enable);

  // TODO : 更为复杂的滤镜链
  private native void NativeEnableSerialFilter(String name, String[] filtersName, boolean enable);

  private native void NativeEnableParallelFilter(String name, String[] filtersName, boolean enable);

  private native void NativeUpdateTargetPos(int x, int y);

  /**
   * 视频录制
   */
  private native void NativeRecord(boolean start, int fps, float factor);

  private native void NativeSetCalibrateParams(int boardSizeWidth, int boardSizeHeight,
                                               float boardSquareSizeWidth, float boardSquareSizeHeight,
                                               float markerSizeWidth, float markerSizeHeight);

  private native String NativeGetParams(String paramGroupName);

  /**
   * 定义预览状态
   */
  public static class State {
    public static final int STATE_INIT = 0;
    public static final int STATE_START = 1;
    public static final int STATE_RESUME = 2;
    public static final int STATE_PAUSE = 3;
    public static final int STATE_STOP = 4;
    public static final int STATE_DEINIT = 5;


    @IntDef({STATE_INIT, STATE_START, STATE_RESUME, STATE_PAUSE, STATE_STOP, STATE_DEINIT})
    @Retention(RetentionPolicy.SOURCE)
    public @interface Value {
    }
  }
}
