#pragma once

#include <memory>

class HelperImpl;

class Helper {
public:
  Helper();
  ~Helper();

  void CallHelper() const;

private:
  std::unique_ptr<HelperImpl> impl_;
};
