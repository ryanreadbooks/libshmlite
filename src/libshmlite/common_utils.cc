#include "libshmlite/common_utils.h"

namespace shmlite {

std::string ConcatStringLimited(const std::string& prefix, const std::string& suffix, size_t max_len) {
  std::string res = prefix + suffix;
  /* 如果超过最大长度则截断 */
  if (res.size() > max_len) {
    return res.substr(0, max_len);
  }
  return res;
}

std::string ConcatStringLimited(const char* prefix, const std::string& suffix, size_t max_len) {
  return ConcatStringLimited(std::string(prefix), suffix, max_len);
}

} // namespace shmlite
