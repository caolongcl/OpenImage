//
// Created by caolong on 2021/11/7.
//

#pragma once

#include <ncnn/net.h>
#include <opencv2/core.hpp>
#include <softarch/IComFunc.hpp>

namespace clt {
  class Yolov5 final : public IComFunc<const std::string &, const std::string &, bool> {
  ClassDeclare(Yolov5)
  public:
    using BoxInfo = struct {
      float x1;
      float y1;
      float x2;
      float y2;
      float score;
      int label;
    };

    Yolov5();

    ~Yolov5() = default;

    bool Init(const std::string &paramPath, const std::string &modelPath, bool useGPU) override;

    void DeInit() override;

    std::vector<BoxInfo> Detect(const cv::Mat &image, float threshold, float nmsThreshold);

    const std::string &LabelStr(int label) const;

  private:
    using Size = struct {
      int width;
      int height;
    };

    using LayerData = struct {
      std::string name;
      int stride;
      std::vector<Size> anchors;
    };

    static void nms(std::vector<BoxInfo> &result, float nmsThreshold);

    static std::vector<BoxInfo>
    decodeInfer(ncnn::Mat &data, int stride, const Size &frameSize, int netSize, int numClasses,
                const std::vector<Size> &anchors, float threshold);

  private:
    static const int s_inputSize = 640;
    static const int s_numClass = 80;

    bool m_useGPU;
    ncnn::PoolAllocator m_workspacePoolAlloc;
    ncnn::UnlockedPoolAllocator m_blobPoolAlloc;
    ncnn::Net m_net;
    std::vector<LayerData> m_layers;
    std::vector<std::string> m_labels;
  };
}