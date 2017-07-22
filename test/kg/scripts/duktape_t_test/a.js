(function(g){
	alert(g.alert)
	alert("ok")
	alert(456)

	try{
		var m = kg_import("123","js");
		alert(m);
	}
	catch(e){
		alert("error :",e);
	}

	return alert
}(this))

