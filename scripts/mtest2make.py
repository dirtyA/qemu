#! /usr/bin/env python3

# Create Makefile targets to run tests, from Meson's test introspection data.
#
# Author: Paolo Bonzini <pbonzini@redhat.com>

from collections import defaultdict
import itertools
import json
import os
import shlex
import sys

class Suite(object):
    def __init__(self):
        self.deps = set()

print('''
SPEED = quick

.mtestargs = --no-rebuild -t 0
.mtestargs += $(subst -j,--num-processes , $(filter-out -j, $(lastword -j1 $(filter -j%, $(MAKEFLAGS)))))
ifneq ($(SPEED), quick)
.mtestargs += --setup $(SPEED)
endif

.check.mtestargs = $(MTESTARGS) $(.mtestargs) $(if $(V),--verbose,--print-errorlogs)
.bench.mtestargs = $(MTESTARGS) $(.mtestargs) --benchmark --verbose
''')

introspect = json.load(sys.stdin)

def process_tests(test, targets, suites):
    executable = test['cmd'][0]
    try:
        executable = os.path.relpath(executable)
    except:
        pass

    deps = (targets.get(x, []) for x in test['depends'])
    deps = itertools.chain.from_iterable(deps)
    deps = list(deps)

    test_suites = test['suite'] or ['default']
    for s in test_suites:
        # The suite name in the introspection info is "PROJECT:SUITE"
        s = s.split(':')[1]
        if s == 'slow':
            continue
        suites[s].deps.update(deps)

def emit_prolog(suites, prefix):
    all_targets = ' '.join((f'{prefix}-{k}' for k in suites.keys()))
    all_xml = ' '.join((f'{prefix}-report-{k}.junit.xml' for k in suites.keys()))
    print(f'all-{prefix}-targets = {all_targets}')
    print(f'all-{prefix}-xml = {all_xml}')
    print(f'.PHONY: {prefix} do-meson-{prefix} {prefix}-report.junit.xml $(all-{prefix}-targets) $(all-{prefix}-xml)')
    print(f'ifeq ($(filter {prefix}, $(MAKECMDGOALS)),)')
    print(f'.{prefix}.mtestargs += $(foreach s,$(sort $(.{prefix}.mtest-suites)), --suite $s)')
    print(f'endif')
    print(f'{prefix}-build: run-ninja')
    print(f'{prefix} $(all-{prefix}-targets): do-meson-{prefix}')
    print(f'do-meson-{prefix}: run-ninja; $(if $(MAKE.n),,+)$(MESON) test $(.{prefix}.mtestargs)')
    print(f'{prefix}-report.junit.xml $(all-{prefix}-xml): {prefix}-report%.junit.xml: run-ninja')
    print(f'\t$(MAKE) {prefix}$* MTESTARGS="$(MTESTARGS) --logbase {prefix}-report$*" && ln -f meson-logs/$@ .')

def emit_suite_deps(name, suite, prefix):
    deps = ' '.join(suite.deps)
    print(f'.{prefix}-{name}.deps = {deps}')
    print(f'ifneq ($(filter {prefix}-build, $(MAKECMDGOALS)),)')
    print(f'.{prefix}.build-suites += {name}')
    print(f'endif')

def emit_suite(name, suite, prefix):
    emit_suite_deps(name, suite, prefix)
    targets = f'{prefix}-{name} {prefix}-report-{name}.junit.xml {prefix} {prefix}-report.junit.xml'
    print(f'ifneq ($(filter {targets}, $(MAKECMDGOALS)),)')
    print(f'.{prefix}.mtest-suites += {name}')
    print(f'endif')

targets = {t['id']: [os.path.relpath(f) for f in t['filename']]
           for t in introspect['targets']}

testsuites = defaultdict(Suite)
for test in introspect['tests']:
    process_tests(test, targets, testsuites)
# HACK: check-block is a separate target so that it runs with --verbose;
# only write the dependencies
emit_suite_deps('block', testsuites['block'], 'check')
del testsuites['block']
emit_prolog(testsuites, 'check')
for name, suite in testsuites.items():
    emit_suite(name, suite, 'check')

benchsuites = defaultdict(Suite)
for test in introspect['benchmarks']:
    process_tests(test, targets, benchsuites)
emit_prolog(benchsuites, 'bench')
for name, suite in benchsuites.items():
    emit_suite(name, suite, 'bench')
