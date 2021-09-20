package com.cwdx.opensrc.av.core;

public interface IPreviewView {
  IPreviewController GetController();

  void SetController(IPreviewController controller);
}
