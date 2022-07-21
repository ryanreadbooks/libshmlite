#include <cerrno>
#include <cstring>
#include <iostream>
#include <utility>

/**
 * @brief 打印带有errno的错误信息
 *
 */
#define PRINT_ERRMSG(msg, errnum)                                              \
  do {                                                                         \
    std::cout << msg << " [errno " << errnum << ": " << strerror(errnum)       \
              << "]\n";                                                        \
  } while (0)

#define HANDLE_ERR(retval, msg)                                                \
  do {                                                                         \
    if (retval == -1) {                                                        \
      PRINT_ERRMSG(msg, errno);                                                \
    }                                                                          \
  } while (0)

/**
 * @brief 禁止类拷贝的宏
 *
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

} // namespace shmlite