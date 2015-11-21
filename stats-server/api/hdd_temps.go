package main

import (
  "net/http"
  "strings"
  "strconv"
  "encoding/json"
  "sort"
//  "os"
//  "fmt"
)

type HDD_stat struct {
  Temp    int `json:"temp"`
  Mountpoint  string  `json:"mountpoint"`
}

type HDD_stats struct {
  Time    int64 `json:"time"`
  Drives  []HDD_stat  `json:"drives"`
}

type Drive struct {
  Manufacturer  string  `json:"manufacturer"`
  Model  string  `json:"model"`
  Last_mountpoint  string  `json:"last-mnt"`
  First int64 `json:"first"`
  Last  int64 `json:"last"`
  Sn  string `json:"sn"`
  name string
}

type Drives []Drive

func (slice Drives) Len() int {
    return len(slice)
}

func (slice Drives) Less(i, j int) bool {
    return slice[i].Sn < slice[j].Sn;
}

func (slice Drives) Swap(i, j int) {
    slice[i], slice[j] = slice[j], slice[i]
}

type Response struct {
  Drives  *Drives `json:"drives"`
  Stats   *[]HDD_stats `json:"stats"`
}

type Entry struct {
  Time  int64
  Temp  int
  Manufacturer, Model, Mountpoint, Name, Sn string
}

func hdd_temps (config *Config) http.Handler {
  return http.HandlerFunc(func(w http.ResponseWriter, r *http.Request) {
    var hdd_stats []HDD_stats
    var entries []Entry
    drives := map[string]*Drive{}

    lines, err := readLines(config.HDD_temp_path)
    if err != nil {
      panic(err)
    }

    var last_stamp int64;
    hdd_stats_one := map[string]HDD_stat{};

    // get unique drives
    for _,line := range lines {
      chunks := strings.Fields(line)
      if(len(chunks) == 6) {
        timestamp_str, mountpoint, manufacturer, model, temp_str, sn :=
          chunks[0], chunks[1], chunks[2], chunks[3], chunks[4], chunks[5];

        drive_id := manufacturer + "-" + sn;

        temp_str = strings.TrimSuffix(temp_str, "°C");
        temp, err := strconv.Atoi(temp_str);

        timestamp, err := strconv.ParseInt(timestamp_str, 10, 64)
        if err != nil {
          panic(err)
        }

        entries = append(entries, Entry{
          Time: timestamp,
          Temp: temp,
          Manufacturer: manufacturer,
          Model: model,
          Mountpoint: mountpoint,
          Name: drive_id,
          Sn: sn,
        })

        //fmt.Println(drive_id);
        //fmt.Println(mountpoint);

        _, present := drives[drive_id]
        if(!present) {
          drives[drive_id] = &Drive{
            Manufacturer: manufacturer,
            Model: model,
            First: timestamp,
            Last: timestamp,
            Sn: sn,
          }

          //fmt.Println(drive_id);
        } else {
          //drive := drives[drive_id];
          drives[drive_id].Last_mountpoint = mountpoint
          drives[drive_id].Last = timestamp
          //drives[drive_id] = drive
        }
      }
    }


    // get drives sorted by id
    var drive_names Drives
    for _, value := range drives {
      value.name = value.Manufacturer + "-" + value.Sn
      drive_names = append(drive_names, *value)
    }
    sort.Sort(drive_names)


    // get stats for each drive
    for _,e := range entries {
      drive_id := e.Name

      //fmt.Printf("time %d, temp %s\n", timestamp, temp_str)
      //fmt.Printf("%s %s %s\n", mountpoint, manufacturer, id)


      // count position instead
      hdd_stat := HDD_stat{
        Temp: e.Temp,
        Mountpoint: e.Mountpoint,
        //Manufacturer: manufacturer,
        //Id: id,
      }

      hdd_stats_one[drive_id] = hdd_stat

      //hdd_stats_one = append(hdd_stats_one, hdd_stat);
      if(last_stamp > 0 && last_stamp != e.Time) {

        //fmt.Println(hdd_stats_one);
        //fmt.Println(drives);
        //fmt.Println(hdd_stats);
        append_hdd_stat(drive_names, hdd_stats_one, &hdd_stats, &e.Time)

        last_stamp = e.Time;
        hdd_stats_one = map[string]HDD_stat{};
      }
      if(last_stamp == 0) {
        last_stamp = e.Time;
      }
      //}
    }

    //insert last
    append_hdd_stat(drive_names, hdd_stats_one, &hdd_stats, &last_stamp)
    //hdd_stats = append(hdd_stats, HDD_stats{last_stamp, hdd_stats_one})

    //fmt.Println(hdd_stats);


    var response = Response{
      Drives: &drive_names,
      Stats: &hdd_stats,
    }

    hdd_temps_json, err := json.Marshal(response)

    if err != nil {
      panic(err)
    }

    //os.Stdout.Write(hdd_temps_json);


    w.Header().Set("Content-Type", "application/json")
    w.Header().Set("Access-Control-Allow-Origin", "*")
    w.Write(hdd_temps_json)
  })
}

func append_hdd_stat(
    drives Drives,
    hdd_stats_one map[string]HDD_stat,
    hdd_stats * []HDD_stats,
    timestamp * int64,
  ) {
    var all_hdd_stats []HDD_stat
    for _, drive := range drives {
      key := drive.name
      _, present := hdd_stats_one[key]
      if(present) {
        all_hdd_stats = append(all_hdd_stats, hdd_stats_one[key])
      } else {
        all_hdd_stats = append(all_hdd_stats, HDD_stat{})
      }
    }

    *hdd_stats = append(*hdd_stats, HDD_stats{
      Time: *timestamp,
      Drives: all_hdd_stats,
    })
}
