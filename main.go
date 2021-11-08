package main

import (
	"flag"
	"fmt"
	"log"
	"net/http"

	"github.com/nireo/prxy/server"
)

func main() {
	port := flag.Int("port", 8080, "the port where the load balancer should be hosted on")

	log.Printf("starting the service on port: %d\n", *port)

	srv := http.Server{
		Addr:    fmt.Sprintf(":%d", *port),
		Handler: http.HandlerFunc(server.Balance),
	}

	if err := srv.ListenAndServe(); err != nil {
		log.Fatal(err)
	}
}
