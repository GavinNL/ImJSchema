from conan.tools.files import copy
from conan import ConanFile
from conan.tools.cmake import CMakeToolchain
import os

class ConanTutorialRecipe(ConanFile):
    settings = "os", "compiler", "build_type", "arch"
    generators = "CMakeDeps", "CMakeToolchain"

    def requirements(self):
        self.requires("catch2/3.6.0")
        self.requires("nlohmann_json/3.11.3")
        self.requires("imgui/1.90.3-docking")
        #self.requires("fmt/11.1.4")

        #self.requires("websocketpp/0.8.2")

        if self.settings.os != "Emscripten":
            self.requires("sdl/2.30.2")

    def configure(self):
        pass
        #self.options["websocketpp"].asio = "standalone"


    def layout(self):
        self.build_folder_vars = ["settings.compiler", "settings.arch", "options.shared"]
        pass

    def generate(self):
        copy(self, "res/*", self.dependencies["imgui"].package_folder, os.path.join(self.build_folder, "imgui_src") )
        copy(self, "include/*", self.dependencies["imgui"].package_folder, os.path.join(self.build_folder, "imgui_src") )

