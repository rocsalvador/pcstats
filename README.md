# Linux CPU and RAM monitor
Just created this to play a little bit with /proc/ folder on Linux and test DEB packages
## Installation
### Debian based distributions 
Download the [deb package](https://github.com/001roc20/pcstats/releases)

Open a terminal in the download folder and run the following command:
```bash
sudo dpkg -i pcstats.deb
```
If you get a dependency error, run:
```bash
sudo apt install -f
```
### Build from source

#### Requirements
-  ```g++```
- ```make``` 
- ```libncurses-dev```

```bash
git clone https://github.com/rocsalvador/pcstats.git
cd pcstats
make -j
```

## Usage
```bash
pcstats [OPTIONS]
OPTIONS:
-n refres_rate     set refresh rate (default: 1s)
```
## Example
![Image](https://user-images.githubusercontent.com/58912154/206488777-3a661d71-5355-4b5e-8665-6735bd569f69.png)
![Image](https://user-images.githubusercontent.com/58912154/206488882-3aa3b74e-7852-4559-9a50-19de94ce4446.png)

