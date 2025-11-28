/**
 * Example 4: Resource Management Evolution
 *
 * This example shows how resource management has evolved from manual memory
 * management to smart pointers, RAII, and modern safety features.
 *
 * Task: Implement a simple file processing system with proper resource management
 */

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <memory>
#include <span>       // C++20
#include <print>      // C++23
#include <concepts>   // C++20

// ============================================================================
// C++11 Style: Manual Memory Management
// ============================================================================

namespace cpp11_style {

class FileProcessor {
    char* buffer_;
    size_t size_;
    std::string filename_;

public:
    FileProcessor(const std::string& filename, size_t buffer_size)
        : buffer_(nullptr), size_(buffer_size), filename_(filename) {

        buffer_ = new char[size_];  // Manual allocation
        if (!buffer_) {
            throw std::bad_alloc();
        }
    }

    ~FileProcessor() {
        delete[] buffer_;  // Manual cleanup
    }

    // Must explicitly delete or implement
    FileProcessor(const FileProcessor&) = delete;
    FileProcessor& operator=(const FileProcessor&) = delete;

    bool process() {
        std::ifstream file(filename_);
        if (!file) {
            return false;
        }

        file.read(buffer_, size_);
        // Process buffer...

        return true;
    }

    const char* getBuffer() const { return buffer_; }
    size_t getSize() const { return size_; }
};

// Managing multiple resources
class DataManager {
    FileProcessor* processor1_;
    FileProcessor* processor2_;
    int* data_;

public:
    DataManager(const std::string& file1, const std::string& file2)
        : processor1_(nullptr), processor2_(nullptr), data_(nullptr) {

        try {
            processor1_ = new FileProcessor(file1, 1024);
            processor2_ = new FileProcessor(file2, 1024);
            data_ = new int[100];
        } catch (...) {
            // Cleanup in case of exception
            cleanup();
            throw;
        }
    }

    ~DataManager() {
        cleanup();
    }

    DataManager(const DataManager&) = delete;
    DataManager& operator=(const DataManager&) = delete;

private:
    void cleanup() {
        delete processor1_;
        delete processor2_;
        delete[] data_;

        processor1_ = nullptr;
        processor2_ = nullptr;
        data_ = nullptr;
    }
};

void demo() {
    std::cout << "=== C++11 Style: Manual Memory Management ===\n\n";

    std::cout << "Manual resource management:\n";

    // Must carefully manage lifetimes
    FileProcessor* processor = new FileProcessor("data.txt", 1024);

    if (processor->process()) {
        std::cout << "  Processing succeeded\n";
    }

    delete processor;  // Must remember to delete!

    // Problems:
    // - Easy to forget delete (memory leaks)
    // - Exception safety is difficult
    // - Complex cleanup in destructors
    // - No move semantics by default
    // - Ownership unclear
    // - Copy operations are dangerous

    std::cout << "\n";
}

} // namespace cpp11_style

// ============================================================================
// C++14 Style: Smart Pointers and RAII
// ============================================================================

namespace cpp14_style {

class FileProcessor {
    std::unique_ptr<char[]> buffer_;
    size_t size_;
    std::string filename_;

public:
    FileProcessor(const std::string& filename, size_t buffer_size)
        : buffer_(std::make_unique<char[]>(buffer_size)),  // C++14
          size_(buffer_size),
          filename_(filename) {
    }

    // Destructor automatically generated
    // Move operations automatically generated
    // Copy operations deleted automatically (because of unique_ptr)

    bool process() {
        std::ifstream file(filename_);
        if (!file) {
            return false;
        }

        file.read(buffer_.get(), size_);
        return true;
    }

    const char* getBuffer() const { return buffer_.get(); }
    size_t getSize() const { return size_; }
};

// RAII wrapper for file handles
class FileHandle {
    std::unique_ptr<std::ifstream> file_;
    std::string filename_;

public:
    FileHandle(const std::string& filename)
        : file_(std::make_unique<std::ifstream>(filename)),
          filename_(filename) {

        if (!file_->is_open()) {
            throw std::runtime_error("Failed to open: " + filename);
        }
    }

    std::ifstream& get() { return *file_; }

    bool readLine(std::string& line) {
        return static_cast<bool>(std::getline(*file_, line));
    }

    // Automatically closed when destroyed
};

// Managing multiple resources is now safe
class DataManager {
    std::unique_ptr<FileProcessor> processor1_;
    std::unique_ptr<FileProcessor> processor2_;
    std::vector<int> data_;  // Automatic memory management

public:
    DataManager(const std::string& file1, const std::string& file2)
        : processor1_(std::make_unique<FileProcessor>(file1, 1024)),
          processor2_(std::make_unique<FileProcessor>(file2, 1024)),
          data_(100) {
        // If any constructor throws, already-constructed members
        // are automatically cleaned up!
    }

    // No destructor needed - everything cleans up automatically
    // Move operations work automatically
};

void demo() {
    std::cout << "=== C++14 Style: Smart Pointers ===\n\n";

    std::cout << "Automatic resource management:\n";

    {
        auto processor = std::make_unique<FileProcessor>("data.txt", 1024);
        processor->process();
        // Automatically deleted when scope ends
        std::cout << "  Processing completed, memory auto-freed\n";
    }

    // Shared ownership when needed
    {
        auto shared1 = std::make_shared<FileProcessor>("shared.txt", 512);
        auto shared2 = shared1;  // Reference counted

        std::cout << "  Shared resource count: " << shared1.use_count() << "\n";
        // Deleted when last shared_ptr goes out of scope
    }

    // Move semantics
    {
        auto original = std::make_unique<FileProcessor>("move.txt", 256);
        auto moved = std::move(original);  // Ownership transferred

        std::cout << "  Original pointer: "
                  << (original ? "valid" : "nullptr") << "\n";
        std::cout << "  Moved pointer: "
                  << (moved ? "valid" : "nullptr") << "\n";
    }

    // Advantages:
    // - Automatic cleanup (no leaks)
    // - Exception safe by default
    // - Clear ownership semantics
    // - Move semantics built-in
    // - Less error-prone

    std::cout << "\n";
}

} // namespace cpp14_style

// ============================================================================
// C++20 Style: Concepts for safer APIs
// ============================================================================

namespace cpp20_style {

// Concept for resources that can be processed
template<typename T>
concept Processable = requires(T t) {
    { t.process() } -> std::convertible_to<bool>;
    { t.getSize() } -> std::convertible_to<size_t>;
};

// Concept for movable but not copyable resources
template<typename T>
concept UniqueResource = std::movable<T> && !std::copyable<T>;

template<UniqueResource T>
class ResourceGuard {
    std::unique_ptr<T> resource_;

public:
    explicit ResourceGuard(std::unique_ptr<T> res)
        : resource_(std::move(res)) {}

    T* operator->() { return resource_.get(); }
    const T* operator->() const { return resource_.get(); }

    T& get() { return *resource_; }
    const T& get() const { return *resource_; }
};

class FileProcessor {
    std::unique_ptr<char[]> buffer_;
    size_t size_;
    std::string filename_;

public:
    FileProcessor(const std::string& filename, size_t buffer_size)
        : buffer_(std::make_unique<char[]>(buffer_size)),
          size_(buffer_size),
          filename_(filename) {}

    bool process() {
        std::ifstream file(filename_);
        if (!file) return false;

        file.read(buffer_.get(), size_);
        return true;
    }

    size_t getSize() const { return size_; }

    // C++20: std::span for safe buffer access
    std::span<char> getBufferSpan() {
        return std::span<char>(buffer_.get(), size_);
    }

    std::span<const char> getBufferSpan() const {
        return std::span<const char>(buffer_.get(), size_);
    }
};

// Template function with concept constraint
template<Processable P>
bool processResource(P& processor) {
    if (!processor.process()) {
        return false;
    }

    std::println("  Processed {} bytes", processor.getSize());
    return true;
}

// Concept-based factory
template<typename T>
requires std::constructible_from<T, std::string, size_t>
std::unique_ptr<T> makeProcessor(const std::string& filename, size_t size) {
    return std::make_unique<T>(filename, size);
}

void demo() {
    std::println("=== C++20 Style: Concepts and Span ===\n");

    auto processor = makeProcessor<FileProcessor>("data.txt", 1024);

    std::println("Using concepts for type safety:");
    processResource(*processor);

    // std::span for safe array access
    {
        std::println("\nUsing std::span for safe buffer access:");
        auto span = processor->getBufferSpan();

        std::println("  Buffer size: {}", span.size());
        std::println("  Buffer empty: {}", span.empty());

        // Can create subspans safely
        auto first100 = span.subspan(0, 100);
        std::println("  Subspan size: {}", first100.size());

        // No bounds checking needed - span knows its size
        // for (char c : span) { ... }  // Safe iteration
    }

    // Advantages:
    // - Concepts document and enforce requirements
    // - Better error messages
    // - std::span prevents buffer overruns
    // - More expressive APIs

    std::println("");
}

} // namespace cpp20_style

// ============================================================================
// C++23/26 Style: Enhanced safety with contracts (proposed)
// ============================================================================

namespace cpp23_style {

class FileProcessor {
    std::unique_ptr<char[]> buffer_;
    size_t size_;
    std::string filename_;

public:
    // C++26 contracts (proposed syntax)
    /*
    FileProcessor(const std::string& filename, size_t buffer_size)
        pre(buffer_size > 0)           // Precondition
        pre(!filename.empty())         // Another precondition
        post(size_ == buffer_size)     // Postcondition
        : buffer_(std::make_unique<char[]>(buffer_size)),
          size_(buffer_size),
          filename_(filename) {
    }
    */

    FileProcessor(const std::string& filename, size_t buffer_size)
        : buffer_(std::make_unique<char[]>(buffer_size)),
          size_(buffer_size),
          filename_(filename) {
        // Runtime checks as substitute for contracts
        if (buffer_size == 0) {
            throw std::invalid_argument("Buffer size must be > 0");
        }
        if (filename.empty()) {
            throw std::invalid_argument("Filename cannot be empty");
        }
    }

    /*
    // With contracts (proposed)
    std::span<char> getSubBuffer(size_t offset, size_t length)
        pre(offset < size_)
        pre(offset + length <= size_)
        post(result: result.size() == length)
    {
        return std::span<char>(buffer_.get() + offset, length);
    }
    */

    std::span<char> getSubBuffer(size_t offset, size_t length) {
        if (offset >= size_ || offset + length > size_) {
            throw std::out_of_range("Invalid buffer range");
        }
        return std::span<char>(buffer_.get() + offset, length);
    }

    bool process() {
        std::ifstream file(filename_);
        if (!file) return false;
        file.read(buffer_.get(), size_);
        return true;
    }

    size_t getSize() const { return size_; }
};

// C++23: std::out_ptr for C API interop
void demo_out_ptr() {
    std::println("C++23 std::out_ptr for C API interop:");

    // Safely use smart pointers with C APIs
    // std::unique_ptr<FILE, decltype(&fclose)> file(nullptr, fclose);
    // c_api_open_file(std::out_ptr(file), "data.txt");
    // File automatically closed when unique_ptr destroys

    std::println("  Allows safe interop with C APIs");
}

// C++23: std::expected for resource acquisition
std::expected<std::unique_ptr<FileProcessor>, std::string>
createProcessor(const std::string& filename, size_t size) {
    if (filename.empty()) {
        return std::unexpected("Filename cannot be empty");
    }

    if (size == 0) {
        return std::unexpected("Size must be greater than 0");
    }

    try {
        return std::make_unique<FileProcessor>(filename, size);
    } catch (const std::exception& e) {
        return std::unexpected(std::string("Failed to create: ") + e.what());
    }
}

void demo() {
    std::println("=== C++23/26 Style: Enhanced Safety ===\n");

    // Using std::expected for resource creation
    auto result = createProcessor("data.txt", 1024);

    if (result) {
        std::println("Created processor successfully");
        (*result)->process();
    } else {
        std::println("Error: {}", result.error());
    }

    // Test error cases
    auto error1 = createProcessor("", 1024);
    if (!error1) {
        std::println("Expected error: {}", error1.error());
    }

    auto error2 = createProcessor("file.txt", 0);
    if (!error2) {
        std::println("Expected error: {}", error2.error());
    }

    demo_out_ptr();

    std::println("\nProposed C++26 Contracts would provide:");
    std::println("  - Compile-time and runtime checks");
    std::println("  - Self-documenting preconditions/postconditions");
    std::println("  - Class invariants");
    std::println("  - Better optimization opportunities");
    std::println("  - Explicit contracts in API");

    std::println("");
}

} // namespace cpp23_style

// ============================================================================
// Comparison: Exception Safety
// ============================================================================

void exception_safety_demo() {
    std::println("=== Exception Safety Comparison ===\n");

    std::println("C++11 (Manual management):");
    std::println("  - Must carefully order cleanup in catch blocks");
    std::println("  - Easy to leak resources on exceptions");
    std::println("  - Complex error handling code");

    std::println("\nC++14 (Smart pointers):");
    std::println("  - Automatic cleanup even during exceptions");
    std::println("  - RAII ensures resources are freed");
    std::println("  - Exception-safe by default");

    std::println("\nC++20 (Concepts):");
    std::println("  - Type-safe resource handling");
    std::println("  - Better compile-time guarantees");
    std::println("  - std::span prevents buffer overruns");

    std::println("\nC++23/26 (Expected + Contracts):");
    std::println("  - Explicit error handling with std::expected");
    std::println("  - Contract-based safety guarantees");
    std::println("  - Both compile-time and runtime safety");

    std::println("");
}

// ============================================================================
// Performance Comparison
// ============================================================================

void performance_demo() {
    std::println("=== Performance Comparison ===\n");

    std::println("Zero-overhead principle maintained:");
    std::println("  - Smart pointers: No runtime cost vs raw pointers");
    std::println("  - std::span: Zero-cost abstraction");
    std::println("  - Concepts: Zero runtime cost (compile-time only)");
    std::println("  - std::expected: No heap allocation");
    std::println("  - Move semantics: Eliminates unnecessary copies");

    std::println("\nActually faster in practice:");
    std::println("  - RAII enables more optimizations");
    std::println("  - Move semantics reduces overhead");
    std::println("  - Compiler can optimize unique_ptr better");
    std::println("  - No need for try/catch everywhere");

    std::println("");
}

// ============================================================================
// Main
// ============================================================================

int main() {
    cpp11_style::demo();
    cpp14_style::demo();
    cpp20_style::demo();
    cpp23_style::demo();
    exception_safety_demo();
    performance_demo();

    return 0;
}
