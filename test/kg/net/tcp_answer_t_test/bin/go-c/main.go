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

	buf := make([]byte, BufferLen)
	c.Read(buf)

	fmt.Println("ok")
}
