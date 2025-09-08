#include "nj_vertex.h"

namespace nj::ren {

// clang-format off

auto Vertex::BindingDescription() -> vk::VertexInputBindingDescription { 
    return vk::VertexInputBindingDescription{}
        .setBinding(0)
        .setStride(sizeof(Vertex))
        .setInputRate(vk::VertexInputRate::eVertex)
    ;
}

auto Vertex::AttributeDescriptions() -> std::vector<vk::VertexInputAttributeDescription> { 
    return { 
        vk::VertexInputAttributeDescription{
            /* location */ 0,
            /* binding  */ 0,
            /* format   */ vk::Format::eR32G32Sfloat,
            /* offset   */ offsetof(Vertex, pos)
        },
        vk::VertexInputAttributeDescription{
            /* location */ 1,
            /* binding  */ 0,
            /* format   */ vk::Format::eR32G32B32Sfloat,
            /* offset   */ offsetof(Vertex, col)
        }
    };
}

// clang-format on

} // namespace nj::ren
