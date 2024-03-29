//
// Created by caolong on 2020/6/22.
//

#pragma once

namespace clt {

#define ClassWrapper(Type) public:using Wrapper = Type;
#define ClassDeclare(type) public:constexpr static const char *target = #type;

#define VarDeclare(var) public:constexpr static const char *var_##var = #var;

  /**
   * 关键组件的超基类，利用统一接口，规范设计
   * @tparam Args
   */
  template<typename ...Args>
  class IComFunc {
  public:
    IComFunc() = default;

    virtual ~IComFunc() = default;

    virtual bool Init(Args ...args) = 0;

    virtual void DeInit() = 0;
  };

  template<>
  class IComFunc<> {
  public:
    IComFunc() = default;

    virtual ~IComFunc() = default;

    virtual bool Init() = 0;

    virtual void DeInit() = 0;
  };

  /**
   * 提供更新
   * @tparam Args
   */
  template<typename ...Args>
  struct IComUpdate {
    IComUpdate() = default;

    virtual ~IComUpdate() = default;

    virtual void Update(Args ...) = 0;
  };

  template<>
  struct IComUpdate<> {
    IComUpdate() = default;

    virtual ~IComUpdate() = default;

    virtual void Update() = 0;
  };

}