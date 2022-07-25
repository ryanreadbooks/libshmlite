#pragma once

#include <fcntl.h>
#include <semaphore.h>

#include "common_utils.h"

namespace shmlite {

#define SHMLOCK_NAME_MAX 251 /**< 信号量的名字最大长度 */
constexpr const char *kShmLockNamePrefix =
    "/lsmll-"; /**< 命名信号量名字的前缀，必须以/开头。libshmlitelock 缩写为 lsmll */

/**
 * @brief 对POSIX下的命名信号量的封装
 *
 * 封装POSIX下的命名信号量的基础操作
 */
class ShmLock : public NamedClass {
public:
  /**
   * @brief 删除指定的信号量文件
   *
   * @param name 信号量文件
   * @return true 删除成功
   * @return false 删除失败
   */
  static bool UnLink(const std::string &name);

  LIBSHMLITE_NO_COPYABLE(ShmLock)

  /**
   * @brief 构造函数
   *
   * @param name 命名信号量的名字
   * @param value 信号量的初始值
   * @param auto_unlink 析构时是否自动删除信号量文件
   */
  explicit ShmLock(std::string name, unsigned int value = 1,
                   bool auto_unlink = false);

  /**
   * @brief 析构函数
   *
   */
  ~ShmLock();

  /**
   * @brief 信号量的Post操作，表示释放资源，相当于解锁
   *
   */
  void Post();

  /**
   * @brief 信号量的Wait操作，表示获取资源，相当于上锁
   *
   */
  void Wait();

  /**
   * @brief 获取信号量当前的值
   *
   * @return 信号量当前的值
   */
  int GetValue() const;

  /**
   * @brief 检查信号量是否成功初始化
   *
   * @return true 已经初始化
   * @return false 未初始化
   */
  inline bool IsValid() const { return sem_ptr_ != nullptr; }

  /**
   * @brief 获取底层的sem_t指针
   *
   * @return sem_t指针
   */
  inline sem_t *SemPtr() const { return sem_ptr_; }

private:
  sem_t *sem_ptr_ = nullptr; /**< 信号量描述符 */
  bool auto_unlink_;         /**< 析构时是否自动删除信号量文件 */
};

} // namespace shmlite