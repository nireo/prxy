# prxy: a load balancer in C++

A simple load balancer using Round Robin Balancing for now. Configurable algorithms will be available in the future. Prxy balances http requests using [drogon](https://github.com/drogonframework/drogon) and no other dependencies. Drogon is used since it provides a robust http server and client, which can easily be used to build a proxy.

## Configuration

The configuration is in the config.json file. It is the default drogon config with the exception of having a `plugins` fields which contains configuration for the reverse proxy. In the `plugins/config/backends` backends can be added to be load balanced. The default configuration is also held in the `plugins` field and it contains some comments to help configuration.

## Contributing

Anyone can contribute by sending a pull request with some sort of explanation and it will most likely get accepted.
