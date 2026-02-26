#!/usr/bin/env python3
"""
Plot Speedup and Efficiency for Exercise 4 - Matrix-Vector Product
"""

import matplotlib
matplotlib.use('Agg')
import matplotlib.pyplot as plt
import numpy as np

# Benchmark data: {N: {np: time_in_seconds}}
data = {
    500:  {1: 0.004050, 2: 0.002698, 4: 0.001836},
    1000: {1: 0.008398, 2: 0.007074, 4: 0.011607},
    2000: {1: 0.035917, 2: 0.022597, 4: 0.023321},
}

proc_counts = [1, 2, 4]
colors = ['tab:blue', 'tab:orange', 'tab:green']
markers = ['o', 's', '^']
sizes = [500, 1000, 2000]

# ---- Speedup ----
fig, ax = plt.subplots(figsize=(7, 5))
for i, N in enumerate(sizes):
    t1 = data[N][1]
    speedups = [t1 / data[N][np] for np in proc_counts]
    ax.plot(proc_counts, speedups, marker=markers[i], color=colors[i], label=f'N={N}')

ax.plot(proc_counts, proc_counts, 'k--', label='Ideal Speedup')
ax.set_xlabel('Number of Processes')
ax.set_ylabel('Speedup')
ax.set_title('Speedup - Matrix-Vector Product')
ax.set_xticks(proc_counts)
ax.legend()
ax.grid(True, alpha=0.3)
plt.tight_layout()
plt.savefig('speedup.png', dpi=150)
plt.close()
print("Saved speedup.png")

# ---- Efficiency ----
fig, ax = plt.subplots(figsize=(7, 5))
for i, N in enumerate(sizes):
    t1 = data[N][1]
    efficiencies = [(t1 / data[N][np]) / np for np in proc_counts]
    ax.plot(proc_counts, efficiencies, marker=markers[i], color=colors[i], label=f'N={N}')

ax.axhline(1.0, color='k', linestyle='--', label='Ideal Efficiency')
ax.set_xlabel('Number of Processes')
ax.set_ylabel('Efficiency')
ax.set_title('Efficiency - Matrix-Vector Product')
ax.set_xticks(proc_counts)
ax.set_ylim(0, 1.2)
ax.legend()
ax.grid(True, alpha=0.3)
plt.tight_layout()
plt.savefig('efficiency.png', dpi=150)
plt.close()
print("Saved efficiency.png")
