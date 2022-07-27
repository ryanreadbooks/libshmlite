#include <gtest/gtest.h>
#include "libshmlite/container/shm_array.hpp"
#include "libshmlite/shm_handle.h"

struct Foo {
  int a;
  char b;
  double c;
};

#pragma pack(1)
struct Bar {
  int a;
  char b;
  double c;
};

TEST(ShmArrayTest, BasicTest) {
  shmlite::ShmArray<char> arr1("arr1", 10);  // 8 + 10 = 18
  shmlite::ShmArray<int> arr2("arr2", 16);   // 8 + 16 * 4 = 72
  shmlite::ShmArray<Foo> arr3("arr3", 5);  // 8 + 5 * (4+1+8) -> 8 + 5 * 16 = 88 (结构体自动对齐)
  shmlite::ShmArray<Bar> arr4("arr4", 5);  // 8 + 5 * (4+1+8) = 73 (一字节对齐)
}

TEST(ShmArrayTest, BasicTestFillCstr) {
  { shmlite::ShmArray<char> arr5("arr5", 10, 'w'); }
  shmlite::ShmArray<char> arr5(
      "arr5", 5,
      'z');  // 请求的大小比已经存在的要小，则原来已有的大小会被truncate掉，会导致原来的内容丢失
}

TEST(ShmArrayTest, Operator_BracketTest) {
  shmlite::ShmArray<char> arr1("arr1", 10);  // 8 + 10 = 18
  arr1[0] = 'a';                             // 放一个a在arr1[0]里面的位置
  ASSERT_EQ(arr1[0], 'a');
  for (int i = 0; i < 10; i++) {
    arr1[i] = 'a' + i;
  }
  arr1.Fill('z');
  ASSERT_EQ(arr1[5], 'z');
  ASSERT_EQ(arr1[4], 'z');
  ASSERT_EQ(arr1[2], 'z');
  ASSERT_EQ(arr1[9], 'z');
  try {
    arr1[10] = 96;  // 索引越界抛异常
  } catch (const std::out_of_range &ex) {
    std::cout << ex.what() << '\n';
  }
}

// 测试存放类的数组
TEST(ShmArrayTest, StructTest) {
  shmlite::ShmArray<Foo> arr3("arr3", 5);
  struct Foo f1 {
    .a = 0x12345678, .b = 'c', .c = 3.14159
  };
  arr3[0] = f1;

  ASSERT_EQ(arr3[0].a, 0x12345678);
  ASSERT_EQ(arr3[0].b, 'c');
  ASSERT_DOUBLE_EQ(arr3[0].c, 3.14159);
}

int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}