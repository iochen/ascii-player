# ASCII Player
ASCII Player is a player that plays video files with ASCII characters (greyscale).

![Example with Never Gonna Give You Up](.img/example1.gif)

## Features
- Support any video format as long as FFmpeg supports it.
- Support playing audio stream in video file.
- Support processing the video file in advance to cache(`.apcache`) file.
- Scale video to your terminal size by default.

## Installation
### Linux
#### Manually with Package Manager
This is an example for Debian Linux.   
Audio playing is not tested.
1. Install building tools:
    ```shell
    $ apt install git make clang
    ```
2. Install libraries:
    ```shell
    $ apt install libavcodec-dev libavformat-dev libavdevice-dev libswresample-dev libswscale-dev libavutil-dev libz-dev libbz2-dev libncurses-dev libasound-dev portaudio19-dev libportaudio2 
    ```
3. Clone the project.
4. Build with `make`.
5. Find the executable file in `build/asciiplayer`.
### macOS
#### Manually from Source Code
1. Install building tools with `xcode-select --install`
2. Download [PortAudio](https://files.portaudio.com/archives/pa_stable_v190700_20210406.tgz), use `./configure`, then `make` and then `make install` to build and install PortAudio library.
2. Download [FFmpeg](https://ffmpeg.org/releases/ffmpeg-snapshot.tar.bz2), use `./configure`, then `make` and then `make install` to build and install PortAudio library.
3. Clone the project.
4. Build with `make`.
5. Find the executable file in `build/asciiplayer`.

### Docker
Under testing...   
PR is welcome!

### Windows
PR is welcome!

## Usage
### Directly play a video or a cache file
```shell
$ asciiplayer <URI/PATH>
```
### Process the video to cache file
```shell
$ asciiplayer <URI/PATH> --cache <PATH>
```
### Other options
```
ASCII Player v1.0.0
A media player that plays video file in ASCII characters.
Usage: asciiplayer <file> [-h | --help] [-l | --license] [-c | --cache <file>]
                          [-n | —no-audio] [-g | --grayscale <string>] [-r | --reverse]
                          [--log <log file>] [--loglevel <level num>]

       --help -h            Print this help page
       --license -l         Show license and author info
       --cache -c <file>    Process video into a cached file
                            example: $ asciiplayer video.mp4 --cache cached.apcache
       --grayscale -g <string>
                            Grayscale string (default: " .:-=+*#%@")
       --reverse -r         Reverse grayscale string
       --no-audio -n        Play video without playing audio
       --log <log file>     Path to log file
       --loglevel <level num>
                            Log level number {TRACE: 0, DEBUG: 1, INFO: 2, WARN: 3,
                                              ERROR: 4, FATAL: 5}
```

## License
This project is under the GNU General Public License (Version 3) or GPLv3 License.   
View [LICENSE](LICENSE) file for more detailed information.