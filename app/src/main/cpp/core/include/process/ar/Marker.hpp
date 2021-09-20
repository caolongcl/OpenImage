//
// Created by caolong on 2021/3/14.
//

#pragma once

#include <softarch/std.hpp>
#include <opencv2/opencv.hpp>
#include <softarch/VarType.hpp>
#include <softarch/IComFunc.hpp>

namespace clt {
  /*
   * 检测、标记、解码
   */
  class Marker final : public IComFunc<> {
  public:
    struct Item {
      Item() : code(""), id(-1) {}

      std::vector<cv::Point2f> corners; // 4 个角点
      std::string code; // id 解码字符串
      int id; // id 解码后的数字
    };

    using Code = std::bitset<10>;

  public:
    Marker() = default;

    ~Marker() = default;

    bool Init() override;

    void DeInit() override;

    void Detect(cv::Mat &imgGray);

    const std::vector<Item> &GetMakers() const;

  private:
    /**
     * 检测角点，得到可能的 marker 轮廓点集
     * @param imgGray
     * @param possibleMarkers
     * @param minContoursNum 最少点数
     * @param minSideLength 每边最小长度
     */
    static void detect(cv::Mat &imgGray, std::vector<Item> &possibleMarkers, int minContoursNum, int minSideLength);

    /**
     * 解析候选 marker 区域中的汉明码
     * @param imgGray
     * @param possibleMarkers
     * @param codes
     * @param finalMarkers
     */
    static void recognize(cv::Mat &imgGray, std::vector<Item> &possibleMarkers, const std::vector<Code> &codes,
                          std::vector<Item> &finalMarkers);

    /**
     * 对 marker 轮廓坐标坐标精细化采样
     * @param imgGray
     * @param finalMarkers
     */
    static void adjust(const cv::Mat &imgGray, std::vector<Item> &finalMarkers);

    /**
     * 根据边界是否全黑检查有效 marker
     * @param image
     * @return
     */
    static bool validMarkerByBoard(const cv::Mat &image);

    /**
     * 解析 marker 的汉明码
     * @param image
     * @param bitMatrix
     */
    static void decodeMarker(cv::Mat &image, cv::Mat &bitMatrix);

    /**
     * 计算汉明距离
     * @param bitMatrix
     * @param codes
     * @param rotateCounts
     * @param id
     * @return
     */
    static bool validMarkerByHammingDistance(cv::Mat &bitMatrix, const std::vector<Code> &codes,
                                             int &rotateCounts, int &id);

    static int hammingDistance(const Code &f, const Code &s);

    /**
     * 5 x 5 位图解析 marker id
     * @param bitMatrix
     * @return
     */
    static Code hammingCode(const cv::Mat &bitMatrix);

  private:
    constexpr static const double APPROXIMATION_CURVE_EPS = 0.1;
    constexpr static const int MARKER_COLS_ROWS = 7;
    constexpr static const int MARKER_CELL_SIZE = 4;
    constexpr static const int MARKER_SIZE = MARKER_COLS_ROWS * MARKER_CELL_SIZE;

  private:
    std::unordered_map<int, Item> m_makers;
    std::vector<Item> m_makersVec;
    std::vector<Code> m_normalCodes;
  };
}
