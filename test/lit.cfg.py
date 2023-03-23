import glob
import os

from lit import formats

ROOT_PATH = os.path.realpath(os.path.join(
    os.path.dirname(os.path.realpath(__file__)), ".."))

# We already implicitly expect the installation structure to be set up such that
# llvm-kompile-testing etc. are under build/install/bin, so this path is fine to
# hard-code for our testing setup. The build directory (i.e. before
# installation) doesn't work the same because the python package structure for
# kllvm hasn't been set up yet (no __init__.py).
#
# There are some cases (e.g. in Nix) where we need to override this path, so we
# allow it to be set in the environment as an option.
BINDINGS_INSTALL_PATH = os.environ.get(
    'BINDINGS_INSTALL_PATH',
    os.path.join(ROOT_PATH, "build", "install", "lib", "kllvm", "python"))

INCLUDE_INSTALL_PATH = os.environ.get(
    'INCLUDE_INSTALL_PATH',
    '$(llvm-kompile --include-dir)')

PYTHON_INTERPRETER = os.environ.get('PYTHON_INTERPRETER', 'python3')

config.name = 'llvm-backend'
config.test_source_root = os.path.join(ROOT_PATH, "test")

config.test_format = formats.ShTest(True)

config.suffixes = ['.kore', '.py']

config.excludes = [
    'macros.kore',
    'syntaxDefinition.kore',
    'Inputs',
    'lit.cfg.py',
    'test_bindings.py'
]

# When lit is launched, it doesn't inherit any environment variables from the
# parent process' environment. This breaks the Nix derivations for the host
# compiler and linker, as they depend on NIX_* environment variables being set.
#
# The solution is to manually add every NIX_* enviroment variable to the test
# runner's environment if the LIT_USE_NIX variable is set (see test.nix for
# where this gets used).
if os.getenv('LIT_USE_NIX'):
    config.environment.update({
        k: v for k, v in os.environ.items()
        if k.startswith('NIX')
    })

# An interaction between lit and the shell on macOS means that we can't have
# multiline substitutions natively. This function sanitizes them so that we can
# use them cross-platform while retaining nice source code.
def one_line(s):
    return s.strip().replace('\n', ' ; ').replace('do ;', 'do').replace("' ; '", r"'\\n'")


config.substitutions.extend([
    ('%kompile', 'llvm-kompile-testing'),
    ('%interpreter', '%kompile %s main -o %t.interpreter'),
    ('%search-interpreter', '%kompile %s search -o %t.interpreter'),
    ('%convert-input', '%kore-convert %test-input -o %t.bin'),
    ('%strip-binary', 'kore-strip'),
    ('%arity', 'kore-arity'),

    ('%kllvm-clang', 'clang -I %include-path -I Inputs Inputs/api.c'),

    ('%bindings-path', BINDINGS_INSTALL_PATH),
    ('%include-path', INCLUDE_INSTALL_PATH),
    ('%py-interpreter', PYTHON_INTERPRETER),
    ('%python', 'BINDINGS_INSTALL_PATH=%bindings-path ' + PYTHON_INTERPRETER),

    ('%check-grep', one_line('''
        %run | grep -f %test-grep-out -q
        %run-binary | grep -f %test-grep-out -q
        %run-binary-out ; %kore-convert %t.out.bin | tr -d '\n' | grep -f %test-grep-out -q
    ''')),
    ('%check-diff', one_line('''
        %run | diff - %test-diff-out
        %run-binary | diff - %test-diff-out
        %run-binary-out
        %kore-convert %t.out.bin -o %t.out.kore
        %kore-convert %test-diff-out --to=text | diff - %t.out.kore
    ''')),

    ('%check-dir-grep', one_line('''
        for out in %test-dir-out/*.out.grep; do
            in=%test-dir-in/`basename $out .out.grep`.in
            %t.interpreter $in -1 /dev/stdout | grep -f $out -q || (echo $in && exit 1)
        done
    ''')),

    ('%check-dir-diff', one_line('''
        for out in %test-dir-out/*.out.diff; do
            in=%test-dir-in/`basename $out .out.diff`.in
            %t.interpreter $in -1 /dev/stdout | diff - $out || (echo $in && exit 1)
        done
    ''')),

    ('%run-binary-out', '%t.interpreter %test-input -1 %t.out.bin --binary-output'),
    ('%run-binary', '%convert-input && %t.interpreter %t.bin -1 /dev/stdout'),
    ('%run', '%t.interpreter %test-input -1 /dev/stdout'),

    ('%kprint-check', 'kprint %S %s true | diff - %s.out'),

    ('%input-dir', os.path.join(ROOT_PATH, 'test', 'input')),
    ('%output-dir', os.path.join(ROOT_PATH, 'test', 'output')),

    ('%test-input', os.path.join('%input-dir', '%test-basename.in')),
    ('%test-grep-out', os.path.join('%output-dir', '%test-basename.out.grep')),
    ('%test-diff-out', os.path.join('%output-dir', '%test-basename.out.diff')),
    ('%test-dir-out', os.path.join('%output-dir', '%test-basename')),
    ('%test-dir-in', os.path.join('%input-dir', '%test-basename')),
    ('%test-basename', '`basename %s .kore`'),

    ('%allow-pipefail', 'set +o pipefail'),

    ('%kore-convert', 'kore-convert'),
])

config.recursiveExpansionLimit = 10
