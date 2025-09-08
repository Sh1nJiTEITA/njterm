#pragma once
#ifndef NJ_VERTEX_H
#define NJ_VERTEX_H

#include <glm/glm.hpp>
#include <vulkan/vulkan.hpp>
#include <vulkan/vulkan_structs.hpp>

namespace nj::ren {

// clang-format off
struct Vertex {
    glm::vec2 pos;
    glm::vec3 col;

    static auto BindingDescription() -> vk::VertexInputBindingDescription;
    static auto AttributeDescriptions() -> std::vector<vk::VertexInputAttributeDescription>;
};
// clang-format on

} // namespace nj::ren

#endif
