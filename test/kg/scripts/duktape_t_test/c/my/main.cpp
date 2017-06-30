#include "duktape.h"
#include <string>

#if defined(__GNUC__) && ((__GNUC__ >= 4) || (__GNUC__ == 3 && __GNUC_MINOR__ >= 3))
#define NP_VISIBILITY_DEFAULT __attribute__((visibility("default")))
#else
#define NP_VISIBILITY_DEFAULT
#endif

#define NP_EXPORT(__type) NP_VISIBILITY_DEFAULT __type

extern "C"
{
	std::string _g_name("king");

	duk_ret_t _native_get_name(duk_context* ctx)
    {
    	duk_push_lstring(ctx,_g_name.data(),_g_name.size());
    	return 1;
    }
    duk_ret_t _native_set_name(duk_context* ctx)
    {
    	_g_name = duk_safe_to_string(ctx,0);
    	return 0;
    }

    duk_ret_t kg_duk_package(duk_context* ctx)
    {
    	duk_idx_t id = duk_push_object(ctx);

    	duk_push_c_function(ctx,_native_get_name,0);
    	duk_put_prop_string(ctx,id,"GetName");

    	duk_push_c_function(ctx,_native_set_name,1);
    	duk_put_prop_string(ctx,id,"SetName");

    	return 1;
    }


}
