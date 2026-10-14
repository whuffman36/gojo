from conan import ConanFile
from conan.tools.cmake import cmake_layout


# This class defines the rules for any dependencies you
# bring into your project. You can find more info at
# https://docs.conan.io/2/reference/conanfile.html
class exampleRecipe(ConanFile):
    # Each time you change one of these in your .gojo
    # config file, conan will need to reinstall and
    # rebuild your dependencies.
    settings = "os", "compiler", "build_type", "arch"
    generators = "CMakeDeps", "CMakeToolchain"

    def requirements(self):
        # Add dependencies here, ex.
        # self.requires(<package_name>/<version>)
        self.requires("gtest/1.17.0")

    def layout(self):
        cmake_layout(self)
