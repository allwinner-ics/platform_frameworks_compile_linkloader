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

toolkits = {
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
toolkit = ARGUMENTS.get('toolkit', 'gcc')

if not mode in build_configurations:
    print 'ERROR: Unknown building mode:', mode
    Exit(1)

build_config = build_configurations[mode]
build_toolkit = toolkits[toolkit]

print '===> BUILDING IN ' + mode.upper() + ' MODE ...'

import os

c_include_path = os.environ['C_INCLUDE_PATH'] \
                      if 'C_INCLUDE_PATH' in os.environ else ''

cplus_include_path = os.environ['CPLUS_INCLUDE_PATH'] \
                      if 'CPLUS_INCLUDE_PATH' in os.environ else ''


env = Environment(CC=build_toolkit['CC'],
                  CXX=build_toolkit['CXX'],
                  CFLAGS=build_config['CFLAGS'],
                  CXXFLAGS=build_config['CXXFLAGS'],
                  CPPPATH=['utils'],
                  ENV = {'PATH' : os.environ['PATH'],
                         'C_INCLUDE_PATH' : c_include_path,
                         'CPLUS_INCLUDE_PATH' : cplus_include_path})

env.Program('elfreader',
            source=['ELFHeader.cpp',
                    'ELFSectionHeader.cpp',
                    'ELFSectionSymTabEntry.cpp',
                    'ELFTypes.cpp',
                    'main.cpp',
                    'utils/helper.cpp',
                    'utils/term.cpp'])
