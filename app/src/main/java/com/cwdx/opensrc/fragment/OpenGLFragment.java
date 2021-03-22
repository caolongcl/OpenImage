package com.cwdx.opensrc.fragment;

import android.opengl.GLSurfaceView;
import android.os.Bundle;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.FrameLayout;

import androidx.annotation.NonNull;
import androidx.annotation.Nullable;
import androidx.fragment.app.Fragment;

import com.cwdx.opensrc.R;
import com.cwdx.opensrc.opengl.MyGLSurfaceView;

public class OpenGLFragment extends Fragment {
    public static OpenGLFragment instance;
    private GLSurfaceView mGLSv;

    public static OpenGLFragment get() {
        if (instance == null) {
            synchronized (OpenGLFragment.class) {
                if (instance == null) {
                    instance = new OpenGLFragment();
                }
            }
        }
        return instance;
    }

    @Nullable
    @Override
    public View onCreateView(@NonNull LayoutInflater inflater, @Nullable ViewGroup container, @Nullable Bundle savedInstanceState) {
        return inflater.inflate(R.layout.fragment_opengl, container, false);
    }

    @Override
    public void onViewCreated(@NonNull View view, @Nullable Bundle savedInstanceState) {
        super.onViewCreated(view, savedInstanceState);
        mGLSv = new MyGLSurfaceView(getContext());
        FrameLayout container = view.findViewById(R.id.container1);
        container.addView(mGLSv);
//    ConstraintLayout constraintLayout = (ConstraintLayout)view;
//    constraintLayout.addView(mGLSv);
    }
}
