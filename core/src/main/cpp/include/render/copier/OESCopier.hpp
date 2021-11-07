//
// Created by caolong on 2020/8/10.
//

#pragma once

#include <softarch/ComType.hpp>
#include <softarch/Observer.hpp>
#include <utils/Utils.hpp>
#include <softarch/GLUtils.hpp>

namespace clt {

  class Shader;

  class Texture;

  class SquareModel;

  /**
   * 拷贝 OES 格式纹理为 RGBA 格式纹理，便于之后 FilterPipe 和 ProcessPipe 处理
   */
  class OESCopier final : public IComFunc<>,
                          public Observable<OPreviewSize> {
  VarDeclare(vflip)
  VarDeclare(hflip)
  VarDeclare(ratio)
  public:
    OESCopier();

    ~OESCopier() = default;

    bool Init() override;

    void DeInit() override;

    /**
     * 绘制输入纹理到输出纹理
     */
    void CopyFrame();

    /**
     * 根据 m_ratio, m_rotation, 输入纹理大小更新纹理坐标（纹理裁剪，旋转，翻转）
     */
    bool Update();

    void SetInput(std::shared_ptr<Texture> input);

    void SetOutput(std::shared_ptr<Texture> output);

    std::shared_ptr<Texture> &GetOutput();

    OESCopier &SetSize(int width, int height);

    OESCopier &SetRotation(int rotation);

    OESCopier &SetRatio(int ratio);

    OESCopier &SetVFlip(bool vFlip);

    OESCopier &SetHFlip(bool hFlip);

  private:
    /**
     * 对预览纹理进行裁剪、旋转等
     */
    void update();

    /**
     * 根据设置宽高比计算纹理坐标偏移（用于裁剪）
     * @param ratio
     * @param originPreviewW
     * @param originPreviewH
     * @param xOffset
     * @param yOffset
     * @param targetPreviewW
     * @param targetPreviewH
     */
    static void update(int ratio, int originPreviewW, int originPreviewH,
                       float &xOffset, float &yOffset,
                       int &targetPreviewW, int &targetPreviewH);

    /**
     * 旋转纹理
     * @param rotate
     * @param tex
     */
    static void rotateTex(int rotate, GLfloat *tex);

    /**
     * 翻转纹理坐标
     * @param vFlip
     * @param hFlip
     * @param tex
     */
    static void flipTex(bool vFlip, bool hFlip, GLfloat *tex);

    /**
     * 将纹理 row0 和 row1 行的坐标交换
     * @param row0
     * @param row1
     * @param tex
     */
    static void swapTexRow(int row0, int row1, GLfloat *tex);

    /**
     * 值翻转，1.0 - origin
     * @param origin
     * @param flip
     * @return
     */
    static float flip(float origin, bool flip);

  private:
    std::shared_ptr<Shader> m_shader;
    std::shared_ptr<Texture> m_input;
    std::shared_ptr<Texture> m_output;
    std::shared_ptr<SquareModel> m_square;

    int m_rotation{};
    int m_ratio{};
    bool m_vFlip{};
    bool m_hFlip{};
  };

}
