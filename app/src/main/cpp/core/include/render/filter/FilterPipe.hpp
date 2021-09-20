//
// Created by caolong on 2020/8/10.
//

#pragma once

#include <render/filter/IFilter.hpp>
#include <softarch/ComType.hpp>
#include <render/RenderObserver.hpp>
#include <softarch/Observer.hpp>
#include <utils/Utils.hpp>

namespace clt {

  class ProcessPipe;

  class OESCopier;

  class Copier;

  class IProcessTextureReader;

  class Texture;

  class SquareModel;

  class FilterWithShader;

  /**
   * 滤镜渲染管道,管理所有滤镜
   */
  class FilterPipe final
      : public IComFunc<std::shared_ptr<OESCopier>, std::shared_ptr<Copier>, std::shared_ptr<IProcessTextureReader>>,
        public Observer<OPreviewSize>,
        public std::enable_shared_from_this<FilterPipe> {
  VarDeclare(target_position)
  VarDeclare(position_ratio)
  public:
    FilterPipe() = default;

    ~ FilterPipe() = default;

    bool Init(std::shared_ptr<OESCopier> oesCopier,
              std::shared_ptr<Copier> copier,
              std::shared_ptr<IProcessTextureReader>) override;

    void DeInit() override;

    /**
     * 使能某个滤镜
     * @param type
     * @param enable
     */
    void EnableFilter(const std::string &type, bool enable);

    void ClearFilters();

    /**
     * 根据参数名字分派参数到对应的滤镜
     * @param name
     * @param var
     */
    void DispatchVar(const std::string &name, const Var &var);

    /**
     * 获取输入OESCopier的纹理
     * @return
     */
    GLuint PreviewTexId() const;

    /**
     * 执行所有滤镜操作一帧
     */
    void Filter();

  private:
    /**
     * 监测到预览大小变化
     * @param t
     */
    void OnUpdate(OPreviewSize &&t) override;

    /**
     * 更新所有滤镜的输入输出纹理
     */
    void updateFiltersTexture();

    /**
     * 根据滤镜优先级排序
     */
    void updateFiltersPriority();

    /**
     * 更新所有滤镜输入输出纹理大小
     */
    void updateFilterSize();

    /**
     * 添加滤镜
     * @param type
     */
    void push(const std::string &type);

    /**
     * 删除滤镜
     * @param type
     */
    void pop(const std::string &type);

    /**
     * 更新当前纹理索引
     * @param index
     */
    static void updateCurTextureIndex(int &index);

    /**
     * 共有两个纹理作为每个滤镜的输入输出
     * 前一个纹理的输入是后一个纹理的输出，构成一个链条
     * TODO 待支持复杂的滤镜图
     */
    static const int sFilterTextureCount = 2;

  private:
    /**
     * 用于绘制到纹理
     */
    GLuint m_fbo;

    std::shared_ptr<Texture> m_oesTexture;
    std::shared_ptr<OESCopier> m_oesCopier;
    std::shared_ptr<Copier> m_copier;
    std::shared_ptr<IProcessTextureReader> m_processTextureReader;
    std::shared_ptr<SquareModel> m_squareModel;

    /**
     * 滤镜输入输出纹理
     */
    int m_curTextureIndex;
    std::shared_ptr<Texture> m_filterTextures[sFilterTextureCount];

    // 可用于拷贝到屏幕上的纹理
    std::shared_ptr<Texture> m_copierBlitTexture;

    /**
     * 存储滤镜链中的滤镜
     */
    std::list<std::shared_ptr<FilterWithShader>> m_filters;
    std::unordered_map<std::string, std::shared_ptr<FilterWithShader>> m_filtersMap;
  };
}
