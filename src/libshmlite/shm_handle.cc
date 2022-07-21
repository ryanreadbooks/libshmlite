#include "libshmlite/shm_handle.h"

#include <utility>

namespace shmlite {

ShmHandle::ShmHandle(std::string name, size_t size, OpenFlags flags,
                     bool auto_unlink)
    : NamedClass(std::move(name)), size_(size), auto_unlink_(auto_unlink) {
  std::string real_shmname = ConcatShmRealname(name_);
#ifdef DEV_DEBUG
  std::cout << "opening... " << real_shmname << '\n';
#endif
  fd_ = shm_open(real_shmname.c_str(), flags, 0640);
  if (fd_ == -1) {
    /* 创建失败 */
    PRINT_ERRMSG("Can not allocate fd for shmhandle.", errno);
  }
  struct stat fd_stat;
  int ret = fstat(fd_, &fd_stat);
  if (ret == -1) {
    PRINT_ERRMSG("Can not read stat of " << fd_, errno);
    close(fd_);
  }
  /* 调整文件大小到预定的大小 */
  if (fd_stat.st_size < (long int)size_) {
    ret = ftruncate(fd_, size);
    if (ret == -1) {
      PRINT_ERRMSG(
          "Can not ftruncate the size to " << size << " for fd=" << fd_, errno);
      close(fd_);
    }
  }
  ptr_ = mmap(nullptr, size_, PROT_READ | PROT_WRITE, MAP_SHARED, fd_, 0);
  if (ptr_ == nullptr) {
    PRINT_ERRMSG("Can not mmap for shared memory", errno);
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
      std::string real_shmname = ConcatShmRealname(name_);
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

bool ShmHandle::CheckExists(const std::string &shm_name) {
  std::string real_shmname = ConcatShmRealname(shm_name);
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

std::string ShmHandle::ConcatShmRealname(const std::string& name) {
  std::string real_shmname = kShmNamePrefix + name;
  return real_shmname;
}

} // namespace shmlite
