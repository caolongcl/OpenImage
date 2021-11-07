//
// Created by caolong on 2021/11/7.
//

#include <process/yolov5/Yolov5.hpp>
#include <utils/AlgoMath.hpp>
#include <ncnn/cpu.h>
#include <utils/Log.hpp>

using namespace clt;

Yolov5::Yolov5() :
    m_useGPU(true),
    m_layers({
                 {"394",    32, {{116, 90}, {156, 198}, {373, 326}}},
                 {"375",    16, {{30,  61}, {62,  45},  {59,  119}}},
                 {"output", 8,  {{10,  13}, {16,  30},  {33,  23}}},
             }),
    m_labels({"person", "bicycle", "car", "motorcycle", "airplane", "bus", "train", "truck", "boat", "traffic light",
              "fire hydrant", "stop sign", "parking meter", "bench", "bird", "cat", "dog", "horse", "sheep", "cow",
              "elephant", "bear", "zebra", "giraffe", "backpack", "umbrella", "handbag", "tie", "suitcase", "frisbee",
              "skis", "snowboard", "sports ball", "kite", "baseball bat", "baseball glove", "skateboard", "surfboard",
              "tennis racket", "bottle", "wine glass", "cup", "fork", "knife", "spoon", "bowl", "banana", "apple",
              "sandwich", "orange", "broccoli", "carrot", "hot dog", "pizza", "donut", "cake", "chair", "couch",
              "potted plant", "bed", "dining table", "toilet", "tv", "laptop", "mouse", "remote", "keyboard",
              "cell phone",
              "microwave", "oven", "toaster", "sink", "refrigerator", "book", "clock", "vase", "scissors", "teddy bear",
              "hair drier", "toothbrush"}) {
}

bool Yolov5::Init(const std::string &paramPath, const std::string &modelPath, bool useGPU) {
  bool hasGPU = ncnn::get_gpu_count() > 0;
  m_useGPU = useGPU && hasGPU;

  Log::d(Log::PROCESSOR_TAG, "has gpu %d, use gpu %d", hasGPU, m_useGPU);

  ncnn::set_cpu_powersave(0);
  ncnn::set_omp_num_threads(ncnn::get_big_cpu_count());

  m_net.opt.use_vulkan_compute = m_useGPU;
  m_net.opt.num_threads = ncnn::get_big_cpu_count();
//  m_net.opt.blob_allocator = &m_blobPoolAlloc;
//  m_net.opt.workspace_allocator = &m_workspacePoolAlloc;
  m_net.opt.use_fp16_arithmetic = true;  // fp16运算加速
  m_net.load_param(paramPath.c_str());
  m_net.load_model(modelPath.c_str());

  return true;
}

void Yolov5::DeInit() {
  m_net.clear();
  m_blobPoolAlloc.clear();
  m_workspacePoolAlloc.clear();
}

std::vector<Yolov5::BoxInfo> Yolov5::Detect(const cv::Mat &image, float threshold, float nmsThreshold) {
  int inWidth = image.cols;
  int inHeight = image.rows;
  int targetWidth = s_inputSize / 2;
  int targetHeight = s_inputSize / 2;

  ncnn::Mat in_net = ncnn::Mat::from_pixels_resize(image.data, ncnn::Mat::PIXEL_RGB, inWidth, inHeight,
                                                   targetWidth, targetHeight);

  const float norm[3] = {1 / 255.f, 1 / 255.f, 1 / 255.f};
  const float mean[3] = {0, 0, 0};
  in_net.substract_mean_normalize(mean, norm);

  auto ex = m_net.create_extractor();
  ex.set_light_mode(true);
  ex.set_num_threads(4);
  if (m_useGPU) {
    ex.set_vulkan_compute(m_useGPU);
  }
  ex.input(0, in_net);

  std::vector<BoxInfo> result;
  for (const auto &layer: m_layers) {
    ncnn::Mat blob;
    ex.extract(layer.name.c_str(), blob);
    auto boxes = decodeInfer(blob, layer.stride, {inWidth, inHeight}, s_inputSize,
                             s_numClass, layer.anchors, threshold);
    result.insert(result.begin(), boxes.begin(), boxes.end());
  }
  nms(result, nmsThreshold);
  return result;
}

const std::string &Yolov5::LabelStr(int label) const {
  return m_labels.at(label);
}

std::vector<Yolov5::BoxInfo>
Yolov5::decodeInfer(ncnn::Mat &data, int stride, const Size &frameSize, int netSize, int numClasses,
                    const std::vector<Size> &anchors, float threshold) {
  std::vector<BoxInfo> result;
  int grid_size = int(sqrt(data.h));
  float *mat_data[data.c];
  for (int i = 0; i < data.c; i++) {
    mat_data[i] = data.channel(i);
  }
  float cx, cy, w, h;
  for (int shift_y = 0; shift_y < grid_size; shift_y++) {
    for (int shift_x = 0; shift_x < grid_size; shift_x++) {
      int loc = shift_x + shift_y * grid_size;
      for (int i = 0; i < 3; i++) {
        float *record = mat_data[i];
        float *cls_ptr = record + 5;
        for (int cls = 0; cls < numClasses; cls++) {
          float score = AlgoMath::Sigmoid(cls_ptr[cls]) * AlgoMath::Sigmoid(record[4]);
          if (score > threshold) {
            cx = (AlgoMath::Sigmoid(record[0]) * 2.f - 0.5f + (float) shift_x) * (float) stride;
            cy = (AlgoMath::Sigmoid(record[1]) * 2.f - 0.5f + (float) shift_y) * (float) stride;
            w = pow(AlgoMath::Sigmoid(record[2]) * 2.f, 2) * anchors[i].width;
            h = pow(AlgoMath::Sigmoid(record[3]) * 2.f, 2) * anchors[i].height;
            //printf("[grid size=%d, stride = %d]x y w h %f %f %f %f\n",grid_size,stride,record[0],record[1],record[2],record[3]);
            BoxInfo box;
            box.x1 = std::max(0, std::min(frameSize.width,
                                          int((cx - w / 2.f) * (float) frameSize.width / (float) netSize)));
            box.y1 = std::max(0, std::min(frameSize.height,
                                          int((cy - h / 2.f) * (float) frameSize.height / (float) netSize)));
            box.x2 = std::max(0, std::min(frameSize.width,
                                          int((cx + w / 2.f) * (float) frameSize.width / (float) netSize)));
            box.y2 = std::max(0, std::min(frameSize.height,
                                          int((cy + h / 2.f) * (float) frameSize.height / (float) netSize)));
            box.score = score;
            box.label = cls;
            result.push_back(box);
          }
        }
      }
      for (auto &ptr:mat_data) {
        ptr += (numClasses + 5);
      }
    }
  }
  return result;
}

void Yolov5::nms(std::vector<BoxInfo> &input_boxes, float NMS_THRESH) {
  std::sort(input_boxes.begin(), input_boxes.end(),
            [](const BoxInfo &a, const BoxInfo &b) { return a.score > b.score; });

  std::vector<float> vArea(input_boxes.size());
  for (int i = 0; i < int(input_boxes.size()); ++i) {
    vArea[i] = (input_boxes.at(i).x2 - input_boxes.at(i).x1 + 1)
               * (input_boxes.at(i).y2 - input_boxes.at(i).y1 + 1);
  }

  for (int i = 0; i < int(input_boxes.size()); ++i) {
    for (int j = i + 1; j < int(input_boxes.size());) {
      float xx1 = std::max(input_boxes[i].x1, input_boxes[j].x1);
      float yy1 = std::max(input_boxes[i].y1, input_boxes[j].y1);
      float xx2 = std::min(input_boxes[i].x2, input_boxes[j].x2);
      float yy2 = std::min(input_boxes[i].y2, input_boxes[j].y2);
      float w = std::max(float(0), xx2 - xx1 + 1);
      float h = std::max(float(0), yy2 - yy1 + 1);
      float inter = w * h;
      float ovr = inter / (vArea[i] + vArea[j] - inter);
      if (ovr >= NMS_THRESH) {
        input_boxes.erase(input_boxes.begin() + j);
        vArea.erase(vArea.begin() + j);
      } else {
        j++;
      }
    }
  }
}