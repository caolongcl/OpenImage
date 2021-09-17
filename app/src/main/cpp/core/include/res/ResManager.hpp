//
// Created by caolong on 2020/8/3.
//

#pragma once

#include <res/ResHub.hpp>
#include <softarch/IComFunc.hpp>
#include <res/YamlParse.hpp>
#include <opencv2/opencv.hpp>

namespace clt {

    class Font;

    class Shader;

    struct ResParams {
        ResParams() {
            boardSize.w = 6; // 棋盘每行内点数
            boardSize.h = 8; // 棋盘每列内点数
            boardSquareSize.w = 17.8; // 棋盘每格宽度 mm
            boardSquareSize.h = 16.8; // 棋盘每格高度 mm

            markerSize.w = 63.5; // marker 实际宽度 mm
            markerSize.h = 67.5; // marker 实际高度 mm

            bufRatio = 1.0;
        }

        // chessboard
        Integer2 boardSize;
        Float2 boardSquareSize;

        // marker
        Float2 markerSize;

        // process standard buf ratio (buf.w / preview.h)
        float bufRatio;
    };

    /**
     * 校正工具参数
     */
    struct CalibrateData {
        CalibrateData() = default;

        CalibrateData(
                const Integer2 &_boardSize,
                const Float2 &_boardSquareSize,
                const Float2 &_markerSize) :
                boardSize(_boardSize),
                boardSquareSize(_boardSquareSize),
                markerSize(_markerSize) {}

        static YAML::Node Encode(const CalibrateData &rhs) {
            YAML::Node node;
            YAML::Node infoNode;
            infoNode["board_width"] = rhs.boardSize.w;
            infoNode["board_height"] = rhs.boardSize.h;
            infoNode["board_square_width"] = rhs.boardSquareSize.w;
            infoNode["board_square_height"] = rhs.boardSquareSize.h;
            infoNode["marker_width"] = rhs.markerSize.w;
            infoNode["marker_height"] = rhs.markerSize.h;
            node["info"] = infoNode;
            return node;
        }

        static bool Decode(const YAML::Node &node, CalibrateData &rhs) {
            if (!node.IsDefined()
                || !node["info"].IsDefined()) {
                return false;
            }
            rhs.boardSize = {node["info"]["board_width"].as<int>(),
                             node["info"]["board_height"].as<int>()};
            rhs.boardSquareSize = {node["info"]["board_square_width"].as<float>(),
                                   node["info"]["board_square_height"].as<float>()};
            rhs.boardSize = {node["info"]["marker_width"].as<int>(),
                             node["info"]["marker_height"].as<int>()};
            return true;
        }

        const Integer2 GetBoardSize() const {
            return boardSize;
        }

        const Float2 GetBoardSquareSize() const {
            return boardSquareSize;
        }

        const Float2 GetMarkerSize() const {
            return markerSize;
        }

        std::string GetJsonString() const {
            std::stringstream ss;
            ss << "{" << "\"board_width:\"" << boardSize.w << ","
               << "\"board_height:\"" << boardSize.h << ","
               << "\"board_square_width:\"" << boardSquareSize.w << ","
               << "\"board_square_height:\"" << boardSquareSize.h << ","
               << "\"marker_width:\"" << markerSize.w << ","
               << "\"marker_height:\"" << markerSize.h << "}";
            return ss.str();
        }

    private:
        Integer2 boardSize;
        Float2 boardSquareSize;
        Float2 markerSize;
    };

    class ResManager final
            : public IComFunc<> {
    public:
        bool Init() override;

        void DeInit() override;

        /**
         * 扫描path下的文件并加载
         * @param path
         */
        void ScanAndLoad(const std::string &path);

        std::string GetResAbsolutePath(const std::string &file) const;

        void RegisterBaseDir(const std::string &path);

        void RegisterFunctionDir(const std::string &path);

        const std::string &GetBaseAbsolutePath() const;

        const std::string &GetFunctionAbsolutePath() const;

        /**
         * 加载shader
         * @param name
         * @return
         */
        std::shared_ptr<Shader> LoadShader(const std::string &name);

        /**
         * 获取字体
         * @return
         */
        std::shared_ptr<Font> GetGUIFont();

        static void SaveMatImage(const std::string &fileName, const cv::Mat &image);

        static void SaveMatImageImmediate(const std::string &fileName, const cv::Mat &image);

        std::string GetCameraParamsFile() const;

        void UpdateResParamsBufRatio(float ratio);

        const CalibrateData &LoadCalibrateParams();

        const float GetBufferRatio() const {
            return m_realWorldParam.bufRatio;
        }

        void SaveCalibrateParams(Integer2 boardSize, Float2 boardSquareSize, Float2 markerSize);

    private:
        static std::string getShaderName(const YamlParse &yaml);

        static std::string getShaderSrc(const YamlParse &yaml, const std::string &type);

        static std::string getVertexShaderSrc(const YamlParse &yaml);

        static std::string getComputeShaderSrc(const YamlParse &yaml);

        static std::string getFragmentShaderSrc(const YamlParse &yaml);

        static std::shared_ptr<Shader> makeShader(const YamlParse &yaml);

        static std::shared_ptr<Shader> makeShader(const std::string &vertexSrc,
                                                  const std::string &fragmentSrc,
                                                  const std::string &computeSrc);

        void registerFonts(const std::string &name, std::shared_ptr<Font> font);

        bool scanResFiles(const std::string &rootPath);

        void loadFonts();

        void clearOldCalibrateImages();

    private:
        ResHub<Shader> m_shaders;
        ResHub<Font> m_fonts;
        std::list<std::string> m_baseShaderFiles;
        std::list<std::string> m_otherShaderFiles;
        std::list<std::string> m_resFiles;
        std::string m_resPath;  // 读取资源的地方
        std::string m_basePath; // 保存文件地方
        std::string m_functionPath; // 保存功能文件文件地方
        ResParams m_realWorldParam; // 原始默认参数
        CalibrateData m_calibrateData;

    private:
        // 预定义的最小化运行资源
        // oes/default shader
        static const char *s_oes;
        static const char *s_copier;
        static const char *s_oesVertex;
        static const char *s_oesFragment;
        static const char *s_copierVertex;
        static const char *s_copierFragment;

        /**
         * singleton
         */
    public:
        ResManager(const ResManager &) = delete;

        ResManager &operator=(const ResManager &) = delete;

        ResManager(ResManager &&) = delete;

        ResManager &operator=(ResManager &&) = delete;

        ~ResManager() = default;

        static std::shared_ptr<ResManager> Self() {
            std::lock_guard<std::mutex> locker(s_mutex);
            static std::shared_ptr<ResManager> instance(new ResManager());
            return instance;
        }

    private:
        ResManager() = default;

        static std::mutex s_mutex;
    };

}