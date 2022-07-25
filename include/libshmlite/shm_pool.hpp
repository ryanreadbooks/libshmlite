#pragma once

#include "common_utils.h"
#include "shm_handle.h"
#include <memory>
#include <unordered_map>

namespace shmlite {
using ShmHandleMap =
    std::unordered_map<std::string, std::shared_ptr<ShmHandle>>;

/**
 * @brief 共享内存变量池，提供统一的访问接口
 * 
 */
class ShmPool {
public:
  /**
   * @brief 从共享内存中获取基础类型变量，不指定初始值
   *
   * @tparam T    基础类型
   * @param name  共享内存变量名称
   * @return T*   共享内存的指针
   */
  template <typename T> static T *Get(const std::string &name) {
    if (s_pool_.find(name) == s_pool_.end()) {
      auto handle_ptr = std::make_shared<ShmHandle>(
          name, sizeof(T), ShmHandle::CREAT_RDWR, false);
      if (!handle_ptr->IsValid()) {
        return nullptr;
      }
      T *val_ptr = static_cast<T *>(handle_ptr->Ptr());
      s_pool_[name] = handle_ptr;
      return val_ptr;
    }
    return static_cast<T *>(s_pool_[name]->Ptr());
  }

  /**
   * @brief 从共享内存中获取基础类型变量
   * @tparam              T 基础类型
   * @param name          共享内存变量名称
   * @param default_value 指定该变量的初始值
   * @return              共享内存的指针
   */
  template <typename T>
  static T *Get(const std::string &name, T default_value) {
    T *val_ptr = nullptr;
    // TODO 需要处理key存在和key不存在两种情况
    if (s_pool_.find(name) == s_pool_.end()) { /* 先前还没有获取过这个变量 */
      /* 去共享内存拿 */
      auto handle_ptr = std::make_shared<ShmHandle>(name, &default_value, sizeof(T), false);
      if (!handle_ptr->IsValid()) { /* 创建失败直接返回nullptr */
        return nullptr;
      }
      val_ptr = static_cast<T *>(handle_ptr->Ptr());
      s_pool_[name] = handle_ptr;
      return val_ptr;
    }
    return static_cast<T *>(s_pool_[name]->Ptr());
  }

  LIBSHMLITE_NO_COPYABLE(ShmPool)

private:
  /**
   * @brief 私有构造函数，不对外提供构造此对象的方法
   * 
   */
  ShmPool() {}

private:
  static ShmHandleMap s_pool_; /**< 存放所有当前程序的共享内存ShmHandle对象键值对 */
};

ShmHandleMap ShmPool::s_pool_ = {};

} // namespace shmlite

/**
 * @brief 获取基础类型
 * 
 * 
 * @param type 类型
 * @param name 名字
 */
#define GET(type, name) shmlite::ShmPool::Get<type>(name)

/**
 * @brief 获取基础类型，并且设置初始值
 * 
 * @param type 类型
 * @param name 名字
 * @param default_value 初始值
 * 
 */
#define GET_DEFAULT(type, name, default_value) shmlite::ShmPool::Get<type>(name, default_value)

/**
 * @brief 获取int类型
 * 
 * @param name 名字
 */
#define GET_INT(name) GET(int, name)

/**
 * @brief 获取int类型，并且指定初始值
 * 
 * @param name 名字
 * @param default_value 初始值
 */
#define GET_INT_DEFAULT(name, default_value) GET_DEFAULT(int, name, default_value)

/**
 * @brief 获取long类型
 *
 * @param name 名字
 */
#define GET_LONG(name) GET(long, name)

/**
 * @brief 获取long类型，并且指定初始值
 *
 * @param name 名字
 * @param default_value 初始值
 */
#define GET_LONG_DEFAULT(name, default_value) GET_DEFAULT(long, name, default_value)

/**
 * @brief 获取char类型
 *
 * @param name 名字
 */
#define GET_CHAR(name) GET(char, name)

/**
 * @brief 获取char类型，并且指定初始值
 *
 * @param name 名字
 * @param default_value 初始值
 */
#define GET_CHAR_DEFAULT(name, default_value) GET_DEFAULT(char, name, default_value)

/**
 * @brief 获取float类型
 *
 * @param name 名字
 */
#define GET_FLOAT(name) GET(float, name)

/**
 * @brief 获取float类型，并且指定初始值
 *
 * @param name 名字
 * @param default_value 初始值
 */
#define GET_FLOAT_DEFAULT(name, default_value) GET_DEFAULT(float, name, default_value)

/**
 * @brief 获取double类型
 *
 * @param name 名字
 */
#define GET_DOUBLE(name) GET(double, name)

/**
 * @brief 获取double类型，并且指定初始值
 *
 * @param name 名字
 * @param default_value 初始值
 */
#define GET_DOUBLE_DEFAULT(name, default_value) GET_DEFAULT(double, name, default_value)
