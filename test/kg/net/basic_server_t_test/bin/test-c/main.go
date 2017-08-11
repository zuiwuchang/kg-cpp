package main

import (
	"errors"
	"fmt"
	"log"
	"net"
	"time"
)

const (
	Addr       = "127.0.0.1:1102"
	TimeOut    = 0
	HeaderFlag = 1102
)

func main() {

	last := time.Now()

	count := 10000
	ch := make(chan int)

	for i := 0; i < count; i++ {
		go test_one(ch)
	}
	for count > 0 {
		<-ch
		count--
	}
	fmt.Println(time.Now().Sub(last))
}
func test_one(ch chan int) {
	defer func() {
		ch <- 1
	}()

	//連接服務器
	c, e := net.Dial("tcp", Addr)
	if e != nil {
		log.Fatalln(e)
	}
	defer c.Close()

	e = requestStr(c, "i'm king")
	if e != nil {
		log.Fatalln(e)
	}
	e = requestStr(c, "cerberus is an idea")
	if e != nil {
		log.Fatalln(e)
	}
}
func requestStr(c net.Conn, str string) error {
	b := []byte(str)
	_, e := c.Write(b)
	if e != nil {
		return e
	}

	data := make([]byte, len(b))
	pos := 0
	for pos != len(b) {
		n, e := c.Read(data[pos:])
		if e != nil {
			return e
		}
		pos += n
	}

	if string(data) != str {
		return errors.New("rs not equal")
	}
	return nil
}
