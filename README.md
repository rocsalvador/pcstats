# Linux CPU and RAM monitor
Just created this to play a little bit with /proc/ folder on Linux and test DEB packages
## Installation
### Debian based distributions 
Download the [deb package](https://github.com/001roc20/pcstats/releases)

Open a terminal in the download folder and run the following command:
```bash
$ sudo dpkg -i pcstats.deb
```
If you get a dependency error, run:
```bash
$ sudo apt install -f
```
### Build from source
Clone the repository and open a terminal in the src folder and run:
- ```make```: generates the executable file
- ```make install```: generates the executable and installs it (```make uninstall``` to uninstall it)

Make sure that you have ```g++``` , ```make``` and ```libncurses-dev``` installed in your machine
## Usage
```bash
pcstats [OPTIONS]
OPTIONS:
-n refres_rate     set refresh rate (default: 1s)
```
## Example
![Image](https://user-images.githubusercontent.com/58912154/111274431-45f4ec80-8635-11eb-9b13-fbe8ed0f4c04.png)
