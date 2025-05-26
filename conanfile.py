from conan import ConanFile


class CompressorRecipe(ConanFile):
    settings = "os", "compiler", "build_type", "arch"
    generators = "CMakeToolchain", "CMakeDeps"

    def requirements(self):
        self.requires("fmt/11.2.0")
        self.requires("lua/5.4.7")
        self.requires("glfw/3.4")
        self.requires("glm/1.0.1")
        self.requires("vulkan-headers/1.4.309.0", override=True)
        self.requires("vulkan-memory-allocator/3.0.1")

    # def build_requirements(self):
    #     self.tool_requires("cmake/4.0.1")
