// SPDX-License-Identifier: Apache-2.0
// Copyright 2023 The Foundry Visionmongers Ltd
#include <iostream>

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

/**
 * A dummy type that logs when it is constructed, assigned, etc.
 */
struct ConstructionLogger {
  ConstructionLogger() { std::cout << "ConstructionLogger() -> " << this << "\n"; }
  ~ConstructionLogger() { std::cout << "~ConstructionLogger() -> " << this << "\n"; }
  ConstructionLogger(const ConstructionLogger& other) {
    std::cout << "ConstructionLogger(const ConstructionLogger& " << &other << ") -> " << this
              << "\n";
  }
  ConstructionLogger(ConstructionLogger&& other) noexcept {
    std::cout << "ConstructionLogger(ConstructionLogger&& " << &other << ") -> " << this << "\n";
  }
  ConstructionLogger& operator=(const ConstructionLogger& other) noexcept {
    std::cout << "operator=(const ConstructionLogger& " << &other << ") -> " << this << "\n";
    return *this;
  }
  ConstructionLogger& operator=(ConstructionLogger&& other) noexcept {
    std::cout << "operator=(ConstructionLogger&& " << &other << ") -> " << this << "\n";
    return *this;
  }
  bool operator==(const ConstructionLogger& other) const { return this == &other; }
  int count{0};
};

/**
 * Specialise std::hash so that ConstructionLogger can be put in a
 * std::unordered_set.
 */
template <>
struct std::hash<ConstructionLogger> {
  std::size_t operator()(const ConstructionLogger& key) const {
    return reinterpret_cast<std::size_t>(&key);
  }
};

/**
 * Wrap unordered_set to add logging.
 */
struct ConstructionLoggerSet : std::unordered_set<ConstructionLogger> {
  using Base = std::unordered_set<ConstructionLogger>;
  ConstructionLoggerSet() { std::cout << "ConstructionLoggerSet() -> " << this << "\n"; }
  ~ConstructionLoggerSet() {
    //    Base::~Base();
    std::cout << "~ConstructionLoggerSet() -> " << this << "\n";
  }
  ConstructionLoggerSet(const ConstructionLoggerSet& other) : Base{other} {
    std::cout << "ConstructionLoggerSet(const ConstructionLoggerSet& " << &other << ") -> " << this
              << "\n";
  }
  ConstructionLoggerSet(ConstructionLoggerSet&& other) noexcept : Base{std::move(other)} {
    // NOLINTNEXTLINE(*-use-after-move)
    std::cout << "ConstructionLoggerSet(ConstructionLoggerSet&& " << &other << ") -> " << this
              << "\n";
  }
  ConstructionLoggerSet& operator=(const ConstructionLoggerSet& other) noexcept {
    std::cout << "operator=(const ConstructionLoggerSet& " << &other << ") -> " << this << "\n";
    Base::operator=(other);
    return *this;
  }
  ConstructionLoggerSet& operator=(ConstructionLoggerSet&& other) noexcept {
    std::cout << "operator=(ConstructionLoggerSet&& " << &other << ") -> " << this << "\n";
    Base::operator=(std::move(other));
    return *this;
  }
};

/**
 * Use the same pybind11 `set_caster` for our wrapped type as is used
 * for `std::unordered_set`.
 */
namespace pybind11::detail {
template <>
struct type_caster<ConstructionLoggerSet> : set_caster<ConstructionLoggerSet, ConstructionLogger> {
};
}  // namespace pybind11::detail

struct DefaultArged {
  // NOLINTNEXTLINE(readability-convert-member-functions-to-static)
  int useUserDefinedType(ConstructionLogger& constructionLogger) {
    std::cout << "DefaultArged::useUserDefinedType(ConstructionLogger& " << &constructionLogger
              << ")\n";
    std::cout << "++constructionLogger.count = " << (++constructionLogger.count) << "\n";
    return constructionLogger.count;
  }

  // NOLINTNEXTLINE(readability-convert-member-functions-to-static)
  std::size_t useSet(ConstructionLoggerSet& container) {
    std::cout << "DefaultArged::useSet(ConstructionLoggerSet& " << &container << ")\n";
    std::cout << "container.size() = " << container.size() << "\n";
    std::cout << "container.emplace()\n";
    container.emplace();
    std::cout << "container.size() = " << container.size() << "\n";
    return container.size();
  }
};

namespace py = pybind11;

void registerConstructionLogger(py::module_& mod) {
  std::cout << ">>> registerConstructionLogger begin\n";
  py::class_<ConstructionLogger>(mod, "ConstructionLogger").def(py::init<>());

  py::class_<DefaultArged>(mod, "DefaultArged")
      .def(py::init<>())
      .def("useUserDefinedType", &DefaultArged::useUserDefinedType,
           py::arg("constructionLogger") = ConstructionLogger{})
      .def("useSet", &DefaultArged::useSet, py::arg("container") = ConstructionLoggerSet{});
  std::cout << "<<< registerConstructionLogger end\n";
}
