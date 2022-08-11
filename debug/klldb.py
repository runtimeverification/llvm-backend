#!/usr/bin/env python3

import sys
sys.path.append("@LLDB_PYTHONPATH@")  # noqa

import argparse
import lldb
import os
import pathlib


def dispatch_k(debugger, command, result, internal_dict):
    print('K-specific support for LLDB is not yet implemented')


def parse_args():
    parser = argparse.ArgumentParser(
        description="Run a K definition under LLDB")
    parser.add_argument(
        'interpreter', help='Path to the compiled K interpreter to run')
    parser.add_argument('input', help='Path to input KORE term')
    parser.add_argument('depth', help='Number of rewriting steps to take')
    parser.add_argument('output', help='Path to write output KORE term to')
    return parser.parse_args()


if __name__ == "__main__":
    args = parse_args()

    debugger = lldb.SBDebugger.Create()
    debugger.SetAsync(False)

    debugger.HandleCommand(
        f"command script import {pathlib.Path(__file__).resolve()}")

    target = debugger.CreateTargetWithFileAndArch(
        args.interpreter, lldb.LLDB_ARCH_DEFAULT)

    main_bp = target.BreakpointCreateByName(
        "main", target.GetExecutable().GetFilename())

    process = target.LaunchSimple([args.input, args.depth, args.output], None, os.getcwd())

    n_errors, quit_requested, has_crashed = debugger.RunCommandInterpreter(
        True, False, lldb.SBCommandInterpreterRunOptions(), 0, False, False)


def __lldb_init_module(debugger, internal_dict):
    debugger.HandleCommand("command script add -f klldb.dispatch_k k")
