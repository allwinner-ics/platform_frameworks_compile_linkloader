COMMON_CFLAGS = ['-Wall', '-Werror']
COMMON_CXXFLAGS = ['-Wall', '-Werror']

build_configurations = {
    'debug': {
        'CFLAGS': COMMON_CFLAGS + ['-g'],
        'CXXFLAGS': COMMON_CXXFLAGS + ['-g']
    },

    'release': {
        'CFLAGS': COMMON_CFLAGS + ['-O2'],
        'CXXFLAGS': COMMON_CXXFLAGS + ['-O2', '-fno-exceptions']
        # FIXME: We should move -fno-exceptions to COMMON_CXXFLAGS.
    },
}

mode = ARGUMENTS.get('mode', 'release')

if not mode in build_configurations:
    print 'ERROR: Unknown building mode:', mode
    Exit(1)

build_config = build_configurations[mode]

print '===> BUILDING IN ' + mode.upper() + ' MODE ...'

env = Environment(CFLAGS=build_config['CFLAGS'],
                  CXXFLAGS=build_config['CXXFLAGS'],
                  CPPPATH=['utils'])

env.Program('elfreader',
            source=['ELFHeader.cpp',
                    'ELFTypes.cpp',
                    'main.cpp',
                    'utils/helper.cpp',
                    'utils/term.cpp'])
