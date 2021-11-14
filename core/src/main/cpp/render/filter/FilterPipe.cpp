//
// Created by caolong on 2020/6/14.
//

#include <render/Flow.hpp>
#include <utils/Utils.hpp>
#include <render/filter/FilterPipe.hpp>
#include <render/texture/Texture.hpp>
#include <render/copier/OESCopier.hpp>
#include <render/RenderObserver.hpp>
#include <render/filter/FilterCom.hpp>
#include <render/filter/FilterFactory.hpp>
#include <render/texture/OESTexture.hpp>
#include <render/texture/NormalTexture.hpp>
#include <render/copier/Copier.hpp>
#include <process/base/ProcessSource.hpp>
#include <res/SquareModel.hpp>
#include <render/filter/FilterCom.hpp>

using namespace clt;

bool FilterPipe::Init(std::shared_ptr<OESCopier> oesCopier,
                      std::shared_ptr<Copier> copier,
                      std::shared_ptr<IFeeder> feeder) {
  assert(oesCopier != nullptr);
  assert(copier != nullptr);
  assert(processTextureReader != nullptr);

  Log::v(target, "FilterPipe::Init");

  m_squareModel = std::make_shared<SquareModel>();
  m_squareModel->Init();

  // filter 使用fbo绘制在Texture上
  gles::CreateFbo(m_fbo);

  // 输入纹理，OES格式，内容直接从相机预览而来
  m_oesTexture = std::make_shared<OESTexture>();
  m_oesTexture->Init();

  // m_oesCopier 的输入是相机采集的数据
  m_oesCopier = std::move(oesCopier);
  m_oesCopier->Init();
  m_oesCopier->SetInput(m_oesTexture);

  // 预览纹理大小改变时通知FilterPipe
  m_oesCopier->AddObserver(std::weak_ptr<Observer<OPreviewSize>>(shared_from_this()));

  // m_copier 的输入是滤镜链条最后输出的纹理
  m_copier = std::move(copier);
  m_copier->Init();

  // 负责读取纹理像素传递给 ProcessPipe 处理
  m_feeder = feeder;

  // 初始化备用纹理
  for (auto &tex: m_filterTextures) {
    tex = std::make_shared<NormalTexture>();
    tex->Init();
  }

  m_copierBlitTexture = std::make_shared<NormalTexture>();
  m_copierBlitTexture->Init();

  // 构建纹理链条
  updateFiltersTexture();

  return true;
};

/**
 * 销毁资源
 */
void FilterPipe::DeInit() {
  m_oesCopier->DeInit();
  m_oesCopier = nullptr;

  m_oesTexture->DeInit();
  m_oesTexture = nullptr;

  for (auto &tex: m_filterTextures) {
    tex->DeInit();
    tex = nullptr;
  }

  for (auto &filter:m_filters) {
    filter->DeInit();
  }
  m_filters.clear();
  m_filtersMap.clear();

  m_copier->DeInit();
  m_copier = nullptr;

  m_copierBlitTexture->DeInit();
  m_copierBlitTexture = nullptr;

  m_feeder = nullptr;

  m_squareModel->DeInit();
  m_squareModel = nullptr;

  gles::DeleteFbo(m_fbo);

  Log::v(target, "FilterPipe::DeInit");
};

void FilterPipe::OnUpdate(OPreviewSize &&t) {
  Log::d(target, "FilterPipe::OnUpdate preview size (%d %d)", t.width, t.height);
  updateFilterSize();
}

void FilterPipe::Filter() {
  /// 1. 拷贝给 process。TODO 考虑融合 1 2
//  auto time = std::chrono::steady_clock::now();
  m_feeder->Feed([this](std::shared_ptr<Texture> &texture) {
    gles::SetViewport(0, 0, texture->Width(), texture->Height());
    gles::UseFbo(m_fbo, texture->Id(), [this]() {
      m_oesCopier->CopyFrame();
    });
  });

  /// 2. 处理 OESCopier 输出纹理
  auto output = m_oesCopier->GetOutput();
  gles::SetViewport(0, 0, output->Width(), output->Height());

  const GLuint ids[]{output->Id(), 0};

  gles::UseFbo2MRT(m_fbo, ids, [this]() {
    m_oesCopier->CopyFrame();
  });

  /// 3. 处理通用滤镜
  for (auto &filter : m_filters) {
    output = filter->GetOutput();

    gles::SetViewport(0, 0, output->Width(), output->Height());
    gles::UseFbo(m_fbo, output->Id(), [this, &filter]() {
      filter->Filter();
    });
  }

  /// 4. 处理 Copier 输出纹理
  output = m_copier->GetOutput();
  gles::SetViewport(0, 0, output->Width(), output->Height());
  gles::UseFbo(m_fbo, output->Id(), [this]() {
    m_copier->Copy();
  });

//  Log::d(target, "duration %f ms",
//         std::chrono::duration<double, std::milli>(std::chrono::steady_clock::now() - time).count());
}

GLuint FilterPipe::PreviewTexId() const {
  return m_oesTexture->Id();
}

void FilterPipe::EnableFilter(const std::string &type, bool enable) {
  if (enable) {
    push(type);
  } else {
    pop(type);
  }
}

void FilterPipe::ClearFilters() {
  m_filters.clear();

  updateFiltersPriority();

  // 由于m_filters 变化了，需要更新各个filer的输入输出纹理
  updateFiltersTexture();

  for (auto &filter:m_filtersMap) {
    filter.second->DeInit();
  }
  m_filtersMap.clear();
}

void FilterPipe::DispatchVar(const std::string &name, const Var &var) {
  for (auto &filter : m_filters) {
    if (filter->VarIn(name)) {
      filter->VarSet(name, var);
    }
  }
}

void FilterPipe::updateCurTextureIndex(int &index) {
  index = (index + 1) % sFilterTextureCount;
}

void FilterPipe::updateFiltersTexture() {
  m_curTextureIndex = 0;
  m_oesCopier->SetOutput(m_filterTextures[m_curTextureIndex]);

  for (auto &filter : m_filters) {
    filter->SetInput(m_filterTextures[m_curTextureIndex]);
    updateCurTextureIndex(m_curTextureIndex);
    filter->SetOutput(m_filterTextures[m_curTextureIndex]);
  }

  m_copier->SetInput(m_filterTextures[m_curTextureIndex]);
  m_copier->SetOutput(m_copierBlitTexture);
}

void FilterPipe::updateFiltersPriority() {
  using Type = decltype(m_filters)::value_type;

  m_filters.sort([](const Type &f, const Type &s) -> bool {
    return f->Priority() > s->Priority();  // 按优先级从大到小排列
  });
}

void FilterPipe::updateFilterSize() {
  auto &output = m_oesCopier->GetOutput();

  for (auto &tex : m_filterTextures) {
    if (tex->Width() != output->Width() || tex->Height() != output->Height())
      tex->Upload(output->Width(), output->Height());
  }

  if (m_copierBlitTexture->Width() != output->Width()
      || m_copierBlitTexture->Height() != output->Height())
    m_copierBlitTexture->Upload(output->Width(), output->Height());

  // copier的输出大小依赖于filter的输出大小
  m_copier->OnUpdate({output->Width(), output->Height()});
}

void FilterPipe::push(const std::string &type) {
  auto f = m_filtersMap.find(type);

  if (f != m_filtersMap.cend()) {
    Log::w(target, "FilterPipe::push filter %s already pushed", type.c_str());
    return;
  }

  auto filter = FilterFactory::Create(type);
  if (filter == nullptr) {
    Log::w(target, "FilterPipe::push filter %s invalid", type.c_str());
    return;
  }

  filter->Init(m_squareModel);

  m_filtersMap.emplace(type, filter);
  m_filters.push_back(filter);

  updateFiltersPriority();

  // 由于 m_filters 变化了，需要更新各个 filter 的输入输出纹理
  updateFiltersTexture();

  Log::d(target, "FilterPipe::push filter %s successfully", type.c_str());
}

void FilterPipe::pop(const std::string &type) {
  auto f = m_filtersMap.find(type);

  if (f == m_filtersMap.cend()) {
    Log::w(target, "FilterPipe::pop filter %s already popped", type.c_str());
    return;
  }

  auto filter = f->second;

  m_filters.remove(filter);
  m_filtersMap.erase(type);

  updateFiltersPriority();

  // 由于 m_filters 变化了，需要更新各个 filter 的输入输出纹理
  updateFiltersTexture();

  filter->DeInit();

  Log::d(target, "FilterPipe::pop filter %s successfully", type.c_str());
}