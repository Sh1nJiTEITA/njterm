#pragma once
#ifndef NJ_VK_UTILS_H
#define NJ_VK_UTILS_H

#include <any>
#include <vector>

namespace nj::ren {

class VarHandles {
  public:
    VarHandles() { vars.reserve(1000); }

    template <typename T> std::decay_t<T> &Handle(T &&var) {
        using U = std::decay_t<T>;
        vars.push_back(std::forward<T>(var));
        return std::any_cast<U &>(vars.back());
    }

  private:
    std::vector<std::any> vars;
};

} // namespace nj::ren

#endif
