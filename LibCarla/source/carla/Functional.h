// Copyright (c) 2017 Computer Vision Center (CVC) at the Universitat Autonoma
// de Barcelona (UAB).
//
// This work is licensed under the terms of the MIT license.
// For a copy, see <https://opensource.org/licenses/MIT>.

#pragma once  // 防止头文件被多次包含

#include <utility>  // 包含标准库中用于处理完美转发的头文件

namespace carla {

  class Functional {
  public:

      /// 创建一个递归调用对象，将自身作为第一个参数传递给 @a func。
      /// 用法示例：创建递归 lambda
    template <typename FuncT>
    static auto MakeRecursive(FuncT &&func) {
      return Recursive<FuncT>(std::forward<FuncT>(func));
    }

    /// Creates an "overloaded callable object" out of one or more callable
    /// objects, each callable object will contribute with an overload of
    /// operator(). Use case: combine several lambdas into a single lambda.
    template <typename... FuncTs>
    static auto MakeOverload(FuncTs &&... fs) {
      return Overload<FuncTs...>(std::forward<FuncTs>(fs)...);
    }

    /// @see MakeRecursive and MakeOverload.
    template <typename... FuncTs>
    static auto MakeRecursiveOverload(FuncTs &&... fs) {
      return MakeRecursive(MakeOverload(std::forward<FuncTs>(fs)...));
    }

  private:

    template <typename... Ts>
    struct Overload;

    template <typename T, typename... Ts>
    struct Overload<T, Ts...> : T, Overload<Ts...> {
      Overload(T &&func, Ts &&... rest)
        : T(std::forward<T>(func)),
          Overload<Ts...>(std::forward<Ts>(rest)...) {}

      using T::operator();

      using Overload<Ts...>::operator();
    };

    template <typename T>
    struct Overload<T> : T {
      Overload(T &&func) : T(std::forward<T>(func)) {}

      using T::operator();
    };

    template<typename T>
    struct Recursive {

      explicit Recursive(T &&func) : _func(std::forward<T>(func)) {}

      template<typename... Ts>
      auto operator()(Ts &&... arguments) const {
        return _func(*this, std::forward<Ts>(arguments)...);
      }

    private:

      T _func;
    };

  };

} // namespace carla
