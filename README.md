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
## Example (Compiled version)
![Image](https://user-images.githubusercontent.com/58912154/132912041-1e72937c-59fe-423b-8cea-547cea55f207.png)
![Image](https://user-images.githubusercontent.com/58912154/132911826-d9a24ac5-34c9-4666-ba2f-9f2444dbb2a7.png)
