import os

from lit import formats

ROOT_PATH = os.path.realpath(os.path.join(os.path.dirname(os.path.realpath(__file__)), ".."))

config.name = 'llvm-backend'
config.test_source_root = os.path.join(ROOT_PATH, "test")

config.test_format = formats.ShTest(True)

config.suffixes = ['.kore']

config.excludes = [
    'input',
    'int',
    'unparse',
    'output'
]

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

    ('%test-input', os.path.join(ROOT_PATH, 'test', 'input', '%test-basename.in')),
    ('%test-grep-out', os.path.join(ROOT_PATH, 'test', 'output', '%test-basename.out.grep')),
    ('%test-diff-out', os.path.join(ROOT_PATH, 'test', 'output', '%test-basename.out.diff')),
    ('%test-dir-out', os.path.join(ROOT_PATH, 'test', 'output', '%test-basename')),
    ('%test-dir-in', os.path.join(ROOT_PATH, 'test', 'input', '%test-basename')),
    ('%test-basename', '`basename %s .kore`'),

    # ('%kcc', os.path.join(ROOT_PATH, ".build", "dist", "bin", "kcc")),
    # ('%compile-object', '%kcc -c %s -o %t'),
    # ('%compile', "%kcc %s -o %t"),
    # ('%check-empty', '%t > %t.out && ! [ -s %t.out ]'),
    # ('%check', "%t 2>&1 | FileCheck %s --allow-empty"),

    # ('%ref-name', '`basename %s .c`.ref'),
    # ('%ref-check', '%t | diff - %ref-name'),

    # ('%kore-check', 'diff %t.pretty %s.kore'),
    # ('%kore-pretty', '%kcc -fpretty-print-kore %t.kore -o %t.pretty'),
    # ('%kore', '%kcc -fsyntax-only %s -o %t.kore'),

    # ('%csmith-home', os.path.join(ROOT_PATH, 'deps', 'csmith')),
    # ('%csmith-cflags', '-I %csmith-home/runtime -DCSMITH_MINIMAL -DNO_PRINTF'),
    # ('%csmith-gcc', 'gcc -std=c11 %csmith-cflags %s -o %t.gcc'),
    # ('%csmith-compile', '%kcc %csmith-cflags %s -o %t.kcc'),
    # ('%csmith-run', '%t.kcc > %t.kcc.out && %t.gcc > %t.gcc.out'),
    # ('%csmith-check', '%csmith-run && diff %t.kcc.out %t.gcc.out'),

    # ('%link-basename', '`basename %s .1.c`'),
    # ('%link-rest', '%link-basename.*.c'),
    # ('%link-compile', '%kcc %link-rest -o %t'),

    # In some situations, the semantics will fail to fully evaluate a program
    # (e.g. when UB occurs and there is no specified way to resume execution
    # sensibly). When this happens, we still want to pipe the output of the
    # compiled program to FileCheck despite the executable producing a non-zero
    # return code; the option is necessary because lit sets pipefail on by
    # default.
    #
    # Setting up this as an extra run-line rather than a config option means
    # that it can be enabled per-test, rather than on the test suite as a whole
    # (so that if a program we expect to fully evaluate, doesn't, then we'll be
    # warned by a test failure).
    ('%allow-pipefail', 'set +o pipefail'),
])

config.recursiveExpansionLimit = 10
