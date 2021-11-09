//
// Created by caolong on 2021/3/19.
//

#pragma once

#include <softarch/std.hpp>
#include <softarch/Math.hpp>

namespace clt {
  /**
   * 抽象出统一设置参数的接口
   * 不同类型变量设置接口
   * @tparam T
   */
  template<typename T>
  struct IVariableSet {
    virtual ~IVariableSet() = default;

    virtual void SetVariable(const std::string &varName, const T &var) = 0;
  };

  template<typename T>
  struct VariableSetter {
    using Setter = std::function<void(const T &)>;

    VariableSetter(const Setter &s) : setter(s) {}

    void operator()(const T &var) { setter(var); }

  private:
    Setter setter;
  };

  /**
   * 注册变量，以及变量设置方式
   * @tparam T
   */
  template<typename T>
  struct IVariableRegister {
    virtual ~IVariableRegister() = default;

    virtual bool In(const std::string &varName) = 0;

    virtual void Register(const std::string &varName, VariableSetter<T> &&setter) = 0;

    virtual void UnRegister(const std::string &varName) = 0;

    virtual void Clear() = 0;
  };

  /**
   * 其他组件依赖此类可以获得配置参数的能力
   * @tparam T
   */
  template<typename T>
  struct VariableGroup final : public IVariableSet<T>, public IVariableRegister<T> {
    void Register(const std::string &varName, VariableSetter<T> &&setter) override {
      if (!In(varName)) {
        vars.emplace(varName, std::forward<VariableSetter<T>>(setter));
      }
    }

    void UnRegister(const std::string &varName) override {
      if (In(varName)) {
        vars.erase(varName);
      }
    }

    void SetVariable(const std::string &varName, const T &var) override {
      if (In(varName)) {
        vars.find(varName)->second(var);
      }
    }

    void Clear() override { vars.clear(); }

  private:
    bool In(const std::string &varName) override {
      return vars.find(varName) != vars.cend();
    }

  private:
    std::unordered_map<std::string, VariableSetter<T>> vars;
  };
}
