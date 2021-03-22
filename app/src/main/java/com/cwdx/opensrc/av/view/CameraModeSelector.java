package com.cwdx.opensrc.av.view;

import android.graphics.Color;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.TextView;

import androidx.annotation.NonNull;
import androidx.recyclerview.widget.RecyclerView;

import com.cwdx.opensrc.R;

import java.util.List;

public class CameraModeSelector extends RecyclerView.Adapter<CameraModeSelector.ModeViewHolder> implements AutoLocateHorizontalView.IAutoLocateHorizontalView {
    private View mRootV;
    private List<String> mModes;

    public CameraModeSelector(@NonNull List<String> modes) {
        mModes = modes;
    }

    @Override
    public View getItemView() {
        return mRootV;
    }

    @Override
    public void onViewSelected(boolean isSelected, int pos, RecyclerView.ViewHolder holder, int itemWidth) {
        ((ModeViewHolder) holder).UpdateState(isSelected);
    }

    @NonNull
    @Override
    public ModeViewHolder onCreateViewHolder(@NonNull ViewGroup parent, int viewType) {
        mRootV = LayoutInflater.from(parent.getContext()).inflate(R.layout.view_text_item, parent, false);
        return new ModeViewHolder(mRootV);
    }

    @Override
    public void onBindViewHolder(@NonNull ModeViewHolder holder, int position) {
        holder.SetTextStr(mModes.get(position));
    }

    @Override
    public int getItemCount() {
        return mModes.size();
    }

    static class ModeViewHolder extends RecyclerView.ViewHolder {
        private TextView textV;

        public ModeViewHolder(@NonNull View itemView) {
            super(itemView);
            textV = itemView.findViewById(R.id.text_id);
        }

        public void SetTextStr(String str) {
            textV.setText(str);
        }

        public void UpdateState(boolean highlight) {
            if (highlight) {
                textV.setTextColor(Color.parseColor("#FFFFD700"));
            } else {
                textV.setTextColor(Color.parseColor("#FFFFFFFF"));
            }
        }
    }
}
