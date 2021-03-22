//
// Created by caolong on 2021/3/14.
//

#include <process/ar/Marker.hpp>
#include <opencv2/core/types_c.h>
#include <res/ResManager.hpp>

using namespace clt;

bool Marker::Init() {
    m_normalCodes.emplace_back(0b0011100010); // 0
    m_normalCodes.emplace_back(0b0010000101); // 1
    m_normalCodes.emplace_back(0b1111010110); // 2
    m_normalCodes.emplace_back(0b0001001101); // 3
    m_normalCodes.emplace_back(0b1110101000); // 4
    return true;
}

void Marker::DeInit() {
    m_makersVec.clear();
    m_makers.clear();
}

void Marker::Detect(cv::Mat &imgGray) {
    std::vector<Item> possibleMarkers;
    std::vector<Item> finalMarkers;
    int minContoursNum = 140;
    int minSideLength = 20;

    detect(imgGray, possibleMarkers, minContoursNum, minSideLength);
    recognize(imgGray, possibleMarkers, m_normalCodes, finalMarkers);
    adjust(imgGray, finalMarkers);

    m_makersVec = std::move(finalMarkers);
//    m_makers.clear();
//
//    for (auto &item : m_makersVec) {
//        m_makers[item.id] = item;
//    }
}

const std::vector<Marker::Item> &Marker::GetMakers() const {
    return m_makersVec;
}

void Marker::detect(cv::Mat &imgGray, std::vector<Item> &possibleMarkers, int minContoursNum, int minSideLength) {
    cv::Mat imgBin;

    // 自适应二值化
    int threshBlockSize = (minContoursNum / 4) * 2 + 1;// 71
    cv::adaptiveThreshold(imgGray, imgBin, 255,
                          cv::ADAPTIVE_THRESH_GAUSSIAN_C,
                          cv::THRESH_BINARY_INV, // 反色处理，方便以轮廓提取
                          threshBlockSize, threshBlockSize / 3.0);

    //ResManager::Self()->SaveMatImage("adaptiveThreshold" + std::to_string(Utils::CurTimeMilli()), imgBin);

    // 形态学开运算去噪和小块
    cv::morphologyEx(imgBin, imgBin, cv::MORPH_OPEN, cv::Mat());

//    ResManager::Self()->SaveMatImage("morphologyEx" + std::to_string(Utils::CurTimeMilli()), imgBin);

    // 检测轮廓
    std::vector<std::vector<cv::Point>> allContours;
    std::vector<std::vector<cv::Point>> contours;
    cv::findContours(imgBin, allContours, cv::RETR_LIST, cv::CHAIN_APPROX_NONE);

    // 排除点少的轮廓
    for (auto &contour : allContours) {
        if (contour.size() > minContoursNum) {
            contours.push_back(contour);
        }
    }

    Log::n(Log::PROCESSOR_TAG, "mark contour size %d size1 %d", allContours.size(), contours.size());

    std::vector<cv::Point> approxPolygon;
    for (auto &contour : contours) {
        // 多边形拟合
        double eps = contour.size() * APPROXIMATION_CURVE_EPS; // 点距离拟合线段最大距离
        cv::approxPolyDP(contour, approxPolygon, eps, true);

        Log::n(Log::PROCESSOR_TAG, "mark contour2 size %d size1 %d", contour.size(), approxPolygon.size());

        // 必须4个点，凸多边形
        if (approxPolygon.size() == 4 && cv::isContourConvex(approxPolygon)) {
            float minSide = std::numeric_limits<float>::max();
            for (int j = 0; j < 4; ++j) {
                cv::Point side = approxPolygon[j] - approxPolygon[(j + 1) % 4];
                minSide = std::min(minSide, static_cast<float>(side.dot(side)));
            }

            // 去掉边较短的
            if (static_cast<int>(minSide) >= minSideLength * minSideLength) {
                // 将点逆时针排序，图像坐标Y轴向下
                auto vec1 = approxPolygon[1] - approxPolygon[0];
                auto vec2 = approxPolygon[2] - approxPolygon[0];
                cv::Point2f v1 = cv::Point2f(vec1.x, vec1.y);
                cv::Point2f v2 = cv::Point2f(vec2.x, vec2.y);
                if (v1.cross(v2) > 0) {
                    std::swap(approxPolygon[1], approxPolygon[3]);
                }

                Item item;
                item.corners.reserve(approxPolygon.size());

                for (int k = 0; k < 4; ++k) {
                    item.corners.emplace_back(approxPolygon[k].x, approxPolygon[k].y);
                }
                possibleMarkers.push_back(item);
            }
        }
    }
}

void Marker::recognize(cv::Mat &imgGray, std::vector<Item> &possibleMarkers, const std::vector<Code> &codes,
                       std::vector<Item> &finalMarkers) {
    finalMarkers.clear();

    cv::Mat markerImage;
    cv::Mat bitMatrix(5, 5, CV_8UC1);

    std::vector<cv::Point2f> dstMakerCoords;
    dstMakerCoords.emplace_back(0.f, 0.f);
    dstMakerCoords.emplace_back(0.f, MARKER_SIZE - 1.f);
    dstMakerCoords.emplace_back(MARKER_SIZE - 1.f, MARKER_SIZE - 1.f);
    dstMakerCoords.emplace_back(MARKER_SIZE - 1.f, 0.f);
    for (auto &possibleMarker : possibleMarkers) {
//        cv::InputArray A(possibleMarker.corners);
//        cv::InputArray B(dstMakerCoords);
//        Log::v(Log::PROCESSOR_TAG, "marker contour checkVector %d %d", A.getMat().checkVector(2, CV_32F),
//               B.getMat().checkVector(2, CV_32F));
        cv::Mat M = cv::getPerspectiveTransform(possibleMarker.corners, dstMakerCoords);
        cv::warpPerspective(imgGray, markerImage, M, cv::Size(MARKER_SIZE, MARKER_SIZE));
        cv::threshold(markerImage, markerImage, 125, 255, cv::THRESH_BINARY | cv::THRESH_OTSU);

//        ResManager::Self()->SaveMatImage("warpPerspective" + std::to_string(Utils::CurTimeMilli()), markerImage);

        if (validMarkerByBoard(markerImage)) {
            decodeMarker(markerImage, bitMatrix);

            Code code = hammingCode(bitMatrix);
            Log::n(Log::PROCESSOR_TAG, "marker hammingCode origin %s", code.to_string().c_str());

            int rotateCounts;
            int id;
            if (validMarkerByHammingDistance(bitMatrix, codes, rotateCounts, id)) {
                Item item;
                item.corners = possibleMarker.corners;
                item.id = id;
                item.code = code.to_string();

                std::rotate(item.corners.begin(), item.corners.begin() + rotateCounts, item.corners.end());
                finalMarkers.push_back(item);
            }
        }
    }
}

void Marker::adjust(const cv::Mat &imgGray, std::vector<Item> &finalMarkers) {
    for (auto &finalMarker : finalMarkers) {
        std::vector<cv::Point2f> &corners = finalMarker.corners;
        cv::cornerSubPix(imgGray, corners, cv::Size(5, 5), cv::Size(-1, -1),
                         cv::TermCriteria(CV_TERMCRIT_ITER, 30, 0.1));
    }
}

bool Marker::validMarkerByBoard(const cv::Mat &image) {
    // 利用边界是否全黑判断 marker 有效性
    for (int y = 0; y < 7; ++y) {
        int cellDelta = (y == 0 || y == 6) ? 1 : 6;
        int cellY = y * MARKER_CELL_SIZE;

        for (int x = 0; x < 7; x += cellDelta) {
            int cellX = x * MARKER_CELL_SIZE;
            int countNonZeros = cv::countNonZero(
                    image(cv::Rect(cellX, cellY, MARKER_CELL_SIZE, MARKER_CELL_SIZE)));
            if (countNonZeros > MARKER_CELL_SIZE * MARKER_CELL_SIZE / 4) {
                return false;
            }
        }
    }

    return true;
}

// 每一行为一个 word， 每个 word 2、4位构成id
void Marker::decodeMarker(cv::Mat &image, cv::Mat &bitMatrix) {
    //Decode the marker
    for (int y = 0; y < 5; ++y) {
        int cellY = (y + 1) * MARKER_CELL_SIZE;

        for (int x = 0; x < 5; ++x) {
            int cellX = (x + 1) * MARKER_CELL_SIZE;
            int countNonZeros = countNonZero(
                    image(cv::Rect(cellX, cellY, MARKER_CELL_SIZE, MARKER_CELL_SIZE)));
            if (countNonZeros > MARKER_CELL_SIZE * MARKER_CELL_SIZE / 2)
                bitMatrix.at<uchar>(y, x) = 1;
            else
                bitMatrix.at<uchar>(y, x) = 0;
        }
    }
}

bool Marker::validMarkerByHammingDistance(cv::Mat &bitMatrix, const std::vector<Code> &codes,
                                          int &rotateCounts, int &id) {
    for (int i = 0; i < codes.size(); ++i) {
        rotateCounts = 0;
        for (; rotateCounts < 4; ++rotateCounts) {
            Code code = hammingCode(bitMatrix);
            if (hammingDistance(code, codes[i]) == 0) {
                id = i;
                return true;
            }
            // 顺时针旋转一次
            cv::rotate(bitMatrix, bitMatrix, cv::ROTATE_90_CLOCKWISE);
        }
    }
    return false;
}

int Marker::hammingDistance(const Code &f, const Code &s) {
    return (f ^ s).count();
}

Marker::Code Marker::hammingCode(const cv::Mat &bitMatrix) {
    Code code;
    for (int i = 0; i < 5; ++i) {
        code[i * 2] = bitMatrix.at<uchar>(4 - i, 3);
        code[i * 2 + 1] = bitMatrix.at<uchar>(4 - i, 1);
    }
    return code;
}
