package com.cwdx.opensrc.av.model;

import java.util.List;

public class Model {
  public static final int ITEM_TYPE_SPINNER = 0;
  public static final int ITEM_TYPE_SEEKBAR = 1;

  public interface OnSeekBarProgressListener {
    float onProgressChanged(int progress, int max);
  }

  public static class Item {
    public String title;
    public int type;

    public Item(String title, int type) {
      this.title = title;
      this.type = type;
    }

    public static class Value {
      public String value;
      public Runnable runnable;

      public Value(String value, Runnable runnable) {
        this.value = value;
        this.runnable = runnable;
      }
    }
  }

  public static class ButtonItem extends Item {
    public Runnable value;

    public ButtonItem(String title, int type, Runnable value) {
      super(title, type);
      this.value = value;
    }
  }

  public static class SpinnerItem extends Item {
    public List<Value> values;

    public SpinnerItem(String title, int type, List<Value> values) {
      super(title, type);
      this.values = values;
    }
  }

  public static class SeekBarItem extends Item {
    public OnSeekBarProgressListener listener;

    public SeekBarItem(String title, int type, OnSeekBarProgressListener listener) {
      super(title, type);
      this.listener = listener;
    }
  }
}
