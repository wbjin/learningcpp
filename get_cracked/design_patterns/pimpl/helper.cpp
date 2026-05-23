#include "helper.hpp"

#include <cstdio>

class HelperImpl {
public:
	void CallHelperImpl() {
    std::printf("HelperImpl::CallHelperImpl\n");
    std::printf("This was added later\n");
  };
};

Helper::Helper() : impl_ {std::make_unique<HelperImpl>()} {}

Helper::~Helper() = default;

void Helper::CallHelper() const {
  std::printf("Helper::CallHelper\n");
  impl_->CallHelperImpl();
}
