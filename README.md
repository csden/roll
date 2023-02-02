## roll

Given an input file containing digits ranging from 0 through 9, reads each digit and generates a frequency audible to the human ear, then plays it through the PulseAudio daemon using the [simple API](https://freedesktop.org/software/pulseaudio/doxygen/simple.html).  

### Usage
```sh
$ roll path/to/file
```

### Install
Clone the repository, then:
```sh
$ make all
```
