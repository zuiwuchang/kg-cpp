package main

import (
	"bytes"
	"encoding/binary"
	"errors"
	"log"
	"net"
	"time"
)

const (
	Addr      = "127.0.0.1:1102"
	BufferLen = 1024

	CmdEcho   = 1
	CmdExit   = 2
	CmdUnknow = 666
)

type Client struct {
	conn   net.Conn
	buffer bytes.Buffer
}

func main() {
	now := time.Now()
	ch := make(chan int)
	count := 1000
	for i := 0; i < count; i++ {
		go testOne(ch)
	}

	for count > 0 {
		<-ch
		count--
	}
	log.Println("ok", time.Now().Sub(now))
}
func testOne(ch chan int) {
	c, e := net.Dial("tcp", Addr)
	if e != nil {
		log.Fatalln(e)
	}
	defer c.Close()

	client := Client{conn: c}
	client.testEcho()
	client.testUnknow()
	client.testExit()

	ch <- 1
}
func (c *Client) testExit() {
	e := c.writeCmd(CmdExit)
	if e != nil {
		log.Fatalln(e)
	}
	b := make([]byte, 1)
	if _, e := c.conn.Read(b); e != nil {
		return
	}

	log.Fatalln("CmdExit fatal rs")

}
func (c *Client) testUnknow() {
	e := c.writeCmd(CmdExit + 100)
	if e != nil {
		log.Fatalln(e)
	}

	b, e := c.readMessage()
	if e != nil {
		log.Fatalln(e)
	}
	b = b[4:6]
	cmd := binary.LittleEndian.Uint16(b)
	if cmd != CmdUnknow {
		log.Fatalln("CmdUnknow fatal rs")
	}
}
func (c *Client) testEcho() {
	e := c.writeCmd(CmdEcho)
	if e != nil {
		log.Fatalln(e)
	}

	b, e := c.readMessage()
	if e != nil {
		log.Fatalln(e)
	}
	b = b[4:6]
	cmd := binary.LittleEndian.Uint16(b)
	if cmd != CmdEcho {
		log.Fatalln("CmdEcho fatal rs")
	}
}
func (c *Client) readMessage() ([]byte, error) {
	buf := &c.buffer
	conn := c.conn
	b := make([]byte, 1024)
	for {
		n, e := conn.Read(b)
		if e != nil {
			log.Fatalln(e)
		}

		_, e = buf.Write(b[:n])
		if e != nil {
			log.Fatalln(e)
		}

		bytes := buf.Bytes()
		if len(bytes) < 4 {
			continue
		}
		flag := binary.LittleEndian.Uint16(bytes)
		if flag != 1102 {
			return nil, errors.New("bad header flag")
		}
		size := int(binary.LittleEndian.Uint16(bytes[2:]))
		if size < 6 {
			return nil, errors.New("bad header len")
		}

		if size > len(bytes) {
			continue
		}

		rs := make([]byte, size)
		if _, e = buf.Read(rs); e != nil {
			return nil, e
		}

		return rs, nil
	}
	return nil, nil
}
func (c *Client) writeCmd(cmd uint16) error {
	n := 4 + 2
	b := make([]byte, n)

	binary.LittleEndian.PutUint16(b, 1102)
	binary.LittleEndian.PutUint16(b[2:], 6)
	binary.LittleEndian.PutUint16(b[4:], cmd)

	_, e := c.conn.Write(b)

	return e
}
