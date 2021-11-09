//
// Created by caolong on 2021/3/16.
//

#include <process/ar/CalibrateCamera.hpp>
#include <res/ResManager.hpp>
#include <process/base/ProcessUtils.hpp>
#include <dirent.h>
#include <render/Flow.hpp>
#include <render/copier/Drawer.hpp>
#include <opencv2/opencv.hpp>
#include <res/YamlParse.hpp>
#include <render/GLRender.hpp>

using namespace clt;

CalibrateCamera::CalibrateCamera()
  : m_images() {}

bool CalibrateCamera::Init() {
  Log::v(target, "CalibrateCamera::Init");

  std::string functionDir = ResManager::Self()->GetFunctionAbsolutePath();

  m_calibrateData = ResManager::Self()->LoadCalibrateParams();

  scanImages(functionDir);

  return true;
}

void CalibrateCamera::DeInit() {
  m_images.clear();
  Log::v(target, "CalibrateCamera::DeInit");
}

void CalibrateCamera::Process() {
  process();
}

void CalibrateCamera::process() {
  Log::v(target, "CalibrateCamera::process");

  if (m_images.empty()) {
    Log::w(target, "no calibrate images.");
    Flow::Self()->SendMsg(PosInfoMsg(GLRender::target, "no calibrate images!"));
    return;
  }

  try {
    std::vector<cv::Point3f> corners3f;
    generateChessboardRealCorners(cv::Size(m_calibrateData.GetBoardSize().w,
                                           m_calibrateData.GetBoardSize().h),
                                  cv::Size(m_calibrateData.GetBoardSquareSize().w,
                                           m_calibrateData.GetBoardSquareSize().h), corners3f);

    int count = m_images.size();
    if (count > 5) count = 5;

    cv::Mat cameraMatrix, distCoeffs, R, T;
    std::vector<std::vector<cv::Point3f>> realCorners;
    std::vector<std::vector<cv::Point2f>> boardCorners;

    cv::Size imageSize;
    for (int i = 0; i < count; ++i) {
      std::vector<cv::Point2f> corners2f;
      if (findCorners(m_images[i], cv::Size(m_calibrateData.GetBoardSize().w,
                                            m_calibrateData.GetBoardSize().h),
                      corners2f, imageSize)) {
        boardCorners.push_back(corners2f);
        realCorners.push_back(corners3f);
      }
    }

    calibrate(realCorners, boardCorners, imageSize,
              m_calibrateData.GetBoardSize(), m_calibrateData.GetBoardSquareSize(),
              cameraMatrix, distCoeffs);

    for (int i = 0; i < count; ++i) {
      unDistort(m_images[i], cameraMatrix, distCoeffs);
    }

    Flow::Self()->SendMsg(PosInfoMsg(GLRender::target, "calibrate successfully"));

  } catch (std::exception &e) {
    Log::e(target, "calibrate exception occur %s", e.what());

    Flow::Self()->SendMsg(
      PosInfoMsg(GLRender::target, "calibrate failed, " + std::string(e.what())));
  }
}

void CalibrateCamera::calibrate(const std::vector<std::vector<cv::Point3f> > &realCorners,
                                const std::vector<std::vector<cv::Point2f> > &corners,
                                const cv::Size &imageSize,
                                const Integer2 &boardSize,
                                const Float2 &boardSquareSize,
                                cv::Mat &cameraMatrix,
                                cv::Mat &distCoeffs) {

  std::vector<cv::Mat> rvecs, tvecs;
  std::vector<float> perViewErrors;

  // 获取校正参数
  cv::calibrateCamera(realCorners, corners, imageSize, cameraMatrix, distCoeffs, rvecs, tvecs);

  double avgErr = computeProjectionErrors(realCorners, corners,
                                          cameraMatrix, distCoeffs, rvecs, tvecs,
                                          perViewErrors);

  Log::d(target, "calibrate average err : %f", avgErr);

  std::stringstream ss;
  // 内参矩阵 https://blog.csdn.net/qq_33446100/article/details/96845829
  // 相机坐标系下坐标(x,y,z)变为图像坐标系像素(u,v)
  // 相机坐标系：x 左，y 下，z 指向成像平面（感光元件）
  ss << "cameraMatrix : " << cameraMatrix << std::endl;

  // 透镜畸变系数 k1,k2,p1,p2,k3
  ss << "distCoeffs : " << distCoeffs << std::endl;

  Log::d(target, "calibrate params :\n %s", ss.str().c_str());

  saveCalibrateParams(cameraMatrix, distCoeffs, imageSize, boardSize, boardSquareSize, avgErr);
}

void CalibrateCamera::saveCalibrateParams(const cv::Mat &cameraMatrix,
                                          const cv::Mat &distCoeffs,
                                          const cv::Size &imageSize,
                                          const Integer2 &boardSize,
                                          const Float2 &boardSquareSize,
                                          const double avgErr) {

  std::vector<double> matrix;
  matrix.emplace_back(cameraMatrix.at<double>(0, 0)); // fx
  matrix.emplace_back(cameraMatrix.at<double>(0, 2)); // cx
  matrix.emplace_back(cameraMatrix.at<double>(1, 1)); // fy
  matrix.emplace_back(cameraMatrix.at<double>(1, 2)); // cy

  std::vector<double> dist;
  dist.emplace_back(distCoeffs.at<double>(0, 0)); // k1
  dist.emplace_back(distCoeffs.at<double>(0, 1)); // k2
  dist.emplace_back(distCoeffs.at<double>(0, 2)); // p1
  dist.emplace_back(distCoeffs.at<double>(0, 3)); // p2
  dist.emplace_back(distCoeffs.at<double>(0, 4)); // k3

  Integer2 _imageSize = {imageSize.width, imageSize.height};

  YamlCreator creator(ResManager::Self()->GetCameraParamsFile());
  creator.Emit()
    << CameraData::Encode({matrix, dist, _imageSize, boardSize, boardSquareSize, avgErr});
  creator.Save();
}

bool CalibrateCamera::findCorners(const std::string &file,
                                  const cv::Size &boardSize,
                                  std::vector<cv::Point2f> &corners,
                                  cv::Size &imageSize) {
  cv::Mat image = cv::imread(ResManager::Self()->GetFunctionAbsolutePath() + "/" + file);

  imageSize.width = image.cols;
  imageSize.height = image.rows;

  if (!cv::findChessboardCorners(image, cv::Size(boardSize.width, boardSize.height), corners)) {
    Log::w(target, "calibrate images %s find no corners", file.c_str());
    return false;
  }

  cv::Mat gray;
  cv::cvtColor(image, gray, cv::COLOR_BGR2GRAY);

//    ResManager::Self()->SaveMatImageImmediate(file + "Gray", gray);

  cv::TermCriteria criteria(cv::TermCriteria::EPS | cv::TermCriteria::Type::MAX_ITER, 30, 0.001);
  cv::cornerSubPix(gray, corners, cv::Size(11, 11), cv::Size(-1, -1), criteria);

  cv::drawChessboardCorners(image, boardSize, corners, true);

  ResManager::Self()->SaveMatImageImmediate(file + "ChessboardCorners", image);

  return true;
}

void CalibrateCamera::unDistort(const std::string &file,
                                const cv::Mat &cameraMatrix,
                                const cv::Mat &distCoeffs) {
  std::string unFile = ResManager::Self()->GetFunctionAbsolutePath() + "/" + file;
  cv::Mat image = cv::imread(unFile);
  cv::Mat output;

  // 校正图像
  cv::undistort(image, output, cameraMatrix, distCoeffs);

  ResManager::Self()->SaveMatImageImmediate(file + "undistort", output);
}

void CalibrateCamera::generateChessboardRealCorners(const cv::Size &boardSize,
                                                    const cv::Size &boardSquareSize,
                                                    std::vector<cv::Point3f> &realCorners) {
  // 假设标定板放在世界坐标系中 z=0 的平面上
  for (int i = 0; i < boardSize.height; i++) { // Y 轴点数
    for (int j = 0; j < boardSize.width; j++) { // X 轴点数
      realCorners.emplace_back(j * boardSquareSize.width, i * boardSquareSize.height, 0);
    }
  }
}

void CalibrateCamera::scanImages(const std::string &functionDir) {
  DIR *dirItem;
  struct dirent *dir;

  if ((dirItem = opendir(functionDir.c_str())) != nullptr) {
    while ((dir = readdir(dirItem)) != nullptr) {
      std::string dirName(dir->d_name);
      auto pos = dirName.find_last_of('.');
      if (dirName != "." && dirName != ".." && dirName.substr(pos) == ".jpg"
          && dirName.find("ChessboardCorners") == std::string::npos
          && dirName.find("undistort") == std::string::npos) {
        m_images.emplace_back(dirName);
        Log::v(target, "function file : %s", dirName.c_str());
      }
    }

    closedir(dirItem);
  }
}

double
CalibrateCamera::computeProjectionErrors(const std::vector<std::vector<cv::Point3f> > &objectPoints,
                                         const std::vector<std::vector<cv::Point2f> > &imagePoints,
                                         const cv::Mat &cameraMatrix, const cv::Mat &distCoeffs,
                                         const std::vector<cv::Mat> &rvecs,
                                         const std::vector<cv::Mat> &tvecs,
                                         std::vector<float> &perViewErrors) {
  std::vector<cv::Point2f> imagePoints2;
  size_t totalPoints = 0;
  double totalErr = 0, err;
  perViewErrors.resize(objectPoints.size());

  for (size_t i = 0; i < objectPoints.size(); ++i) {
    projectPoints(objectPoints[i], rvecs[i], tvecs[i], cameraMatrix, distCoeffs, imagePoints2);
    err = norm(imagePoints[i], imagePoints2, cv::NORM_L2);

    size_t n = objectPoints[i].size();
    perViewErrors[i] = (float) std::sqrt(err * err / n);
    totalErr += err * err;
    totalPoints += n;
  }

  return std::sqrt(totalErr / totalPoints);
}

/// CameraData
void CameraData::GetCameraMatrix(cv::Mat &_matrix) {
  _matrix.create(3, 3, CV_64FC1);
  _matrix.setTo(0);

  _matrix.at<double>(0, 0) = matrix[0];
  _matrix.at<double>(0, 2) = matrix[1];
  _matrix.at<double>(1, 1) = matrix[2];
  _matrix.at<double>(1, 2) = matrix[3];
  _matrix.at<double>(2, 2) = 1.0;

  Log::n(target, "GetCameraMatrix %f %f %f %f", _matrix.at<double>(0, 0),
         _matrix.at<double>(0, 2), _matrix.at<double>(1, 1), _matrix.at<double>(1, 2));
}

void CameraData::GetCameraDistCoeffs(cv::Mat &_dist) {
  _dist.create(1, 5, CV_64FC1);
  for (int i = 0; i < dist.size(); ++i) {
    _dist.at<double>(0, i) = dist[i];
  }

  Log::n(target, "GetCameraDistCoeffs %f %f %f %f %f", _dist.at<double>(0, 0),
         _dist.at<double>(0, 1), _dist.at<double>(0, 2), _dist.at<double>(0, 3),
         _dist.at<double>(0, 4));
}