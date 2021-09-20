package com.cwdx.opensrc.av.view;

public class StateListDrawable {
  private int[] mResIds;
  private StateListener mListener;

  public StateListDrawable(int[] resIds, StateListener listener) {
    mResIds = resIds;
    mListener = listener;
  }

  public interface StateListener {
    void OnStateChanged(int resId);
  }
}
