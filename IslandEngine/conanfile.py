from conans import ConanFile, CMake

dstFolder='../../bin'

class IslandEngineConan(ConanFile):
    name = 'IslandEngine'
    version = '1.0.0'

    settings = 'os', 'compiler', 'build_type', 'arch'
    generators = 'cmake'

    default_options = { 'boost:shared': True, 'glfw:shared': True }

    def requirements(self):
        self.requires('boost/1.69.0@conan/stable')
        self.requires('stb/20180214@conan/stable')
        self.requires('glfw/3.2.1@bincrafters/stable')
        self.requires('glm/0.9.9.1@g-truc/stable')

    def build(self):
        cmake=CMake(self)
        cmake.configure(defs={
            'PROJECT_VERSION': self.version
        })
        cmake.configure()
        cmake.build()

    def package(self):
        self.copy(self.name+'*', dst=dstFolder)

    def imports(self):
        self.copy('*.dll', dst=dstFolder, src='bin')
        self.copy('*.dylib*', dst=dstFolder, src='lib')
        self.copy('*.so*', dst=dstFolder, src='lib')
