package com.cwdx.opensrc.av.view;

import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.BaseAdapter;
import android.widget.TextView;

import com.cwdx.opensrc.R;
import com.cwdx.opensrc.av.model.Model;

import java.util.List;
import java.util.Map;

public class CharsAdapter extends BaseAdapter {
    private List<String> mTitles;
    private Map<String, List<Model.Item>> mData;

//  public void setData(@NonNull Map<String, List<Item>> data) {
//    mData = data;
//    mTitles = new ArrayList<>(data.keySet());
//  }
//
//  public void addData(String title, List<Item> data) {
//    if (!mData.containsKey(title)) mTitles.add(title);
//    mData.put(title, data);
//  }

    @Override
    public int getCount() {
        return mData != null ? mData.size() : 0;
    }

    @Override
    public Object getItem(int position) {
        return mData != null ? mData.get(mTitles.get(position)) : null;
    }

    @Override
    public long getItemId(int position) {
        return position;
    }

    @Override
    public View getView(int position, View convertView, ViewGroup parent) {
        ViewHolder holder;
        if (convertView != null) {
            holder = (ViewHolder) convertView.getTag();
        } else {
            convertView = LayoutInflater.from(parent.getContext()).inflate(R.layout.view_chars_layout, null);
            holder = new ViewHolder(convertView);
            convertView.setTag(holder);
//      holder.update(mTitles.get(position), mData.get(mTitles.get(position)));
        }

        return convertView;
    }

    public static final class ViewHolder {
        public TextView title;
        public CSpinner cSpinner;

        public ViewHolder(View view) {
            title = view.findViewById(R.id.cspinner_title);
            cSpinner = view.findViewById(R.id.cspinner_item);
        }

//    public void update(String titleStr, List<Item> items) {
//      title.setText(titleStr);
//      cSpinner.setData(items);
//    }
    }
}
