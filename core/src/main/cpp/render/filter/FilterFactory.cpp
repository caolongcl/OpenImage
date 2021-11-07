//
// Created by caolong on 2020/7/20.
//

#include <render/filter/FilterFactory.hpp>
#include <render/filter/filters/ExposureFilter.hpp>
#include <render/filter/filters/BaseFilter.hpp>
#include <render/filter/filters/ColorInvertFilter.hpp>
#include <render/filter/filters/OpacityFilter.hpp>
#include <render/filter/filters/BlurFilter.hpp>
#include <render/filter/filters/WhiteBalanceFilter.hpp>
#include <render/filter/filters/GrayscaleFilter.hpp>
#include <render/filter/filters/GammaFilter.hpp>
#include <render/filter/filters/HueFilter.hpp>
#include <render/filter/filters/MonochromeFilter.hpp>
#include <render/filter/filters/RGBFilter.hpp>
#include <render/filter/filters/SharpenFilter.hpp>
#include <render/filter/filters/MosaicFilter.hpp>
#include <render/filter/filters/WhirlpoolFilter.hpp>
#include <render/filter/filters/TimeColorFilter.hpp>
#include <render/filter/filters/FireFilter.hpp>

using namespace clt;

#define CreateFilter(type, filter) \
if (type == filter::target) {\
return std::make_shared<filter>();}

std::shared_ptr<FilterWithShader> FilterFactory::Create(const std::string &type) {
  CreateFilter(type, ExposureFilter)
  CreateFilter(type, BaseFilter)
  CreateFilter(type, ColorInvertFilter)
  CreateFilter(type, OpacityFilter)
  CreateFilter(type, BlurFilter)
  CreateFilter(type, WhiteBalanceFilter)
  CreateFilter(type, GrayscaleFilter)
  CreateFilter(type, GammaFilter)
  CreateFilter(type, HueFilter)
  CreateFilter(type, MonochromeFilter)
  CreateFilter(type, RGBFilter)
  CreateFilter(type, SharpenFilter)
  CreateFilter(type, MosaicFilter)
  CreateFilter(type, WhirlpoolFilter)
  CreateFilter(type, TimeColorFilter)
  CreateFilter(type, FireFilter)
  return nullptr;
}