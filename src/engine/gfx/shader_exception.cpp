#include "engine/gfx/shader_exception.hpp"

#include <filesystem>
#include <format>
#include <stdexcept>

namespace ls::gfx {

  CompileError::CompileError(const std::filesystem::path& file, const std::string& infoLog)
      : std::runtime_error("Failed to compile shader.") {
    std::string formattedInfoLog{ infoLog };
    formatInfoLog(formattedInfoLog, "\n", "\n\t\t\t");

    formattedMessage_ = std::format(
        "\tShader error: ["
        "\n\t\tCause: failed to compile shader."
        "\n\t\tFile: \"{}\"."
        "\n\t\tInfo log: [ \n\t\t\t{} \n\t\t]"
        "\n\t]",
        file.string(),
        std::move(formattedInfoLog)
    );
  }

  const char* CompileError::what() const noexcept { return formattedMessage_.c_str(); }

  OpenFileError::OpenFileError(const std::filesystem::path& file)
      : std::runtime_error("Failed to load file.") {
    formattedMessage_ = std::format(
        "\tShader error: ["
        "\n\t\tCause: failed to open file."
        "\n\t\tFile: \"{}\"."
        "\n\t]",
        file.string()
    );
  }

  const char* OpenFileError::what() const noexcept { return formattedMessage_.c_str(); }

  LinkError::LinkError(
      const std::filesystem::path& vertexShaderPath,
      const std::filesystem::path& fragmentShaderPath,
      const std::string& infoLog
  )
      : std::runtime_error("Failed to link program.") {
    std::string formattedInfoLog{ infoLog };
    formatInfoLog(formattedInfoLog, "\n", "\n\t\t\t");

    formattedMessage_ = std::format(
        "\tShader error: ["
        "\n\t\tCause: failed to link program."
        "\n\t\tFiles: ["
        "\n\t\t\t\"{}\""
        "\n\t\t\t\"{}\""
        "\n\t\t]"
        "\n\t\tInfo log: [ \n\t\t\t{} \n\t\t]"
        "\n\t]",
        vertexShaderPath.string(),
        fragmentShaderPath.string(),
        std::move(formattedInfoLog)
    );
  }

  const char* LinkError::what() const noexcept { return formattedMessage_.c_str(); }

  void formatInfoLog(std::string& source, std::string_view from, std::string_view to) {
    if (from.empty())
      return;

    if (!source.empty() && source.back() == '\n')
      source.pop_back();

    std::string::size_type startPos{ 0 };

    while ((startPos = source.find(from, startPos)) != std::string::npos) {
      source.replace(startPos, from.length(), to);
      startPos += to.length();
    }
  }
}  // namespace ls::gfx
