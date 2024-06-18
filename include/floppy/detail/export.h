#pragma once

#include <string>
#include <string_view>

#if defined(_WIN32)
# if defined(FLOPPY_LIBRARY)
#   define FLOPPY_EXPORT __declspec(dllexport)
# elif defined(FLOPPY_STATIC_LIBRARY)
#   define FLOPPY_EXPORT
# else
#   define FLOPPY_EXPORT __declspec(dllimport)
# endif
#else
# define FLOPPY_EXPORT
#endif

#define _stringify$(x) #x
#define stringify$(x) _stringify$(x)

/// \brief Main namespace for the library.
/// \note Use <tt>fl</tt> instead of <tt>floppy</tt> if you want shorter name.
namespace floppy { // NOLINT(*-concat-nested-namespaces)
  /// \brief Metadata definitions, such as library version or name.
  namespace meta {
    namespace detail {
      constexpr auto is_digit(char c) -> bool { return c <= '9' && c >= '0'; }
      constexpr auto stoi_impl(char const* str, int value = 0) -> int {
        return *str ? is_digit(*str)
          ? stoi_impl(str + 1, (*str - '0') + value * 10)
          : throw "compile-time-error: not a digit" : value;
      }
      constexpr auto stoi(char const* str) -> int { return stoi_impl(str); }
    } // namespace detail

    /// \brief Version number structure.
    class version
    {
     public:
      /// \brief Creates version object at compile-time.
      /// \param major Version number major as an integer.
      /// \param minor Version number minor as an integer.
      /// \param patch Version number patch as an integer.
      constexpr version(int major, int minor, int patch)
        : major_(major)
        , minor_(minor)
        , patch_(patch)
      {}

      /// \brief Creates version object from string.
      /// \param version Version number as a string.
      constexpr explicit version(std::string_view version) noexcept
      {
        auto major = version.substr(0, version.find('.'));
        auto minor = version.substr(version.find('.') + 1, version.find('.', version.find('.') + 1));
        auto patch = version.substr(version.find('.', version.find('.') + 1) + 1);
        this->major_ = detail::stoi(major.data());
        this->minor_ = detail::stoi(minor.data());
        this->patch_ = detail::stoi(patch.data());
      }

      /// \brief Returns constant expression version number major as an integer.
      /// \see minor
      /// \see patch
      [[nodiscard]] constexpr auto major() const noexcept -> int { return this->major_; }

      /// \brief Returns constant expression version number minor as an integer.
      /// \see major
      /// \see patch
      [[nodiscard]] constexpr auto minor() const noexcept -> int { return this->minor_; }

      /// \brief Returns constant expression version number patch as an integer.
      /// \see major
      /// \see minor
      [[nodiscard]] constexpr auto patch() const noexcept -> int { return this->patch_; }

      /// \brief Returns version as a string.
      /// \see major
      /// \see minor
      /// \see patch
      [[nodiscard]] auto as_str() const noexcept -> std::string {
        return std::to_string(this->major_) + "." + std::to_string(this->minor_) + "." + std::to_string(this->patch_);
      }

     private:
      int major_{}; ///< Major version.
      int minor_{}; ///< Minor version.
      int patch_{}; ///< Patch version.
    };

    static_assert(version(1, 2, 3).major() == 1);
    static_assert(version(1, 2, 3).minor() == 2);
    static_assert(version(1, 2, 3).patch() == 3);

    /// \brief Project metadata constexpr structure.
    class [[maybe_unused]] project_meta
    {
     public:
      /// \brief Creates version object at compile-time.
      /// \param version Project version.
      /// \param name Project name.
      /// \param domain Project domain.
      /// \param organization Project organization.
      /// \see project_name
      consteval project_meta(version version, std::string_view name, std::string_view domain, std::string_view organization)
        : version_(version)
        , name_(name)
        , domain_(domain)
        , organization_(organization)
      {}

      /// \brief Returns constant expression project version.
      [[nodiscard]] constexpr auto version() const noexcept -> version { return this->version_; }

      /// \brief Returns constant expression project name.
      [[nodiscard]] constexpr auto name() const noexcept -> std::string_view { return this->name_; }

      /// \brief Returns constant expression project domain.
      [[nodiscard]] constexpr auto domain() const noexcept -> std::string_view { return this->domain_; }

      /// \brief Returns constant expression project organization.
      [[nodiscard]] constexpr auto organization() const noexcept -> std::string_view { return this->organization_; }

     private:
      class version version_;        ///< Version number.
      std::string_view name_;        ///< Project name.
      std::string_view domain_;      ///< Project domain.
      std::string_view organization_;///< Project organization.
    };

    /// \brief Project metadata object, available at compile-time.
    [[maybe_unused]] constexpr inline auto floppy_meta = project_meta(
      version(CMAKE_PROJECT_VERSION_MAJOR, CMAKE_PROJECT_VERSION_MINOR, CMAKE_PROJECT_VERSION_PATCH),
      std::string_view(stringify$(CMAKE_TARGET_NAME)),
      "io.github.whs31",
      "whs31"
    );

    static_assert(floppy_meta.version().major() == CMAKE_PROJECT_VERSION_MAJOR, "major version isn't the same");
    static_assert(floppy_meta.version().minor() == CMAKE_PROJECT_VERSION_MINOR, "minor version isn't the same");
    static_assert(floppy_meta.version().patch() == CMAKE_PROJECT_VERSION_PATCH, "patch version isn't the same");
    static_assert(floppy_meta.name() == std::string_view(stringify$(CMAKE_TARGET_NAME)), "project name isn't the same");
    static_assert(floppy_meta.domain() == "io.github.whs31", "project domain isn't the same");
    static_assert(floppy_meta.organization() == "whs31", "project organization isn't the same");
  } // namespace meta
} // namespace floppy

#undef _stringify$
#undef stringify$

/// \brief Alias for the main namespace \ref floppy.
namespace fl = floppy; // NOLINT(*-unused-alias-decls)