package main

import (
	"flag"
	"log"
)

func main() {
	port := flag.Int("port", 8080, "the port where the load balancer should be hosted on")

	log.Printf("starting the service on port: %d\n", *port)
}
