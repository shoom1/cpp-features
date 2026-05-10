/**
 * Example 4: Resource Management Evolution
 *
 * This example keeps the baseline realistic: C++11 code should already use
 * RAII. The progression shows how later standards make ownership, paths,
 * buffer views, and recoverable acquisition errors clearer.
 *
 * Task: Own a file reader and expose its buffer safely.
 */

#include <concepts>
#include <cstddef>
#include <expected>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <memory>
#include <print>
#include <span>
#include <stdexcept>
#include <string>
#include <vector>

// ============================================================================
// C++11 Style: RAII with unique_ptr and move-only resources
// ============================================================================

namespace cpp11_style {

class Buffer {
    std::unique_ptr<char[]> data_;
    std::size_t size_;

public:
    explicit Buffer(std::size_t size)
        : data_(new char[size]), size_(size) {}

    Buffer(Buffer&&) noexcept = default;
    Buffer& operator=(Buffer&&) noexcept = default;

    Buffer(const Buffer&) = delete;
    Buffer& operator=(const Buffer&) = delete;

    char* data() { return data_.get(); }
    const char* data() const { return data_.get(); }
    std::size_t size() const { return size_; }
};

class FileReader {
    std::ifstream file_;
    Buffer buffer_;

public:
    FileReader(const std::string& filename, std::size_t buffer_size)
        : file_(filename.c_str(), std::ios::binary),
          buffer_(buffer_size) {}

    FileReader(FileReader&&) noexcept = default;
    FileReader& operator=(FileReader&&) noexcept = default;

    FileReader(const FileReader&) = delete;
    FileReader& operator=(const FileReader&) = delete;

    bool readBlock() {
        if (!file_) {
            return false;
        }

        file_.read(buffer_.data(), static_cast<std::streamsize>(buffer_.size()));
        return file_.gcount() > 0;
    }

    const Buffer& buffer() const {
        return buffer_;
    }
};

void demo() {
    std::cout << "=== C++11 Style: RAII and Move-Only Ownership ===\n\n";

    std::unique_ptr<FileReader> reader(
        new FileReader("src/example4_resource_management.cpp", 1024)
    );

    if (reader->readBlock()) {
        std::cout << "Read into " << reader->buffer().size()
                  << "-byte owned buffer\n";
    }

    std::cout << "Ownership is explicit, but construction is still verbose.\n\n";
}

} // namespace cpp11_style

// ============================================================================
// C++14 Style: make_unique and simpler factories
// ============================================================================

namespace cpp14_style {

class FileReader {
    std::unique_ptr<char[]> buffer_;
    std::size_t size_;
    std::ifstream file_;

public:
    FileReader(const std::string& filename, std::size_t buffer_size)
        : buffer_(std::make_unique<char[]>(buffer_size)),
          size_(buffer_size),
          file_(filename, std::ios::binary) {}

    bool readBlock() {
        if (!file_) {
            return false;
        }

        file_.read(buffer_.get(), static_cast<std::streamsize>(size_));
        return file_.gcount() > 0;
    }

    std::size_t bufferSize() const {
        return size_;
    }
};

auto makeReader(const std::string& filename, std::size_t buffer_size) {
    return std::make_unique<FileReader>(filename, buffer_size);
}

void demo() {
    std::cout << "=== C++14 Style: make_unique Factories ===\n\n";

    auto reader = makeReader("src/example4_resource_management.cpp", 1024);
    if (reader->readBlock()) {
        std::cout << "Read into " << reader->bufferSize()
                  << "-byte owned buffer\n";
    }

    auto allocate = [](auto count) {
        return std::make_unique<char[]>(count);
    };
    auto scratch = allocate(256);
    scratch[0] = '\0';

    std::cout << "Factories remove raw new from calling code.\n\n";
}

} // namespace cpp14_style

// ============================================================================
// C++17 Style: filesystem paths and byte buffers
// ============================================================================

namespace cpp17_style {

struct FileInfo {
    std::filesystem::path filename;
    std::filesystem::path extension;
};

FileInfo describePath(const std::filesystem::path& path) {
    return {path.filename(), path.extension()};
}

class ByteBuffer {
    std::vector<std::byte> bytes_;

public:
    explicit ByteBuffer(std::size_t size) : bytes_(size) {}

    std::byte* data() { return bytes_.data(); }
    const std::byte* data() const { return bytes_.data(); }
    std::size_t size() const { return bytes_.size(); }
};

void demo() {
    std::cout << "=== C++17 Style: filesystem and std::byte ===\n\n";

    const std::filesystem::path path{"src/example4_resource_management.cpp"};
    const auto [filename, extension] = describePath(path);

    std::cout << "Path filename: " << filename.string() << "\n";
    std::cout << "Path extension: " << extension.string() << "\n";

    ByteBuffer buffer(4);
    buffer.data()[0] = std::byte{'C'};
    buffer.data()[1] = std::byte{'+'};
    buffer.data()[2] = std::byte{'+'};
    buffer.data()[3] = std::byte{'\0'};

    std::cout << "Binary buffer size: " << buffer.size() << " bytes\n\n";
}

} // namespace cpp17_style

// ============================================================================
// C++20 Style: span and concepts for non-owning buffer APIs
// ============================================================================

namespace cpp20_style {

class ByteBuffer {
    std::vector<std::byte> bytes_;

public:
    explicit ByteBuffer(std::size_t size) : bytes_(size) {}

    std::span<std::byte> bytes() {
        return bytes_;
    }

    std::span<const std::byte> bytes() const {
        return bytes_;
    }
};

template<typename T>
concept MutableByteBuffer = requires(T buffer) {
    { buffer.bytes() } -> std::same_as<std::span<std::byte>>;
};

void writeMagicHeader(MutableByteBuffer auto& buffer) {
    auto bytes = buffer.bytes();
    if (bytes.size() < 4) {
        throw std::invalid_argument("buffer must contain at least four bytes");
    }

    bytes[0] = std::byte{'C'};
    bytes[1] = std::byte{'P'};
    bytes[2] = std::byte{'P'};
    bytes[3] = std::byte{'\0'};
}

void demo() {
    std::println("=== C++20 Style: span and Concepts ===\n");

    ByteBuffer buffer(16);
    writeMagicHeader(buffer);

    const auto bytes = buffer.bytes();
    const auto header = bytes.first(4);

    std::println("Buffer size: {}", bytes.size());
    std::println("Header bytes: {} {} {} {}",
        static_cast<char>(header[0]),
        static_cast<char>(header[1]),
        static_cast<char>(header[2]),
        static_cast<int>(header[3]));

    std::println("span makes ownership separate from access.");
    std::println("");
}

} // namespace cpp20_style

// ============================================================================
// C++23 Style: expected for recoverable resource acquisition
// ============================================================================

namespace cpp23_style {

enum class OpenError {
    EmptyPath,
    NotFound
};

std::string message(OpenError error) {
    switch (error) {
        case OpenError::EmptyPath:
            return "path is empty";
        case OpenError::NotFound:
            return "file could not be opened";
    }
    return "unknown error";
}

[[nodiscard]] std::expected<std::ifstream, OpenError>
openTextFile(const std::filesystem::path& path) {
    if (path.empty()) {
        return std::unexpected(OpenError::EmptyPath);
    }

    std::ifstream file(path);
    if (!file) {
        return std::unexpected(OpenError::NotFound);
    }

    return std::move(file);
}

std::filesystem::path exampleSourcePath() {
    const std::filesystem::path source_path{__FILE__};
    if (std::filesystem::exists(source_path)) {
        return source_path;
    }

    const auto parent_relative = std::filesystem::path{".."} / source_path;
    if (std::filesystem::exists(parent_relative)) {
        return parent_relative;
    }

    return source_path;
}

void demo() {
    std::println("=== C++23 Style: expected for Acquisition ===\n");

    auto file = openTextFile(exampleSourcePath());
    if (file) {
        std::string first_line;
        std::getline(*file, first_line);
        std::println("Opened source file, first line: {}", first_line);
    } else {
        std::println("Open failed: {}", message(file.error()));
    }

    auto missing = openTextFile("missing-input.txt");
    if (!missing) {
        std::println("Missing file: {}", message(missing.error()));
    }

    auto invalid = openTextFile("");
    if (!invalid) {
        std::println("Invalid request: {}", message(invalid.error()));
    }

    std::println("");
}

} // namespace cpp23_style

// ============================================================================
// Main
// ============================================================================

int main() {
    cpp11_style::demo();
    cpp14_style::demo();
    cpp17_style::demo();
    cpp20_style::demo();
    cpp23_style::demo();

    return 0;
}
