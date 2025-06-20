#include "handles/nj_instance.h"
#include "nj_build_instance.h"

namespace nj::ren {

Instance::Instance(const std::set<std::string> &inext) {
    handle = build::Builder<vk::Instance>(inext).Build();
}
Instance::Instance(const std::vector<std::string> &inext) {
    handle = build::Builder<vk::Instance>(inext).Build();
}

auto Instance::HandleName() const noexcept -> std::string { return "Instance"; }

} // namespace nj::ren
