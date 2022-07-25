#pragma once

#include <cerrno>
#include <cstring>
#include <iostream>
#include <utility>

/**
 * @brief 打印带有errno的错误信息
 * 
 * @param msg 需要打印的消息
 * @param errnum 错误码
 */
#define PRINT_ERRMSG(msg, errnum)                                              \
  do {                                                                         \
    std::cerr << msg << " [errno " << errnum << ": " << strerror(errnum)       \
              << "]\n";                                                        \
  } while (0)

/**
 * @brief 处理错误
 * 
 * @param retval 返回值
 * @param msg 需要打印的信息
 */
#define HANDLE_ERR(retval, msg)                                                \
  do {                                                                         \
    if (retval == -1) {                                                        \
      PRINT_ERRMSG(msg, errno);                                                \
    }                                                                          \
  } while (0)

/**
 * @brief 禁止类拷贝的宏
 *
 * @param ClassName 类名
 */
#define LIBSHMLITE_NO_COPYABLE(ClassName)                                      \
  ClassName(const ClassName &) = delete;                                       \
  ClassName &operator=(const ClassName &) = delete;

namespace shmlite {

/**
 * @brief 公共类，带有名字属性的类的父类
 *
 * 带有名字属性的类的父类
 */
class NamedClass {
public:
  /**
   * @brief 构造函数
   *
   * @param name 名字
   */
  explicit NamedClass(std::string name) : name_(std::move(name)) {}

  /**
   * @brief 获取名字属性值
   *
   * @return 名字
   */
  inline std::string GetName() const { return name_; }

  /**
   * @brief 获取名字属性的值
   *
   * @return 以C字符串格式返回名字
   */
  inline const char* GetNameCstr() const { return name_.c_str(); }

protected:
  const std::string name_;
};

/**
 * @brief 将两个字符串拼接
 *
 * @param prefix 第一个字符串
 * @param suffix 第二个字符串
 * @param max_len 拼接后的最大允许长度
 * @return
 */
std::string ConcatStringLimited(const std::string& prefix, const std::string& suffix, size_t max_len);

/**
 * @brief 将两个字符串拼接
 *
 * @param prefix 第一个字符串
 * @param suffix 第二个字符串
 * @param max_len 拼接后的最大允许长度
 * @return
 */
std::string ConcatStringLimited(const char* prefix, const std::string& suffix, size_t max_len);


} // namespace shmlite