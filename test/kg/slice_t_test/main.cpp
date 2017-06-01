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
typedef kg::slice_t<std::size_t> slice_t;
TEST(slice_t_append_Test, HandleNoneZeroInput)
{
	//0
	slice_t s;
	EXPECT_EQ(s.size(),0);
	EXPECT_EQ(s.capacity(),0);

	//1
	slice_t s1 = s.append(1);
	EXPECT_EQ(s.size(),0);
	EXPECT_EQ(s.capacity(),0);
	EXPECT_EQ(s1.size(),1);
	EXPECT_EQ(s1.capacity(),1);
	EXPECT_EQ(s1[0],1);

	//2
	slice_t s2 = s1.append(2);
	EXPECT_EQ(s1.size(),1);
	EXPECT_EQ(s1.capacity(),1);
	EXPECT_EQ(s1[0],1);

	EXPECT_EQ(s2.size(),2);
	EXPECT_EQ(s2.capacity(),2);
	EXPECT_EQ(s2[0],1);
	EXPECT_EQ(s2[1],2);

	//4
	slice_t s3 = s2.append(3);
	EXPECT_EQ(s3.size(),3);
	EXPECT_EQ(s3.capacity(),4);
	for(std::size_t i=0;i<s3.size();++i)
	{
		EXPECT_EQ(s3[i],i+1);
	}

	slice_t s3_1 = s3.append(4);
	EXPECT_EQ(s3.size(),3);
	EXPECT_EQ(s3.capacity(),4);
	for(std::size_t i=0;i<s3.size();++i)
	{
		EXPECT_EQ(s3[i],i+1);
	}

	EXPECT_EQ(s3_1.size(),4);
	EXPECT_EQ(s3_1.capacity(),4);
	for(std::size_t i=0;i<s3_1.size();++i)
	{
		EXPECT_EQ(s3_1[i],i+1);
	}
	EXPECT_EQ(s3.size(),3);
	EXPECT_EQ(s3.capacity(),4);
	for(std::size_t i=0;i<s3.size();++i)
	{
		EXPECT_EQ(s3[i],i+1);
	}

	//8
	slice_t s4 = s3_1.append(5);
	EXPECT_EQ(s4.size(),5);
	EXPECT_EQ(s4.capacity(),8);
	for(std::size_t i=0;i<s4.size();++i)
	{
		EXPECT_EQ(s4[i],i+1);
	}
}
int main(int argc, char* argv[])
{

	testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}
