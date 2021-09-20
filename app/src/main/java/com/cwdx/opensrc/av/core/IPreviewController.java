package com.cwdx.opensrc.av.core;

import android.content.Context;
import android.os.Handler;
import android.view.Surface;

import java.io.File;

public interface IPreviewController {
  /**
   * 创建环境，设置资源根目录及加载资源
   */
  void Create(Context context);

  /**
   * 销毁环境
   */
  void Destroy();

  /**
   * 初始化渲染环境
   */
  void Init();

  /**
   * 反初始化渲染环境
   */
  void DeInit();

  /**
   * 根据参数selector选择相机
   *
   * @param selector
   */
  void SelectCamera(CameraSelector selector);

  /**
   * 根据参数selector切换相机
   *
   * @param selector
   */
  void SwitchCamera(CameraSelector selector);

  CameraSelector GetCameraSelector();

  void UpdatePreviewMode();

  /**
   * 开启预览，但不进行渲染
   */
  void Start();

  /**
   * 进行渲染
   */
  void Resume();

  /**
   * 停止渲染
   */
  void Pause();

  /**
   * 停止预览
   */
  void Stop();

  /**
   * 创建EGL预览surface
   *
   * @param surface
   */
  void SetSurface(Surface surface);

  void SetSurfaceSize(int width, int height);

  int GetState();

  void Capture(boolean normal);

  void SetMediaDir(File[] dir);

  void SetMainHandler(Handler handler);

  void Record();

  void StartRecord();

  void StopRecord();

  boolean Recording();

  void SetFloatVar(String name, float[] var);

  void SetBoolVar(String name, boolean var);

  void SetIntVar(String name, int[] var);

  void SetStringVar(String name, String var);

  void EnableFilter(String name, boolean enable);

  void EnableProcess(String name, boolean enable);

  void UpdateTargetPos(int x, int y);

  void SetFFmpegDebug(boolean debug);

  void SetCalibrateParams(int boardSizeWidth, int boardSizeHeight,
                          float boardSquareSizeWidth, float boardSquareSizeHeight,
                          float markerSizeWidth, float markerSizeHeight);

  String GetParamsFromNative(String paramsGroupName);
}
