package com.cwdx.core.preview;

public interface IPreviewView {
  IPreviewController GetController();

  void SetController(IPreviewController controller);
}
