package com.cwdx.opensrc.av.view;

import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.BaseAdapter;
import android.widget.TextView;

import com.cwdx.opensrc.av.model.Model;

import java.util.List;

public class CSpinnerAdapter extends BaseAdapter {
    private List<Model.Item.Value> mData;

    public void setData(List<Model.Item.Value> items) {
        mData = items;
    }

    @Override
    public int getCount() {
        return mData != null ? mData.size() : 0;
    }

    @Override
    public Object getItem(int position) {
        return mData != null ? mData.get(position) : null;
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
            convertView = LayoutInflater.from(parent.getContext()).inflate(android.R.layout.simple_list_item_1, null);
            holder = new ViewHolder(convertView);
            convertView.setTag(holder);
        }

        holder.update(mData.get(position).value);

        return convertView;
    }

    static final class ViewHolder {
        private TextView textView;

        public ViewHolder(View view) {
            textView = view.findViewById(android.R.id.text1);
        }

        public void update(String str) {
            textView.setText(str);
        }
    }
}
