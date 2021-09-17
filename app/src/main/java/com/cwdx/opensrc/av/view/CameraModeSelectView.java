package com.cwdx.opensrc.av.view;

import android.content.Context;
import android.util.AttributeSet;
import android.view.LayoutInflater;
import android.view.View;
import android.view.animation.Animation;
import android.view.animation.AnimationUtils;
import android.widget.FrameLayout;
import android.widget.ImageButton;

import androidx.annotation.NonNull;
import androidx.annotation.Nullable;
import androidx.recyclerview.widget.LinearLayoutManager;
import androidx.recyclerview.widget.RecyclerView;

import com.cwdx.opensrc.R;

import java.util.ArrayList;
import java.util.List;

public class CameraModeSelectView extends FrameLayout {
    private ImageButton mCalibrateV;
    private ImageButton mCaptureV;
    private ImageButton mSwitchCameraV;
    private Animation mBtnPressedAnim;
    private AutoLocateHorizontalView mModeView;
    private CameraModeSelector mModeSelector;
    private boolean mIsCurCapture;
    private boolean mIsCalibrate;

    public CameraModeSelectView(@NonNull Context context) {
        this(context, null, 0);
    }

    public CameraModeSelectView(@NonNull Context context, @Nullable AttributeSet attrs) {
        this(context, attrs, 0);
    }

    public CameraModeSelectView(@NonNull Context context, @Nullable AttributeSet attrs, int defStyleAttr) {
        super(context, attrs, defStyleAttr);

        View view = LayoutInflater.from(context).inflate(R.layout.view_camera_mode, this, false);
        mCalibrateV = view.findViewById(R.id.camera_mode_calibrate);
        mCaptureV = view.findViewById(R.id.camera_mode_capture);
        mSwitchCameraV = view.findViewById(R.id.camera_mode_switch_camera);
        mModeView = view.findViewById(R.id.camera_mode_list);
        addView(view);

        mBtnPressedAnim = AnimationUtils.loadAnimation(getContext(), R.anim.button_pressed_anim);

        List<String> modes = new ArrayList<>();
        modes.add(getResources().getString(R.string.camera_mode_record));
        modes.add(getResources().getString(R.string.camera_mode_capture));
        modes.add(getResources().getString(R.string.camera_mode_calibrate));
        mModeSelector = new CameraModeSelector(modes);
        mModeView.setLayoutManager(new LinearLayoutManager(getContext(), RecyclerView.HORIZONTAL, false));
        mIsCurCapture = true;
        mModeView.setOnSelectedPositionChangedListener(pos -> {
            if (pos == 0) {
                mCaptureV.setImageResource(R.drawable.ic_camera_record);
                mCalibrateV.setEnabled(false);
            } else if (pos == 1 || pos == 2) {
                mCaptureV.setImageResource(R.drawable.ic_camera_capture);
                mIsCurCapture = true;
                mCalibrateV.setEnabled(pos == 2);
            }
            mIsCurCapture = pos == 1 || pos == 2;
            mIsCalibrate = pos == 2;
        });
        mModeView.setInitPos(modes.size() / 2);
        mModeView.setAdapter(mModeSelector);
    }

    public void SetCalibrateFunc(Runnable runnable) {
        mCalibrateV.setOnClickListener(v -> {
            playButtonEffect(v);
            runnable.run();
        });
    }

    public void SetCalibrateParams(Runnable runnable) {
        mCalibrateV.setOnLongClickListener(v -> {
            playButtonEffect(v);
            runnable.run();
            return true;
        });
    }

    public void SetCaptureFunc(CaptureModeFunc func) {
        mCaptureV.setOnClickListener(v -> {
            playButtonEffect(v);

            boolean state = func.OnCapture(mIsCurCapture, !mIsCalibrate);
            if (!mIsCurCapture) {
                mCaptureV.setImageResource(state ? R.drawable.ic_camera_recording : R.drawable.ic_camera_record);
                // 正在录制
                if (state) {
                    mModeView.setOnTouchListener((view, event) -> true);
                    mCalibrateV.setEnabled(false);
                    mSwitchCameraV.setEnabled(false);
                } else {
                    mModeView.setOnTouchListener(null);
                    mCalibrateV.setEnabled(true);
                    mSwitchCameraV.setEnabled(true);
                }
            }
        });
    }

    public void SetSwitchCameraFunc(Runnable runnable) {
        mSwitchCameraV.setOnClickListener(v -> {
            playButtonEffect(v);
            runnable.run();
        });
    }

    private void playButtonEffect(View v) {
        v.startAnimation(mBtnPressedAnim);
//    v.playSoundEffect(SoundEffectConstants.CLICK);
    }

    public interface CaptureModeFunc {
        boolean OnCapture(boolean capture, boolean normal);
    }
}
