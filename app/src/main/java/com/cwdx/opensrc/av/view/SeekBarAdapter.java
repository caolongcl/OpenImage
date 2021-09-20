package com.cwdx.opensrc.av.view;


import static com.cwdx.opensrc.av.model.Model.ITEM_TYPE_SEEKBAR;
import static com.cwdx.opensrc.av.model.Model.ITEM_TYPE_SPINNER;

import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.SeekBar;
import android.widget.TextView;

import androidx.annotation.NonNull;
import androidx.recyclerview.widget.RecyclerView;

import com.cwdx.opensrc.R;
import com.cwdx.opensrc.av.model.Model;

import java.security.InvalidParameterException;
import java.util.ArrayList;
import java.util.List;
import java.util.Map;

public class SeekBarAdapter extends RecyclerView.Adapter<SeekBarAdapter.ViewHolder> {
  private List<String> mTitles;
  private Map<String, Model.Item> mData;

  public void setData(@NonNull Map<String, Model.Item> data) {
    mData = data;
    mTitles = new ArrayList<>(data.keySet());
    notifyDataSetChanged();
  }

  public void addData(String title, Model.Item data) {
    if (!mData.containsKey(title)) mTitles.add(title);
    mData.put(title, data);
    notifyDataSetChanged();
  }

  @NonNull
  @Override
  public ViewHolder onCreateViewHolder(@NonNull ViewGroup parent, int viewType) {
    View view;

    if (viewType == ITEM_TYPE_SPINNER) {
      view = LayoutInflater.from(parent.getContext()).inflate(R.layout.view_chars_layout, parent, false);
      return new ViewHolderSpinner(view);
    } else if (viewType == ITEM_TYPE_SEEKBAR) {
      view = LayoutInflater.from(parent.getContext()).inflate(R.layout.view_seekbar_layout, parent, false);
      return new ViewHolderSeekBar(view);
    } else {
      throw new InvalidParameterException("viewType " + viewType);
    }
  }

  @Override
  public void onBindViewHolder(@NonNull ViewHolder holder, int position) {
    if (holder instanceof ViewHolderSpinner) {
      ((ViewHolderSpinner) holder).BindView((Model.SpinnerItem) mData.get(mTitles.get(position)));
    } else if (holder instanceof ViewHolderSeekBar) {
      ((ViewHolderSeekBar) holder).BindView((Model.SeekBarItem) mData.get(mTitles.get(position)));
    }
  }

  @Override
  public int getItemViewType(int position) {
    return mData.get(mTitles.get(position)).type;
  }

  @Override
  public long getItemId(int position) {
    return position;
  }

  @Override
  public int getItemCount() {
    return mData.size();
  }

  public static class ViewHolder extends RecyclerView.ViewHolder {
    public ViewHolder(@NonNull View itemView) {
      super(itemView);
    }
  }

  public static class ViewHolderSpinner extends ViewHolder {
    public TextView title;
    public CSpinner cSpinner;

    public ViewHolderSpinner(@NonNull View itemView) {
      super(itemView);
      this.title = itemView.findViewById(R.id.cspinner_title);
      this.cSpinner = itemView.findViewById(R.id.cspinner_item);
    }

    public void BindView(Model.SpinnerItem item) {
      this.title.setText(item.title);
      this.cSpinner.setData(item.values);
    }
  }

  public static class ViewHolderSeekBar extends ViewHolder {
    private TextView title;
    private TextView value;
    private SeekBar seekBar;

    public ViewHolderSeekBar(@NonNull View itemView) {
      super(itemView);
      this.title = itemView.findViewById(R.id.seekbar_title);
      this.value = itemView.findViewById(R.id.seekbar_value);
      this.seekBar = itemView.findViewById(R.id.seekbar);
    }

    public void BindView(Model.SeekBarItem item) {
      this.title.setText(item.title);
      this.seekBar.setOnSeekBarChangeListener(new SeekBar.OnSeekBarChangeListener() {
        @Override
        public void onProgressChanged(SeekBar seekBar, int progress, boolean fromUser) {

        }

        @Override
        public void onStartTrackingTouch(SeekBar seekBar) {

        }

        @Override
        public void onStopTrackingTouch(SeekBar seekBar) {
          float ret = item.listener.onProgressChanged(seekBar.getProgress(), seekBar.getMax());
          value.setText(String.valueOf(ret));
        }
      });
    }
  }
}
