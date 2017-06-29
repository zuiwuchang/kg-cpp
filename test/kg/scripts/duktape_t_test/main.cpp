#include <gtest/gtest.h>
#include <kg/scripts/duktape.hpp>
#include <kg/scripts/duktape_extras.hpp>

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

TEST(ArrayTest, HandleNoneZeroInput)
{
	kg::scripts::duktape_t duk;
	duk_idx_t idx = duk.push_array();
	EXPECT_EQ(duk.get_length(idx),0);
	EXPECT_FALSE(duk.has_prop_index(idx,0));

	duk_size_t len = 10;
	for(duk_size_t i=0;i<len;++i)
	{
		//set value
		duk.push(i);
		//array.push
		duk.put_prop_index(idx,i);

	}
	//調整數組長度
	duk.set_length(idx,len);
	EXPECT_EQ(duk.get_length(idx),len);

	for(duk_size_t i=0;i<len;++i)
	{
		EXPECT_TRUE(duk.has_prop_index(idx,i));

		duk.get_prop_index(idx,i);
		EXPECT_EQ(duk.to_uint(-1),i);
		duk.pop();
	}
	EXPECT_TRUE(duk.is_array(idx));

	duk.push_enum(idx);
	int i = 0;
	while(duk.next(-1,true))
	{
		EXPECT_EQ(duk.to_int(-2),i);
		EXPECT_EQ(duk.to_int(-1),i);
		++i;
		duk.pop_2();
	}
	duk.pop();


	duk.pop();	//array
	//duk.dump_context_stdout();
}

TEST(ObjectTest, HandleNoneZeroInput)
{
	kg::scripts::duktape_t duk;
	duk_idx_t idx = duk.push_object();
	EXPECT_FALSE(duk.has_prop_string(idx,"name"));
	duk.push("king");
	duk.put_prop_string(idx,"name");
	EXPECT_TRUE(duk.has_prop_string(idx,"name"));

	duk.get_prop_string(idx,"name");
	EXPECT_TRUE(duk.is_string(-1));
	EXPECT_EQ(duk.to_string(-1),"king");
	duk.pop();

	duk.push_enum(idx);
	while(duk.next(-1,true))
	{
		EXPECT_EQ(duk.to_string(-2),"name");
		EXPECT_EQ(duk.to_string(-1),"king");
		duk.pop_2();
	}
	duk.pop();

	duk.pop();	//obj
	//duk.dump_context_stdout();
}

TEST(EvalTest, HandleNoneZeroInput)
{
	kg::scripts::duktape_t duk;

	EXPECT_TRUE(duk.peval("(function(x,y) { return x+y; })"));
	duk.push(1);
	duk.push(2);
	EXPECT_TRUE(duk.pcall(2));
	EXPECT_EQ(duk.to_int(-1),3);
	duk.pop();

	EXPECT_TRUE(duk.peval("(function(x,y) { return this+x+y; })"));
	duk.push(10); // this
	duk.push(1);
	duk.push(2);
	EXPECT_TRUE(duk.pcall_method(2));
	EXPECT_EQ(duk.to_int(-1),13);
	duk.pop();

	EXPECT_TRUE(duk.peval("({ Base:10,MyAdd:function(x,y) { return this.Base + x + y; }})"));
	duk.push("MyAdd"); //key
	duk.push(1);
	duk.push(2);
	EXPECT_TRUE(duk.pcall_prop(-4,2));
	EXPECT_EQ(duk.to_int(-1),13);
	duk.pop_2();

	EXPECT_EQ(duk.get_top(),0);
	//duk.dump_context_stdout();
}

TEST(GetTest, HandleNoneZeroInput)
{
	kg::scripts::duktape_t duk;
	{
		duk.push_nan();
		EXPECT_EQ(duk.get_int(-1,123),0);
		EXPECT_EQ(duk.get_int(-1),0);
		duk.pop();

		duk.push_null();
		EXPECT_EQ(duk.get_int(-1,123),123);
		EXPECT_EQ(duk.get_int(-1),0);
		duk.pop();

		duk.push_undefined();
		EXPECT_EQ(duk.get_int(-1,123),123);
		EXPECT_EQ(duk.get_int(-1),0);
		duk.pop();
	}

	{
		duk.push_nan();
		EXPECT_EQ(duk.get_uint(-1,123),0);
		EXPECT_EQ(duk.get_uint(-1),0);
		duk.pop();

		duk.push_null();
		EXPECT_EQ(duk.get_uint(-1,123),123);
		EXPECT_EQ(duk.get_uint(-1),0);
		duk.pop();

		duk.push_undefined();
		EXPECT_EQ(duk.get_uint(-1,123),123);
		EXPECT_EQ(duk.get_uint(-1),0);
		duk.pop();
	}

	{
		duk.push_nan();
		EXPECT_NE(duk.get_number(-1,123),duk.get_number(-1,123)); // nan == nan return false
		EXPECT_NE(duk.get_number(-1),duk.get_number(-1));
		duk.pop();

		duk.push_null();
		EXPECT_EQ(duk.get_number(-1,123),123);
		EXPECT_NE(duk.get_number(-1),duk.get_number(-1));
		duk.pop();

		duk.push_undefined();
		EXPECT_EQ(duk.get_number(-1,123),123);
		EXPECT_NE(duk.get_number(-1),duk.get_number(-1));
		duk.pop();
	}

	{
		duk.push_nan();
		EXPECT_FALSE(duk.get_string(-1));
		EXPECT_EQ(std::string("king"),duk.get_string(-1,"king"));
		duk.pop();

		duk.push_null();
		EXPECT_FALSE(duk.get_string(-1));
		EXPECT_EQ(std::string("king"),duk.get_string(-1,"king"));
		duk.pop();

		duk.push_undefined();
		EXPECT_FALSE(duk.get_string(-1));
		EXPECT_EQ(std::string("king"),duk.get_string(-1,"king"));
		duk.pop();
	}

	{
		duk_size_t len;
		const char* p = NULL;
		duk.push_nan();
		EXPECT_FALSE(duk.get_lstring(-1,NULL));
		EXPECT_FALSE(duk.get_lstring(-1,&len));
		p = duk.get_lstring(-1,&len,"ok",2);
		EXPECT_EQ(len,2);
		EXPECT_EQ(std::string(p,len),"ok");
		duk.pop();

		duk.push_null();
		EXPECT_FALSE(duk.get_lstring(-1,NULL));
		EXPECT_FALSE(duk.get_lstring(-1,&len));
		p = duk.get_lstring(-1,&len,"ok",2);
		EXPECT_EQ(len,2);
		EXPECT_EQ(std::string(p,len),"ok");
		duk.pop();

		duk.push_undefined();
		EXPECT_FALSE(duk.get_lstring(-1,NULL));
		EXPECT_FALSE(duk.get_lstring(-1,&len));
		p = duk.get_lstring(-1,&len,"ok",2);
		EXPECT_EQ(len,2);
		EXPECT_EQ(std::string(p,len),"ok");
		duk.pop();
	}

}
TEST(CPtrTest, HandleNoneZeroInput)
{
	kg::scripts::duktape_t duk;
	int x = 123;
	duk.push_pointer(&x);
	EXPECT_TRUE(duk.is_pointer(-1));
	int* p = (int*)duk.get_pointer(-1);
	EXPECT_EQ(p,&x);
	EXPECT_EQ(*p,123);
	*p = 456;
	EXPECT_EQ(x,456);
	duk.pop();


	duk.push(123);
	EXPECT_FALSE(duk.is_pointer(-1));
	EXPECT_FALSE(duk.to_pointer(-1));
}
TEST(DumpTest, HandleNoneZeroInput)
{
	kg::scripts::duktape_t duk;

	//object
	{
		duk_idx_t id = duk.push_object();
		duk.push("king");
		duk.put_prop_string(id,"name");
		duk.dup_top();
		EXPECT_EQ(2,duk.get_top());

		duk.push("kate");
		duk.put_prop_string(-2,"name");
		EXPECT_EQ(2,duk.get_top());

		duk.get_prop_string(id,"name");
		EXPECT_EQ(std::string("kate"),duk.get_string(-1));
		duk.pop();

		duk.get_prop_string(-2,"name");
		EXPECT_EQ(std::string("kate"),duk.get_string(-1));
		duk.pop();

		duk.pop_2();
	}

	//array
	{
		duk_idx_t id = duk.push_array();
		duk.push("king");
		duk.put_prop_index(id,0);
		duk.push("anita");
		duk.put_prop_index(id,1);
		duk.set_length(id,2);
		duk.dup_top();
		EXPECT_EQ(2,duk.get_top());

		{
			duk.push("kate");
			duk.put_prop_index(-2,0);
			EXPECT_EQ(duk.get_length(-1),2);

			duk.get_prop_index(id,0);
			EXPECT_EQ(std::string("kate"),duk.get_string(-1));
			duk.pop();

			duk.get_prop_index(id,1);
			EXPECT_EQ(std::string("anita"),duk.get_string(-1));
			duk.pop();

			duk.get_prop_index(-1,0);
			EXPECT_EQ(std::string("kate"),duk.get_string(-1));
			duk.pop();

			duk.get_prop_index(-1,1);
			EXPECT_EQ(std::string("anita"),duk.get_string(-1));
			duk.pop();
		}

		{
			duk.push("dark");
			duk.put_prop_index(id,2);
			duk.set_length(id,3);
			EXPECT_EQ(duk.get_length(-1),3);
			EXPECT_EQ(duk.get_length(id),3);

			duk.get_prop_index(-1,2);
			EXPECT_EQ(std::string("dark"),duk.get_string(-1));
			duk.pop();

			duk.get_prop_index(id,2);
			EXPECT_EQ(std::string("dark"),duk.get_string(-1));
			duk.pop();

			duk.pop_2();
		}
	}

	//duk.dump_context_stdout();
}
int main(int argc, char* argv[])
{
    testing::InitGoogleTest(&argc, argv);
	int rs = RUN_ALL_TESTS();
	if(!rs)
	{
		std::cout<<"\n\n***	extras begin	***\n\n";
		kg::scripts::duktape_t duk;
		EXPECT_TRUE(kg::scripts::duktape_extras::init(duk));
		if(duk.get_top())
		{
			std::cout<<"extras not pop\n";
			return 1;
		}

		//print alert
		std::cout<<"\n\n	******		print alert		******\n";
		duk.eval_noresult(R"(
				alert(123,456);
				print(123,456,[1,2,3,4],{
					name:'king',
					lv:'10',
				},
				'abc'
				);
		)");

		//print alert
		std::cout<<"\n\n	******		modules		******\n";
		putenv("KG_DUK_JS_PATH=.");
		duk.eval_noresult(R"(
				alert("_g_kg_modules = ",_g_kg_modules);
				alert("kg_package = ",kg_package);
				alert("kg_import = ",kg_import);
		)");


		std::cout<<"\n\n";
		duk.dump_context_stdout();
		std::cout<<"***	extras end	***\n";
	}
	return rs;
}
