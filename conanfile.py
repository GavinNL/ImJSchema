from conan.tools.files import copy
from conan import ConanFile
from conan.tools.cmake import CMakeToolchain
import os

class ImJSchemaBuildRecipe(ConanFile):
    settings = "os", "compiler", "build_type", "arch"
    generators = "CMakeDeps", "CMakeToolchain"

    def requirements(self):
        self.requires("nlohmann_json/3.11.3")
        self.requires("imgui/1.91.8-docking")

        if self.settings.os != "Emscripten":
            self.requires("catch2/3.6.0")
            self.requires("sdl/2.30.2")

    def generate(self):
        # Copy the source code and some of the extra src files to the build/imgui_src folder
        copy(self, "res/*", self.dependencies["imgui"].package_folder, os.path.join(self.build_folder, "imgui_src") )
        copy(self, "include/*", self.dependencies["imgui"].package_folder, os.path.join(self.build_folder, "imgui_src") )

