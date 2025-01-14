from conan import ConanFile

class yatRecipe(ConanFile):
    name = "yat"
    version = "1.21.3"
    package_type = "library"
    user = "soleil"
    python_requires = "base/[>=1.0]@soleil/stable"
    python_requires_extend = "base.Library"

    license = "GPL-2"
    author = "stephane.poirier@synchrotron-soleil.fr"
    url = "https://gitlab.synchrotron-soleil.fr/software-control-system/libraries/yat"
    description = "Yet Another Toolkit Library"
    topics = ("utility", "control-system")

    settings = "os", "compiler", "build_type", "arch"
    options = {"shared": [True, False], "fPIC": [True, False]}
    default_options = {"shared": False, "fPIC": True}

    exports_sources = "CMakeLists.txt", "src/**", "include/**"

    def package_info(self):
        self.cpp_info.libs = ["yat"]

        if self.settings.os == "Linux":
            self.cpp_info.system_libs = [ "pthread", "dl"]

        if self.settings.os == "Windows":
            self.cpp_info.system_libs += [ "WS2_32", "SHELL32"]
            if self.options.shared:
                self.cpp_info.defines += ["YAT_DLL"]
