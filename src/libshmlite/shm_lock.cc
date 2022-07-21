#include "libshmlite/shm_lock.h"

#include <utility>

namespace shmlite {

ShmLock::ShmLock(std::string name) : NamedClass(std::move(name)) {
  sem_open(name_.c_str(), O_CREAT);
}

} // namespace shmlite