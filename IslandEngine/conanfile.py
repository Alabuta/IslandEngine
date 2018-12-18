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

    def imports(self):
        self.copy('*.dll', dst=dstFolder, src='bin')
        self.copy('*.dylib*', dst=dstFolder, src='lib')
        self.copy('*.so*', dst=dstFolder, src='lib', excludes=[
            '*boost_atomic*',
            '*boost_chrono*',
            '*boost_container*',
            '*boost_context*',
            '*boost_contract*',
            '*boost_coroutine*',
            '*boost_date_time*',
            '*boost_fiber*',
            '*boost_filesystem*',
            '*boost_graph*',
            '*boost_iostreams*',
            '*boost_locale*',
            '*boost_log_setup*',
            '*boost_log*',
            '*boost_math_c99*',
            '*boost_math_tr1*',
            '*boost_prg_exec_monitor*',
            '*boost_program_options*',
            '*boost_random*',
            '*boost_regex*',
            '*boost_serialization*',
            '*boost_stacktrace_addr2line*',
            '*boost_stacktrace_backtrace*',
            '*boost_stacktrace_basic*',
            '*boost_stacktrace_noop*',
            '*boost_system*',
            '*boost_thread*',
            '*boost_timer*',
            '*boost_type_erasure*',
            '*boost_unit_test_framework*',
            '*boost_wave*',
            '*boost_wserialization*'
        ])








































