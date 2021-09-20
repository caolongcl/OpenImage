package com.cwdx.opensrc.av.view;

import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.Button;

import androidx.annotation.NonNull;
import androidx.recyclerview.widget.RecyclerView;

import com.cwdx.opensrc.R;
import com.cwdx.opensrc.av.model.Model;

import java.util.ArrayList;
import java.util.List;

public class BtnAdapter extends RecyclerView.Adapter<BtnAdapter.ViewHolder> {
  private List<Model.ButtonItem> mData = new ArrayList<>();

  public void setData(List<Model.ButtonItem> data) {
    if (data != null) {
      mData = data;
      notifyDataSetChanged();
    }
  }

  @NonNull
  @Override
  public ViewHolder onCreateViewHolder(@NonNull ViewGroup parent, int viewType) {
    View view = LayoutInflater.from(parent.getContext()).inflate(R.layout.view_btns_layout, parent, false);
    return new ViewHolder(view);
  }

  @Override
  public void onBindViewHolder(@NonNull ViewHolder holder, int position) {
    holder.BindView(mData.get(position));
  }

  @Override
  public int getItemCount() {
    return mData.size();
  }

  public static class ViewHolder extends RecyclerView.ViewHolder {
    private Button btn;

    public ViewHolder(@NonNull View itemView) {
      super(itemView);
      btn = itemView.findViewById(R.id.btn);
    }

    public void BindView(Model.ButtonItem item) {
      btn.setText(item.title);
      btn.setOnClickListener(v -> {
        item.value.run();
      });
    }
  }
}
