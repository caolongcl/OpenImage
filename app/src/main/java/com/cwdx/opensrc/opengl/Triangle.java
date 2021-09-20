package com.cwdx.opensrc.opengl;

import android.opengl.GLES30;
import android.util.Log;

import java.nio.ByteBuffer;
import java.nio.ByteOrder;
import java.nio.FloatBuffer;

public class Triangle {
  static final int COORDS_PER_VERTEX = 3;
  static float[] triangleCoords = {   // in counterclockwise order:
      0.0f, 0.622008459f, 0.0f, // top
      -0.5f, -0.311004243f, 0.0f, // bottom left
      0.5f, -0.311004243f, 0.0f  // bottom right
  };
  private final int vertexCount = triangleCoords.length / COORDS_PER_VERTEX;
  private final int vertexStride = COORDS_PER_VERTEX * 4; // 4 bytes per vertex
  float[] color = {0.63671875f, 0.76953125f, 0.22265625f, 1.0f};
  private FloatBuffer vertexBuffer;
  private int program;
  private int positionHandle;
  private int colorHandle;
  private int vPMatrixHandle;

  public Triangle() {
    ByteBuffer bb = ByteBuffer.allocateDirect(triangleCoords.length * 4);
    bb.order(ByteOrder.nativeOrder());

    vertexBuffer = bb.asFloatBuffer();
    vertexBuffer.put(triangleCoords);
    vertexBuffer.position(0); // set the buffer to read the first coordinate

    int vertexShader = MyShader.loadShader(GLES30.GL_VERTEX_SHADER, MyShader.vertexShaderCode);
    int fragmentShader = MyShader.loadShader(GLES30.GL_FRAGMENT_SHADER, MyShader.fragmentShaderCode);
    program = MyShader.createProgram(vertexShader, fragmentShader);

    Log.d("OpengSrc", "program:" + program);
  }

  public void draw(float[] mvpMatrix) {
    GLES30.glUseProgram(program);

    positionHandle = GLES30.glGetAttribLocation(program, "vPosition");
    GLES30.glEnableVertexAttribArray(positionHandle);
    // C function void glVertexAttribPointer ( GLuint indx, GLint size, GLenum type, GLboolean normalized, GLsizei stride, const GLvoid *ptr )
    GLES30.glVertexAttribPointer(positionHandle, COORDS_PER_VERTEX, GLES30.GL_FLOAT, false, vertexStride, vertexBuffer);

    colorHandle = GLES30.glGetUniformLocation(program, "vColor");
    GLES30.glUniform4fv(colorHandle, 1, color, 0);

    vPMatrixHandle = GLES30.glGetUniformLocation(program, "uMVPMatrix");
    Log.d("OpengSrc", "vPMatrixHandle:" + vPMatrixHandle);

    // Pass the projection and view transformation to the shader
    GLES30.glUniformMatrix4fv(vPMatrixHandle, 1, false, mvpMatrix, 0);

    GLES30.glDrawArrays(GLES30.GL_TRIANGLES, 0, vertexCount);
    GLES30.glDisableVertexAttribArray(positionHandle);
  }
}
