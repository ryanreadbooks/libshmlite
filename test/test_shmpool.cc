#include <gtest/gtest.h>

#include "libshmlite/shm_handle.h"
#include "libshmlite/shm_pool.hpp"

// get int
TEST(ShmPoolTest, FuncTest_Get_type_int) {
  int *a = GET_INT_DEFAULT("a", 100);
  int *b = GET_INT_DEFAULT("a", 200);  // 创建获取同一个变量值
  ASSERT_EQ(*a, 100);
  ASSERT_EQ(*b, 100);
  *a = 300;
  ASSERT_EQ(*a, 300);
  ASSERT_EQ(*b, 300);
  *b = 100000;
  ASSERT_EQ(*a, *b);
  // 不指定默认值
  int *ac = GET_INT("a");
  ASSERT_EQ(*ac, 100000);
  shmlite::ShmHandle::UnLink("b");
  int *bc = GET_INT("b");  // 没有的共享内存变量，没有初始值
  *bc = 0x964510;
}

// get float浮点
TEST(ShmPoolTest, FuncTest_Get_type_float) {
  {
    float *f1 = GET_FLOAT_DEFAULT("f1", 1.5);
    float *f2 = GET_FLOAT_DEFAULT("f2", 2.6);  // 创建获取同一个变量值
    ASSERT_FLOAT_EQ(*f1, 1.5);
    ASSERT_FLOAT_EQ(*f2, 2.6);
    *f1 = 365.785;
    ASSERT_FLOAT_EQ(*f1, 365.785);
    *f2 = 856.36;
    ASSERT_FLOAT_EQ(*f2, 856.36);
  }
  {
    float *f1 = GET_FLOAT("f1");
    ASSERT_FLOAT_EQ(*f1, 365.785);
    float *f2 = GET_FLOAT("f2");
    ASSERT_FLOAT_EQ(*f2, 856.36);
  }
}

// get double
TEST(ShmPoolTest, FuncTest_Get_type_double) {
  {
    double *d1 = GET_DOUBLE_DEFAULT("d1", 1.5);
    double *d2 = GET_DOUBLE_DEFAULT("d2", 2.6);  // 创建获取同一个变量值
    ASSERT_DOUBLE_EQ(*d1, 1.5);
    ASSERT_DOUBLE_EQ(*d2, 2.6);
    *d1 = 365.785;
    ASSERT_DOUBLE_EQ(*d1, 365.785);
    *d2 = 856.36;
    ASSERT_DOUBLE_EQ(*d2, 856.36);
  }
  {
    double *d1 = GET_DOUBLE("d1");
    ASSERT_DOUBLE_EQ(*d1, 365.785);
    double *d2 = GET_DOUBLE("d2");
    ASSERT_DOUBLE_EQ(*d2, 856.36);
  }
}

// get char
TEST(ShmPoolTest, FuncTest_Get_type_char) {
  {
    char *c1 = GET_CHAR_DEFAULT("c1", 'a');
    char *c2 = GET_CHAR_DEFAULT("c2", 'c');  // 创建获取同一个变量值
    ASSERT_EQ(*c1, 'a');
    ASSERT_EQ(*c2, 'c');
    *c1 = 't';
    ASSERT_EQ(*c1, 't');
    *c2 = 'p';
    ASSERT_EQ(*c2, 'p');
  }
  {
    char *c1 = GET_CHAR("c1");
    ASSERT_EQ(*c1, 't');
    char *c2 = GET_CHAR("c2");
    ASSERT_EQ(*c2, 'p');
  }
}

// 测试获取的类型和已经有的类型不一样时的行为
TEST(ShmPoolTest, FuncTest_Get_type_not_match) {
  // 小类型用大类型去接收　ok
  int *c1 = GET_INT("c1");
  ASSERT_EQ(*c1, 't');
  double *d1 = GET_DOUBLE("d1");
  ASSERT_DOUBLE_EQ(*d1, 365.785);
  // 本来是float，用int来接收
  int *int_f1 = GET_INT("f1");  // f1 = 365.785 -> 0x43b6e47b
  std::cout << std::hex << std::showbase << "int_f1 = " << *int_f1 << '\n';
  float f1_value = 365.785;
  ASSERT_EQ(*int_f1, *reinterpret_cast<int *>(&f1_value));  // 将float类型的内存位置强行用int去解析

  // 本来是double，用int接收
  int *int_d2 = GET_INT("d2");  // d2 = 856.36
  double d2_value = 856.36;
  std::cout << std::hex << std::ios::showbase << "int_d2 = " << *int_d2 << '\n';
  ASSERT_EQ(*int_d2,
            *reinterpret_cast<int *>(
                &d2_value));  // 将double型的内存位置强行用int去解析,会造成截断，仅保留了低位

  long *long_d2 = GET_LONG("d2");  // d2 = 856.36
  std::cout << std::hex << std::ios::showbase << "long_d2 = " << *long_d2 << '\n';
  ASSERT_EQ(*long_d2,
            *reinterpret_cast<long *>(&d2_value));  // 将double型的内存位置强行用long去解析
  std::cout.unsetf(std::ios::showbase);
  std::cout.unsetf(std::ios::hex);
}

int main(int argc, char *argv[]) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}