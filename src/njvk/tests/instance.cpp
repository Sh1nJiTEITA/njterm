#include "handles/builders/nj_build_debug_messanger.h"
#include "handles/builders/nj_build_instance.h"
#include "njvk.h"
#include <catch2/catch_all.hpp>
#include <catch2/catch_test_macros.hpp>
#include <vulkan/vulkan_handles.hpp>

TEST_CASE("Instance", "[creation]") {
    using namespace nj::build;
    auto inst = Builder<vk::Instance>({}).Build();
    auto messenger = Builder<vk::DebugUtilsMessengerEXT>(inst).Build();
}
