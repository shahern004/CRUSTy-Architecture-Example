# Benchmark Results: C++ vs. CRUST/y (Optimized `no_std`)

**Date:** 2025-03-28

This document compares the final optimized versions of the pure C++ baseline (`main_cpp.exe`) and the CRUST/y implementation (`main_rust.exe` using a `no_std` Rust library) after applying size optimizations (`-Os`, `-s`, Rust `opt-level="z"`, etc.).

## Methodology

- **Size:** Measured using `dir *.exe` on Windows.A
- **Speed:** Measured using PowerShell's `Measure-Command`. The average of 20 consecutive runs is reported in milliseconds (ms). The specific commands used were:

  ```powershell
  # For main_cpp.exe
  $cpp_times = @(); for ($i=0; $i -lt 20; $i++) { $cpp_times += (Measure-Command { .\main_cpp.exe }).TotalMilliseconds }; $cpp_avg = ($cpp_times | Measure-Object -Average).Average; Write-Output "CPP Avg Time (ms): $cpp_avg"

  # For main_rust.exe
  $rust_times = @(); for ($i=0; $i -lt 20; $i++) { $rust_times += (Measure-Command { .\main_rust.exe }).TotalMilliseconds }; $rust_avg = ($rust_times | Measure-Object -Average).Average; Write-Output "Rust Avg Time (ms): $rust_avg"
  ```

## Results

| Metric       | `main_cpp.exe` (Baseline) | `main_rust.exe` (CRUST/y) | Comparison (CRUST/y vs. CPP) |
| ------------ | ------------------------- | ------------------------- | ---------------------------- |
| Size (bytes) | 19,456                    | 19,968                    | ~2.6% bigger                 |
| Speed (ms)   | ~10.40                    | ~9.83                     | ~5.5% faster                 |

## Conclusion

After transitioning the Rust library to `no_std` and applying aggressive build optimizations to both the Rust and C++ components, the CRUST/y executable (`main_rust.exe`) achieves:

- **Comparable Size:** It is only ~2.6% larger than the highly optimized C++ baseline.
- **Slightly Faster Speed:** It executes approximately 5.5% faster than the C++ baseline for the tested FIFO workload (based on 20 runs).

These results demonstrate that the CRUST/y architecture, when optimized correctly using `no_std`, can provide Rust's memory safety benefits with minimal size overhead and potentially even a slight performance improvement compared to a standard C++ implementation for this specific task.
