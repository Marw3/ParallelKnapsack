#! /usr/bin/env python3

import os
import sys
import numpy as np
import pandas as pd
import matplotlib.pyplot as plt
import seaborn as sns

sns.set()

if len(sys.argv) == 1:
    if os.path.isfile('benchmark.csv'):
        FILE = 'benchmark.csv'
    else:
        print("Usage: python3 plot.py FILE")
        sys.exit(0)
else:
    FILE = sys.argv[1]
EXT = 'png'

data = pd.read_csv(FILE, skiprows=1)
data.sort_values(by=['n', 'inst', 'p'], inplace=True)


def rel_speedup(group):
    base = float(group[group['p'] == 1]['t'])
    group['rel_speedup'] = (base / group['t'])
    return group

data['tpn'] = data['t'] / data['n']
data = data.groupby(['inst']).apply(rel_speedup)

sns.relplot(x='n', y='t', hue='p', marker='p', style='p',
            kind='line', palette='tab10', data=data)
plt.savefig('runtime.%s' % EXT)

sns.relplot(x='n', y='tpn', hue='p', marker='p', style='p',
            kind='line', palette='tab10', data=data)
plt.savefig('runtime_per_item.%s' % EXT)

sns.relplot(x='n', y='rel_speedup', hue='p', marker='p', style='p',
            kind='line', palette='tab10', data=data)
plt.savefig('speedup_n.%s' % EXT)

sns.catplot(x='p', y='rel_speedup',
            kind='violin', data=data)
plt.savefig('speedup_p.%s' % EXT)

isRunningInPyCharm = "PYCHARM_HOSTED" in os.environ
if isRunningInPyCharm:
    plt.show()
