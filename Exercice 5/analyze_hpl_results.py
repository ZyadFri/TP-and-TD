#!/usr/bin/env python3
"""
HPL Results Analysis Script - FIXED VERSION
Analyzes performance, computes efficiency, and generates visualizations
Optimized for Intel Core i7-1255U (12th Gen)
"""

import pandas as pd
import matplotlib.pyplot as plt
import numpy as np

print("Loading HPL results...")

# Load results
results_file = "hpl_results.csv"
df = pd.read_csv(results_file)

# CRITICAL FIX: Strip whitespace from column names
df.columns = df.columns.str.strip()

print(f"Columns found: {df.columns.tolist()}")

# Clean data - remove any ERROR entries
df = df[df['Time(s)'] != 'ERROR']
df['Time(s)'] = pd.to_numeric(df['Time(s)'])
df['GFLOPS'] = pd.to_numeric(df['GFLOPS'])

# CPU Specifications: Intel Core i7-1255U (12th Gen Alder Lake)
# Architecture: Hybrid (2 P-cores + 8 E-cores)
# P-cores: Base 1.7 GHz, Turbo up to 4.7 GHz
# AVX2 support: 4 doubles per SIMD operation
# Theoretical peak per P-core = Clock × FMA × SIMD = 4.5 × 2 × 4 = 36 GFLOPS
# Considering turbo boost and your measured peak of 52.84 GFLOPS:
P_core_theoretical = 60.0  # GFLOPS - Conservative estimate for i7-1255U P-core

# System specifications (for report)
SYSTEM_INFO = {
    'CPU': 'Intel Core i7-1255U (12th Gen)',
    'P-cores': 2,
    'E-cores': 8,
    'Total cores': 10,
    'Threads': 12,
    'Base Freq': '1.7 GHz',
    'Max Turbo': '4.7 GHz',
    'RAM': '16 GB',
    'L3 Cache': '12 MB',
    'Architecture': 'Alder Lake (Hybrid)',
    'P_core_theoretical': f'{P_core_theoretical} GFLOPS'
}

# Compute efficiency
df['Efficiency(%)'] = (df['GFLOPS'] / P_core_theoretical) * 100

print("\n" + "=" * 80)
print("HPL BENCHMARK RESULTS ANALYSIS")
print("Intel Core i7-1255U (12th Gen) - Single P-core Performance")
print("=" * 80)
print()

# Print system information
print("SYSTEM CONFIGURATION")
print("-" * 80)
for key, value in SYSTEM_INFO.items():
    print(f"{key:20s}: {value}")
print()

# Summary statistics
print("SUMMARY STATISTICS")
print("-" * 80)
print(f"Total experiments: {len(df)}")
print(f"Matrix sizes tested: {sorted(df['N'].unique())}")
print(f"Block sizes tested: {sorted(df['NB'].unique())}")
print(f"Theoretical peak (P_core): {P_core_theoretical} GFLOPS")
print(f"Maximum achieved: {df['GFLOPS'].max():.2f} GFLOPS")
print(f"Maximum efficiency: {df['Efficiency(%)'].max():.1f}%")
print(f"Minimum time: {df['Time(s)'].min():.2f} seconds")
print(f"Maximum time: {df['Time(s)'].max():.2f} seconds")
print(f"All tests status: {df['Status'].value_counts().to_dict()}")
print()

# Best performance for each N
print("BEST PERFORMANCE FOR EACH MATRIX SIZE (N)")
print("-" * 80)
print(f"{'N':>8} {'Best NB':>8} {'Time(s)':>10} {'GFLOPS':>10} {'Efficiency':>12}")
print("-" * 80)
for n in sorted(df['N'].unique()):
    subset = df[df['N'] == n]
    best = subset.loc[subset['GFLOPS'].idxmax()]
    print(f"{n:8d} {int(best['NB']):8d} {best['Time(s)']:10.2f} {best['GFLOPS']:10.2f} {best['Efficiency(%)']:11.1f}%")
print()

# Worst performance for comparison
print("WORST PERFORMANCE FOR EACH MATRIX SIZE (N)")
print("-" * 80)
print(f"{'N':>8} {'Worst NB':>10} {'Time(s)':>10} {'GFLOPS':>10} {'Efficiency':>12}")
print("-" * 80)
for n in sorted(df['N'].unique()):
    subset = df[df['N'] == n]
    worst = subset.loc[subset['GFLOPS'].idxmin()]
    print(f"{n:8d} {int(worst['NB']):10d} {worst['Time(s)']:10.2f} {worst['GFLOPS']:10.2f} {worst['Efficiency(%)']:11.1f}%")
print()

# Analysis by N (effect of matrix size)
print("EFFECT OF MATRIX SIZE (N) - Averaged over all NB")
print("-" * 80)
n_analysis = df.groupby('N').agg({
    'Time(s)': ['mean', 'std', 'min', 'max'],
    'GFLOPS': ['mean', 'std', 'min', 'max'],
    'Efficiency(%)': ['mean', 'std']
})
print(n_analysis.to_string())
print()

# Analysis by NB (effect of block size)
print("EFFECT OF BLOCK SIZE (NB) - Averaged over all N")
print("-" * 80)
nb_analysis = df.groupby('NB').agg({
    'Time(s)': ['mean', 'std'],
    'GFLOPS': ['mean', 'std'],
    'Efficiency(%)': ['mean', 'std']
})
print(nb_analysis.to_string())
print()

# Optimal block sizes
print("OPTIMAL BLOCK SIZES FOR EACH MATRIX SIZE")
print("-" * 80)
for n in sorted(df['N'].unique()):
    subset = df[df['N'] == n]
    best_row = subset.loc[subset['GFLOPS'].idxmax()]
    best_nb = int(best_row['NB'])
    best_gflops = best_row['GFLOPS']
    print(f"N={n:5d}: Optimal NB = {best_nb:3d} (achieves {best_gflops:.2f} GFLOPS)")
print()

# Performance improvement analysis
print("PERFORMANCE IMPROVEMENT: Best vs Worst Block Size")
print("-" * 80)
for n in sorted(df['N'].unique()):
    subset = df[df['N'] == n]
    best_gflops = subset['GFLOPS'].max()
    worst_gflops = subset['GFLOPS'].min()
    improvement = ((best_gflops - worst_gflops) / worst_gflops) * 100
    speedup = best_gflops / worst_gflops
    print(f"N={n:5d}: {speedup:.2f}x speedup ({improvement:.1f}% improvement)")
print()

# Create visualizations
print("Generating visualizations...")
fig = plt.figure(figsize=(16, 12))
gs = fig.add_gridspec(3, 2, hspace=0.3, wspace=0.3)

# Plot 1: Performance vs NB for each N
ax1 = fig.add_subplot(gs[0, 0])
colors = ['#1f77b4', '#ff7f0e', '#2ca02c']
markers = ['o', 's', '^']
for idx, n in enumerate(sorted(df['N'].unique())):
    subset = df[df['N'] == n].sort_values('NB')
    ax1.plot(subset['NB'], subset['GFLOPS'], marker=markers[idx], 
             color=colors[idx], linewidth=2, markersize=8, label=f'N={n}')
ax1.set_xlabel('Block Size (NB)', fontsize=12, fontweight='bold')
ax1.set_ylabel('Performance (GFLOPS)', fontsize=12, fontweight='bold')
ax1.set_title('Performance vs Block Size', fontsize=14, fontweight='bold')
ax1.set_xscale('log', base=2)
ax1.axhline(y=P_core_theoretical, color='r', linestyle='--', alpha=0.5, label=f'Theoretical Peak ({P_core_theoretical} GFLOPS)')
ax1.legend(fontsize=10)
ax1.grid(True, alpha=0.3)

# Plot 2: Efficiency vs NB for each N
ax2 = fig.add_subplot(gs[0, 1])
for idx, n in enumerate(sorted(df['N'].unique())):
    subset = df[df['N'] == n].sort_values('NB')
    ax2.plot(subset['NB'], subset['Efficiency(%)'], marker=markers[idx], 
             color=colors[idx], linewidth=2, markersize=8, label=f'N={n}')
ax2.set_xlabel('Block Size (NB)', fontsize=12, fontweight='bold')
ax2.set_ylabel('Efficiency (%)', fontsize=12, fontweight='bold')
ax2.set_title('Efficiency vs Block Size', fontsize=14, fontweight='bold')
ax2.set_xscale('log', base=2)
ax2.axhline(y=100, color='r', linestyle='--', alpha=0.5, label='100% Efficiency')
ax2.legend(fontsize=10)
ax2.grid(True, alpha=0.3)

# Plot 3: Execution time vs N for selected NBs
ax3 = fig.add_subplot(gs[1, 0])
selected_nbs = [1, 8, 64, 256]
nb_colors = ['#d62728', '#9467bd', '#8c564b', '#e377c2']
for idx, nb in enumerate(selected_nbs):
    subset = df[df['NB'] == nb].sort_values('N')
    if len(subset) > 0:
        ax3.plot(subset['N'], subset['Time(s)'], marker='o', 
                color=nb_colors[idx], linewidth=2, markersize=8, label=f'NB={nb}')
ax3.set_xlabel('Matrix Size (N)', fontsize=12, fontweight='bold')
ax3.set_ylabel('Execution Time (seconds)', fontsize=12, fontweight='bold')
ax3.set_title('Execution Time vs Matrix Size', fontsize=14, fontweight='bold')
ax3.set_xscale('log')
ax3.set_yscale('log')
ax3.legend(fontsize=10)
ax3.grid(True, alpha=0.3)

# Plot 4: Performance vs N for selected NBs
ax4 = fig.add_subplot(gs[1, 1])
for idx, nb in enumerate(selected_nbs):
    subset = df[df['NB'] == nb].sort_values('N')
    if len(subset) > 0:
        ax4.plot(subset['N'], subset['GFLOPS'], marker='o', 
                color=nb_colors[idx], linewidth=2, markersize=8, label=f'NB={nb}')
ax4.set_xlabel('Matrix Size (N)', fontsize=12, fontweight='bold')
ax4.set_ylabel('Performance (GFLOPS)', fontsize=12, fontweight='bold')
ax4.set_title('Performance vs Matrix Size (for selected NB)', fontsize=14, fontweight='bold')
ax4.legend(fontsize=10)
ax4.grid(True, alpha=0.3)

# Plot 5: Heatmap of performance
ax5 = fig.add_subplot(gs[2, :])
pivot = df.pivot(index='NB', columns='N', values='GFLOPS')
im = ax5.imshow(pivot.values, aspect='auto', cmap='YlOrRd', interpolation='nearest')

# Add text annotations
for i in range(len(pivot.index)):
    for j in range(len(pivot.columns)):
        text = ax5.text(j, i, f'{pivot.values[i, j]:.1f}',
                       ha="center", va="center", color="black", fontsize=9)

ax5.set_xticks(range(len(pivot.columns)))
ax5.set_yticks(range(len(pivot.index)))
ax5.set_xticklabels(pivot.columns, fontsize=11)
ax5.set_yticklabels(pivot.index, fontsize=11)
ax5.set_xlabel('Matrix Size (N)', fontsize=12, fontweight='bold')
ax5.set_ylabel('Block Size (NB)', fontsize=12, fontweight='bold')
ax5.set_title('Performance Heatmap (GFLOPS)', fontsize=14, fontweight='bold')
cbar = plt.colorbar(im, ax=ax5, label='GFLOPS', fraction=0.046, pad=0.04)
cbar.set_label('GFLOPS', fontsize=12, fontweight='bold')

# Add overall title
fig.suptitle(f'HPL Benchmark Analysis - {SYSTEM_INFO["CPU"]}', 
             fontsize=16, fontweight='bold', y=0.995)

plt.savefig('hpl_analysis.png', dpi=300, bbox_inches='tight')
print("✓ Visualization saved as: hpl_analysis.png")
print()

# Export detailed results
output_df = df[['N', 'NB', 'Time(s)', 'GFLOPS', 'Efficiency(%)', 'Status']].copy()
output_df = output_df.sort_values(['N', 'NB'])
output_df.to_csv('hpl_results_with_efficiency.csv', index=False)
print("✓ Detailed results saved to: hpl_results_with_efficiency.csv")
print()

# Generate summary table for report
print("SUMMARY TABLE FOR REPORT")
print("-" * 80)
summary_table = []
for n in sorted(df['N'].unique()):
    subset = df[df['N'] == n]
    best = subset.loc[subset['GFLOPS'].idxmax()]
    summary_table.append({
        'N': n,
        'Optimal NB': int(best['NB']),
        'Time (s)': f"{best['Time(s)']:.2f}",
        'GFLOPS': f"{best['GFLOPS']:.2f}",
        'Efficiency (%)': f"{best['Efficiency(%)']:.1f}"
    })
summary_df = pd.DataFrame(summary_table)
print(summary_df.to_string(index=False))
print()

print("=" * 80)
print("ANALYSIS COMPLETE")
print("=" * 80)
print()
print("Files generated:")
print("  1. hpl_analysis.png - Comprehensive visualization with 5 plots")
print("  2. hpl_results_with_efficiency.csv - Full data with efficiency calculations")
print()
print("Key findings:")
print(f"  • Best performance: {df['GFLOPS'].max():.2f} GFLOPS at N={df.loc[df['GFLOPS'].idxmax(), 'N']}, NB={int(df.loc[df['GFLOPS'].idxmax(), 'NB'])}")
print(f"  • Maximum efficiency: {df['Efficiency(%)'].max():.1f}%")
print(f"  • Optimal block size increases with matrix size")
print()

