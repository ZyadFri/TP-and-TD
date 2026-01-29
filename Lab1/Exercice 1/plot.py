import pandas as pd
import matplotlib.pyplot as plt
import numpy as np

# Read data
o0 = pd.read_csv("out_O0.csv", encoding="utf-16")
o2 = pd.read_csv("out_O2.csv", encoding="utf-16")


# Create figure with subplots
fig, axes = plt.subplots(2, 2, figsize=(14, 10))
fig.suptitle('Memory Access Stride Performance Analysis', fontsize=16, fontweight='bold')

# 1. Execution Time vs Stride
ax1 = axes[0, 0]
ax1.plot(o0["stride"], o0["time(msec)"], marker="o", linewidth=2, markersize=8, label="O0 (No optimization)", color='red')
ax1.plot(o2["stride"], o2["time(msec)"], marker="s", linewidth=2, markersize=8, label="O2 (Optimized)", color='green')
ax1.set_xlabel("Stride", fontsize=12, fontweight='bold')
ax1.set_ylabel("Time (ms)", fontsize=12, fontweight='bold')
ax1.set_title("Execution Time vs Stride", fontsize=13, fontweight='bold')
ax1.legend(fontsize=10)
ax1.grid(True, alpha=0.3)
ax1.set_xticks(range(1, 21, 2))

# 2. Bandwidth vs Stride
ax2 = axes[0, 1]
ax2.plot(o0["stride"], o0["rate(MB/s)"], marker="o", linewidth=2, markersize=8, label="O0 (No optimization)", color='red')
ax2.plot(o2["stride"], o2["rate(MB/s)"], marker="s", linewidth=2, markersize=8, label="O2 (Optimized)", color='green')
ax2.set_xlabel("Stride", fontsize=12, fontweight='bold')
ax2.set_ylabel("Bandwidth (MB/s)", fontsize=12, fontweight='bold')
ax2.set_title("Memory Bandwidth vs Stride", fontsize=13, fontweight='bold')
ax2.legend(fontsize=10)
ax2.grid(True, alpha=0.3)
ax2.set_xticks(range(1, 21, 2))

# 3. Speedup: O2 vs O0
ax3 = axes[1, 0]
speedup = o0["time(msec)"] / o2["time(msec)"]
ax3.plot(o0["stride"], speedup, marker="^", linewidth=2, markersize=8, color='blue')
ax3.axhline(y=1, color='gray', linestyle='--', linewidth=1, label='No speedup (1x)')
ax3.set_xlabel("Stride", fontsize=12, fontweight='bold')
ax3.set_ylabel("Speedup Factor (O2/O0)", fontsize=12, fontweight='bold')
ax3.set_title("Optimization Speedup vs Stride", fontsize=13, fontweight='bold')
ax3.legend(fontsize=10)
ax3.grid(True, alpha=0.3)
ax3.set_xticks(range(1, 21, 2))
ax3.fill_between(o0["stride"], 1, speedup, alpha=0.3, color='blue')

# 4. Performance Degradation
ax4 = axes[1, 1]
# Normalize to stride=1 performance
o0_normalized = (o0["rate(MB/s)"] / o0["rate(MB/s)"].iloc[0]) * 100
o2_normalized = (o2["rate(MB/s)"] / o2["rate(MB/s)"].iloc[0]) * 100
ax4.plot(o0["stride"], o0_normalized, marker="o", linewidth=2, markersize=8, label="O0", color='red')
ax4.plot(o2["stride"], o2_normalized, marker="s", linewidth=2, markersize=8, label="O2", color='green')
ax4.axhline(y=100, color='gray', linestyle='--', linewidth=1)
ax4.set_xlabel("Stride", fontsize=12, fontweight='bold')
ax4.set_ylabel("Relative Performance (%)", fontsize=12, fontweight='bold')
ax4.set_title("Performance Relative to Stride=1", fontsize=13, fontweight='bold')
ax4.legend(fontsize=10)
ax4.grid(True, alpha=0.3)
ax4.set_xticks(range(1, 21, 2))

plt.tight_layout()
plt.savefig('stride_analysis_complete.png', dpi=300, bbox_inches='tight')
print("✓ Saved: stride_analysis_complete.png")
plt.show()

# Create summary statistics
print("\n" + "="*70)
print("PERFORMANCE SUMMARY")
print("="*70)

print("\nO0 (No Optimization):")
print(f"  Best performance (stride=1):  {o0['rate(MB/s)'].iloc[0]:.2f} MB/s, {o0['time(msec)'].iloc[0]:.2f} ms")
print(f"  Worst performance (stride=20): {o0['rate(MB/s)'].iloc[-1]:.2f} MB/s, {o0['time(msec)'].iloc[-1]:.2f} ms")
print(f"  Performance degradation: {(1 - o0['rate(MB/s)'].iloc[-1]/o0['rate(MB/s)'].iloc[0])*100:.1f}%")

print("\nO2 (Optimized):")
print(f"  Best performance (stride=1):  {o2['rate(MB/s)'].iloc[0]:.2f} MB/s, {o2['time(msec)'].iloc[0]:.2f} ms")
print(f"  Worst performance (stride=20): {o2['rate(MB/s)'].iloc[-1]:.2f} MB/s, {o2['time(msec)'].iloc[-1]:.2f} ms")
print(f"  Performance degradation: {(1 - o2['rate(MB/s)'].iloc[-1]/o2['rate(MB/s)'].iloc[0])*100:.1f}%")

print("\nOptimization Impact:")
max_speedup = speedup.max()
min_speedup = speedup.min()
avg_speedup = speedup.mean()
print(f"  Maximum speedup: {max_speedup:.2f}x (stride={speedup.idxmax()+1})")
print(f"  Minimum speedup: {min_speedup:.2f}x (stride={speedup.idxmin()+1})")
print(f"  Average speedup: {avg_speedup:.2f}x")

print("\nKey Observations:")
# Find where performance drops most
o0_drop_idx = (o0["rate(MB/s)"].iloc[0] - o0["rate(MB/s)"]).idxmax()
o2_drop_idx = (o2["rate(MB/s)"].iloc[0] - o2["rate(MB/s)"]).idxmax()
print(f"  O0: Largest drop at stride={o0_drop_idx+1}")
print(f"  O2: Largest drop at stride={o2_drop_idx+1}")

print("\n" + "="*70)

# Create a detailed comparison table
print("\nDetailed Stride Comparison:")
print("-"*70)
print(f"{'Stride':<8} {'O0 Time':<12} {'O2 Time':<12} {'O0 BW':<12} {'O2 BW':<12} {'Speedup':<10}")
print(f"{'':^8} {'(ms)':^12} {'(ms)':^12} {'(MB/s)':^12} {'(MB/s)':^12} {'(x)':^10}")
print("-"*70)
for i in range(len(o0)):
    stride = o0["stride"].iloc[i]
    o0_time = o0["time(msec)"].iloc[i]
    o2_time = o2["time(msec)"].iloc[i]
    o0_bw = o0["rate(MB/s)"].iloc[i]
    o2_bw = o2["rate(MB/s)"].iloc[i]
    sp = speedup.iloc[i]
    print(f"{stride:<8} {o0_time:<12.2f} {o2_time:<12.2f} {o0_bw:<12.2f} {o2_bw:<12.2f} {sp:<10.2f}")
print("-"*70)