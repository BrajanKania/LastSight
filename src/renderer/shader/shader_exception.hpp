#pragma once

#include <filesystem>
#include <stdexcept>
#include <string>

namespace ls {

class CompileError : public std::runtime_error {
public:
  CompileError(const std::filesystem::path& file, const std::string& infoLog);

  const char* what() const noexcept override;

private:
  std::string formattedMessage_;
};

class OpenFileError : public std::runtime_error {
public:
  OpenFileError(const std::filesystem::path& file);
  const char* what() const noexcept override;

private:
  std::string formattedMessage_;
};

class LinkError : public std::runtime_error {
public:
  LinkError(const std::filesystem::path& vertexShaderPath,
            const std::filesystem::path& fragmentShaderPath,
            const std::string& infoLog);
  const char* what() const noexcept override;

private:
  std::string formattedMessage_;
};

void formatInfoLog(std::string& source, std::string_view from, std::string_view to);

}  // namespace ls
