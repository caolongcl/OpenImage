//
// Created by caolong on 2021/3/16.
//

#pragma once

#include <utils/Utils.hpp>
#include <process/base/NormalProcessTask.hpp>
#include <process/base/ProcessUtils.hpp>
#include <softarch/VarType.hpp>
#include <opencv2/opencv.hpp>
#include <res/YamlParse.hpp>
#include <res/ResManager.hpp>

namespace clt {

  class CalibrateCamera final : public INormalProcessTask {
  ClassDeclare(CalibrateCamera)
  ClassWrapper(NormalProcessTask)
  public:
    CalibrateCamera();

    ~CalibrateCamera() = default;

    bool Init() override;

    void DeInit() override;

    void Process() override;

  private:
    void process();

    /**
     * 生成棋盘内角点实际三维坐标
     * @param boardSize
     * @param realCorners
     */
    static void generateChessboardRealCorners(const cv::Size &boardSize,
                                              const cv::Size &boardSquareSize,
                                              std::vector<cv::Point3f> &realCorners);

    /**
     * 寻找标定图片上棋盘角点
     * @param file
     * @param boardSize
     * @param corners
     * @param imageSize
     * @return
     */
    static bool findCorners(const std::string &file,
                            const cv::Size &boardSize,
                            std::vector<cv::Point2f> &corners,
                            cv::Size &imageSize);

    /**
     * 校正，获取摄像机内参矩阵
     * @param realCorners
     * @param corners
     * @param imageSize
     * @param cameraMatrix
     * @param distCoeffs
     * @param R
     * @param T
     */
    static void calibrate(const std::vector<std::vector<cv::Point3f>> &realCorners,
                          const std::vector<std::vector<cv::Point2f>> &corners,
                          const cv::Size &imageSize,
                          const Integer2 &boardSize,
                          const Float2 &boardSquareSize,
                          cv::Mat &cameraMatrix,
                          cv::Mat &distCoeffs);

    /**
     * 图像去畸变
     * @param file
     * @param cameraMatrix
     * @param distCoeffs
     */
    static void unDistort(const std::string &file,
                          const cv::Mat &cameraMatrix,
                          const cv::Mat &distCoeffs);

    /**
     * 计算校正重投影误差
     * @param objectPoints
     * @param imagePoints
     * @param rvecs
     * @param tvecs
     * @param cameraMatrix
     * @param distCoeffs
     * @param perViewErrors
     * @return
     */
    static double computeProjectionErrors(const std::vector<std::vector<cv::Point3f> > &objectPoints,
                                          const std::vector<std::vector<cv::Point2f> > &imagePoints,
                                          const cv::Mat &cameraMatrix, const cv::Mat &distCoeffs,
                                          const std::vector<cv::Mat> &rvecs, const std::vector<cv::Mat> &tvecs,
                                          std::vector<float> &perViewErrors);

    /**
     * 保存校正参数
     * @param cameraMatrix
     * @param distCoeffs
     */
    static void saveCalibrateParams(const cv::Mat &cameraMatrix,
                                    const cv::Mat &distCoeffs,
                                    const cv::Size &imageSize,
                                    const Integer2 &boardSize,
                                    const Float2 &boardSquareSize,
                                    const double avgErr);

    /**
     * 扫描校正图片
     * @param functionDir
     */
    void scanImages(const std::string &functionDir);

  private:
    std::vector<std::string> m_images;
    CalibrateData m_calibrateData;
    TimeStatics m_fpsStatics;
  };

  /**
   * 内参矩阵等参数
   */
  struct CameraData {
  VarDeclare(camera_matrix);
  VarDeclare(dist_coeffs);

    CameraData() = default;
    CameraData(const std::vector<double> &m,
               const std::vector<double> &d,
               const Integer2 &_imageSize,
               const Integer2 &_boardSize,
               const Float2 &_boardSquareSize,
               const double _avgErr) :
        matrix(m),
        dist(d),
        imageSize(_imageSize),
        boardSize(_boardSize), boardSquareSize(_boardSquareSize),
        avgErr(_avgErr) {}

    static YAML::Node Encode(const CameraData &rhs) {
      YAML::Node node;
      YAML::Node infoNode;
      infoNode["board_width"] = rhs.boardSize.w;
      infoNode["board_height"] = rhs.boardSize.h;
      infoNode["board_square_width"] = rhs.boardSquareSize.w;
      infoNode["board_square_height"] = rhs.boardSquareSize.h;
      infoNode["image_width"] = rhs.imageSize.w;
      infoNode["image_height"] = rhs.imageSize.h;
      node["info"] = infoNode;
      node[CameraData::var_camera_matrix] = rhs.matrix;
      node[CameraData::var_dist_coeffs] = rhs.dist;
      node["avgErr"] = rhs.avgErr;
      return node;
    }

    static bool Decode(const YAML::Node &node, CameraData &rhs) {
      if (!node.IsDefined()
          || !node["info"].IsDefined()
          || !node[CameraData::var_camera_matrix].IsDefined()
          || !node[CameraData::var_dist_coeffs].IsDefined()
          || !node["avgErr"].IsDefined()) {
        return false;
      }
      rhs.imageSize = {node["info"]["image_width"].as<int>(), node["info"]["image_height"].as<int>()};
      rhs.boardSize = {node["info"]["board_width"].as<int>(), node["info"]["board_height"].as<int>()};
      rhs.boardSquareSize = {node["info"]["board_square_width"].as<float>(),
                             node["info"]["board_square_height"].as<float>()};
      rhs.matrix = node[CameraData::var_camera_matrix].as<std::vector<double>>();
      rhs.dist = node[CameraData::var_dist_coeffs].as<std::vector<double>>();
      rhs.avgErr = node["avgErr"].as<double>();
      return true;
    }

    void GetCameraMatrix(cv::Mat &_matrix);

    void GetCameraDistCoeffs(cv::Mat &_dist);

  private:
    std::vector<double> matrix;
    std::vector<double> dist;
    double avgErr;
    Integer2 imageSize;
    Integer2 boardSize;
    Float2 boardSquareSize;
  };
}
