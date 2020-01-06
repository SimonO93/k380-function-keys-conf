# k380-function-keys-conf
MacOS app to make function keys default on Logitech k380 bluetooth keyboard

## Instalation

1. Install `hidapi` library
```shell
brew install hidapi
```
2. Build this app
```
./build.sh
```
3. Run it as root (Note that you might be asked to give permissions for terminal app to access input device)
```
sudo ./k380 -f on
```

## Based on
https://github.com/jergusg/k380-function-keys-conf
