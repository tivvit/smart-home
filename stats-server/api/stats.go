package main

import (
  "fmt"
  "net/http"
  "encoding/json"
  "os"
  "time"
)

type Stat struct {
  Key    string `json:"key"`
  Value  string  `json:"value"`
}

type Stats struct {
  Stats  []Stat  `json:"stats"`
}

func addLog (config *Config) http.HandlerFunc {
  return http.HandlerFunc(func (w http.ResponseWriter, r *http.Request) {
		stats := Stats{}
		err := json.NewDecoder(r.Body).Decode(&stats)
		if err != nil {
			panic(err);
		}

		f, err := os.OpenFile(config.Stats_path, os.O_APPEND|os.O_WRONLY|os.O_CREATE , 0644)
		if err != nil {
		    panic(err)
		}

		defer f.Close()

		for _, stat := range stats.Stats {	
			stat_str := fmt.Sprintf("%d\t%s\t%s\n", int32(time.Now().Unix()), stat.Key, stat.Value) 
			if _, err = f.WriteString(stat_str); err != nil {
		    	panic(err)
			}	
		  	//fmt.Print(stat_str);
		}

		w.WriteHeader(200)
	})
}
