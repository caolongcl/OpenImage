package com.cwdx.opensrc.av.view;

import android.content.Context;
import android.util.AttributeSet;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.view.animation.Animation;
import android.view.animation.AnimationUtils;
import android.widget.FrameLayout;
import android.widget.ImageButton;

import com.cwdx.opensrc.R;

public class CameraParamSelectView extends FrameLayout {
    private static final String TAG = "CameraParamSelectView";
    private ImageButton mFlashIB;
    private ImageButton mHDRIB;
    private ImageButton mAIIB;
    private ImageButton mEffectIB; // 美颜
    private ImageButton mFilterIB; // 滤镜
    private ImageButton mSettingsIB;
    private ViewGroup mDetailContainerVG;
    private View mFlashDetailVG;
    private View mHDRDetailVG;
    private View mOldDetailVG;
    private View mSettingsVG;
    private Animation mViewDisplayAnim;
    private Animation mViewHideAnim;

    public CameraParamSelectView(Context context) {
        this(context, null);
    }

    public CameraParamSelectView(Context context, AttributeSet attrs) {
        this(context, attrs, 0);
    }

    public CameraParamSelectView(Context context, AttributeSet attrs, int defStyle) {
        super(context, attrs, defStyle);

        View view = LayoutInflater.from(context).inflate(R.layout.view_camera_param, this, false);
//    mFlashIB = view.findViewById(R.id.camera_param_shortkey_flash);
//    mHDRIB = view.findViewById(R.id.camera_param_shortkey_hdr);
//    mAIIB = view.findViewById(R.id.camera_param_shortkey_ai);
//    mEffectIB = view.findViewById(R.id.camera_param_shortkey_effect);
//    mFilterIB = view.findViewById(R.id.camera_param_shortkey_filter);
        mSettingsIB = view.findViewById(R.id.camera_param_shortkey_settings);
        addView(view);

//    mFlashDetailVG = inflate(getContext(), R.layout.view_camera_flash, null);
//    mHDRDetailVG = inflate(getContext(), R.layout.view_camera_hdr, null);
        mSettingsVG = inflate(getContext(), R.layout.view_camera_settings, null);

        mSettingsIB.setOnClickListener(v -> {
            showDetailView(mSettingsVG);
        });
//    mFlashIB.setOnClickListener(v -> {
//      showDetailView(mFlashDetailVG);
//    });
//
//    mHDRIB.setOnClickListener(v -> {
//      showDetailView(mHDRDetailVG);
//    });

        mViewDisplayAnim = AnimationUtils.loadAnimation(getContext(), R.anim.view_display_set_anim);
//    mViewHideAnim = AnimationUtils.loadAnimation(getContext(), R.anim.view_hide_anim);
    }

    public void SetDetailContainerVG(ViewGroup viewGroup) {
        mDetailContainerVG = viewGroup;
    }

    public void HideDetailContainerVG() {
        if (mDetailContainerVG != null && mDetailContainerVG.getVisibility() != View.GONE) {
            mDetailContainerVG.removeAllViews();
//      mDetailContainerVG.startAnimation(mViewHideAnim);
            mDetailContainerVG.setVisibility(View.GONE);
        }
    }

    public View GetParamSelectView() {
        return mSettingsVG;
    }

    private void showDetailView(View v) {
        if (mDetailContainerVG != null && v != null) {
            mDetailContainerVG.removeAllViews();
            boolean oldShow = mDetailContainerVG.getVisibility() == View.VISIBLE;
            if (!oldShow || v != mOldDetailVG) {
                mDetailContainerVG.addView(v);
                mDetailContainerVG.setVisibility(View.VISIBLE);
                if (!oldShow) {
                    v.startAnimation(mViewDisplayAnim);
                }
                mOldDetailVG = v;
            } else {
//        v.startAnimation(mViewHideAnim);
                mDetailContainerVG.setVisibility(View.GONE);
            }
        }
    }
}
