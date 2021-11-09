package config

import (
	"encoding/json"
	"io/ioutil"
)

type Server struct {
	URL string `json:"addr"`
}

type Config struct {
	Port int      `json:"port"`
	URLs []Server `json:"servers"`
}

func ConfigFromFile(path string) (Config, error) {
	var conf Config

	bytes, err := ioutil.ReadFile(path)
	if err != nil {
		return conf, err
	}

	if err := json.Unmarshal(bytes, &conf); err != nil {
		return conf, err
	}

	return conf, nil
}
