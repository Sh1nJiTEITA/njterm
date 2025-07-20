#include "njvkutils.h"

namespace nj::ren {

auto GenerateId() -> uint32_t {
    static uint32_t id_counter{0};
    return id_counter++;
}

} // namespace nj::ren
