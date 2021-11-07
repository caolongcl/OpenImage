//
// Created by caolong on 2020/7/21.
//

#include <res/ResManager.hpp>
#include <render/filter/filters/MosaicFilter.hpp>

using namespace clt;

MosaicFilter::MosaicFilter()
    : FilterWithShader(target),
      m_mosaicSize({16.0f, 16.0f}) {

}

void MosaicFilter::loadShader() {
  m_shader = ResManager::Self()->LoadShader("mosaic");
  assert(m_shader != nullptr);

  m_uniformLocInputSize = glGetUniformLocation(m_shader->ProgramId(), "uInputSize");
  m_uniformLocMosaicSize = glGetUniformLocation(m_shader->ProgramId(), "uMosaicSize");
}

void MosaicFilter::updateValue() {
  assert(m_input != nullptr);

  m_shader->SetFloat2(m_uniformLocInputSize, Float2(m_input->Width(), m_input->Height()));
  m_shader->SetFloat2(m_uniformLocMosaicSize, m_mosaicSize);
}

void MosaicFilter::registerVar() {
  m_varGroup->VarRegister(var_mosaic_size,
                          {
                              [this](const Var &var) {
                                setMosaicSize(var.ToFloat());
                              },
                              Float2TypeTag{}
                          });
}

MosaicFilter &MosaicFilter::setMosaicSize(const Float &mosaicSize) {
  m_mosaicSize = {mosaicSize.x, mosaicSize.y};
  return *this;
}