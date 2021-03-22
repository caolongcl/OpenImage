package com.cwdx.opensrc.av.view;

import android.content.Context;
import android.content.res.Resources;
import android.util.AttributeSet;
import android.view.View;
import android.widget.AdapterView;

import androidx.appcompat.widget.AppCompatSpinner;

import com.cwdx.opensrc.av.model.Model;
import com.cwdx.opensrc.common.Utils;

import java.util.List;

public class CSpinner extends AppCompatSpinner implements AdapterView.OnItemSelectedListener {
    private static final String TAG = "CSpinner";
    private Context mContext;
    private boolean mFirst = true;

    public CSpinner(Context context) {
        super(context);
        init(context);
    }

    public CSpinner(Context context, int mode) {
        super(context, mode);
        init(context);
    }

    public CSpinner(Context context, AttributeSet attrs) {
        super(context, attrs);
        init(context);
    }

    public CSpinner(Context context, AttributeSet attrs, int defStyleAttr) {
        super(context, attrs, defStyleAttr);
        init(context);
    }

    public CSpinner(Context context, AttributeSet attrs, int defStyleAttr, int mode) {
        super(context, attrs, defStyleAttr, mode);
        init(context);
    }

    public CSpinner(Context context, AttributeSet attrs, int defStyleAttr, int mode, Resources.Theme popupTheme) {
        super(context, attrs, defStyleAttr, mode, popupTheme);
        init(context);
    }

    public void setData(List<Model.Item.Value> values) {
        CSpinnerAdapter adapter = new CSpinnerAdapter();
        adapter.setData(values);
        setAdapter(adapter);
        mFirst = true;
    }

    private void init(Context context) {
        mContext = context;
        setOnItemSelectedListener(this);
    }

    @Override
    public void onItemSelected(AdapterView<?> parent, View view, int position, long id) {
        Model.Item.Value value = (Model.Item.Value) parent.getItemAtPosition(position);
        if (!mFirst && value.runnable != null) {
            Utils.d(TAG, value.value);
            value.runnable.run();
        }
        mFirst = false;
    }

    @Override
    public void onNothingSelected(AdapterView<?> parent) {

    }
}
