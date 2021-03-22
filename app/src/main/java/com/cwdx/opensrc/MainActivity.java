package com.cwdx.opensrc;

import android.Manifest;
import android.graphics.Rect;
import android.os.Bundle;
import android.view.View;
import android.widget.Toast;

import androidx.annotation.NonNull;
import androidx.appcompat.app.AppCompatActivity;
import androidx.fragment.app.FragmentManager;
import androidx.recyclerview.widget.LinearLayoutManager;
import androidx.recyclerview.widget.RecyclerView;

import com.cwdx.opensrc.av.Camera2PreviewFragment;
import com.cwdx.opensrc.av.model.Model;
import com.cwdx.opensrc.av.view.BtnAdapter;
import com.cwdx.opensrc.common.Utils;
import com.tbruyelle.rxpermissions2.RxPermissions;

import java.io.File;
import java.util.ArrayList;
import java.util.List;

public class MainActivity extends AppCompatActivity {
    private static final int REQUEST_INTERNET_CODE = 0;
    private static final int REQUEST_CAMERA_CODE = 1;

    static {
        System.loadLibrary("core");
    }

    private RecyclerView mFunctionEntryRV;
    private File mMediaDir;
    private boolean mCameraPermissions = false;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);

        setContentView(R.layout.activity_main);
        mFunctionEntryRV = findViewById(R.id.function_entry);
        mFunctionEntryRV.setLayoutManager(new LinearLayoutManager(this, RecyclerView.VERTICAL, false));
        mFunctionEntryRV.addItemDecoration(new MyDecoration());
        BtnAdapter adapter = new BtnAdapter();
        mFunctionEntryRV.setAdapter(adapter);

        List<Model.ButtonItem> items = new ArrayList<>();
        items.add(new Model.ButtonItem("Preview", 0, () -> {
            if (mMediaDir == null || !mCameraPermissions) {
                Toast.makeText(this, "Need Permissions", Toast.LENGTH_SHORT).show();
                return;
            }
            FragmentManager fragmentManager = getSupportFragmentManager();
            fragmentManager.beginTransaction()
                    .replace(R.id.container, Camera2PreviewFragment.getInstance())
                    .addToBackStack(Camera2PreviewFragment.class.getName()).commit();
        }));

        adapter.setData(items);

        String[] normalPermissions = {
                Manifest.permission.CAMERA,
                Manifest.permission.WRITE_EXTERNAL_STORAGE,
                Manifest.permission.READ_EXTERNAL_STORAGE
        };
        RxPermissions rxPermissions = new RxPermissions(this);
        rxPermissions.requestEach(normalPermissions)
                .subscribe(permission -> {
                    if (permission.granted) {
                        Utils.d("GLRender", "permission.name" + permission.name);
                        if (permission.name.equals(Manifest.permission.WRITE_EXTERNAL_STORAGE)) {
                            mMediaDir = createDirs();
                        } else {
                            mCameraPermissions = true;
                        }
                    } else if (permission.shouldShowRequestPermissionRationale) {
                    } else {
                        Toast.makeText(this, permission.name + " denied", Toast.LENGTH_SHORT).show();
                    }
                });
    }

    @Override
    protected void onStart() {
        super.onStart();
    }

    @Override
    protected void onResume() {
        super.onResume();
    }

    @Override
    protected void onPause() {
        super.onPause();
    }

    @Override
    protected void onDestroy() {
        super.onDestroy();
    }

    @Override
    public void onBackPressed() {
        super.onBackPressed();
    }

    private File createDirs() {
        File[] mediaDirs = getExternalMediaDirs();
        File mediaDir;
        if (mediaDirs == null) mediaDir = null;
        else mediaDir = mediaDirs[0];

        String dirs = getApplicationContext().getResources().getString(R.string.main_dir) + "/"
                + getApplicationContext().getResources().getString(R.string.function_dir);
        File appDir = new File(mediaDir, dirs);

        if (appDir.exists()) {
            Utils.d("GLRender", appDir.getAbsolutePath());
            return mediaDir;
        } else if (!appDir.mkdirs()) {
            Utils.d("GLRender", "Create dir failed " + appDir.getAbsolutePath());
        } else {
            Utils.d("GLRender", "Create dir " + appDir.getAbsolutePath());
            return mediaDir;
        }

        return null;
    }

    public File[] getMediaDir() {
        File mainDir = new File(mMediaDir, getApplicationContext().getResources().getString(R.string.main_dir));
        File functionDir = new File(mainDir, getApplicationContext().getResources().getString(R.string.function_dir));
        return new File[]{mainDir, functionDir};
    }

    private static class MyDecoration extends RecyclerView.ItemDecoration {
        @Override
        public void getItemOffsets(@NonNull Rect outRect, @NonNull View view, @NonNull RecyclerView parent, @NonNull RecyclerView.State state) {
            super.getItemOffsets(outRect, view, parent, state);
            int divider = view.getContext().getResources().getDimensionPixelOffset(R.dimen.divider_width);
            outRect.set(divider, divider, divider, divider);
        }
    }
}