/**
 * Example 1: Error Handling Evolution
 *
 * This example compares realistic C++ error-handling APIs:
 * - C++11: exceptions for failure, or error codes in no-exception codebases
 * - C++17: std::optional when "no value" is a normal outcome
 * - C++23: std::expected when callers need typed failure information
 */

#include <cctype>
#include <expected>     // C++23
#include <iostream>
#include <map>
#include <optional>     // C++17
#include <print>        // C++23
#include <stdexcept>
#include <string>
#include <string_view>

// ============================================================================
// C++11 Style: Exceptions, or error codes for no-exception APIs
// ============================================================================

namespace cpp11_style {

enum class ErrorCode {
    Success,
    UserNotFound,
    InvalidId,
    DatabaseError
};

const char* errorMessage(ErrorCode code) noexcept {
    switch (code) {
        case ErrorCode::Success: return "success";
        case ErrorCode::UserNotFound: return "user not found";
        case ErrorCode::InvalidId: return "invalid user ID";
        case ErrorCode::DatabaseError: return "database error";
    }
    return "unknown error";
}

struct User {
    int id;
    std::string name;
    std::string email;
};

class UserNotFound : public std::runtime_error {
public:
    explicit UserNotFound(int id)
        : std::runtime_error("user " + std::to_string(id) + " was not found") {}
};

class UserDatabase {
    std::map<int, User> users_;

public:
    UserDatabase() {
        users_[1] = {1, "Alice", "alice@example.com"};
        users_[2] = {2, "Bob", "bob@example.com"};
        users_[3] = {3, "Charlie", "charlie@example.com"};
    }

    // Common C++11 style: return the value and report failure with exceptions.
    User findUserOrThrow(int id) const {
        if (id <= 0) {
            throw std::invalid_argument("user ID must be positive");
        }

        auto it = users_.find(id);
        if (it == users_.end()) {
            throw UserNotFound(id);
        }

        return it->second;
    }

    // No-exception style: return status and write through a non-null reference.
    ErrorCode tryFindUser(int id, User& out_user) const {
        if (id <= 0) {
            return ErrorCode::InvalidId;
        }

        auto it = users_.find(id);
        if (it == users_.end()) {
            return ErrorCode::UserNotFound;
        }

        out_user = it->second;
        return ErrorCode::Success;
    }
};

void demo() {
    std::cout << "=== C++11 Style: Exceptions or Error Codes ===\n\n";

    UserDatabase db;

    // Exception-based API: idiomatic when failures should interrupt normal flow.
    try {
        User user = db.findUserOrThrow(1);
        std::cout << "Exception API found: " << user.name
                  << " (" << user.email << ")\n";
    } catch (const std::exception& e) {
        std::cout << "Exception API error: " << e.what() << "\n";
    }

    try {
        db.findUserOrThrow(999);
    } catch (const std::exception& e) {
        std::cout << "Exception API missing user: " << e.what() << "\n";
    }

    // Error-code API: useful in low-level or no-exception codebases.
    {
        User user{};
        ErrorCode result = db.tryFindUser(2, user);

        if (result == ErrorCode::Success) {
            std::cout << "Error-code API found: " << user.name << "\n";
        } else {
            std::cout << "Error-code API failed: " << errorMessage(result) << "\n";
        }
    }

    {
        User user{};
        ErrorCode result = db.tryFindUser(-5, user);
        if (result != ErrorCode::Success) {
            std::cout << "Error-code API failed: " << errorMessage(result) << "\n";
        }
    }

    // Tradeoffs:
    // - Exceptions keep the success path clean, but failures are not visible in the type.
    // - Error codes make failures explicit, but every caller must check them.
    // - Output references avoid nullable output parameters but are still less composable.

    std::cout << "\n";
}

} // namespace cpp11_style

// ============================================================================
// C++17 Style: std::optional for normal absence
// ============================================================================

namespace cpp17_style {

struct User {
    int id;
    std::string name;
    std::string email;
};

class UserCache {
    std::map<int, User> cached_users_;

public:
    UserCache() {
        cached_users_[1] = {1, "Alice", "alice@example.com"};
        cached_users_[2] = {2, "Bob", "bob@example.com"};
    }

    // std::optional is appropriate when absence is expected and needs no reason.
    std::optional<User> findCachedUser(int id) const {
        auto it = cached_users_.find(id);
        if (it == cached_users_.end()) {
            return std::nullopt;
        }

        return it->second;
    }

    std::optional<std::string> cachedEmailFor(int id) const {
        auto user = findCachedUser(id);
        if (!user) {
            return std::nullopt;
        }

        return user->email;
    }
};

void demo() {
    std::cout << "=== C++17 Style: std::optional for Normal Absence ===\n\n";

    UserCache cache;

    if (auto user = cache.findCachedUser(1)) {
        std::cout << "Cache hit: " << user->name
                  << " (" << user->email << ")\n";
    }

    if (auto user = cache.findCachedUser(999)) {
        std::cout << "Cache hit: " << user->name << "\n";
    } else {
        std::cout << "Cache miss: fetch from the database if needed\n";
    }

    std::cout << "Cached email: "
              << cache.cachedEmailFor(2).value_or("not cached") << "\n";

    // Takeaway:
    // - optional<T> means "maybe T"; it is not an error-reporting type.
    // - Good fit: cache miss, optional config value, optional relationship.
    // - Poor fit: invalid input, permission failure, I/O failure, database outage.

    std::cout << "\n";
}

} // namespace cpp17_style

// ============================================================================
// C++23 Style: std::expected for typed recoverable failures
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
    MissingEmail,
    PermissionDenied
};

std::string_view errorMessage(UserError error) noexcept {
    switch (error) {
        case UserError::InvalidId: return "Invalid user ID";
        case UserError::NotFound: return "User not found";
        case UserError::MissingEmail: return "User record has no email";
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
        users_[3] = {3, "Charlie", ""};
    }

    [[nodiscard]] std::expected<User, UserError> findUser(
        int id,
        bool can_view_restricted = true
    ) const {
        if (id <= 0) {
            return std::unexpected(UserError::InvalidId);
        }

        auto it = users_.find(id);
        if (it == users_.end()) {
            return std::unexpected(UserError::NotFound);
        }

        if (id == 3 && !can_view_restricted) {
            return std::unexpected(UserError::PermissionDenied);
        }

        return it->second;
    }

    [[nodiscard]] std::expected<std::string, UserError> getUserEmail(int id) const {
        return findUser(id)
            .and_then([](const User& user) -> std::expected<std::string, UserError> {
                if (user.email.empty()) {
                    return std::unexpected(UserError::MissingEmail);
                }
                return user.email;
            });
    }

    [[nodiscard]] std::expected<std::string, UserError> getUserNameUpper(int id) const {
        return findUser(id)
            .transform([](const User& user) {
                std::string upper = user.name;
                for (char& c : upper) {
                    c = static_cast<char>(
                        std::toupper(static_cast<unsigned char>(c))
                    );
                }
                return upper;
            });
    }

    [[nodiscard]] std::expected<User, std::string> findUserWithMessage(int id) const {
        return findUser(id)
            .or_else([](UserError error) -> std::expected<User, std::string> {
                return std::unexpected(std::string(errorMessage(error)));
            });
    }
};

void demo() {
    std::println("=== C++23 Style: std::expected ===\n");

    UserDatabase db;

    {
        auto result = db.findUser(1);
        if (result) {
            std::println("Found user: {} ({})", result->name, result->email);
        } else {
            std::println("Error: {}", errorMessage(result.error()));
        }
    }

    {
        auto result = db.findUser(999);
        if (!result) {
            std::println("Lookup failed: {}", errorMessage(result.error()));
        }
    }

    {
        auto result = db.findUser(3, false);
        if (!result) {
            std::println("Restricted lookup failed: {}", errorMessage(result.error()));
        }
    }

    {
        auto email = db.getUserEmail(3);
        if (email) {
            std::println("Email: {}", *email);
        } else {
            std::println("Email lookup failed: {}", errorMessage(email.error()));
        }
    }

    {
        auto name = db.getUserNameUpper(3);
        std::println("Uppercase name: {}", name.value_or("ERROR"));
    }

    {
        auto result = db.findUserWithMessage(-5);
        if (!result) {
            std::println("Error message: {}", result.error());
        }
    }

    // Takeaway:
    // - expected<T, E> means "T or a typed failure E".
    // - It keeps failures visible in the function signature.
    // - It composes like optional, but preserves the reason for failure.

    std::println("");
}

} // namespace cpp23_style

// ============================================================================
// Comparison: Matching the type to the API semantics
// ============================================================================

void comparison_demo() {
    std::println("=== Side-by-Side Comparison ===\n");

    {
        cpp11_style::UserDatabase db;
        try {
            auto user = db.findUserOrThrow(999);
            std::println("C++11 exception API: {}", user.name);
        } catch (const std::exception& e) {
            std::println("C++11 exception API: {}", e.what());
        }
    }

    {
        cpp17_style::UserCache cache;
        auto user = cache.findCachedUser(999);

        std::println("C++17 optional API: {}",
            user.has_value() ? user->name : "cache miss, not an error");
    }

    {
        cpp23_style::UserDatabase db;
        auto user = db.findUser(999);

        if (user) {
            std::println("C++23 expected API: {}", user->name);
        } else {
            std::println("C++23 expected API: {}", errorMessage(user.error()));
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
