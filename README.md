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
![Image](https://private-user-images.githubusercontent.com/58912154/540137856-02b8b2e6-d82f-4f6f-8bd2-4e5c9ca15b72.png?jwt=eyJ0eXAiOiJKV1QiLCJhbGciOiJIUzI1NiJ9.eyJpc3MiOiJnaXRodWIuY29tIiwiYXVkIjoicmF3LmdpdGh1YnVzZXJjb250ZW50LmNvbSIsImtleSI6ImtleTUiLCJleHAiOjE3NjkyNzIzNjIsIm5iZiI6MTc2OTI3MjA2MiwicGF0aCI6Ii81ODkxMjE1NC81NDAxMzc4NTYtMDJiOGIyZTYtZDgyZi00ZjZmLThiZDItNGU1YzljYTE1YjcyLnBuZz9YLUFtei1BbGdvcml0aG09QVdTNC1ITUFDLVNIQTI1NiZYLUFtei1DcmVkZW50aWFsPUFLSUFWQ09EWUxTQTUzUFFLNFpBJTJGMjAyNjAxMjQlMkZ1cy1lYXN0LTElMkZzMyUyRmF3czRfcmVxdWVzdCZYLUFtei1EYXRlPTIwMjYwMTI0VDE2Mjc0MlomWC1BbXotRXhwaXJlcz0zMDAmWC1BbXotU2lnbmF0dXJlPTkyMWU1NjFlZTIyZDczMzhmNjIwM2JjOWY5OWYxOTRlOWM1MjNlMjI1Y2JiZTU1OTk0YmQ2Y2JmZTI4NTMzOTAmWC1BbXotU2lnbmVkSGVhZGVycz1ob3N0In0.iBtHXNZ3Dkj8zlRogqL1OadX0JreSuj2hjYmKajd3u8)
![Image](https://private-user-images.githubusercontent.com/58912154/540137691-84d80d9c-14c2-4a26-8610-1613017b2d1b.png?jwt=eyJ0eXAiOiJKV1QiLCJhbGciOiJIUzI1NiJ9.eyJpc3MiOiJnaXRodWIuY29tIiwiYXVkIjoicmF3LmdpdGh1YnVzZXJjb250ZW50LmNvbSIsImtleSI6ImtleTUiLCJleHAiOjE3NjkyNzIyODksIm5iZiI6MTc2OTI3MTk4OSwicGF0aCI6Ii81ODkxMjE1NC81NDAxMzc2OTEtODRkODBkOWMtMTRjMi00YTI2LTg2MTAtMTYxMzAxN2IyZDFiLnBuZz9YLUFtei1BbGdvcml0aG09QVdTNC1ITUFDLVNIQTI1NiZYLUFtei1DcmVkZW50aWFsPUFLSUFWQ09EWUxTQTUzUFFLNFpBJTJGMjAyNjAxMjQlMkZ1cy1lYXN0LTElMkZzMyUyRmF3czRfcmVxdWVzdCZYLUFtei1EYXRlPTIwMjYwMTI0VDE2MjYyOVomWC1BbXotRXhwaXJlcz0zMDAmWC1BbXotU2lnbmF0dXJlPTY2ZDkzZTAzNzY0Njc5YzNmZmQwNjhlNWY5MGQyZGNiZWU3YmRlYmRiOWUwZDk4NWI1MDZmYjAxNDUyMmEyYzEmWC1BbXotU2lnbmVkSGVhZGVycz1ob3N0In0.e-MTo8-h5xRFor3oI9l5AjF0J0faN19t1UhAWvDcrOc)