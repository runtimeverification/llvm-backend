import glob
import os
import platform
import subprocess

from lit import formats

def llvm_major_version():
    result = subprocess.run(['llvm-backend-version', '--llvm'], capture_output=True)
    if result.returncode != 0:
        raise RuntimeError('Failed to get LLVM package version')

    major, _, _ = result.stdout.decode('utf-8').strip().split('.')
    return int(major)

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

if llvm_major_version() >= 16:
    config.available_features.add('opaque-pointers')

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
    return 'set -e; ' + s.strip() \
        .replace('&\n', '& ') \
        .replace('\n', ' ; ') \
        .replace('do ;', 'do') \
        .replace('then ;', 'then') \
        .replace("' ; '", r"'\\n'") \

# We want to be able to disable some checks on macOS, this function allows us to
# do that by checking the OS and replacing the input with just `exit 0` in case
# of macOS.
def exclude_macos(s):
  return 'exit 0' if platform.system() == 'Darwin' else s


config.substitutions.extend([
    ('%kompile', 'llvm-kompile-testing'),
    ('%interpreter', one_line('''
        output=$(%kompile %s main -o %t.interpreter 2>&1)
        if [[ -n "$output" ]]; then
            echo "llvm-kompile error or warning: $output"
            exit 1
        fi
    ''')),
    ('%proof-interpreter', one_line('''
        output=$(%kompile %s main --proof-hint-instrumentation -o %t.interpreter 2>&1)
        if [[ -n "$output" ]]; then
            echo "llvm-kompile error or warning: $output"
            exit 1
        fi
    ''')),
    ('%proof-slow-interpreter', one_line('''
        output=$(%kompile %s main --proof-hint-instrumentation-slow -o %t.interpreter 2>&1)
        if [[ -n "$output" ]]; then
            echo "llvm-kompile error or warning: $output"
            exit 1
        fi
    ''')),
    ('%search-interpreter', '%kompile %s search -o %t.interpreter'),
    ('%convert-input', '%kore-convert %test-input -o %t.bin'),
    ('%strip-binary', 'kore-strip'),
    ('%arity', 'kore-arity'),
    ('%apply-rule', 'k-rule-apply'),
    ('%find-rule', 'k-rule-find'),
    ('%compute-loc', 'llvm-kompile-compute-loc %S'),
    ('%compute-ordinal', 'llvm-kompile-compute-ordinal %S'),

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
        %run-binary-out | diff - <(echo -n)
        %kore-convert %t.out.bin -o %t.out.kore
        %kore-convert %test-diff-out --to=text | diff - %t.out.kore
    ''')),

    ('%check-dir-grep', one_line('''
        for out in %test-dir-out/*.out.grep; do
            in=%test-dir-in/`basename $out .out.grep`.in
            %t.interpreter $in -1 /dev/stdout | grep -f $out -q
            result="$?"
            if [ "$result" -ne 0 ]; then
                echo "$in"
                exit 1
            fi
        done
    ''')),

    ('%check-dir-diff', one_line('''
        for out in %test-dir-out/*.out.diff; do
            in=%test-dir-in/`basename $out .out.diff`.in
            %t.interpreter $in -1 /dev/stdout | diff - $out
            result="$?"
            if [ "$result" -ne 0 ]; then
                echo "$in"
                exit 1
            fi
        done
    ''')),

    ('%check-proof-out', one_line('''
        %run-proof-out
        %kore-rich-header %s > %t.header.bin
        %kore-proof-trace --verbose --expand-terms %t.header.bin %t.out.bin | diff - %test-proof-diff-out
        result="$?"
        if [ "$result" -ne 0 ]; then
            echo "kore-proof-trace error while parsing proof hint trace with expanded kore terms"
            exit 1
        fi
        %kore-proof-trace --streaming-parser --verbose --expand-terms %t.header.bin %t.out.bin | diff - %test-proof-diff-out -q
        result="$?"
        if [ "$result" -ne 0 ]; then
            echo "kore-proof-trace error while parsing proof hint trace with expanded kore terms and streaming parser"
            exit 1
        fi
    ''')),

    ('%check-proof-shm-out', exclude_macos(one_line('''
        %kore-rich-header %s > %t.header.bin
        %kore-proof-trace --shared-memory --verbose --expand-terms %t.header.bin %test-shm-buffer | diff - %test-proof-diff-out &
        reader_pid="$!"
        sleep 1
        %run-proof-shm-out
        wait $reader_pid
        result="$?"
        if [ "$result" -ne 0 ]; then
            echo "kore-proof-trace error while parsing proof hint trace with expanded kore terms and shmem parser"
            exit 1
        fi
    '''))),

    ('%check-proof-debug-out', one_line('''
        out=%test-dir-out/*.proof.debug.out.diff
        in=%test-dir-in/`basename $out .proof.debug.out.diff`.in
        hint=%t.`basename $out .proof.debug.out.diff`.hint
        rm -f $hint
        %t.interpreter $in -1 $hint --proof-output
        %kore-rich-header %s > %t.header.bin
        %kore-proof-trace --verbose --expand-terms %t.header.bin $hint %s | diff - $out
        result="$?"
        if [ "$result" -ne 0 ]; then
            echo "kore-proof-trace error while parsing proof hint trace with expanded kore terms"
            exit 1
        fi
     ''')),

    ('%check-dir-proof-out', one_line('''
        %kore-rich-header %s > %t.header.bin
        for out in %test-dir-out/*.proof.out.diff; do
            in=%test-dir-in/`basename $out .proof.out.diff`.in
            hint=%t.`basename $out .proof.out.diff`.hint
            rm -f $hint
            %t.interpreter $in -1 $hint --proof-output
            %kore-proof-trace --verbose --expand-terms %t.header.bin $hint | diff - $out
            result="$?"
            if [ "$result" -ne 0 ]; then
                echo "kore-proof-trace error while parsing proof hint trace with expanded kore terms"
                exit 1
            fi
            %kore-proof-trace --streaming-parser --verbose --expand-terms %t.header.bin $hint | diff - $out
            result="$?"
            if [ "$result" -ne 0 ]; then
                echo "kore-proof-trace error while parsing proof hint trace with expanded kore terms and streaming parser"
                exit 1
            fi
        done
    ''')),

    ('%check-dir-proof-shm-out', exclude_macos(one_line('''
        %kore-rich-header %s > %t.header.bin
        count=0
        for out in %test-dir-out/*.proof.out.diff; do
            in=%test-dir-in/`basename $out .proof.out.diff`.in
            shmbuf=%test-shm-buffer.$count
            %kore-proof-trace --shared-memory --verbose --expand-terms %t.header.bin $shmbuf | diff - $out &
            reader_pid="$!"
            sleep 1
            %t.interpreter $in -1 $shmbuf --proof-output --use-shared-memory
            wait $reader_pid
            result="$?"
            if [ "$result" -ne 0 ]; then
                echo "kore-proof-trace error while parsing proof hint trace with expanded kore terms and shmem parser"
                exit 1
            fi
            count=$(expr $count + 1)
        done
    '''))),

    ('%run-binary-out', 'rm -f %t.out.bin && %t.interpreter %test-input -1 %t.out.bin --binary-output'),
    ('%run-binary', 'rm -f %t.bin && %convert-input && %t.interpreter %t.bin -1 /dev/stdout'),
    ('%run-proof-out', 'rm -f %t.out.bin && %t.interpreter %test-input -1 %t.out.bin --proof-output'),
    ('%run-proof-shm-out', '%t.interpreter %test-input -1 %test-shm-buffer --proof-output --use-shared-memory'),
    ('%run', '%t.interpreter %test-input -1 /dev/stdout'),

    ('%kprint-check', 'kprint %S %s true | diff - %s.out'),
    ('%kprint-nofilter-check', 'kprint %S %s true false | diff - %s.out'),

    ('%input-dir', os.path.join(ROOT_PATH, 'test', 'input')),
    ('%output-dir', os.path.join(ROOT_PATH, 'test', 'output')),

    ('%test-input', os.path.join('%input-dir', '%test-basename.in')),
    ('%test-grep-out', os.path.join('%output-dir', '%test-basename.out.grep')),
    ('%test-diff-out', os.path.join('%output-dir', '%test-basename.out.diff')),
    ('%test-dir-out', os.path.join('%output-dir', '%test-basename')),
    ('%test-dir-in', os.path.join('%input-dir', '%test-basename')),
    ('%test-proof-diff-out', os.path.join('%output-dir', '%test-basename.proof.out.diff')),
    ('%test-proof-debug-diff-out', os.path.join('%output-dir', '%test-basename.proof.debug.out.diff')),
    ('%test-shm-buffer', os.path.join('/', '%test-basename.b')),
    ('%test-basename', '`basename %s .kore`'),

    ('%allow-pipefail', 'set +o pipefail'),

    ('%kore-convert', 'kore-convert'),

    ('%kore-proof-trace', 'kore-proof-trace'),
    ('%kore-proof-trace-shm-writer', 'kore-proof-trace-shm-writer'),
    ('%kore-rich-header', 'kore-rich-header'),
])

config.recursiveExpansionLimit = 10
