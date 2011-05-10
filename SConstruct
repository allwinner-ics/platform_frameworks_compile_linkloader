#from http://www.scons.org/wiki/SconstructMultiple
#usage: scons mode=debug
#get the mode flag from the command line
#default to 'release' if the user didn't specify
mymode = ARGUMENTS.get('mode', 'release')   #holds current mode

#check if the user has been naughty: only 'debug' or 'release' allowed
if not (mymode in ['debug', 'release']):
    print "Error: expected 'debug' or 'release', found: " + mymode
    Exit(1)

#tell the user what we're doing
print '**** Compiling in ' + mymode + ' mode...'

if mymode == 'debug':
    CFLAGS=['-Wall', '-Werror', '-g']
    CXXFLAGS=['-Wall', '-Werror', '-g']
else:  #default mode(release)
    CFLAGS=['-Wall', '-Werror', '-O2']
    CXXFLAGS=['-Wall', '-Werror', '-fno-exceptions', '-O2']

env = Environment(CFLAGS=CFLAGS,
                  CXXFLAGS=CXXFLAGS,
                  CPPPATH=['utils'])

env.Program('elfreader',
            source=['elf_header.cpp',
                    'elf_section_header.cpp',
                    'elf_object.cpp',
                    'main.cpp',
                    'utils/helper.cpp',
                    'utils/term.cpp'])
