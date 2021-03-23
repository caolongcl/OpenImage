//
// Created by caolong on 2021/3/15.
//

#include <process/ar/MarkerAR.hpp>
#include <render/Flow.hpp>
#include <render/polygon/Polygon.hpp>
#include <softarch/Msg.hpp>
#include <render/copier/Copier.hpp>
#include <res/YamlParse.hpp>
#include <res/ResManager.hpp>
#include <render/GLRender.hpp>
#include <render/model/BaseModel.hpp>

using namespace clt;

MarkerAR::MarkerAR()
        : m_marker(new Marker()),
          m_cameraDataGot(false),
          m_timesStatics() {

}

bool MarkerAR::Init() {
    Log::v(Log::PROCESSOR_TAG, "MarkerAR::Init");

    m_timesStatics.Init();
    m_marker->Init();

    auto paramsFile = ResManager::Self()->GetCameraParamsFile();
    std::fstream f;
    f.open(paramsFile, std::ios::in);
    if (!f.is_open()) {
        Log::w(Log::PROCESSOR_TAG, "No camera calibrate file.");
        Flow::Self()->SendMsg(PosInfoMsg(GLRender::target, "No camera calibrate file."));
        f.close();
        return true;
    }
    f.close();

    // 解析摄像头内参和畸变参数
    YamlParse parse(paramsFile);
    m_cameraDataGot = CameraData::Decode(parse.GetNode(), m_params);

    if (!m_cameraDataGot) {
        Flow::Self()->SendMsg(PosInfoMsg(GLRender::target, "No camera calibrate params."));
    }

    return true;
}

void MarkerAR::DeInit() {
    Flow::Self()->SendMsg(BaseModelMsg(BaseModel::target, BaseModel::msg_marker_ar));
    Flow::Self()->SendMsg(PolygonMsg(Copier::target, Copier::msg_detect_marker));
    Flow::Self()->SendMsg(TextMsg(Copier::target, Copier::msg_detect_marker_info));

    m_timesStatics.DeInit();
    m_marker->DeInit();

    Log::v(Log::PROCESSOR_TAG, "MarkerAR::DeInit");
}

void MarkerAR::Process(std::shared_ptr<Buffer> buf) {
    long lastTime = Utils::CurTimeMilli();

    process(*buf);

    long delta = Utils::CurTimeMilli() - lastTime;
    Log::n(Log::PROCESSOR_TAG, "marker detect period %d ms", delta);

    if (delta < 10) {
        std::this_thread::sleep_for(std::chrono::milliseconds(10 - delta));
    }

    Log::debug([this, lastTime]() {
        m_timesStatics.Update(lastTime, [](long period) {
            TextInfo textInfo("marker_detect:" + std::to_string(period) + "ms");
            textInfo.position = {100.0f, 200.0f};
            Flow::Self()->SendMsg(
                    TextMsg(Copier::target, Copier::msg_detect_marker_info,
                            std::make_shared<TextMsgData>(std::move(textInfo))));
        });
    });
}

void MarkerAR::process(const Buffer &buf) {
    /// 1. 检测标记位置和姿态
    try {
        cv::Mat frame(buf.height, buf.width, CV_8UC4, buf.data.get());// buf是RGBA的，frame把它当成BGRA的
        cv::Mat frameGray;
        cv::cvtColor(frame, frameGray, cv::COLOR_RGBA2GRAY);
        cv::flip(frameGray, frameGray, 0); // 上下翻转

        m_marker->Detect(frameGray);

    } catch (std::exception &e) {
        Log::e(Log::PROCESSOR_TAG, "marker detect exception occur %s", e.what());
        Flow::Self()->SendMsg(PolygonMsg(Copier::target, Copier::msg_detect_marker));
        Flow::Self()->SendMsg(BaseModelMsg(BaseModel::target, BaseModel::msg_marker_ar));
        return;
    }

    auto &makers = m_marker->GetMakers();
    if (makers.empty()) {
        Flow::Self()->SendMsg(PolygonMsg(Copier::target, Copier::msg_detect_marker));
        Flow::Self()->SendMsg(BaseModelMsg(BaseModel::target, BaseModel::msg_marker_ar));
        return;
    }

    std::vector<PolygonObject> polygonObjects;
    polygonObjects.reserve(makers.size());

    for (auto &marker : makers) {
        Log::n(Log::PROCESSOR_TAG, "mark id %d :", marker.id);
        std::vector<Float2> points;
        points.reserve(marker.corners.size());

        for (auto &point : marker.corners) {
            Log::n(Log::PROCESSOR_TAG, "region (%f %f)", point.x, point.y);
            points.emplace_back(point.x, point.y);
        }

        polygonObjects.emplace_back(Float2(buf.width, buf.height),
                                    points, GreenColor, "marker_" + std::to_string(marker.id));
    }

    Flow::Self()->SendMsg(PolygonMsg(Copier::target, Copier::msg_detect_marker,
                                     std::make_shared<PolygonMsgData>(std::move(polygonObjects))));

    /// 2. 绘制 AR 模型
    if (!m_cameraDataGot) {
        return;
    }

    // 计算预览图像实际宽高
    float bufRatio = ResManager::Self()->GetResParams().bufRatio;
    float width = buf.width / bufRatio;
    float height = buf.height / bufRatio;

    // 相机远近平面
    float near = 0.01f;
    float far = 1000.0f;

    Log::n(Log::PROCESSOR_TAG, "real image size %f %f", width, height);

    std::vector<BaseModelObject> baseModelObjects;
    baseModelObjects.reserve(makers.size());

    try {
        // 获取相机内参数和畸变参数
        cv::Mat cameraMatrix, distCoeffs;
        m_params.GetCameraMatrix(cameraMatrix);
        m_params.GetCameraDistCoeffs(distCoeffs);

        // marker 3D 坐标
        std::vector<cv::Point3f> realCoords;
        getMarkerRealCoordinates(realCoords);

        for (auto &marker : makers) {
            // marker 像素坐标
            std::vector<cv::Point2f> coords;
            getMarkerCoordinates(marker.corners, coords);

            // 获取摄像机外参数，旋转和平移参数
            cv::Mat rMat, tVec;
            estimateCameraView(realCoords, coords, cameraMatrix, distCoeffs, rMat, tVec);

            // 获取模视矩阵
            cv::Mat modelView;
            estimateModelView(rMat, tVec, modelView);

            // 获取投影矩阵
            cv::Mat projection;
            estimateProjection(cameraMatrix, width, height, near, far, projection);

            // 将 Mat 转为 glm 的矩阵
            glm::mat4 glModelView;
            glm::mat4 glProjection;
            convertMatToGlm(modelView, projection, glModelView, glProjection);

            // 实际模型长宽高限制在（-1.0, 1.0）间，故还需要放大到实际大小
            Float2 markerSize = ResManager::Self()->GetResParams().markerSize;
            float scale = (markerSize.w + markerSize.h) / 2.0f;
            glm::mat4 scaleModel = glm::scale(glm::mat4(), glm::vec3(scale, scale, scale));
            glModelView = glModelView * scaleModel;

            // 立方体中心是原点
            glm::mat4 transModel = glm::translate(glm::mat4(), glm::vec3(0.0f, 0.0f, -scale / 2));

            baseModelObjects.emplace_back(BaseModelObject::Type::CUBE,
                                          glProjection, glm::mat4(1.0), transModel * glModelView, BlueColor);
        }
    } catch (std::exception &e) {
        Log::e(Log::PROCESSOR_TAG, "camera estimate exception occur %s", e.what());
    }

    Flow::Self()->SendMsg(BaseModelMsg(BaseModel::target, BaseModel::msg_marker_ar,
                                       std::make_shared<BaseModelMsgData>(std::move(baseModelObjects))));
}


bool MarkerAR::estimateCameraView(const std::vector<cv::Point3f> &corners3d,
                                  const std::vector<cv::Point2f> &corners2d,
                                  const cv::Mat &cameraMatrix, const cv::Mat &distCoeff,
                                  cv::Mat &RMat, cv::Mat &TVec) {
    cv::Mat rotVec;
    if (!cv::solvePnP(corners3d, corners2d, cameraMatrix, distCoeff, rotVec, TVec)) {
        Log::w(Log::PROCESSOR_TAG, "estimateCameraView failed");
        return false;
    }

    cv::Rodrigues(rotVec, RMat);

//    std::stringstream ss;
//    ss << "camera estimate view " << cameraMatrix << std::endl;
//    ss << "camera estimate view " << distCoeff << std::endl;
//    ss << "camera estimate view " << RMat << std::endl;
//    ss << "camera estimate view " << TVec << std::endl;
//
//    Log::e(Log::PROCESSOR_TAG, "--- %s", ss.str().c_str());
    return true;
}

void MarkerAR::estimateModelView(const cv::Mat &RMat, const cv::Mat &TVec, cv::Mat &modelView) {
    // 绕X轴旋转180度，从 OpenCV 坐标系变换为 OpenGL 坐标系
    cv::Mat rotateByX = cv::Mat::eye(3, 3, CV_64FC1);
    rotateByX.at<double>(1, 1) = -1;
    rotateByX.at<double>(2, 2) = -1;

    cv::Mat rotation = rotateByX * RMat;
    cv::Mat translation = rotateByX * TVec;

//    std::stringstream ss;
//    ss << "translation " << translation << std::endl;
//    Log::e(Log::PROCESSOR_TAG, "=== %s", ss.str().c_str());

    modelView = cv::Mat::zeros(4, 4, CV_64FC1);

    for (int i = 0; i < 3; ++i) { // cols
        for (int j = 0; j < 3; ++j) { // rows
            modelView.at<double>(j, i) = rotation.at<double>(j, i);
        }
    }

    for (int i = 0; i < 3; ++i) { // rows
        modelView.at<double>(i, 3) = translation.at<double>(i, 0);
    }

    modelView.at<double>(3, 3) = 1.0;

//    std::stringstream sss;
//    sss << "camera estimate view " << modelView << std::endl;
//    Log::e(Log::PROCESSOR_TAG, "--- %s", sss.str().c_str());
}

void MarkerAR::estimateProjection(const cv::Mat &cameraMatrix,
                                  float width, float height,
                                  float near, float far,
                                  cv::Mat &projection) {
    // https://blog.csdn.net/aichipmunk/article/details/41489825
    double fx = cameraMatrix.at<double>(0, 0);
    double cx = cameraMatrix.at<double>(0, 2);
    double fy = cameraMatrix.at<double>(1, 1);
    double cy = cameraMatrix.at<double>(1, 2);

    projection = cv::Mat::zeros(4, 4, CV_64FC1);
    projection.at<double>(0, 0) = 2 * fx / width;
    projection.at<double>(1, 1) = 2 * fy / height;
    projection.at<double>(0, 2) = 1.0 - 2 * cx / width;
    projection.at<double>(1, 2) = 2 * cy / height - 1.0;
    projection.at<double>(2, 2) = -(far + near) / (far - near);
    projection.at<double>(3, 2) = -1.0;
    projection.at<double>(2, 3) = -2.0 * far * near / (far - near);

//    std::stringstream sss;
//    sss << "estimateProjection " << projection << std::endl;
//
//    Log::e(Log::PROCESSOR_TAG, "--- %s", sss.str().c_str());
}

void MarkerAR::convertMatToGlm(const cv::Mat &modelView, const cv::Mat &projection,
                               glm::mat4 &glModelView, glm::mat4 &glProjection) {
    glModelView = glm::mat4(0);

    for (int i = 0; i < 3; ++i) { // cols
        for (int j = 0; j < 3; ++j) { // rows
            glModelView[i][j] = modelView.at<double>(j, i);
        }
    }

    for (int i = 0; i < 3; ++i) { // rows
        glModelView[3][i] = modelView.at<double>(i, 3);
    }
    glModelView[3][3] = 1.0;

    // 列主序
    glProjection = glm::mat4(0);
    glProjection[0][0] = projection.at<double>(0, 0);
    glProjection[1][1] = projection.at<double>(1, 1);
    glProjection[2][0] = projection.at<double>(0, 2);
    glProjection[2][1] = projection.at<double>(1, 2);
    glProjection[2][2] = projection.at<double>(2, 2);
    glProjection[2][3] = projection.at<double>(3, 2);
    glProjection[3][2] = projection.at<double>(2, 3);
}

void MarkerAR::getMarkerRealCoordinates(std::vector<cv::Point3f> &coords) {
    coords.clear();

    Float2 markerRealSize = ResManager::Self()->GetResParams().markerSize;

    // 根据 marker 实际大小建立世界坐标系，原点在中心，marker 位于 XY 平面。
    // 当 marker 处于正方向时（左上角是参考点，4 个点逆时针顺序），X 朝右，Y 朝下，Z 朝内

    coords.emplace_back(-0.5 * markerRealSize.w, -0.5 * markerRealSize.h, 0.0); // 左上角
    coords.emplace_back(-0.5 * markerRealSize.w, 0.5 * markerRealSize.h, 0.0); // 左下角
    coords.emplace_back(0.5 * markerRealSize.w, 0.5 * markerRealSize.h, 0.0); // 右下角
    coords.emplace_back(0.5 * markerRealSize.w, -0.5 * markerRealSize.h, 0.0); // 右上角

//    for (int i = 0; i < coords.size(); ++i) {
//        Log::e(Log::PROCESSOR_TAG, "3d coords %f %f %f", coords[i].x, coords[i].y, coords[i].z);
//    }
}

void MarkerAR::getMarkerCoordinates(const std::vector<cv::Point2f> &bufCoords,
                                    std::vector<cv::Point2f> &coords) {
    coords.clear();
    coords.reserve(bufCoords.size());

    float bufRatio = ResManager::Self()->GetResParams().bufRatio;
    for (auto &point : bufCoords) {
        coords.emplace_back(point.x / bufRatio, point.y / bufRatio);
    }

//    Log::e(Log::PROCESSOR_TAG, "marker coords ------------------");
//    for (int i = 0; i < coords.size(); ++i) {
//        Log::e(Log::PROCESSOR_TAG, "marker coords %f %f", coords[i].x, coords[i].y);
//    }
}
