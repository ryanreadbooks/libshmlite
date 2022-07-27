#include <gtest/gtest.h>
#include "libshmlite/shm_handle.h"

struct Foo {
  int a = 0;
  float b = 1.0;
  char c = 'q';
  char buf[16];
};

class ShmHandleTest : public ::testing::Test {
 public:
  shmlite::ShmHandle *pshm3;

 protected:
  void SetUp() override {
    shmlite::ShmHandle shm1("shm1", sizeof(int), shmlite::ShmHandle::CREAT_RDWR);
    shmlite::ShmHandle shm2("shm2", sizeof(double), shmlite::ShmHandle::CREAT_RDWR, true);
    pshm3 = new shmlite::ShmHandle("shm3", 64, shmlite::ShmHandle::CREAT_RDWR, true);
  }

  void TearDown() override { delete pshm3; }
};

TEST_F(ShmHandleTest, BaseTest) {
  EXPECT_NE(pshm3, nullptr);
  EXPECT_EQ(pshm3->GetName(), std::string("shm3"));
  EXPECT_GT(pshm3->GetFd(), -1);
  EXPECT_EQ(pshm3->GetSize(), 64);
  EXPECT_TRUE(pshm3->IsValid());
  EXPECT_TRUE(pshm3->IsAutoUnlink());
  EXPECT_NE(pshm3->Ptr(), nullptr);

  const char *msg = "HelloShmLite!!";
  {
    // 打开一个共享内存
    shmlite::ShmHandle shm4("shm4", sizeof(Foo), shmlite::ShmHandle::CREAT_RDWR, false);
    EXPECT_EQ(shm4.GetSize(), sizeof(Foo));
    //　然后往里面写内容
    Foo *pf = (Foo *)shm4.Ptr();
    pf->a = 100;
    pf->b = 6.69;
    pf->c = 'w';
    memcpy(pf->buf, msg, strlen(msg));
    //　自动析构后，会关闭共享内存
  }
  //　再次由同一个名称构造出共享内存对象
  shmlite::ShmHandle shm4("shm4", sizeof(Foo), shmlite::ShmHandle::CREAT_RDWR, true);
  // 验证现在的值是否和前面设置的一致
  EXPECT_EQ(((Foo *)shm4.Ptr())->a, 100);
  EXPECT_FLOAT_EQ(((Foo *)shm4.Ptr())->b, 6.69);
  EXPECT_EQ(((Foo *)shm4.Ptr())->c, 'w');
  EXPECT_EQ(std::string(((Foo *)shm4.Ptr())->buf), std::string(msg));
};

TEST(ShmHandleUtilityTest, StaticMethodTest) {
  EXPECT_TRUE(shmlite::ShmHandle::CheckExists("shm1"));
  EXPECT_FALSE(shmlite::ShmHandle::CheckExists("shm2"));
  EXPECT_FALSE(shmlite::ShmHandle::CheckExists("shm3"));
  EXPECT_FALSE(shmlite::ShmHandle::CheckExists("shm85"));
}

int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
