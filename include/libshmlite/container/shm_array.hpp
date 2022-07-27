#pragma once

#include <cstring>
#include <memory>
#include <type_traits>

#include "../shm_handle.h"

namespace shmlite {

constexpr size_t kSizeLen = sizeof(size_t);

/**
template<typename T>
void FillArray(T* first, const T& value, size_t n) {
  for (size_t pos = 0; pos < n; ++pos) {
    *(first + pos) = value;
  }
}

template<>
void FillArray<char>(char* first, const char& value, size_t n) {
  const char tmp = value;
  memset(first, tmp, n);
}
 */

/**
 * @brief 以特定内容填充整个char数组，底层使用memset。
 *
 * @param first 开始填充的首地址
 * @param value 填充的内容
 * @param n 数量
 */
template <typename T>
auto FillArray(T *first, const T &value, size_t n) ->
    typename std::enable_if<std::is_same<T, char>::value, char *>::type {
  const char tmp = value;
  memset(first, tmp, n);
  return first + n;
}

/**
 * @brief 以特定内容填充整个数组
 *
 * @tparam T 数组的参数类型
 * @param first 开始填充的首地址
 * @param value 填充的内容
 * @param n 数量
 */
template <typename T>
auto FillArray(T *first, const T &value, size_t n) ->
    typename std::enable_if<!std::is_same<T, char>::value, T *>::type {
  for (size_t pos = 0; pos < n; ++pos) {
    *(first + pos) = value;
  }
  return first + n;
}

/**
 * @brief 检测 ShmArray 是否有效
 *
 */
#define SHMARRAY_CHECK_VALID()                                            \
  do {                                                                    \
    if (!handle_->IsValid()) {                                            \
      std::string msg = "ShmArray '" + handle_->GetName() + "' invalid."; \
      throw std::runtime_error(msg);                                      \
    }                                                                     \
  } while (0)

/**
 * @brief 检查 ShmArray 的索引是否越界
 *
 * @param pos 索引
 */
#define SHMARRAY_CHECK_POS(pos)                                                        \
  do {                                                                                 \
    if (pos >= size_) {                                                                \
      std::string msg = "index " + std::to_string(pos) + " is out of range (size = " + \
                        std::to_string(size_) + ")";                                   \
      throw std::out_of_range(msg);                                                    \
    }                                                                                  \
    SHMARRAY_CHECK_VALID();                                                            \
  } while (0)

#define SHMARRAY_CAST_PTR(Type) \
  reinterpret_cast<Type *>(reinterpret_cast<uintptr_t>(handle_->Ptr()) + sizeof(size_t))

/**
 * @brief 共享内存数组
 *
 * @tparam T 数组存放的数据类型
 */
template <typename T>
class ShmArray {
 public:
  /**
   * @brief 构造一个 ShmArray 对象
   *
   * @param name 数组对象名字
   * @param size 数组大小
   */
  ShmArray(const std::string &name, size_t size) : size_(size) {
    size_t alloc_size = kSizeLen + sizeof(T) * size;
    handle_ = std::make_shared<ShmHandle>(name, alloc_size, ShmHandle::CREAT_RDWR);
#ifdef DEV_DEBUG
    SIMPLE_DEBUG("ShmArray [" << name << "] alloc_size = " << alloc_size << ", size = " << size);
#endif
    /* 多分配了sizeof(size_t)的空间来存放该定长数组的大小 */
    if (handle_->IsValid()) {
      size_t *ptr = reinterpret_cast<size_t *>(handle_->Ptr());
      *ptr = size;
    } else {
      SIMPLE_ERROR("Can not allocate shm array of desired size " << size);
      size_ = 0;
    }
  }

  /**
   * @brief 构造 ShmArray 对象，并使用默认值填充数组
   *
   * @param name
   * @param size
   * @param value
   */
  ShmArray(const std::string &name, size_t size, const T &value) : size_(size) {
    size_t alloc_size = kSizeLen + sizeof(T) * size;
    handle_ = std::make_shared<ShmHandle>(name, alloc_size, ShmHandle::CREAT_RDWR);
#ifdef DEV_DEBUG
    SIMPLE_DEBUG("ShmArray [" << name << "] alloc_size = " << alloc_size << ", size = " << size);
#endif
    if (handle_->IsValid()) {
      size_t *ptr = reinterpret_cast<size_t *>(handle_->Ptr());
      *ptr = size;
      Fill(value);
    } else {
      SIMPLE_ERROR("Can not allocate shm array of desired size " << size);
      size_ = 0;
    }
  }

  ShmArray(const ShmArray &other) = delete;

  ~ShmArray() = default;

  ShmArray &operator=(const ShmArray &other) = delete;

  /**
   * @brief 按照数组索引访问元素
   *
   * @param pos 索引位置
   * @return T& 数组中的元素
   */
  T &operator[](size_t pos) {
    SHMARRAY_CHECK_POS(pos);
    T *start = SHMARRAY_CAST_PTR(T);
    return *(start + pos);
  }

  /**
   * @brief 按照数组索引访问元素
   *
   * @param pos 索引位置
   * @return const T& 数组中的元素
   */
  const T &operator[](size_t pos) const {
    SHMARRAY_CHECK_POS(pos);
    const T *start = SHMARRAY_CAST_PTR(const T);
    return *(start + pos);
  }

  /**
   * @brief 按照数组索引访问元素
   *
   * @param pos 索引位置
   * @return T& 数组中的元素
   */
  T &At(size_t pos) { return this->operator[](pos); }

  /**
   * @brief 按照数组索引访问元素
   *
   * @param pos 索引位置
   * @return const T& 数组中的元素
   */
  const T &At(size_t pos) const { return this->operator[](pos); }

  /**
   * @brief 以特定的内容填充整个数组空间
   *
   * @param value 填充的内容
   */
  void Fill(const T &value) {
    SHMARRAY_CHECK_VALID();
    /* 计算出数组开始的首地址，并且将指针类型转换为对应的类型　*/
    T *start = SHMARRAY_CAST_PTR(T);
    FillArray<T>(start, value, size_);
  }

  /**
   * @brief 获取数组的元素容量大小
   *
   * @return size_t 数组容量大小
   */
  size_t Size() const { return size_; };

  /**
   * @brief 检测共享内存数组是否有效
   *
   * @return true 有效
   * @return false 无效
   */
  bool IsValid() const { return handle_->IsValid(); }

 private:
  size_t size_;                       /**< 数组的大小 */
  std::shared_ptr<ShmHandle> handle_; /**< 底层的 ShmHandle 对象指针 */
};

}  // namespace shmlite
