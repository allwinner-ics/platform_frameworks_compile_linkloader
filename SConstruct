COMMON_CFLAGS = ['-Wall', '-Werror']
COMMON_CXXFLAGS = ['-Wall', '-Werror', '-fno-exceptions']

configs = {
    'debug': {
        'CFLAGS': COMMON_CFLAGS + ['-g'],
        'CXXFLAGS': COMMON_CXXFLAGS + ['-g']
    },

    'release': {
        'CFLAGS': COMMON_CFLAGS + ['-O2'],
        'CXXFLAGS': COMMON_CXXFLAGS + ['-O2']
    },
}

toolsets = {
    'gcc': {
        'CC': 'gcc',
        'CXX': 'g++',
    },

    'clang': {
        'CC': 'clang',
        'CXX': 'clang++',
    },
}

mode = ARGUMENTS.get('mode', 'release')
toolset = ARGUMENTS.get('toolset', 'gcc')

if not mode in configs:
    print 'ERROR: Unknown building mode:', mode
    Exit(1)

if not toolset in toolsets:
    print 'ERROR: Unknown toolset:', toolset
    Exit(1)

build_config = configs[mode]
build_toolset = toolsets[toolset]

print '===> BUILDING IN', mode.upper(), 'MODE ...'

import os

env = Environment(CC=build_toolset['CC'],
                  CXX=build_toolset['CXX'],
                  CFLAGS=build_config['CFLAGS'],
                  CXXFLAGS=build_config['CXXFLAGS'],
                  CPPPATH=['.', 'include'],
                  ENV={'PATH': os.environ['PATH']})

env.ParseConfig('llvm-config --cxxflags --ldflags --libs support')

env.Program('rsloader',
            source=['lib/ELFHeader.cpp',
                    'lib/ELFSectionHeader.cpp',
                    'lib/ELFSymbol.cpp',
                    'lib/ELFTypes.cpp',
                    'utils/helper.cpp',
                    'utils/raw_ostream.cpp',
                    'main.cpp'])
