package com.cwdx.opensrc.av.core;

import android.content.Context;
import android.os.Handler;
import android.util.AttributeSet;
import android.util.Size;
import android.view.MotionEvent;
import android.view.Surface;
import android.view.SurfaceHolder;
import android.view.SurfaceView;
import android.view.View;

import androidx.annotation.NonNull;

import com.cwdx.opensrc.common.Utils;

import java.io.File;


public class PreviewView extends SurfaceView implements IPreviewView, IPreviewController {
    private static final String TAG = "PreviewView";

    private IPreviewController mController;
    private SurfaceHolder.Callback mCallback = new SurfaceHolder.Callback() {
        @Override
        public void surfaceCreated(SurfaceHolder holder) {
            int width = holder.getSurfaceFrame().right - holder.getSurfaceFrame().left;
            int height = holder.getSurfaceFrame().bottom - holder.getSurfaceFrame().top;

            Utils.d(TAG, "surfaceCreated " + width + " " + height);

            if (mController != null) {
                mController.Init();
                mController.SetSurface(holder.getSurface());

                // 选择相机
                SelectCamera(CameraSelector.BackCamera);

                mController.UpdatePreviewMode();
                mController.Start();
                mController.Resume();
            }
        }

        @Override
        public void surfaceChanged(SurfaceHolder holder, int format, int width, int height) {
            Utils.d(TAG, "surfaceChanged ");
            CameraSelector.FrontCamera.SetSurfaceSize(new Size(width, height));
            CameraSelector.BackCamera.SetSurfaceSize(new Size(width, height));

            if (mController != null) {
                mController.SetSurfaceSize(width, height);
            }
        }

        @Override
        public void surfaceDestroyed(SurfaceHolder holder) {
            Utils.d(TAG, "surfaceDestroyed");
            if (mController != null) {
                mController.SetSurface(null);
                mController.Pause();
                mController.Stop();
                mController.DeInit();
            }
        }
    };

    public PreviewView(Context context) {
        this(context, null, 0);
    }

    public PreviewView(Context context, AttributeSet attrs) {
        this(context, attrs, 0);
    }

    public PreviewView(Context context, AttributeSet attrs, int defStyle) {
        super(context, attrs, defStyle);

        getHolder().setKeepScreenOn(true);
        getHolder().addCallback(mCallback);
    }

    @Override
    public boolean performClick() {
        return super.performClick();
    }

    public void OnTouch(View v, @NonNull MotionEvent event) {
        switch (event.getAction()) {
            case MotionEvent.ACTION_DOWN:
            case MotionEvent.ACTION_MOVE:
                mController.UpdateTargetPos((int) event.getX(), (int) event.getY());
                break;
        }
    }

    /*******************************************************/

    @Override
    public IPreviewController GetController() {
        return mController;
    }

    @Override
    public void SetController(IPreviewController controller) {
        mController = controller;
    }

    @Override
    public CameraSelector GetCameraSelector() {
        return mController.GetCameraSelector();
    }

    @Override
    public void Create(Context context) {
        mController.Create(context);
    }

    @Override
    public void Destroy() {
        mController.Destroy();
    }

    @Override
    public void Init() {
        throw new UnsupportedOperationException("Init");
    }

    @Override
    public void DeInit() {
        throw new UnsupportedOperationException("DeInit");
    }

    @Override
    public void SelectCamera(CameraSelector selector) {
        mController.SelectCamera(selector);
    }

    @Override
    public void SwitchCamera(CameraSelector selector) {
        mController.SwitchCamera(selector);
    }

    @Override
    public void UpdatePreviewMode() {
        throw new UnsupportedOperationException("UpdatePreviewMode");
    }

    @Override
    public void Start() {
        mController.Start();
    }

    @Override
    public void Resume() {
        mController.Resume();
    }

    @Override
    public void Pause() {
        mController.Pause();
    }

    @Override
    public void Stop() {
        mController.Stop();
    }

    @Override
    public void SetSurface(Surface surface) {
        throw new UnsupportedOperationException("SetSurface");
    }

    @Override
    public void SetSurfaceSize(int width, int height) {
        mController.SetSurfaceSize(width, height);
    }

    @Override
    public int GetState() {
        throw new UnsupportedOperationException("GetState");
    }

    @Override
    public void Capture(boolean normal) {
        mController.Capture(normal);
    }

    @Override
    public void SetMediaDir(File[] dir) {
        mController.SetMediaDir(dir);
    }

    @Override
    public void SetMainHandler(Handler handler) {
        mController.SetMainHandler(handler);
    }

    @Override
    public void Record() {
        mController.Record();
    }

    @Override
    public void StartRecord() {
        mController.StartRecord();
    }

    @Override
    public void StopRecord() {
        mController.StopRecord();
    }


    @Override
    public boolean Recording() {
        return mController.Recording();
    }

    @Override
    public void SetFloatVar(String name, float[] var) {
        mController.SetFloatVar(name, var);
    }

    @Override
    public void SetBoolVar(String name, boolean var) {
        mController.SetBoolVar(name, var);
    }

    @Override
    public void SetIntVar(String name, int[] var) {
        mController.SetIntVar(name, var);
    }

    @Override
    public void SetStringVar(String name, String var) {
        mController.SetStringVar(name, var);
    }

    @Override
    public void EnableFilter(String name, boolean enable) {
        mController.EnableFilter(name, enable);
    }

    @Override
    public void EnableProcess(String name, boolean enable) {
        mController.EnableProcess(name, enable);
    }

    @Override
    public void UpdateTargetPos(int x, int y) {
        mController.UpdateTargetPos(x, y);
    }

    @Override
    public void SetFFmpegDebug(boolean debug) {
        mController.SetFFmpegDebug(debug);
    }
}
