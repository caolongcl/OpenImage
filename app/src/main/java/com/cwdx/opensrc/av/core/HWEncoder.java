package com.cwdx.opensrc.av.core;

import android.media.MediaCodec;
import android.media.MediaCodecInfo;
import android.media.MediaCodecList;
import android.media.MediaFormat;
import android.media.MediaMuxer;
import android.view.Surface;

import androidx.annotation.NonNull;

import com.cwdx.opensrc.common.Utils;

import java.io.IOException;
import java.nio.ByteBuffer;

public class HWEncoder {
    private static final String TAG = "HWEncoder";
    private MediaCodec mEncoder;
    private MediaMuxer mMuxer;
    private Surface mInputSurface;

    private String VIDEO_MIME_TYPE = MediaFormat.MIMETYPE_VIDEO_AVC;
    private int mFrameRate;
    private int mIFrameRate;
    private int mVideoTrackIndex;
    private boolean mStartMuxer;

    private long mLastPresentationTimeUs;
    private MediaCodec.Callback callback = new MediaCodec.Callback() {
        @Override
        public void onInputBufferAvailable(@NonNull MediaCodec codec, int index) {
            Utils.v(TAG, "onInputBufferAvailable " + index);
        }

        @Override
        public void onOutputBufferAvailable(@NonNull MediaCodec codec, int index, @NonNull MediaCodec.BufferInfo info) {
//      Utils.v(TAG, "onOutputBufferAvailable " + index);

            if (mEncoder == null) return;

            ByteBuffer buffer = codec.getOutputBuffer(index);
            if (buffer == null) throw new RuntimeException("output buffer null");

            if (info.presentationTimeUs >= mLastPresentationTimeUs) {
                if (info.size != 0) {
                    buffer.position(info.offset);
                    buffer.limit(info.offset + info.size);
                    if (mMuxer != null && mStartMuxer) {
                        mMuxer.writeSampleData(mVideoTrackIndex, buffer, info);
                    }
                    mLastPresentationTimeUs = info.presentationTimeUs;
                }
            }

            codec.releaseOutputBuffer(index, false);
        }

        @Override
        public void onError(@NonNull MediaCodec codec, @NonNull MediaCodec.CodecException e) {
            Utils.e(TAG, "error:" + e.getMessage());
            throw e;
        }

        @Override
        public void onOutputFormatChanged(@NonNull MediaCodec codec, @NonNull MediaFormat format) {
            Utils.d(TAG, "format changed, start muxer");
            mVideoTrackIndex = mMuxer.addTrack(format);
            mMuxer.start();
            mStartMuxer = true;
        }
    };

    public HWEncoder() {
        mFrameRate = 30;
        mIFrameRate = 10;
    }

    private void dumpMediaCodecInfo() {
        MediaCodecList list = new MediaCodecList(MediaCodecList.REGULAR_CODECS);
        MediaCodecInfo[] infos = list.getCodecInfos();

        Utils.d(TAG, "encoders:");
        for (MediaCodecInfo info : infos) {
            if (info.isEncoder()) {
                Utils.d(TAG, info.getName());
            }
        }
    }

    public void PrepareVideoEncoder(int width, int height, int frameRate, int bitrate, String outputPath) {
        Utils.d(TAG, "width " + width + " height " + height + " bitrate " + bitrate + " framerate " + frameRate + " output " + outputPath);

        MediaFormat format = MediaFormat.createVideoFormat(VIDEO_MIME_TYPE, width, height);
        format.setInteger(MediaFormat.KEY_COLOR_FORMAT, MediaCodecInfo.CodecCapabilities.COLOR_FormatSurface);
        format.setInteger(MediaFormat.KEY_BIT_RATE, bitrate);
//    format.setInteger(MediaFormat.KEY_BITRATE_MODE, MediaCodecInfo.EncoderCapabilities.BITRATE_MODE_CQ);
        format.setInteger(MediaFormat.KEY_FRAME_RATE, frameRate);
        format.setInteger(MediaFormat.KEY_I_FRAME_INTERVAL, mIFrameRate);
        Utils.d(TAG, "format:" + format);

        try {
            mEncoder = MediaCodec.createEncoderByType(VIDEO_MIME_TYPE);
            mEncoder.setCallback(callback);
            mEncoder.configure(format, null, null, MediaCodec.CONFIGURE_FLAG_ENCODE);
            mInputSurface = mEncoder.createInputSurface();

            mEncoder.start();

            mMuxer = new MediaMuxer(outputPath, MediaMuxer.OutputFormat.MUXER_OUTPUT_MPEG_4);

            mVideoTrackIndex = -1;
            mStartMuxer = false;
            mLastPresentationTimeUs = -1;
        } catch (IOException e) {
            e.printStackTrace();
            throw new RuntimeException("PrepareVideoEncoder failed");
        }
    }

    public void ReleaseVideoEncoder() {
        if (mEncoder != null) {
            mEncoder.stop();
            mEncoder.release();
            mEncoder = null;
        }

        if (mInputSurface != null) {
            mInputSurface.release();
            mInputSurface = null;
        }

        if (mMuxer != null) {
            mMuxer.stop();
            mMuxer.release();
            mMuxer = null;
        }
    }

    public Surface GetInputSurface() {
        return mInputSurface;
    }

    public long GetLastPresentationTimeUs() {
        return mLastPresentationTimeUs;
    }
}
