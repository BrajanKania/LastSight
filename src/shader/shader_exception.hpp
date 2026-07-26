#pragma once

#include <stdexcept>
#include <string>

namespace ls {

class CompileError : public std::runtime_error {
public:
  CompileError(const std::string& file, const std::string& infoLog);

  const char* what() const noexcept override;

private:
  std::string formattedMessage_;
};

class OpenFileError : public std::runtime_error {
public:
  OpenFileError(const std::string& file);
  const char* what() const noexcept override;

private:
  std::string formattedMessage_;
};

class LinkError : public std::runtime_error {
public:
  LinkError(const std::string& vertexShaderPath, const std::string& fragmentShaderPath, const std::string& infoLog);
  const char* what() const noexcept override;

private:
  std::string formattedMessage_;
};

void formatInfoLog(std::string& source, std::string_view from, std::string_view to);

}  // namespace ls
