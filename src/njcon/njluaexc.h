#pragma once
#ifndef NJ_LUA_EXC_H
#define NJ_LUA_EXC_H
#include <exception>
namespace nj::lua::exc {

class NoReturnTable : public std::exception {
  public:
    virtual const char *what() const noexcept override {
        return "No return table is present in executed lua-code";
    };
};

class NoFieldInTable : public std::exception {
  public:
    virtual const char *what() const noexcept override {
        return "No field are present in table";
    };
};

} // namespace nj::lua::exc
#endif
