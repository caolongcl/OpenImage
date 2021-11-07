package com.cwdx.core.preview;

import android.content.Context;
import android.view.Surface;

import java.util.List;

public interface ICamera {

  void Create(Context context);

  void Destroy();

  void Open(String id, List<Surface> surfaces);

  void Close();

  void SetErrorHandler(onError handler);

  interface onError {
    void handleError(int error);
  }

}
