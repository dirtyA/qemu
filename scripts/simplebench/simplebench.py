#!/usr/bin/env python
#
# Simple benchmarking framework
#
# Copyright (c) 2019 Virtuozzo International GmbH.
#
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 2 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <http://www.gnu.org/licenses/>.
#


def bench_one(test_func, test_env, test_case, count=5, initial_run=True):
    """Benchmark one test-case

    test_func   -- benchmarking function with prototype
                   test_func(env, case), which takes test_env and test_case
                   arguments and on success returns dict with 'seconds' or
                   'iops' (or both) fields, specifying the benchmark result.
                   If both 'iops' and 'seconds' provided, the 'iops' is
                   considered the main, and 'seconds' is just an additional
                   info. On failure test_func should return {'error': str}.
                   Returned dict may contain any other additional fields.
    test_env    -- test environment - opaque first argument for test_func
    test_case   -- test case - opaque second argument for test_func
    count       -- how many times to call test_func, to calculate average
    initial_run -- do initial run of test_func, which don't get into result

    Returns dict with the following fields:
        'runs':     list of test_func results
        'dimension': dimension of results, may be 'seconds' or 'iops'
        'average':  average seconds per run (exists only if at least one run
                    succeeded)
        'delta':    maximum delta between test_func result and the average
                    (exists only if at least one run succeeded)
        'n-failed': number of failed runs (exists only if at least one run
                    failed)
    """
    if initial_run:
        print('  #initial run:')
        print('   ', test_func(test_env, test_case))

    runs = []
    for i in range(count):
        print('  #run {}'.format(i+1))
        res = test_func(test_env, test_case)
        print('   ', res)
        runs.append(res)

    result = {'runs': runs}

    successed = [r for r in runs if ('seconds' in r or 'iops' in r)]
    if successed:
        dim = 'iops' if ('iops' in successed[0]) else 'seconds'
        if 'iops' in successed[0]:
            assert all('iops' in r for r in successed)
            dim = 'iops'
        else:
            assert all('seconds' in r for r in successed)
            assert all('iops' not in r for r in successed)
            dim = 'seconds'
        avg = sum(r[dim] for r in successed) / len(successed)
        result['dimension'] = dim
        result['average'] = avg
        result['delta'] = max(abs(r[dim] - avg) for r in successed)

    if len(successed) < count:
        result['n-failed'] = count - len(successed)

    return result


def format_float(x):
    res = round(x)
    if res >= 100:
        return str(res)

    res = f'{x:.1f}'
    if len(res) >= 4:
        return res

    return f'{x:.2f}'


def format_percent(x):
    x *= 100

    res = round(x)
    if res >= 10:
        return str(res)

    return f'{x:.1f}' if res >= 1 else f'{x:.2f}'


def ascii_one(result):
    """Return ASCII representation of bench_one() returned dict."""
    if 'average' in result:
        avg = result['average']
        delta_pr = result['delta'] / avg
        s = f'{format_float(avg)}±{format_percent(delta_pr)}%'
        if 'n-failed' in result:
            s += '\n({} failed)'.format(result['n-failed'])
        return s
    else:
        return 'FAILED'


def bench(test_func, test_envs, test_cases, *args, **vargs):
    """Fill benchmark table

    test_func -- benchmarking function, see bench_one for description
    test_envs -- list of test environments, see bench_one
    test_cases -- list of test cases, see bench_one
    args, vargs -- additional arguments for bench_one

    Returns dict with the following fields:
        'envs':  test_envs
        'cases': test_cases
        'tab':   filled 2D array, where cell [i][j] is bench_one result for
                 test_cases[i] for test_envs[j] (i.e., rows are test cases and
                 columns are test environments)
    """
    tab = {}
    results = {
        'envs': test_envs,
        'cases': test_cases,
        'tab': tab
    }
    n = 1
    n_tests = len(test_envs) * len(test_cases)
    for env in test_envs:
        for case in test_cases:
            print('Testing {}/{}: {} :: {}'.format(n, n_tests,
                                                   env['id'], case['id']))
            if case['id'] not in tab:
                tab[case['id']] = {}
            tab[case['id']][env['id']] = bench_one(test_func, env, case,
                                                   *args, **vargs)
            n += 1

    print('Done')
    return results


def ascii(results):
    """Return ASCII representation of bench() returned dict."""
    import tabulate

    # We want leading whitespace for difference row cells (see below)
    tabulate.PRESERVE_WHITESPACE = True

    dim = None
    tab = [
        # Environment columns are named A, B, ...
        [""] + [chr(ord('A') + i) for i in range(len(results['envs']))],
        [""] + [c['id'] for c in results['envs']]
    ]
    for case in results['cases']:
        row = [case['id']]
        case_results = results['tab'][case['id']]
        for env in results['envs']:
            res = case_results[env['id']]
            if dim is None:
                dim = res['dimension']
            else:
                assert dim == res['dimension']
            row.append(ascii_one(res))
        tab.append(row)

        # Add row of difference between column. For each column starting from
        # B we calculate difference with all previous columns.
        row = ['', '']  # case name and first column
        for i in range(1, len(results['envs'])):
            cell = ''
            env = results['envs'][i]
            res = case_results[env['id']]

            if 'average' not in res:
                # Failed result
                row.append(cell)
                continue

            for j in range(0, i):
                env_j = results['envs'][j]
                res_j = case_results[env_j['id']]

                if 'average' not in res_j:
                    # Failed result
                    cell += ' --'
                    continue

                col_j = chr(ord('A') + j)
                avg_j = res_j['average']
                delta = (res['average'] - avg_j) / avg_j * 100
                delta_delta = (res['delta'] + res_j['delta']) / avg_j * 100
                cell += f' {col_j}{round(delta):+}±{round(delta_delta)}%'
            row.append(cell)
        tab.append(row)

    return f'All results are in {dim}\n\n' + tabulate.tabulate(tab)
