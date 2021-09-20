package com.cwdx.opensrc.opengl;

import android.content.Context;
import android.opengl.GLES30;
import android.opengl.GLSurfaceView;
import android.opengl.Matrix;
import android.os.SystemClock;
import android.util.AttributeSet;
import android.view.MotionEvent;

import javax.microedition.khronos.egl.EGLConfig;
import javax.microedition.khronos.opengles.GL10;

public class MyGLSurfaceView extends GLSurfaceView {
  private final MyGLRenderer mGLRenderer;
  private final float TOUCH_SCALE_FACTOR = 180.0f / 320;
  private float previousX;
  private float previousY;

  public MyGLSurfaceView(Context context) {
    this(context, null);
  }

  public MyGLSurfaceView(Context context, AttributeSet attrs) {
    super(context, attrs);
    // 设置OpengGL ES版本 3.0
    setEGLContextClientVersion(3);

    // 设置renderer
    mGLRenderer = new MyGLRenderer();
    setRenderer(mGLRenderer);

    // Render the view only when there is a change in the drawing data
    setRenderMode(GLSurfaceView.RENDERMODE_WHEN_DIRTY);
  }

  @Override
  public boolean onTouchEvent(MotionEvent event) {
    float x = event.getX();
    float y = event.getY();

    switch (event.getAction()) {
      case MotionEvent.ACTION_MOVE:

        float dx = x - previousX;
        float dy = y - previousY;

        // reverse direction of rotation above the mid-line
        if (y > getHeight() / 2) {
          dx = dx * -1;
        }

        // reverse direction of rotation to left of the mid-line
        if (x < getWidth() / 2) {
          dy = dy * -1;
        }

        mGLRenderer.setAngle(mGLRenderer.getAngle() + ((dx + dy) * TOUCH_SCALE_FACTOR));
        requestRender();
    }

    previousX = x;
    previousY = y;
    return true;
  }

  static class MyGLRenderer implements GLSurfaceView.Renderer {
    // vPMatrix is an abbreviation for "Model View Projection Matrix"
    private final float[] vPMatrix = new float[16];
    private final float[] projectionMatrix = new float[16];
    private final float[] viewMatrix = new float[16];
    public volatile float mAngle;
    float[] scratch = new float[16];
    private Triangle triangle;
    private Square square;
    private float[] rotationMatrix = new float[16];

    public float getAngle() {
      return mAngle;
    }

    public void setAngle(float angle) {
      mAngle = angle;
    }

    @Override
    public void onSurfaceCreated(GL10 gl, EGLConfig config) {
      triangle = new Triangle();
      square = new Square();
      GLES30.glClearColor(0.0f, 1.0f, 0.0f, 1.0f);
    }

    @Override
    public void onSurfaceChanged(GL10 gl, int width, int height) {
      GLES30.glViewport(0, 0, width, height);

      float ratio = (float) width / height;
      Matrix.frustumM(projectionMatrix, 0, -ratio, ratio, -1, 1, 3, 7);
    }

    @Override
    public void onDrawFrame(GL10 gl) {
      GLES30.glClear(GLES30.GL_COLOR_BUFFER_BIT | GLES30.GL_DEPTH_BUFFER_BIT);

      Matrix.setLookAtM(viewMatrix, 0, 0, 0, -3, 0f, 0f, 0f, 0f, 1.0f, 0.0f);
      Matrix.multiplyMM(vPMatrix, 0, projectionMatrix, 0, viewMatrix, 0);

      // Create a rotation transformation for the triangle
      long time = SystemClock.uptimeMillis() % 4000L;
      float angle = 0.090f * ((int) time);
      Matrix.setRotateM(rotationMatrix, 0, angle + mAngle, 0, 0, -1.0f);
      Matrix.multiplyMM(scratch, 0, vPMatrix, 0, rotationMatrix, 0);

      triangle.draw(scratch);
    }
  }
}
