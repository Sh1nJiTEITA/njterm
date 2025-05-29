#pragma once
#ifndef NJ_LUA_EXC_H
#define NJ_LUA_EXC_H
#include <exception>
namespace nj::lua::exc {

class NoReturnTable : public std::exception {
  public:
    NoReturnTable() = default;
    ~NoReturnTable() = default;
    virtual const char *what() const noexcept override {
        return "No return table is present in executed lua-code";
    };
};
} // namespace nj::lua::exc
#endif
