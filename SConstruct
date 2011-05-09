env = Environment(CFLAGS=['-Wall', '-Werror', '-O2'],
                  CXXFLAGS=['-Wall', '-Werror', '-O2'],
                  CPPPATH=['utils'])

env.Program('elfreader',
            source=['elf_ident.cpp',
                    'elf_reader.cpp',
                    'main.cpp',
                    'utils/helper.cpp',
                    'utils/term.cpp'])
