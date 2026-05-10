# cpp-features

[![CI](https://github.com/shoom1/cpp-features/actions/workflows/ci.yml/badge.svg)](https://github.com/shoom1/cpp-features/actions/workflows/ci.yml)

Review modern C++ features with compact, realistic examples.

## Overview

This project demonstrates how practical C++ designs evolve from C++11 through C++23. Each example uses a topic where the newer standard changes the shape of the API, not just the syntax.

## Contents

- **cpp-features-review.md** - Feature reference covering C++14 to C++26
- **src/example1_error_handling.cpp** - Evolution of error handling (C++11 → C++17 → C++23)
- **src/example2_container_processing.cpp** - Container processing (C++11 → C++14 → C++17 → C++20 → C++23)
- **src/example3_type_safe_variants.cpp** - Type-safe alternatives (C++11 → C++17 → C++20)
- **src/example4_resource_management.cpp** - Resource management and buffer access (C++11 → C++14 → C++17 → C++20 → C++23)
- **src/example5_metaprogramming.cpp** - Compile-time dispatch and serialization (C++11 → C++14 → C++17 → C++20)
- **src/example6_concurrency.cpp** - Thread lifecycle and cancellation (C++11 → C++20)

## Compilation Requirements

### Compiler Requirements

The examples require a modern C++ compiler and standard library with support for C++20 and the C++23 features used in the source:

- **GCC**: 14.1 or later with current libstdc++ C++23 library support
- **Clang**: 17.0 or later with libc++ support for `<print>` and `<expected>`
- **MSVC**: Visual Studio 2022 with `/std:c++latest`

### C++ Standard Versions Used

- **Examples 1-6**: Require C++23 (`-std=c++23` or `-std=c++2b`) for this repository build
  - The examples discuss older standards, but the source is compiled as one modern codebase
  - Uses `<print>`, `<expected>`, and guarded C++23 ranges examples

### Compilation Commands

#### Using GCC:
```bash
# Example 1 - Error Handling
g++ -std=c++23 -Wall -Wextra -pthread src/example1_error_handling.cpp -o example1

# Example 2 - Container Processing
g++ -std=c++23 -Wall -Wextra -pthread src/example2_container_processing.cpp -o example2

# Example 3 - Type-Safe Variants
g++ -std=c++23 -Wall -Wextra -pthread src/example3_type_safe_variants.cpp -o example3

# Example 4 - Resource Management
g++ -std=c++23 -Wall -Wextra -pthread src/example4_resource_management.cpp -o example4

# Example 5 - Metaprogramming
g++ -std=c++23 -Wall -Wextra -pthread src/example5_metaprogramming.cpp -o example5

# Example 6 - Concurrency
g++ -std=c++23 -Wall -Wextra -pthread src/example6_concurrency.cpp -o example6

# Or compile all at once
mkdir -p build
for file in src/*.cpp; do \
  name="$(basename "$file" .cpp)"; \
  g++ -std=c++23 -Wall -Wextra -pthread "$file" -o "build/$name"; \
done
```

#### Using Clang:
```bash
clang++ -std=c++23 -Wall -Wextra -pthread src/example1_error_handling.cpp -o example1
# (similar for other examples)
```

#### Using MSVC (Command Line):
```cmd
cl /std:c++latest /EHsc /W4 src\example1_error_handling.cpp
```

### Platform-Specific Notes

- **Linux/macOS**: Use recent GCC/libstdc++ or Clang/libc++. C++23 standard-library support is still uneven across vendor releases.
- **Windows**:
  - Use Visual Studio 2022 or later
  - Some features may require `/std:c++latest` flag
  - Ensure Windows SDK is up to date
- **C++23 ranges**: Some adaptors such as `std::views::enumerate`, `std::views::chunk`, `std::views::slide`, and `std::views::zip` may be unavailable in otherwise recent standard libraries. The C++23 ranges example uses feature-test macros and reports when the local library does not expose the full set.

### Troubleshooting

If you encounter compilation errors:

1. **Missing `<print>` or `<expected>` headers**: Your compiler may not fully support C++23 yet
   - Consider upgrading to GCC 14+, Clang 17+, or MSVC 19.37+
   - Alternatively, use `-std=c++2b` flag

2. **Ranges errors**: Ensure you have a compiler and standard library with full C++20 ranges support
   - GCC 12+ or Clang 15+ recommended for C++20 ranges
   - Several C++23 range adaptors require newer standard-library releases than the compiler version alone suggests

3. **Concepts errors**: Requires C++20 concepts support
   - GCC 10+ or Clang 14+ minimum

## Feature Support by Standard

- **C++11**: RAII with `std::unique_ptr`, move-only types, `std::thread`, type traits
- **C++14**: Variable templates, generic lambdas, `std::make_unique`
- **C++17**: Structured bindings, `std::optional`, `std::variant`, `if constexpr`, `std::filesystem`, `std::byte`
- **C++20**: Concepts, ranges, `std::span`, `std::jthread`, `std::stop_token`
- **C++23**: `std::expected`, `std::print`, ranges enhancements

## Running the Examples

After compilation:
```bash
./example1  # Error handling demonstration
./example2  # Container processing demonstration
./example3  # Type-safe variants demonstration
./example4  # Resource management demonstration
./example5  # Metaprogramming demonstration
./example6  # Concurrency demonstration
```

Each example compares versions only where the progression is realistic. Some topics span C++11 to C++23; others stop earlier because the later standards do not add a clearer design for that specific problem.

## Learning Path

1. Start with **cpp-features-review.md** for a quick reference of all features
2. Run examples in order (1-6) to see practical applications
3. Each example is self-contained and demonstrates evolution of specific concepts

## License

This is an educational project for learning modern C++ features.
