package main

import (
  "encoding/json"
  "fmt"
  "io/ioutil"
)

type Config struct {
  HDD_temp_path, Pinger_path, Stats_path string
  // string
  Port int
}

func get_config() *Config {
  conf := Config{}
  conf_file, err := ioutil.ReadFile("conf.json")
  err = json.Unmarshal(conf_file, &conf)

  if err != nil {
    fmt.Println("error:", err)
  }

  fmt.Println(conf)
  return &conf
}
