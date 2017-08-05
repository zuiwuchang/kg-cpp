package main

import (
	"fmt"
	"log"
	"net"
)

const (
	Addr      = "127.0.0.1:1102"
	BufferLen = 1024
)

func main() {
	c, e := net.Dial("tcp", Addr)
	if e != nil {
		log.Fatalln(e)
	}
	defer c.Close()

	//c.Write([]byte("this is cerberus"))

	buf := make([]byte, BufferLen)
	_, e = c.Read(buf)

	fmt.Println(e)
}
