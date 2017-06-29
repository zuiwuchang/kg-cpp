(function() {
	"use strict";
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