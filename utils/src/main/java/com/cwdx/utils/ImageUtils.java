package com.cwdx.utils;

import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.graphics.ImageFormat;
import android.graphics.Matrix;
import android.graphics.Rect;
import android.graphics.YuvImage;
import android.media.Image;
import android.util.Log;
import android.util.Rational;
import android.util.Size;

import androidx.annotation.Nullable;

import java.io.ByteArrayOutputStream;
import java.io.File;
import java.nio.ByteBuffer;
import java.text.SimpleDateFormat;
import java.util.Locale;

public final class ImageUtils {
  public static final String FILENAME = "yyyyMMddHHmmssSSS";
  public static final String PHOTO_EXTENSION = ".png";
  public static final String VIDEO_EXTENSION = ".mp4";
  public static final String CALIBRATE_EXTENSION = ".jpg";
  private static final String TAG = "ImageUtil";

  private ImageUtils() {
  }

  /**
   * {@link android.media.Image} to JPEG byte array.
   */
  public static byte[] imageToJpegByteArray(Image image) throws EncodeFailedException {
    byte[] data = null;
    if (image.getFormat() == ImageFormat.JPEG) {
      data = jpegImageToJpegByteArray(image);
    } else if (image.getFormat() == ImageFormat.YUV_420_888) {
      data = yuvImageToJpegByteArray(image);
    } else {
      Log.w(TAG, "Unrecognized image format: " + image.getFormat());
    }
    return data;
  }

  /**
   * Crops byte array with given {@link android.graphics.Rect}.
   */
  public static byte[] cropByteArray(byte[] data, Rect cropRect) throws EncodeFailedException {
    if (cropRect == null) {
      return data;
    }

    Bitmap imageBitmap = BitmapFactory.decodeByteArray(data, 0, data.length);
    if (imageBitmap == null) {
      Log.w(TAG, "Source image for cropping can't be decoded.");
      return data;
    }

    Bitmap cropBitmap = cropBitmap(imageBitmap, cropRect);
    ByteArrayOutputStream out = new ByteArrayOutputStream();
    boolean success = cropBitmap.compress(Bitmap.CompressFormat.JPEG, 100, out);
    if (!success) {
      throw new EncodeFailedException("cropImage failed to encode jpeg.");
    }

    imageBitmap.recycle();
    cropBitmap.recycle();

    return out.toByteArray();
  }

  /**
   * Crops bitmap with given {@link android.graphics.Rect}.
   */
  public static Bitmap cropBitmap(Bitmap bitmap, Rect cropRect) {
    if (cropRect.width() > bitmap.getWidth() || cropRect.height() > bitmap.getHeight()) {
      Log.w(TAG, "Crop rect size exceeds the source image.");
      return bitmap;
    }

    return Bitmap.createBitmap(
      bitmap, cropRect.left, cropRect.top, cropRect.width(), cropRect.height());
  }

  /**
   * Flips bitmap.
   */
  public static Bitmap flipBitmap(Bitmap bitmap, boolean flipHorizontal, boolean flipVertical) {
    if (!flipHorizontal && !flipVertical) {
      return bitmap;
    }

    Matrix matrix = new Matrix();
    if (flipHorizontal) {
      if (flipVertical) {
        matrix.preScale(-1.0f, -1.0f);
      } else {
        matrix.preScale(-1.0f, 1.0f);
      }
    } else if (flipVertical) {
      matrix.preScale(1.0f, -1.0f);
    }

    return Bitmap.createBitmap(
      bitmap, 0, 0, bitmap.getWidth(), bitmap.getHeight(), matrix, true);
  }

  /**
   * Rotates bitmap with specified degree.
   */
  public static Bitmap rotateBitmap(Bitmap bitmap, int degree) {
    if (degree == 0) {
      return bitmap;
    }

    Matrix matrix = new Matrix();
    matrix.preRotate(degree);

    return Bitmap.createBitmap(
      bitmap, 0, 0, bitmap.getWidth(), bitmap.getHeight(), matrix, true);
  }

  /**
   * True if the given aspect ratio is meaningful.
   */
  public static boolean isAspectRatioValid(Rational aspectRatio) {
    return aspectRatio != null && aspectRatio.floatValue() > 0 && !aspectRatio.isNaN();
  }

  /**
   * True if the given aspect ratio is meaningful and has effect on the given size.
   */
  public static boolean isAspectRatioValid(Size sourceSize, Rational aspectRatio) {
    return aspectRatio != null
      && aspectRatio.floatValue() > 0
      && isCropAspectRatioHasEffect(sourceSize, aspectRatio)
      && !aspectRatio.isNaN();
  }

  /**
   * Calculates crop rect with the specified aspect ratio on the given size. Assuming the rect is
   * at the center of the source.
   */
  public static Rect computeCropRectFromAspectRatio(Size sourceSize, Rational aspectRatio) {
    if (!isAspectRatioValid(aspectRatio)) {
      Log.w(TAG, "Invalid view ratio.");
      return null;
    }

    int sourceWidth = sourceSize.getWidth();
    int sourceHeight = sourceSize.getHeight();
    float srcRatio = sourceWidth / (float) sourceHeight;
    int cropLeft = 0;
    int cropTop = 0;
    int outputWidth = sourceWidth;
    int outputHeight = sourceHeight;
    int numerator = aspectRatio.getNumerator();
    int denominator = aspectRatio.getDenominator();

    if (aspectRatio.floatValue() > srcRatio) {
      outputHeight = Math.round((sourceWidth / (float) numerator) * denominator);
      cropTop = (sourceHeight - outputHeight) / 2;
    } else {
      outputWidth = Math.round((sourceHeight / (float) denominator) * numerator);
      cropLeft = (sourceWidth - outputWidth) / 2;
    }

    return new Rect(cropLeft, cropTop, cropLeft + outputWidth, cropTop + outputHeight);
  }

  /**
   * Rotate rational by rotation value, which inverse it if the degree is 90 or 270.
   *
   * @param rational Rational to be rotated.
   * @param rotation Rotation value being applied.
   */
  public static Rational rotate(
    Rational rational, int rotation) {
    if (rotation == 90 || rotation == 270) {
      return inverseRational(rational);
    }

    return rational;
  }

  private static byte[] nv21ToJpeg(byte[] nv21, int width, int height, @Nullable Rect cropRect)
    throws EncodeFailedException {
    ByteArrayOutputStream out = new ByteArrayOutputStream();
    YuvImage yuv = new YuvImage(nv21, ImageFormat.NV21, width, height, null);
    boolean success =
      yuv.compressToJpeg(
        cropRect == null ? new Rect(0, 0, width, height) : cropRect, 100, out);
    if (!success) {
      throw new EncodeFailedException("YuvImage failed to encode jpeg.");
    }
    return out.toByteArray();
  }

  private static byte[] yuv_420_888toNv21(Image image) {
    Image.Plane yPlane = image.getPlanes()[0];
    Image.Plane uPlane = image.getPlanes()[1];
    Image.Plane vPlane = image.getPlanes()[2];

    ByteBuffer yBuffer = yPlane.getBuffer();
    ByteBuffer uBuffer = uPlane.getBuffer();
    ByteBuffer vBuffer = vPlane.getBuffer();
    yBuffer.rewind();
    uBuffer.rewind();
    vBuffer.rewind();

    int ySize = yBuffer.remaining();

    int position = 0;
    // TODO(b/115743986): Pull these bytes from a pool instead of allocating for every image.
    byte[] nv21 = new byte[ySize + (image.getWidth() * image.getHeight() / 2)];

    // Add the full y buffer to the array. If rowStride > 1, some padding may be skipped.
    for (int row = 0; row < image.getHeight(); row++) {
      yBuffer.get(nv21, position, image.getWidth());
      position += image.getWidth();
      yBuffer.position(
        Math.min(ySize, yBuffer.position() - image.getWidth() + yPlane.getRowStride()));
    }

    int chromaHeight = image.getHeight() / 2;
    int chromaWidth = image.getWidth() / 2;
    int vRowStride = vPlane.getRowStride();
    int uRowStride = uPlane.getRowStride();
    int vPixelStride = vPlane.getPixelStride();
    int uPixelStride = uPlane.getPixelStride();

    // Interleave the u and v frames, filling up the rest of the buffer. Use two line buffers to
    // perform faster bulk gets from the byte buffers.
    byte[] vLineBuffer = new byte[vRowStride];
    byte[] uLineBuffer = new byte[uRowStride];
    for (int row = 0; row < chromaHeight; row++) {
      vBuffer.get(vLineBuffer, 0, Math.min(vRowStride, vBuffer.remaining()));
      uBuffer.get(uLineBuffer, 0, Math.min(uRowStride, uBuffer.remaining()));
      int vLineBufferPosition = 0;
      int uLineBufferPosition = 0;
      for (int col = 0; col < chromaWidth; col++) {
        nv21[position++] = vLineBuffer[vLineBufferPosition];
        nv21[position++] = uLineBuffer[uLineBufferPosition];
        vLineBufferPosition += vPixelStride;
        uLineBufferPosition += uPixelStride;
      }
    }

    return nv21;
  }

  private static boolean isCropAspectRatioHasEffect(Size sourceSize, Rational aspectRatio) {
    int sourceWidth = sourceSize.getWidth();
    int sourceHeight = sourceSize.getHeight();
    int numerator = aspectRatio.getNumerator();
    int denominator = aspectRatio.getDenominator();

    return sourceHeight != Math.round((sourceWidth / (float) numerator) * denominator)
      || sourceWidth != Math.round((sourceHeight / (float) denominator) * numerator);
  }

  private static Rational inverseRational(Rational rational) {
    if (rational == null) {
      return rational;
    }
    return new Rational(
      /*numerator=*/ rational.getDenominator(),
      /*denominator=*/ rational.getNumerator());
  }

  private static boolean shouldCropImage(Image image) {
    Size sourceSize = new Size(image.getWidth(), image.getHeight());
    Size targetSize = new Size(image.getCropRect().width(), image.getCropRect().height());

    return !targetSize.equals(sourceSize);
  }

  private static byte[] jpegImageToJpegByteArray(Image image) throws EncodeFailedException {
    Image.Plane[] planes = image.getPlanes();
    ByteBuffer buffer = planes[0].getBuffer();
    byte[] data = new byte[buffer.capacity()];
    buffer.get(data);
//    if (shouldCropImage(image)) {
//      data = cropByteArray(data, image.getCropRect());
//    }
    return data;
  }

  private static byte[] yuvImageToJpegByteArray(Image image)
    throws EncodeFailedException {
    return nv21ToJpeg(
      yuv_420_888toNv21(image),
      image.getWidth(),
      image.getHeight(),
      shouldCropImage(image) ? image.getCropRect() : null);
  }

  /**
   * 将字节数组RGBA转为int型的像素值
   *
   * @param data
   * @param width
   * @param height
   * @return
   */
  public static int[] convert(byte[] data, int width, int height) {
    if (data == null) return null;
    int[] tmp = new int[width * height];
    for (int j = 0; j < height; j++) {
      int offset = j * width * 4;
      int start = j * width;
      for (int i = 0; i < width; i++) {
        tmp[start + i] = convertInt(data, offset + i * 4);
      }
    }
    return tmp;
  }

  /**
   * 将RGBA字节转为ARGB
   *
   * @param data
   * @param offset
   * @return
   */
  public static int convertInt(byte[] data, int offset) {
    return ((data[offset + 3] & 0xff) << 24) |  // A
      ((data[offset] & 0xff) << 16) |           // R
      ((data[offset + 1] & 0xff) << 8) |        // G
      (data[offset + 2] & 0xff);                // B
  }

  /**
   * 取出不包含padding的有效像素
   *
   * @param buffer
   * @param width
   * @param height
   * @param rowPadding
   * @param out
   */
  public static void copyToByteArray(ByteBuffer buffer, int width, int height, int rowPadding, byte[] out) {
    int offset = 0;
    for (int i = 0; i < height; i++) {
      buffer.position(offset + i * rowPadding);
      buffer.get(out, offset, width * 4);
      offset += width * 4;
    }
  }

  public static File createFile(File baseFolder, String format, String extension) {
    SimpleDateFormat format1 = new SimpleDateFormat(format, Locale.US);
    String prefix = "N";
    switch (extension) {
      case PHOTO_EXTENSION:
        prefix = "IMG";
        break;
      case VIDEO_EXTENSION:
        prefix = "VID";
        break;
      case CALIBRATE_EXTENSION:
        prefix = "CALIBRATE";
        break;
      default:
        break;

    }
    return new File(baseFolder, prefix + format1.format(System.currentTimeMillis()) + extension);
  }

  /**
   * Exception for error during encoding image.
   */
  public static final class EncodeFailedException extends Exception {
    EncodeFailedException(String message) {
      super(message);
    }
  }
}
