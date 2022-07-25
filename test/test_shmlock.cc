#include <gtest/gtest.h>
#include "libshmlite/shm_handle.h"
#include "libshmlite/shm_lock.h"

TEST(SHMLock, BaseTest) {
  shmlite::ShmLock lk1("lk1", 2, false);
  ASSERT_EQ(lk1.GetName(), "lk1");
  ASSERT_EQ(lk1.GetValue(), 2);
  lk1.Wait();
  ASSERT_EQ(lk1.GetValue(), 1);
  lk1.Post();
  ASSERT_EQ(lk1.GetValue(), 2);
}

TEST(SHMLock, BaseTest2) {
  shmlite::ShmLock lk2("lk2", 1, true);
  ASSERT_FALSE(shmlite::ShmLock::UnLink("lk3"));
  {
    shmlite::ShmLock lk3("lk3", 1, false);
  }
  ASSERT_TRUE(shmlite::ShmLock::UnLink("lk3"));
}

int main(int argc, char** argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
