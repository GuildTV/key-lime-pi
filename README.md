GTVPlayer
=========

GTVPlayer is a commandline OMX player for the Raspberry Pi. It is based upon OMXPlayer 
by huceke, and has been modified to support rendering overlays with OpenGL ES2.
Audio and subtitle support has been removed for lack of need.

Very early and very sloppy :)

Compiling GTVPlayer
-------------------

    make
    make dist


Using GTVPlayer
---------------

    Usage: omxplayer [OPTIONS] [FILE]
    Options :
    -h / --help                    print this help
    -i / --info                    dump stream format and exit
    -s / --stats                   pts and buffer stats
    -d / --deinterlace             deinterlacing
    -3 / --3d                      switch tv into 3d mode
    -y / --hdmiclocksync           adjust display refresh rate to match video
    -t / --sid index               show subtitle with index
    -r / --refresh                 adjust framerate/resolution to video

For example:

    ./omxplayer test.mkv

