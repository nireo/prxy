package main

import (
	"flag"
	"fmt"
	"log"
	"net/http"

	"github.com/nireo/prxy/config"
	"github.com/nireo/prxy/server"
)

func main() {
	conf, err := config.ConfigFromFile("./config.json")
	if err != nil {
		log.Fatalln(err)
	}

	log.Printf("starting the service on port: %d\n", conf.Port)

	for _, srv := range conf.URLs {
		server.AddServer(srv.URL)
	}

	srv := http.Server{
		Addr:    fmt.Sprintf(":%d", conf.Port),
		Handler: http.HandlerFunc(server.Balance),
	}

	if err := srv.ListenAndServe(); err != nil {
		log.Fatal(err)
	}
}
