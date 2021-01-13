# Linux CPU and RAM monitor
Just created this to play a little bit with /proc/ folder on Linux and test DEB packages
## Installation
### Debian based distributions 
Download the [deb package](https://github.com/001roc20/pcstats/releases)

Open a terminal in the download folder and run the following command:
```bash
$ sudo dpkg -i pcstats.deb
```
### Build from source
Clone the repository and open a terminal in the src folder and run ```make```

Make sure that you have installed ```g++``` and ```make``` in your machine
## Usage
```bash
pcstats [OPTIONS]
OPTIONS:
-n refres_rate     set refresh rate (default: 1s)
```
