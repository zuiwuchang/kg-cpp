#include <gtest/gtest.h>
#include <kg/scripts/duktape.hpp>

TEST(CreateCopyTest, HandleNoneZeroInput)
{
	kg::scripts::duktape_t duk0;
	kg::scripts::duktape_t duk1 = duk0;
	kg::scripts::duktape_t duk2(duk0);

	EXPECT_EQ(duk0.get(),duk1.get());
	EXPECT_EQ(duk0.get(),duk2);
}

TEST(StackTest, HandleNoneZeroInput)
{
	kg::scripts::duktape_t duk;

	EXPECT_EQ(duk.get_top_index(),DUK_INVALID_INDEX);

	duk_idx_t i = 0;

	duk.push(1);
	EXPECT_EQ(duk.to_int(i),1);
	EXPECT_EQ(duk.to_int32(i),1);
	EXPECT_TRUE(duk.is_number(i));
	EXPECT_EQ(duk.get_top_index(),i);
	EXPECT_EQ(duk.get_top(),++i);

	duk.push(1.2);
	EXPECT_EQ(duk.to_number(i),1.2);
	EXPECT_TRUE(duk.is_number(i));
	EXPECT_EQ(duk.get_top_index(),i);
	EXPECT_EQ(duk.get_top(),++i);

	duk.push_bool(false);
	EXPECT_EQ(duk.to_bool(i),false);
	EXPECT_TRUE(duk.is_bool(i));
	EXPECT_EQ(duk.get_top_index(),i);
	EXPECT_EQ(duk.get_top(),++i);


	duk.push("");
	EXPECT_EQ(duk.to_string(i),"");
	EXPECT_EQ(duk.safe_to_string(i),"");
	EXPECT_TRUE(duk.is_string(i));
	EXPECT_EQ(duk.get_top_index(),i);
	EXPECT_EQ(duk.get_top(),++i);

	duk.push("abc");
	EXPECT_EQ(duk.to_string(i),"abc");
	EXPECT_EQ(duk.safe_to_string(i),"abc");
	EXPECT_TRUE(duk.is_string(i));
	EXPECT_EQ(duk.get_top_index(),i);
	EXPECT_EQ(duk.get_top(),++i);

	duk.push_cstring(NULL);
	EXPECT_TRUE(duk.is_null(i));
	EXPECT_TRUE(duk.is_null_or_undefined(i));
	EXPECT_EQ(duk.get_top_index(),i);
	EXPECT_EQ(duk.get_top(),++i);

	duk.push_nan();
	EXPECT_TRUE(duk.is_nan(i));
	EXPECT_EQ(duk.get_top_index(),i);
	EXPECT_EQ(duk.get_top(),++i);

	duk.push_null();
	EXPECT_TRUE(duk.is_null(i));
	EXPECT_TRUE(duk.is_null_or_undefined(i));
	EXPECT_EQ(duk.get_top_index(),i);
	EXPECT_EQ(duk.get_top(),++i);

	duk.push_undefined();
	EXPECT_TRUE(duk.is_undefined(i));
	EXPECT_TRUE(duk.is_null_or_undefined(i));
	EXPECT_EQ(duk.get_top_index(),i);
	EXPECT_EQ(duk.get_top(),++i);

	duk.push_array();
	EXPECT_TRUE(duk.is_array(i));
	EXPECT_TRUE(duk.is_object(i));
	EXPECT_EQ(duk.get_top_index(),i);
	EXPECT_EQ(duk.get_top(),++i);

	duk.push_object();
	EXPECT_FALSE(duk.is_array(i));
	EXPECT_TRUE(duk.is_object(i));
	EXPECT_EQ(duk.get_top_index(),i);
	EXPECT_EQ(duk.get_top(),++i);

	duk.push_c_function([](duk_context *ctx){
		return 0;
	},0);
	EXPECT_TRUE(duk.is_c_function(i));
	EXPECT_TRUE(duk.is_object(i));
	EXPECT_EQ(duk.get_top_index(),i);
	EXPECT_EQ(duk.get_top(),++i);


	//duk.dump_context_stdout();
}
int main(int argc, char* argv[])
{
    testing::InitGoogleTest(&argc, argv);

	return RUN_ALL_TESTS();
}
