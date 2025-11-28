/**
 * Example 1: Error Handling Evolution
 *
 * This example shows how error handling has evolved from C++11 to C++23.
 * We implement a simple user database lookup with different error handling strategies.
 */

#include <iostream>
#include <string>
#include <map>
#include <optional>     // C++17
#include <expected>     // C++23
#include <print>        // C++23

// ============================================================================
// C++11 Style: Error codes and output parameters
// ============================================================================

namespace cpp11_style {

enum class ErrorCode {
    Success,
    UserNotFound,
    InvalidId,
    DatabaseError
};

struct User {
    int id;
    std::string name;
    std::string email;
};

class UserDatabase {
    std::map<int, User> users_;

public:
    UserDatabase() {
        users_[1] = {1, "Alice", "alice@example.com"};
        users_[2] = {2, "Bob", "bob@example.com"};
        users_[3] = {3, "Charlie", "charlie@example.com"};
    }

    // Old style: return error code, output via pointer parameter
    ErrorCode findUser(int id, User* out_user) const {
        if (id <= 0) {
            return ErrorCode::InvalidId;
        }

        auto it = users_.find(id);
        if (it == users_.end()) {
            return ErrorCode::UserNotFound;
        }

        if (out_user) {
            *out_user = it->second;
        }
        return ErrorCode::Success;
    }

    // Alternative: return pointer (nullptr on error)
    const User* getUserPtr(int id) const {
        if (id <= 0) return nullptr;

        auto it = users_.find(id);
        return (it != users_.end()) ? &it->second : nullptr;
    }
};

void demo() {
    std::cout << "=== C++11 Style Error Handling ===\n\n";

    UserDatabase db;

    // Method 1: Error code with output parameter
    {
        User user;
        ErrorCode result = db.findUser(1, &user);

        if (result == ErrorCode::Success) {
            std::cout << "Found user: " << user.name << " (" << user.email << ")\n";
        } else {
            std::cout << "Error code: " << static_cast<int>(result) << "\n";
        }
    }

    // Method 2: Pointer return (nullptr check)
    {
        const User* user = db.getUserPtr(999);
        if (user) {
            std::cout << "Found user: " << user->name << "\n";
        } else {
            std::cout << "User not found (nullptr)\n";
        }
    }

    // Problems with this approach:
    // - Easy to forget error checking
    // - Unclear API (what does nullptr mean?)
    // - No error details when using pointer return
    // - Output parameters are less readable
    // - Can't use in functional style

    std::cout << "\n";
}

} // namespace cpp11_style

// ============================================================================
// C++17 Style: std::optional
// ============================================================================

namespace cpp17_style {

struct User {
    int id;
    std::string name;
    std::string email;
};

class UserDatabase {
    std::map<int, User> users_;

public:
    UserDatabase() {
        users_[1] = {1, "Alice", "alice@example.com"};
        users_[2] = {2, "Bob", "bob@example.com"};
        users_[3] = {3, "Charlie", "charlie@example.com"};
    }

    // C++17: std::optional clearly indicates "value or nothing"
    std::optional<User> findUser(int id) const {
        if (id <= 0) {
            return std::nullopt;  // Invalid ID
        }

        auto it = users_.find(id);
        if (it == users_.end()) {
            return std::nullopt;  // Not found
        }

        return it->second;
    }

    // Can chain optional operations
    std::optional<std::string> getUserEmail(int id) const {
        auto user = findUser(id);
        if (user) {
            return user->email;
        }
        return std::nullopt;
    }
};

void demo() {
    std::cout << "=== C++17 Style: std::optional ===\n\n";

    UserDatabase db;

    // Clean value-or-not checking
    if (auto user = db.findUser(1)) {
        std::cout << "Found user: " << user->name << " (" << user->email << ")\n";
    } else {
        std::cout << "User not found\n";
    }

    // Using value_or for defaults
    auto user = db.findUser(999);
    std::string name = user.has_value() ? user->name : "Unknown";
    std::cout << "User name: " << name << "\n";

    // Functional style
    auto email = db.getUserEmail(2);
    std::cout << "Email: " << email.value_or("no email") << "\n";

    // Advantages over C++11:
    // - Clear intent: either has value or doesn't
    // - No nullptr dereferencing danger
    // - Works with value types (no pointers needed)
    // - Supports functional patterns
    //
    // Limitation:
    // - Can't distinguish WHY there's no value (invalid ID vs not found)

    std::cout << "\n";
}

} // namespace cpp17_style

// ============================================================================
// C++23 Style: std::expected
// ============================================================================

namespace cpp23_style {

struct User {
    int id;
    std::string name;
    std::string email;
};

enum class UserError {
    InvalidId,
    NotFound,
    DatabaseError,
    PermissionDenied
};

std::string errorMessage(UserError error) {
    switch (error) {
        case UserError::InvalidId: return "Invalid user ID";
        case UserError::NotFound: return "User not found";
        case UserError::DatabaseError: return "Database error";
        case UserError::PermissionDenied: return "Permission denied";
    }
    return "Unknown error";
}

class UserDatabase {
    std::map<int, User> users_;

public:
    UserDatabase() {
        users_[1] = {1, "Alice", "alice@example.com"};
        users_[2] = {2, "Bob", "bob@example.com"};
        users_[3] = {3, "Charlie", "charlie@example.com"};
    }

    // C++23: std::expected contains either value OR error
    std::expected<User, UserError> findUser(int id) const {
        if (id <= 0) {
            return std::unexpected(UserError::InvalidId);
        }

        auto it = users_.find(id);
        if (it == users_.end()) {
            return std::unexpected(UserError::NotFound);
        }

        return it->second;
    }

    // Chaining operations with and_then
    std::expected<std::string, UserError> getUserEmail(int id) const {
        return findUser(id)
            .and_then([](const User& user) -> std::expected<std::string, UserError> {
                if (user.email.empty()) {
                    return std::unexpected(UserError::DatabaseError);
                }
                return user.email;
            });
    }

    // Transform operation (maps value, preserves error)
    std::expected<std::string, UserError> getUserNameUpper(int id) const {
        return findUser(id)
            .transform([](const User& user) {
                std::string upper = user.name;
                for (char& c : upper) c = std::toupper(c);
                return upper;
            });
    }

    // Error transformation
    std::expected<User, std::string> findUserWithMessage(int id) const {
        return findUser(id)
            .or_else([](UserError error) -> std::expected<User, std::string> {
                return std::unexpected(errorMessage(error));
            });
    }
};

void demo() {
    std::println("=== C++23 Style: std::expected ===\n");

    UserDatabase db;

    // Basic usage: check and access
    {
        auto result = db.findUser(1);
        if (result) {
            std::println("Found user: {} ({})", result->name, result->email);
        } else {
            std::println("Error: {}", errorMessage(result.error()));
        }
    }

    // Error case
    {
        auto result = db.findUser(999);
        if (result) {
            std::println("Found: {}", result->name);
        } else {
            std::println("Error: {}", errorMessage(result.error()));
        }
    }

    // Chaining with and_then
    {
        auto email = db.getUserEmail(2);
        if (email) {
            std::println("Email: {}", *email);
        } else {
            std::println("Failed to get email: {}", errorMessage(email.error()));
        }
    }

    // Transform operation
    {
        auto name = db.getUserNameUpper(3);
        std::println("Uppercase name: {}", name.value_or("ERROR"));
    }

    // Error transformation
    {
        auto result = db.findUserWithMessage(-5);
        if (!result) {
            std::println("Error message: {}", result.error());
        }
    }

    // Advantages over C++17:
    // - Explicit error information (not just "no value")
    // - Type-safe error handling
    // - Functional composition with and_then, or_else, transform
    // - Forces error consideration (can't ignore like exceptions)
    // - Zero overhead (no heap allocation)
    // - Makes error paths visible in code

    std::println("");
}

} // namespace cpp23_style

// ============================================================================
// Comparison: Same operation in all three styles
// ============================================================================

void comparison_demo() {
    std::println("=== Side-by-Side Comparison ===\n");

    // C++11: Verbose, easy to misuse
    {
        cpp11_style::UserDatabase db;
        cpp11_style::User user;
        auto result = db.findUser(999, &user);

        if (result == cpp11_style::ErrorCode::Success) {
            std::println("C++11: {}", user.name);
        } else {
            std::println("C++11: Error code {}", static_cast<int>(result));
        }
    }

    // C++17: Clean, but no error details
    {
        cpp17_style::UserDatabase db;
        auto user = db.findUser(999);

        std::println("C++17: {}",
            user.has_value() ? user->name : "Not found (unknown reason)");
    }

    // C++23: Best of both worlds
    {
        cpp23_style::UserDatabase db;
        auto user = db.findUser(999);

        if (user) {
            std::println("C++23: {}", user->name);
        } else {
            std::println("C++23: {}", errorMessage(user.error()));
        }
    }
}

// ============================================================================
// Main
// ============================================================================

int main() {
    cpp11_style::demo();
    cpp17_style::demo();
    cpp23_style::demo();
    comparison_demo();

    return 0;
}
