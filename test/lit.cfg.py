import glob
import os

from lit import formats

ROOT_PATH = os.path.realpath(os.path.join(os.path.dirname(os.path.realpath(__file__)), ".."))

config.name = 'llvm-backend'
config.test_source_root = os.path.join(ROOT_PATH, "test")

config.test_format = formats.ShTest(True)

config.suffixes = ['.kore']

config.excludes = [
    'macros.kore',
    'syntaxDefinition.kore',
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
        k : v for k, v in os.environ.items()
        if k.startswith('NIX')
    })

config.substitutions.extend([
    ('%kompile', 'llvm-kompile-testing'),
    ('%interpreter', '%kompile %s main -o %t.interpreter'),

    ('%check-grep', '%run | grep -f %test-grep-out -q'),
    ('%check-diff', '%run | diff - %test-diff-out'),

    ('%check-dir-grep', '''
        for out in %test-dir-out/*.out.grep; do
            in=%test-dir-in/`basename $out .out.grep`.in
            %t.interpreter $in -1 /dev/stdout | grep -f $out -q || (echo $in && exit 1)
        done
    '''),

    ('%check-dir-diff', '''
        for out in %test-dir-out/*.out.diff; do
            in=%test-dir-in/`basename $out .out.diff`.in
            %t.interpreter $in -1 /dev/stdout | diff - $out || (echo $in && exit 1)
        done
    '''),

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
