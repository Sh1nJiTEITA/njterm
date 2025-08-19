#include "nj_surface.h"

namespace nj::ren {

Surface::Surface(InstanceH inst, vk::SurfaceKHR sur) {
    handle = vk::UniqueSurfaceKHR(sur, inst->Handle());
}

std::string Surface::HandleName() const noexcept { return "Surface"; }

} // namespace nj::ren
