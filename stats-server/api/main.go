package main

import (
	"fmt"
	"github.com/gorilla/mux"
	"net/http"
)

func main() {
	/*
	   http.Handle("/api/pings", pings(conf))
	   http.Handle("/api/hdd-temps", hdd_temps(conf))
	   http.ListenAndServe(port_addr, nil)
	*/
	conf := get_config()
	port_addr := fmt.Sprintf(":%d", conf.Port)

	r := mux.NewRouter().PathPrefix("/api").Subrouter()
	r.HandleFunc("/add-log", addLog(conf)).Methods("POST")

	http.ListenAndServe(port_addr, r)
}
