#!/usr/bin/env python3
"""
Plot Speedup and Efficiency for Exercise 5 - Pi Calculation
"""

import matplotlib
matplotlib.use('Agg')
import matplotlib.pyplot as plt

# Benchmark data: {N: {np: time_in_seconds}}
data = {
    10_000_000:  {1: 0.034241, 2: 0.025516, 4: 0.021533},
    100_000_000: {1: 0.307316, 2: 0.147369, 4: 0.136153},
}

proc_counts = [1, 2, 4]
colors = ['tab:blue', 'tab:orange']
markers = ['o', 's']
sizes = [10_000_000, 100_000_000]
labels = ['N=10^7', 'N=10^8']

# ---- Speedup ----
fig, ax = plt.subplots(figsize=(7, 5))
for i, N in enumerate(sizes):
    t1 = data[N][1]
    speedups = [t1 / data[N][np] for np in proc_counts]
    ax.plot(proc_counts, speedups, marker=markers[i], color=colors[i], label=labels[i])

ax.plot(proc_counts, proc_counts, 'k--', label='Ideal Speedup')
ax.set_xlabel('Number of Processes')
ax.set_ylabel('Speedup')
ax.set_title('Speedup - Pi Calculation')
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
    ax.plot(proc_counts, efficiencies, marker=markers[i], color=colors[i], label=labels[i])

ax.axhline(1.0, color='k', linestyle='--', label='Ideal Efficiency')
ax.set_xlabel('Number of Processes')
ax.set_ylabel('Efficiency')
ax.set_title('Efficiency - Pi Calculation')
ax.set_xticks(proc_counts)
ax.set_ylim(0, 1.2)
ax.legend()
ax.grid(True, alpha=0.3)
plt.tight_layout()
plt.savefig('efficiency.png', dpi=150)
plt.close()
print("Saved efficiency.png")
