#include "libshmlite/shm_handle.h"
#include <climits>
#include <utility>

namespace shmlite {

/**
 * @brief shm_open打开错误处理
 *
 * @param fd 文件描述符
 */
#define SHM_OPEN_HANDLE_FAIL(fd)                                               \
  do {                                                                         \
    if (fd == -1) {                                                            \
      PRINT_ERRMSG("Can not allocate fd for shmhandle.", errno);               \
    }                                                                          \
  } while (0)

/**
 * @brief 调整文件大小到预定的大小
 *
 * @param fd 文件描述符
 * @param target_size 预期的文件大小
 */
#define SHM_ADJUST_FD(fd, target_size)                                         \
  do {                                                                         \
    struct stat fd_stat;                                                       \
    int ret = fstat(fd, &fd_stat);                                             \
    if (ret == -1) {                                                           \
      PRINT_ERRMSG("Can not read stat of " << fd, errno);                      \
      close(fd);                                                               \
    }                                                                          \
    if (fd_stat.st_size < (long int)target_size) {                             \
      ret = ftruncate(fd, target_size);                                        \
      if (ret == -1) {                                                         \
        PRINT_ERRMSG("Can not ftruncate the size to " << target_size           \
                                                      << " for fd=" << fd,     \
                     errno);                                                   \
        close(fd);                                                             \
      }                                                                        \
    }                                                                          \
  } while (0)

bool ShmHandle::CheckExists(const std::string &shm_name) {
  std::string real_shmname =
      ConcatStringLimited(kShmNamePrefix, shm_name, NAME_MAX);
  /* 尝试打开，如果存在的话，会EEXIST */
  int fd = shm_open(real_shmname.c_str(), O_CREAT | O_EXCL, 0);
  if (fd == -1 && errno == EEXIST) {
    return true;
  }
  /* 不存在会创建，但是这里只是检查，所以需要删掉它 */
  close(fd);
  shm_unlink(real_shmname.c_str());
  return false;
}

bool ShmHandle::UnLink(const std::string &shm_name) {
  std::string real_shmname =
      ConcatStringLimited(kShmNamePrefix, shm_name, NAME_MAX);
  return shm_unlink(real_shmname.c_str()) == 0;
}

ShmHandle::ShmHandle(std::string name, size_t size, OpenFlags flags,
                     bool auto_unlink)
    : NamedClass(std::move(name)), size_(size), auto_unlink_(auto_unlink) {
  std::string real_shmname =
      ConcatStringLimited(kShmNamePrefix, name_, NAME_MAX);
#ifdef DEV_DEBUG
  std::cout << "opening... " << real_shmname << '\n';
#endif
  fd_ = shm_open(real_shmname.c_str(), flags, 0640);
  SHM_OPEN_HANDLE_FAIL(fd_);
  SHM_ADJUST_FD(fd_, size_);
  ptr_ = mmap(nullptr, size_, PROT_READ | PROT_WRITE, MAP_SHARED, fd_, 0);
  if (ptr_ == nullptr) {
    PRINT_ERRMSG("Can not mmap for shared memory", errno);
  }
#ifdef DEV_DEBUG
  std::cout << "ShmHandle-" << name_ << "(fd = " << fd_ << ", ptr = " << ptr_
            << ") constructed\n";
#endif
}

ShmHandle::ShmHandle(std::string name, void *value, size_t size,
                     bool auto_unlink)
    : NamedClass(std::move(name)), size_(size), auto_unlink_(auto_unlink) {
  std::string real_shmname =
      ConcatStringLimited(kShmNamePrefix, name_, NAME_MAX);
#ifdef DEV_DEBUG
  std::cout << "opening... " << real_shmname << " with given initial value"
            << '\n';
#endif
  /* 如果该共享内存不存在，就会创建，并且指定一个初始值 */
  fd_ = shm_open(real_shmname.c_str(), O_CREAT | O_RDWR, 0640);
  SHM_OPEN_HANDLE_FAIL(fd_);
  SHM_ADJUST_FD(fd_, size_);
  ptr_ = mmap(nullptr, size_, PROT_READ | PROT_WRITE, MAP_SHARED, fd_, 0);
  if (ptr_ == nullptr) {
    PRINT_ERRMSG("Can not mmap for shared memory", errno);
  } else {
    /* 设置初始值，将整块value的内存搬过去 */
    memcpy(ptr_, value, size);
  }
#ifdef DEV_DEBUG
  std::cout << "ShmHandle-" << name_ << "(fd = " << fd_ << ", ptr = " << ptr_
            << ") constructed\n";
#endif
}

ShmHandle::~ShmHandle() {
#ifdef DEV_DEBUG
  bool valid = IsValid();
  int fd_back = fd_;
#endif
  if (IsValid()) {
    int ret = munmap(ptr_, size_);
    close(fd_);
    HANDLE_ERR(ret, "Can not munmap for " << ptr_);
    if (auto_unlink_) {
      std::string real_shmname =
          ConcatStringLimited(kShmNamePrefix, name_, NAME_MAX);
      ret = shm_unlink(real_shmname.c_str());
      HANDLE_ERR(ret, "Can not shm_unlink " << real_shmname);
    }
  }
#ifdef DEV_DEBUG
  std::cout << std::boolalpha << "ShmHandle-" << name_ << "(fd = " << fd_back
            << ", ptr = " << ptr_ << ", valid?" << valid << ") "
            << "destructed.\n";
#endif
  ptr_ = nullptr;
}

} // namespace shmlite
