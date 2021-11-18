package com.cwdx.opensrc.av;

import android.content.Context;
import android.media.MediaScannerConnection;
import android.os.Bundle;
import android.os.Handler;
import android.os.Message;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.webkit.MimeTypeMap;
import android.widget.Toast;

import androidx.annotation.NonNull;
import androidx.annotation.Nullable;
import androidx.fragment.app.Fragment;

import com.cwdx.core.platform.GPUVideo;
import com.cwdx.core.preview.AspectRatio;
import com.cwdx.core.preview.CameraSelector;
import com.cwdx.core.preview.IPreviewController;
import com.cwdx.core.preview.PreviewController;
import com.cwdx.core.preview.PreviewView;
import com.cwdx.opensrc.MainActivity;
import com.cwdx.opensrc.R;
import com.cwdx.opensrc.av.view.CalibrateParamsDialog;
import com.cwdx.opensrc.av.view.CameraModeSelectView;
import com.cwdx.opensrc.av.view.CameraParamSelectView;
import com.cwdx.utils.CLog;

import org.json.JSONException;
import org.json.JSONObject;

import java.io.File;
import java.lang.ref.WeakReference;
import java.util.ArrayList;
import java.util.List;


public class Camera2PreviewFragment extends Fragment {
  public static final String TAG = "Camera2PreviewFragment";

  private static Camera2PreviewFragment sInstance;
  private PreviewView mPreviewView;
  private List<String> mCharsList = new ArrayList<>();
  private IPreviewController mPreviewController = new PreviewController();
  private CameraModeSelectView mCameraModeSelectV;
  private CameraParamSelectView mCameraParamSelectView;
  private ViewGroup mCameraParamDetailVG;
  private CalibrateParamsDialog mCalibrateParamsDialog;
  private File[] mediaDirs;

  public static Camera2PreviewFragment getInstance() {
    if (sInstance != null) return sInstance;

    synchronized (Camera2PreviewFragment.class) {
      if (sInstance == null) {
        sInstance = new Camera2PreviewFragment();
      }
    }
    return sInstance;
  }

  public static void releaseFragment() {
    sInstance = null;
  }

  @Override
  public void onCreate(@Nullable Bundle savedInstanceState) {
    super.onCreate(savedInstanceState);
    CLog.d(TAG, "onCreate");

    GPUVideo gpuVideo = new GPUVideo(getContext());
  }

  @Nullable
  @Override
  public View onCreateView(@NonNull LayoutInflater inflater, @Nullable ViewGroup container, @Nullable Bundle savedInstanceState) {
    return inflater.inflate(R.layout.fragment_camera2_preview, container, false);
  }

  @Override
  public void onViewCreated(@NonNull View view, @Nullable Bundle savedInstanceState) {
    CLog.d(TAG, "onViewCreated");

    mPreviewView = view.findViewById(R.id.camera2_preview);

    mCameraModeSelectV = view.findViewById(R.id.camera_mode_view);
    mCameraParamSelectView = view.findViewById(R.id.camera_param_view);

    mCameraParamDetailVG = view.findViewById(R.id.camera_param_detail_container);
    mCameraParamSelectView.SetDetailContainerVG(mCameraParamDetailVG);

    mPreviewView.setOnTouchListener((v, e) -> {
      mPreviewView.performClick();
      mPreviewView.OnTouch(v, e);
      mCameraParamSelectView.HideDetailContainerVG();
      return true;
    });

    mediaDirs = ((MainActivity) getActivity()).getMediaDir();
    mPreviewView.SetController(mPreviewController);
    mPreviewView.Create(getContext());
    mPreviewView.SetFFmpegDebug(true);
    mPreviewView.SetMediaDir(mediaDirs);
    mPreviewView.SetMainHandler(new MyHandler(new WeakReference<>(getContext())));

    setSettingParamView(mCameraParamSelectView);

    mCalibrateParamsDialog = new CalibrateParamsDialog();
    mCalibrateParamsDialog.SetCallback((confirm, calibrateData) -> {
        if (confirm) {
          mPreviewView.SetCalibrateParams(calibrateData.boardSizeWidth, calibrateData.boardSizeHeight,
            calibrateData.boardSquareWidth, calibrateData.boardSquareHeight,
            calibrateData.markerWidth, calibrateData.markerHeight);
        }
      }
    );
  }

  @Override
  public void onStart() {
    CLog.d(TAG, "onStart");
    super.onStart();
  }

  @Override
  public void onResume() {
    CLog.d(TAG, "onResume");
    super.onResume();

    mCameraModeSelectV.SetSwitchCameraFunc(() -> {
      CameraSelector cur = mPreviewView.GetCameraSelector();
      mPreviewView.SwitchCamera(cur != CameraSelector.BackCamera ? CameraSelector.BackCamera : CameraSelector.FrontCamera);
    });
    mCameraModeSelectV.SetCaptureFunc((capture, normal) -> {
      if (capture) {
        mPreviewView.Capture(normal);
        return true;
      } else {
        mPreviewView.Record();
        return mPreviewView.Recording();
      }
    });

    // 长按弹出设置校正参数弹框
    mCameraModeSelectV.SetCalibrateParams(() -> {
      if (!mCalibrateParamsDialog.isVisible()) {
        String params = mPreviewView.GetParamsFromNative("calibrate_params");
        if (params == null || params.equals("")) {
          CLog.e(TAG, "get calibrate_params failed");
          return;
        }

        CLog.d(TAG, "get calibrate_params: " + params);

        try {
          JSONObject paramsJson = new JSONObject(params);
          int boardSizeWidth = paramsJson.getInt("board_width");
          int boardSizeHeight = paramsJson.getInt("board_height");
          float boardSquareWidth = (float) paramsJson.getDouble("board_square_width");
          float boardSquareHeight = (float) paramsJson.getDouble("board_square_height");
          float markerWidth = (float) paramsJson.getDouble("marker_width");
          float markerHeight = (float) paramsJson.getDouble("marker_height");

          mCalibrateParamsDialog.UpdateCalibrateData(new CalibrateParamsDialog
            .CalibrateData(boardSizeWidth, boardSizeHeight,
            boardSquareWidth, boardSquareHeight,
            markerWidth, markerHeight));

          mCalibrateParamsDialog.show(getFragmentManager(), "CalibrateParams");
        } catch (JSONException e) {
          CLog.e(TAG, "json parse failed :" + params);
        }
      }
    });
    // 短按开始校正
    mCameraModeSelectV.SetCalibrateFunc(() ->
      mPreviewView.EnableProcess("CalibrateCamera", true));
  }

  @Override
  public void onPause() {
    CLog.d(TAG, "onPause");
    super.onPause();
  }

  @Override
  public void onStop() {
    CLog.d(TAG, "onStop");
    super.onStop();
  }

  @Override
  public void onDestroyView() {
    CLog.d(TAG, "onDestroyView");
    mCameraParamSelectView.HideDetailContainerVG();
    mCalibrateParamsDialog.SetCallback(null);
    super.onDestroyView();
  }

  @Override
  public void onDestroy() {
    CLog.d(TAG, "onDestroy");
    mPreviewView.Destroy();
    super.onDestroy();
  }

  private void setSettingParamView(CameraParamSelectView view) {
    View settingsView = view.GetParamSelectView();
    if (settingsView != null) {
      // 分辨率切换
      settingsView.findViewById(R.id.aspect_ratio_3_4).setOnClickListener(v -> {
        CLog.v(TAG, "set aspect to 3:4");
        if (mPreviewView.Recording()) return;
        mPreviewView.GetCameraSelector().SetRatio(AspectRatio.RATIO_4_3);
        mPreviewView.SwitchCamera(mPreviewController.GetCameraSelector());
      });
      settingsView.findViewById(R.id.aspect_ratio_9_16).setOnClickListener(v -> {
        CLog.v(TAG, "set aspect to 9:16");
        if (mPreviewView.Recording()) return;
        mPreviewView.GetCameraSelector().SetRatio(AspectRatio.RATIO_16_9);
        mPreviewView.SwitchCamera(mPreviewController.GetCameraSelector());
      });
      settingsView.findViewById(R.id.aspect_ratio_normal).setOnClickListener(v -> {
        CLog.v(TAG, "set aspect to normal");
        if (mPreviewView.Recording()) return;
        mPreviewView.GetCameraSelector().SetRatio(AspectRatio.RATIO_OTHER);
        mPreviewView.SwitchCamera(mPreviewController.GetCameraSelector());
      });

      settingsView.findViewById(R.id.hflip).setOnClickListener(v -> {
        CLog.v(TAG, "hflip");
        mPreviewView.GetCameraSelector().SetHFlip(!mPreviewView.GetCameraSelector().hFlip);
        mPreviewView.SetBoolVar("hflip", mPreviewView.GetCameraSelector().hFlip);
      });
      settingsView.findViewById(R.id.vflip).setOnClickListener(v -> {
        CLog.v(TAG, "vflip");
        mPreviewView.GetCameraSelector().SetVFlip(!mPreviewView.GetCameraSelector().vFlip);
        mPreviewView.SetBoolVar("vflip", mPreviewView.GetCameraSelector().vFlip);
      });

      // 滤镜
      settingsView.findViewById(R.id.color_invert).setOnClickListener(v -> {
        CLog.v(TAG, "color invert");
        mPreviewView.GetCameraSelector().filterState.color_invert = !mPreviewView.GetCameraSelector().filterState.color_invert;
        mPreviewView.EnableFilter("ColorInvertFilter", mPreviewView.GetCameraSelector().filterState.color_invert);
      });

      settingsView.findViewById(R.id.grayscale).setOnClickListener(v -> {
        CLog.v(TAG, "grayscale");
        mPreviewView.GetCameraSelector().filterState.grayscale = !mPreviewView.GetCameraSelector().filterState.grayscale;
        mPreviewView.EnableFilter("GrayscaleFilter", mPreviewView.GetCameraSelector().filterState.grayscale);
      });
      settingsView.findViewById(R.id.time_color).setOnClickListener(v -> {
        CLog.v(TAG, "time color");
        mPreviewView.GetCameraSelector().filterState.time_color = !mPreviewView.GetCameraSelector().filterState.time_color;
        mPreviewView.EnableFilter("TimeColorFilter", mPreviewView.GetCameraSelector().filterState.time_color);
      });
      settingsView.findViewById(R.id.blur).setOnClickListener(v -> {
        CLog.v(TAG, "blur");
        mPreviewView.GetCameraSelector().filterState.blur = !mPreviewView.GetCameraSelector().filterState.blur;
        mPreviewView.EnableFilter("BlurFilter", mPreviewView.GetCameraSelector().filterState.blur);
      });
      settingsView.findViewById(R.id.mosaic).setOnClickListener(v -> {
        CLog.v(TAG, "mosaic");
        mPreviewView.GetCameraSelector().filterState.mosaic = !mPreviewView.GetCameraSelector().filterState.mosaic;
        mPreviewView.EnableFilter("MosaicFilter", mPreviewView.GetCameraSelector().filterState.mosaic);
      });
      settingsView.findViewById(R.id.whirlpool).setOnClickListener(v -> {
        CLog.v(TAG, "whirlpool");
        mPreviewView.GetCameraSelector().filterState.whirlpool = !mPreviewView.GetCameraSelector().filterState.whirlpool;
        mPreviewView.EnableFilter("WhirlpoolFilter", mPreviewView.GetCameraSelector().filterState.whirlpool);
      });
      settingsView.findViewById(R.id.exposure).setOnLongClickListener(v -> {
        CLog.v(TAG, "exposure");
        mPreviewView.GetCameraSelector().filterState.exposure = !mPreviewView.GetCameraSelector().filterState.exposure;
        mPreviewView.EnableFilter("ExposureFilter", mPreviewView.GetCameraSelector().filterState.exposure);
        if (!mPreviewView.GetCameraSelector().filterState.exposure) {
          mPreviewView.GetCameraSelector().filterState.exposure_value = 0.0f;
        }
        return true;
      });
      settingsView.findViewById(R.id.exposure).setOnClickListener(v -> {
        CLog.v(TAG, "exposure");
        if (mPreviewView.GetCameraSelector().filterState.exposure) {
          mPreviewView.GetCameraSelector().filterState.exposure_value += 1.0f;
          if (mPreviewView.GetCameraSelector().filterState.exposure_value > 3.f) {
            mPreviewView.GetCameraSelector().filterState.exposure_value = -3.f;
          }

          mPreviewView.SetFloatVar("exposure", new float[]{mPreviewView.GetCameraSelector().filterState.exposure_value});
        }
      });
      View.OnLongClickListener baseSwitch = v -> {
        CLog.v(TAG, "base");
        mPreviewView.GetCameraSelector().filterState.base = !mPreviewView.GetCameraSelector().filterState.base;
        mPreviewView.EnableFilter("BaseFilter", mPreviewView.GetCameraSelector().filterState.base);
        if (!mPreviewView.GetCameraSelector().filterState.base) {
          mPreviewView.GetCameraSelector().filterState.brightness_value = 0.0f;
          mPreviewView.GetCameraSelector().filterState.contrast_value = 1.0f;
          mPreviewView.GetCameraSelector().filterState.saturation_value = 1.0f;
        }
        return true;
      };
      settingsView.findViewById(R.id.brightness).setOnLongClickListener(baseSwitch);
      settingsView.findViewById(R.id.constrast).setOnLongClickListener(baseSwitch);
      settingsView.findViewById(R.id.saturation).setOnLongClickListener(baseSwitch);
      settingsView.findViewById(R.id.brightness).setOnClickListener(v -> {
        CLog.v(TAG, "brightness");
        if (mPreviewView.GetCameraSelector().filterState.base) {
          mPreviewView.GetCameraSelector().filterState.brightness_value += 0.25f;
          if (mPreviewView.GetCameraSelector().filterState.brightness_value > 1.f) {
            mPreviewView.GetCameraSelector().filterState.brightness_value = -1.f;
          }

          mPreviewView.SetFloatVar("brightness", new float[]{mPreviewView.GetCameraSelector().filterState.brightness_value});
        }
      });
      settingsView.findViewById(R.id.constrast).setOnClickListener(v -> {
        CLog.v(TAG, "contrast");
        if (mPreviewView.GetCameraSelector().filterState.base) {
          mPreviewView.GetCameraSelector().filterState.contrast_value += 1.0f;
          if (mPreviewView.GetCameraSelector().filterState.contrast_value > 4.f) {
            mPreviewView.GetCameraSelector().filterState.contrast_value = 0.f;
          }

          mPreviewView.SetFloatVar("contrast", new float[]{mPreviewView.GetCameraSelector().filterState.contrast_value});
        }
      });
      settingsView.findViewById(R.id.saturation).setOnClickListener(v -> {
        CLog.v(TAG, "saturation");
        if (mPreviewView.GetCameraSelector().filterState.base) {
          mPreviewView.GetCameraSelector().filterState.saturation_value += 0.5f;
          if (mPreviewView.GetCameraSelector().filterState.saturation_value > 2.f) {
            mPreviewView.GetCameraSelector().filterState.saturation_value = 0.f;
          }

          mPreviewView.SetFloatVar("saturation", new float[]{mPreviewView.GetCameraSelector().filterState.saturation_value});
        }
      });

      settingsView.findViewById(R.id.hue).setOnLongClickListener(v -> {
        CLog.v(TAG, "hue");
        mPreviewView.GetCameraSelector().filterState.hue = !mPreviewView.GetCameraSelector().filterState.hue;
        mPreviewView.EnableFilter("HueFilter", mPreviewView.GetCameraSelector().filterState.hue);
        if (!mPreviewView.GetCameraSelector().filterState.hue) {
          mPreviewView.GetCameraSelector().filterState.hue_value = 0.0f;
        }
        return true;
      });
      settingsView.findViewById(R.id.hue).setOnClickListener(v -> {
        CLog.v(TAG, "hue");
        if (mPreviewView.GetCameraSelector().filterState.hue) {
          mPreviewView.GetCameraSelector().filterState.hue_value += 30.0f;
          if (mPreviewView.GetCameraSelector().filterState.hue_value > 360.f) {
            mPreviewView.GetCameraSelector().filterState.hue_value = 0.f;
          }

          mPreviewView.SetFloatVar("hue", new float[]{mPreviewView.GetCameraSelector().filterState.hue_value});
        }
      });
      settingsView.findViewById(R.id.face_detector).setOnClickListener(v -> {
        CLog.v(TAG, "face_detector");
        mPreviewView.GetCameraSelector().filterState.face_detect = !mPreviewView.GetCameraSelector().filterState.face_detect;
        mPreviewView.EnableProcess("FaceLandmarkDetector", mPreviewView.GetCameraSelector().filterState.face_detect);
      });
      settingsView.findViewById(R.id.opencvar).setOnClickListener(v -> {
        CLog.v(TAG, "opencvar");
        mPreviewView.GetCameraSelector().filterState.opencv_ar = !mPreviewView.GetCameraSelector().filterState.opencv_ar;
        mPreviewView.EnableProcess("MarkerAR", mPreviewView.GetCameraSelector().filterState.opencv_ar);
      });
      settingsView.findViewById(R.id.yoloV5).setOnClickListener(v -> {
        CLog.v(TAG, "yoloV5 detect");
        mPreviewView.GetCameraSelector().filterState.yoloV5_detect = !mPreviewView.GetCameraSelector().filterState.yoloV5_detect;
        mPreviewView.EnableProcess("ObjectDetector", mPreviewView.GetCameraSelector().filterState.yoloV5_detect);
      });
    }
  }

  private static class MyHandler extends Handler {
    private WeakReference<Context> mContext;

    public MyHandler(WeakReference<Context> reference) {
      mContext = reference;
    }

    @Override
    public void handleMessage(@NonNull Message msg) {
      switch (msg.what) {
        case PreviewController.MSG_ADD_PICTURE:
          File picture = (File) msg.obj;
          if (picture != null && mContext != null && mContext.get() != null) {
            CLog.d(TAG, "save picture " + picture.getAbsolutePath());
            Toast.makeText(mContext.get(), picture.getAbsolutePath(), Toast.LENGTH_LONG).show();
            String mimeType = MimeTypeMap.getSingleton().getMimeTypeFromExtension("jpg");
            MediaScannerConnection.scanFile(mContext.get(),
              new String[]{picture.getAbsolutePath()},
              new String[]{mimeType}, null);
          }
          break;
        case PreviewController.MSG_ADD_VIDEO:
          File video = (File) msg.obj;
          if (video != null && mContext != null && mContext.get() != null) {
            CLog.d(TAG, "save video " + video.getAbsolutePath());
            Toast.makeText(mContext.get(), video.getAbsolutePath(), Toast.LENGTH_LONG).show();
            String mimeType = MimeTypeMap.getSingleton().getMimeTypeFromExtension("mp4");
            MediaScannerConnection.scanFile(mContext.get(),
              new String[]{video.getAbsolutePath()},
              new String[]{mimeType}, null);
          }
          break;
      }
    }
  }
}
