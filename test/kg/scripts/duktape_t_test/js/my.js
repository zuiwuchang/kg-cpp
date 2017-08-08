/*
	要實現一個 js 模塊
	只需要 創建一個 js檔案 並且 此檔案 返回 一個 函數 func
	func 的返回值 object 作爲模塊
*/
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