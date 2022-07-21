#pragma once

#include <fcntl.h>
#include <semaphore.h>

#include "common_utils.h"

namespace shmlite {

/**
 * @brief 对POSIX下的命名信号量的封装
 *
 * 封装POSIX下的命名信号量的基础操作
 */
class ShmLock : public NamedClass {
public:
  LIBSHMLITE_NO_COPYABLE(ShmLock);

  /**
   * @brief 构造函数
   *
   * @param name 命名信号量的名字
   */
  explicit ShmLock(std::string  name);

  ~ShmLock();

private:
  sem_t sem_fd_;  /**< 信号量描述符 */
};

} // namespace shmlite