(function() {
	"use strict";show_error_module
	var name = "king";
	return {
		GetName:function(){
			return name;
		},
		SetName:function(str){
			name = str;
		},
	};
})