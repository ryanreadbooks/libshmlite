#pragma once

#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string>
#include <iostream>

#include "libshmlite/common_utils.h"

namespace shmlite {

constexpr const char *kShmNamePrefix = "/libshmlite-";  /**< 共享内存名字的前缀，必须以/开头 */

/**
 * @brief 对POSIX API下的共享内存操作的封装。
 *
 * ShmHandle 对 POSIX 标准下的 API 的 shm_* 系列函数进行封装。
 */
class ShmHandle : public NamedClass {
public:
  /**
   * @brief 检查共享内存是否存在
   * 
   * @param shm_name 共享内存名字
   * @return true   存在
   * @return false  不存在
   */
  static bool CheckExists(const std::string &shm_name);

  LIBSHMLITE_NO_COPYABLE(ShmHandle)

  /**
   * @brief 打开共享内存的方式
   *
   * 用法：除读写外可以同时使用多个；比如CREATE | READ_WRITE
   */
  enum OpenFlags {
    READ_ONLY = O_RDONLY, /**< 只读 */
    READ_WRITE = O_RDWR,  /**< 读写 */
    CREATE = O_CREAT,     /**< 创建出来 */
    TRUNCATE = O_TRUNC, /**< 如果已经存在，则将已有的内容清除 */
    CREAT_RDWR = O_CREAT | O_RDWR, /**< 创建出来同时指定读写标记 */
  };

  /**
   * @brief 创建一个 ShmHandle 对象。
   *
   * @param name        对象的名称。
   * @param size        需要的共享内存的大小，单位（字节）。
   * @param flags       打开共享内存的标志。参考@ref OpenFlags "OpenFlags"
   * @param auto_unlink 析构的时候是否同时 shm_unlink 掉这块共享内存
   */
  ShmHandle(std::string name, size_t size, OpenFlags flags,
            bool auto_unlink = false);

  /**
   * @brief 析构 ShmHandle 对象
   *
   * 析构函数中将共享内存归还，并根据 @ref ShmHandle::auto_unlink_
   * "ShmHandle::auto_unlink_" 标记决定是否unlink掉这块共享内存
   */
  ~ShmHandle();

  /**
   * @brief 获取 ShmHandle 对象底层的文件描述符
   *
   * @return int 文件描述符
   */
  inline int GetFd() const { return fd_; }

  /**
   * @brief 获取是否自动删除标记
   *
   * @return true   析构时自动删除共享内存文件
   * @return false  析构时不会删除共享内存文件
   */
  inline bool IsAutoUnlink() const { return auto_unlink_; }

  /**
   * @brief 获取共享内存大小
   *
   * @return size_t 共享内存大小
   */
  inline size_t GetSize() const { return size_; }

  /**
   * @brief
   * 检查该共享内存对象是否能够使用，本质是在检查是否得到有效的文件描述符并且共享内存的地址也是有效的
   *
   * @return true 可以使用
   * @return false 不可以使用
   */
  inline bool IsValid() const { return fd_ != -1 && ptr_ != nullptr; }

  /**
   * @brief 返回共享内存地址
   *
   * @return void* 共享内存地址
   */
  inline void *Ptr() const { return ptr_; }

private:
  /**
   * @brief 拼接共享内存文件的名字
   * @param name 名字
   *
   * @return 拼接后的名字
   */
  static std::string ConcatShmRealname(const std::string& name);

private:
  int fd_ = -1; /**< ShmHandle 底层的文件描述符，由操作系统提供。 */
  size_t size_; /**< 共享内存空间的大小，单位（字节）。 */
  bool auto_unlink_; /**< 析构的时候是否同时 shm_unlink 删除这块共享内存。 */
  void *ptr_ = nullptr; /**< 共享内存的地址位置。 */
};

} // namespace shmlite
