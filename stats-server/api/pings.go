package main

import (
	"encoding/json"
	"net/http"
	"strconv"
	"strings"
)

type Ping struct {
	Time   int64 `json:"time"`
	Status int   `json:"status"`
}

func pings(config *Config) http.Handler {
	return http.HandlerFunc(func(w http.ResponseWriter, r *http.Request) {
		var pings []Ping

		lines, err := readLines(config.Pinger_path)
		if err != nil {
			panic(err)
		}

		for _, line := range lines {
			chunks := strings.Fields(line)
			if len(chunks) == 2 {
				timestamp_str, status_str := chunks[0], chunks[1]
				status, err := strconv.Atoi(status_str)
				timestamp, err := strconv.ParseInt(timestamp_str, 10, 64)
				if err != nil {
					panic(err)
				}
				//fmt.Fprintf(w, "time %s, stat %d\n", timestamp, status)

				pings = append(pings, Ping{
					Time:   timestamp,
					Status: status,
				})
			}
		}

		pings_json, err := json.Marshal(pings)
		if err != nil {
			panic(err)
		}

		w.Header().Set("Content-Type", "application/json")
		w.Header().Set("Access-Control-Allow-Origin", "*")
		w.Write(pings_json)
	})
}
