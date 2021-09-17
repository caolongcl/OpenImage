//
// Created by caolong on 2021/3/14.
//

#pragma once

#include <utils/Utils.hpp>
#include <process/base/BufferProcessTask.hpp>
#include <process/ar/Marker.hpp>
#include <process/base/ProcessUtils.hpp>
#include <process/ar/CalibrateCamera.hpp>
#include <softarch/Math.hpp>
#include <res/ResManager.hpp>

namespace clt {

    class MarkerAR final : public IBufferProcessTask {
    ClassDeclare(MarkerAR)
    ClassWrapper(BufferProcessTask)
    public:
        MarkerAR();

        ~MarkerAR() = default;

        bool Init() override;

        void DeInit() override;

        void Process(std::shared_ptr<Buffer> buf) override;

    private:
        void process(const Buffer &buf);

//        void drawMarkerBoard(const std::vector<Marker::Item>& markers);

        /**
        * 根据实际点和图像点估计相机视图旋转和平移矩阵
        * @param corners3d
        * @param corners2d
        * @param cameraMatrix
        * @param distCoeff
        * @param RMat
        * @param TVec
         * @return
         */
        static bool estimateCameraView(const std::vector<cv::Point3f> &corners3d,
                                       const std::vector<cv::Point2f> &corners2d,
                                       const cv::Mat &cameraMatrix, const cv::Mat &distCoeff,
                                       cv::Mat &RMat, cv::Mat &TVec);

        /**
         * 获取模型视图矩阵 view * model
         * @param RMat
         * @param TVec
         * @param modelView
         */
        static void estimateModelView(const cv::Mat &RMat,
                                      const cv::Mat &TVec,
                                      cv::Mat &modelView);

        /**
         * 获取投影矩阵 projection
         * @param cameraMatrix
         * @param width 预览图像宽
         * @param height 预览图像高
         * @param projection
         */
        static void estimateProjection(const cv::Mat &cameraMatrix,
                                       float width, float height,
                                       float near, float far,
                                       cv::Mat &projection);

        static void convertMatToGlm(const cv::Mat &modelView, const cv::Mat &projection,
                                    glm::mat4 &glModelView, glm::mat4 &glProjection);

        /**
         * 获取 marker 的三维空间坐标
         * @param coords
         */
        static void getMarkerRealCoordinates(std::vector<cv::Point3f> &coords, Float2 markerSize);

        /**
         * 获取 marker 的像素坐标
         * @param coords
         */
        static void getMarkerCoordinates(const std::vector<cv::Point2f> &bufCoords,
                                         std::vector<cv::Point2f> &coords, float bufRatio);
    private:
        std::shared_ptr<Marker> m_marker;
        CameraData m_params;
        CalibrateData m_calibrateData;
        bool m_cameraDataGot;
        TimeStatics m_timesStatics;
    };
}

