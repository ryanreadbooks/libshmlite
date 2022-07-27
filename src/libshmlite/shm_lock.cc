#include <utility>
#include "libshmlite/shm_lock.h"

namespace shmlite {

bool ShmLock::UnLink(const std::string &name) {
  std::string real_shmname = ConcatStringLimited(kShmLockNamePrefix, name, SHMLOCK_NAME_MAX);
  SIMPLE_DEBUG("real_shmname = " << real_shmname);
  return sem_unlink(real_shmname.c_str()) == 0;
}

ShmLock::ShmLock(std::string name, unsigned int value, bool auto_unlink)
    : NamedClass(std::move(name)), auto_unlink_(auto_unlink) {
  std::string real_semname = ConcatStringLimited(kShmLockNamePrefix, name_, SHMLOCK_NAME_MAX);
  sem_ptr_ = sem_open(real_semname.c_str(), O_CREAT, 0640, value);
  if (sem_ptr_ == SEM_FAILED) {
    PRINT_ERRMSG("Can not initialize sem_t " << name_, errno);
  }
#ifdef DEV_DEBUG
  SIMPLE_DEBUG("sem_t " << real_semname << " (" << sem_ptr_ << ") created");
#endif
}

ShmLock::~ShmLock() {
  int ret = sem_close(sem_ptr_);
  HANDLE_ERR(ret, "Can not sem_close " << name_);
  if (auto_unlink_) {
    ShmLock::UnLink(name_);
  }
#ifdef DEV_DEBUG
  SIMPLE_DEBUG("sem_t (" << sem_ptr_ << ") destructed.");
#endif
}

void ShmLock::Post() {
  int ret = sem_post(sem_ptr_);
  HANDLE_ERR(ret, "Can not sem_post " << name_);
}

void ShmLock::Wait() {
  int ret = sem_wait(sem_ptr_);
  HANDLE_ERR(ret, "Can not sem_wait " << name_);
}

int ShmLock::GetValue() const {
  int value = -1;
  int ret = sem_getvalue(sem_ptr_, &value);  /* linux平台上value最小到0 */
  HANDLE_ERR(ret, "Can not sem_getvalue " << name_);
  return value;
}

} // namespace shmlite