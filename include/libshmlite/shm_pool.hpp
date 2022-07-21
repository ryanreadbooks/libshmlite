#pragma once

#include "common_utils.h"
#include "shm_handle.h"

namespace shmlite {

class ShmPool {
public:
  template <typename Type>
  static Type* get(const std::string& name, Type default_value) {
    return nullptr;
  }

  LIBSHMLITE_NO_COPYABLE(ShmPool);

private:
  static ShmPool instance_;
};


} // namespace shmlite