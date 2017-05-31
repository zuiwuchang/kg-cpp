#include <gtest/gtest.h>
#include <kg/slice.hpp>
typedef kg::slice_t<std::size_t> slice_t;
TEST(slice_t_range_Test, HandleNoneZeroInput)
{
	std::size_t size = 5;
	std::size_t capacity = 10;
	slice_t s(size,capacity);
	EXPECT_EQ(s.size(),size);
	EXPECT_EQ(s.capacity(),capacity);

	//測試引用
	slice_t quote = s;
	std::size_t x = 0;
	for(auto& v:quote)
	{
		v=x++;
	}
	EXPECT_EQ(x,size);
	x = 0;
	for(const auto& v:s)
	{
		EXPECT_EQ(x++,v);
	}
	EXPECT_EQ(quote.size(),size);
	EXPECT_EQ(quote.capacity(),capacity);

	EXPECT_EQ(s,quote);

	//測試子 切片
	slice_t c0 = s.range(0);
	EXPECT_TRUE(s==c0);

	c0 = s.range(1);
	EXPECT_NO_THROW(s.range(size));
	EXPECT_THROW(s.range(size+1),std::out_of_range);
	EXPECT_EQ(c0.size(),size-1);
	EXPECT_EQ(c0.capacity(),capacity-1);
	c0 = s.range(2);
	EXPECT_EQ(c0.size(),size-2);
	EXPECT_EQ(c0.capacity(),capacity-2);

	c0 = s.range(size,capacity);
	x = size;
	for(auto& v:c0)
	{
		v=x++;
	}
	slice_t c1 = s.range(0,capacity);
	EXPECT_FALSE(s==c1);
	EXPECT_TRUE(s!=c1);
	EXPECT_THROW(s.range(capacity+1,capacity),std::out_of_range);
	EXPECT_EQ(c1.size(),capacity);
	EXPECT_EQ(c1.capacity(),capacity);

	for(std::size_t i=0;i<capacity;++i)
	{
		EXPECT_EQ(c1[i],i);
	}

	c1 = s.range(0,capacity + 1);
	EXPECT_THROW(s.range(capacity+1,capacity),std::out_of_range);
	EXPECT_EQ(c1.size(),capacity);
	EXPECT_EQ(c1.capacity(),capacity);

	for(std::size_t i=0;i<capacity;++i)
	{
		EXPECT_EQ(c1[i],i);
	}
}

int main(int argc, char* argv[])
{
	testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}
