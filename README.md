# cpp-features
Review new C++ features with examples

## Overview

This project provides a comprehensive exploration of modern C++ features from C++14 through C++26 (including proposed features). It includes detailed documentation and practical implementation examples showing the evolution of key C++ concepts.

## Contents

- **cpp-features-review.md** - Comprehensive feature reference covering C++14 to C++26
- **src/example1_error_handling.cpp** - Evolution of error handling (C++11 → C++17 → C++23)
- **src/example2_container_processing.cpp** - Data processing evolution (C++11 → C++17 → C++20 → C++23)
- **src/example3_type_safe_variants.cpp** - Type-safe unions and variants (C++11 → C++17 → C++20 → C++26)
- **src/example4_resource_management.cpp** - Resource management and RAII (C++11 → C++14 → C++20 → C++23)
- **src/example5_metaprogramming.cpp** - Template metaprogramming evolution (C++11 → C++17 → C++20 → C++23 → C++26)

## Compilation Requirements

### Compiler Requirements

The examples require a modern C++ compiler with support for C++20 and C++23 features:

- **GCC**: 13.0 or later (14.0+ recommended for full C++23 support)
- **Clang**: 16.0 or later (17.0+ recommended)
- **MSVC**: Visual Studio 2022 (19.36+)

### C++ Standard Versions Used

- **Examples 1-5**: Require C++23 (`-std=c++23` or `-std=c++2b`)
  - Uses `<print>`, `<expected>`, and other C++23 features

### Compilation Commands

#### Using GCC:
```bash
# Example 1 - Error Handling
g++ -std=c++23 -Wall -Wextra src/example1_error_handling.cpp -o example1

# Example 2 - Container Processing
g++ -std=c++23 -Wall -Wextra src/example2_container_processing.cpp -o example2

# Example 3 - Type-Safe Variants
g++ -std=c++23 -Wall -Wextra src/example3_type_safe_variants.cpp -o example3

# Example 4 - Resource Management
g++ -std=c++23 -Wall -Wextra src/example4_resource_management.cpp -o example4

# Example 5 - Metaprogramming
g++ -std=c++23 -Wall -Wextra src/example5_metaprogramming.cpp -o example5

# Or compile all at once
g++ -std=c++23 -Wall -Wextra src/example1_error_handling.cpp -o example1 && \
g++ -std=c++23 -Wall -Wextra src/example2_container_processing.cpp -o example2 && \
g++ -std=c++23 -Wall -Wextra src/example3_type_safe_variants.cpp -o example3 && \
g++ -std=c++23 -Wall -Wextra src/example4_resource_management.cpp -o example4 && \
g++ -std=c++23 -Wall -Wextra src/example5_metaprogramming.cpp -o example5
```

#### Using Clang:
```bash
clang++ -std=c++23 -Wall -Wextra src/example1_error_handling.cpp -o example1
# (similar for other examples)
```

#### Using MSVC (Command Line):
```cmd
cl /std:c++latest /EHsc /W4 src\example1_error_handling.cpp
```

### Platform-Specific Notes

- **Linux/macOS**: Should compile without issues on recent GCC/Clang
- **Windows**:
  - Use Visual Studio 2022 or later
  - Some features may require `/std:c++latest` flag
  - Ensure Windows SDK is up to date

### Troubleshooting

If you encounter compilation errors:

1. **Missing `<print>` or `<expected>` headers**: Your compiler may not fully support C++23 yet
   - Consider upgrading to GCC 14+, Clang 17+, or MSVC 19.37+
   - Alternatively, use `-std=c++2b` flag

2. **Ranges errors**: Ensure you have a compiler with full C++20 ranges support
   - GCC 12+ or Clang 15+ recommended

3. **Concepts errors**: Requires C++20 concepts support
   - GCC 10+ or Clang 14+ minimum

## Feature Support by Standard

- **C++14**: Variable templates, generic lambdas, `std::make_unique`
- **C++17**: Structured bindings, `std::optional`, `std::variant`, `if constexpr`
- **C++20**: Concepts, ranges, coroutines, `std::span`, three-way comparison
- **C++23**: `std::expected`, `std::print`, deducing this, ranges enhancements
- **C++26**: Reflection (proposed), pattern matching (proposed), contracts (proposed)

## Running the Examples

After compilation:
```bash
./example1  # Error handling demonstration
./example2  # Container processing demonstration
./example3  # Type-safe variants demonstration
./example4  # Resource management demonstration
./example5  # Metaprogramming demonstration
```

Each example will output comparisons showing how the same tasks are accomplished using different C++ standard versions.

## Learning Path

1. Start with **cpp-features-review.md** for a quick reference of all features
2. Run examples in order (1-5) to see practical applications
3. Each example is self-contained and demonstrates evolution of specific concepts

## Notes on C++26 Features

Some examples include C++26 proposed features (marked clearly in comments):
- **Reflection**: Still in proposal stage (P2996)
- **Pattern matching**: Proposal P1371
- **Contracts**: Under discussion, syntax may change

These are educational and show the direction C++ is heading, but won't compile until adopted into the standard.

## License

This is an educational project for learning modern C++ features.
