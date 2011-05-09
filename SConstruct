env = Environment(CFLAGS=['-Wall', '-Werror', '-O2'],
                  CXXFLAGS=['-Wall', '-Werror', '-fno-exceptions', '-O2'],
                  CPPPATH=['utils'])

env.Program('elfreader',
            source=['elf_header.cpp',
                    'elf_object.cpp',
                    'main.cpp',
                    'utils/helper.cpp',
                    'utils/term.cpp'])
