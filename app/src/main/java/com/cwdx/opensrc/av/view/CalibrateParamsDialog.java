package com.cwdx.opensrc.av.view;

import android.os.Bundle;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.Button;
import android.widget.EditText;

import androidx.annotation.NonNull;
import androidx.annotation.Nullable;
import androidx.fragment.app.DialogFragment;

import com.cwdx.opensrc.R;
import com.cwdx.opensrc.common.Utils;

public class CalibrateParamsDialog extends DialogFragment {
    private static final String TAG = "CalibrateParamsDialog";

    private OnParamsSetConfirmCallback callback;

    private EditText boardSizeWidthV;
    private EditText boardSizeHeightV;
    private EditText boardSquareSizeWidthV;
    private EditText boardSquareSizeHeightV;
    private EditText markerSizeWidthV;
    private EditText markerSizeHeightV;

    private CalibrateData calibrateData;

    @Nullable
    @Override
    public View onCreateView(@NonNull LayoutInflater inflater, @Nullable ViewGroup container, @Nullable Bundle savedInstanceState) {
        return inflater.inflate(R.layout.fragment_settings, container, false);
    }

    @Override
    public void onViewCreated(@NonNull View view, @Nullable Bundle savedInstanceState) {
        Button ok = (Button) view.findViewById(R.id.set_params_ok);
        Button cancel = (Button) view.findViewById(R.id.set_params_cancel);
        boardSizeWidthV = (EditText) view.findViewById(R.id.board_size_width);
        boardSizeHeightV = (EditText) view.findViewById(R.id.board_size_height);
        boardSquareSizeWidthV = (EditText) view.findViewById(R.id.board_square_size_width);
        boardSquareSizeHeightV = (EditText) view.findViewById(R.id.board_square_size_height);
        markerSizeWidthV = (EditText) view.findViewById(R.id.marker_size_width);
        markerSizeHeightV = (EditText) view.findViewById(R.id.marker_size_height);

        ok.setOnClickListener(view1 -> {
            try {
                int boardSizeWidth = Integer.parseInt(boardSizeWidthV.getText().toString());
                int boardSizeHeight = Integer.parseInt(boardSizeHeightV.getText().toString());
                float boardSquareSizeWidth = Float.parseFloat(boardSquareSizeWidthV.getText().toString());
                float boardSquareSizeHeight = Float.parseFloat(boardSquareSizeHeightV.getText().toString());
                float markerSizeWidth = Float.parseFloat(markerSizeWidthV.getText().toString());
                float markerSizeHeight = Float.parseFloat(markerSizeHeightV.getText().toString());

                Utils.d(TAG, "board size:" + boardSizeWidth + "," + boardSizeHeight + ";"
                        + "board square size:" + boardSquareSizeWidth + "," + boardSquareSizeHeight + ";"
                        + "marker size:" + markerSizeWidth + "," + markerSizeHeight);

                if (this.callback != null) {
                    if (boardSizeWidth > 0 && boardSizeHeight > 0
                            && boardSquareSizeWidth > 0.0f && boardSquareSizeHeight > 0.0f
                            && markerSizeWidth > 0.0f && markerSizeHeight > 0.0f) {
                        callback.onParamsSetConfirm(true, new CalibrateData(boardSizeWidth, boardSizeHeight,
                                boardSquareSizeWidth, boardSquareSizeHeight,
                                markerSizeWidth, markerSizeHeight));
                    }
                }
            } catch (Exception e) {
                Utils.e(TAG, e.getMessage());
            }

            dismiss();
        });

        cancel.setOnClickListener(view1 -> {
            if (this.callback != null) {
                callback.onParamsSetConfirm(false, new CalibrateData(0, 0,
                        0.0f, 0.0f,
                        0.0f, 0.0f));
            }

            dismiss();
        });
    }

    @Override
    public void onResume() {
        super.onResume();

        boardSizeWidthV.setText(String.valueOf(calibrateData.boardSizeWidth));
        boardSizeHeightV.setText(String.valueOf(calibrateData.boardSizeHeight));
        boardSquareSizeWidthV.setText(String.valueOf(calibrateData.boardSquareWidth));
        boardSquareSizeHeightV.setText(String.valueOf(calibrateData.boardSquareHeight));
        markerSizeWidthV.setText(String.valueOf(calibrateData.markerWidth));
        markerSizeHeightV.setText(String.valueOf(calibrateData.markerHeight));
    }

    public void UpdateCalibrateData(CalibrateData calibrateData) {
        this.calibrateData = calibrateData;
    }

    public void SetCallback(OnParamsSetConfirmCallback callback) {
        this.callback = callback;
    }

    @FunctionalInterface
    public interface OnParamsSetConfirmCallback {
        void onParamsSetConfirm(boolean confirm, CalibrateData calibrateData);
    }

    public static class CalibrateData {
        public CalibrateData(int boardSizeWidth, int boardSizeHeight,
                             float boardSquareSizeWidth, float boardSquareSizeHeight,
                             float markerSizeWidth, float markerSizeHeight) {
            this.boardSizeWidth = boardSizeWidth;
            this.boardSizeHeight = boardSizeHeight;
            this.boardSquareWidth = boardSquareSizeWidth;
            this.boardSquareHeight = boardSquareSizeHeight;
            this.markerWidth = markerSizeWidth;
            this.markerHeight = markerSizeHeight;
        }

        public int boardSizeWidth;
        public int boardSizeHeight;
        public float boardSquareWidth;
        public float boardSquareHeight;
        public float markerWidth;
        public float markerHeight;
    }
}
