#include <gtest/gtest.h>
#include <kg/crypto/k3xsx_salt.hpp>
TEST(k3xsx_salt_t_test, HandleNoneZeroInput)
{
	kg::crypto::k3xsx_salt_t k3(2);

	char buf[1024] = {0};
	char dec[1024] = {0};

	std::string str = "this is 測試 text";
	std::size_t n = str.size();
	k3.encryption((const kg::byte_t*)str.data(),n,(kg::byte_t*)buf);

	k3.decryption((const kg::byte_t*)buf,n+3,(kg::byte_t*)dec);

	std::string dStr(dec,n);

	EXPECT_EQ(str,dStr);
}
int main(int argc, char* argv[])
{
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
