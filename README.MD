# Stream Transcoder


## IMPORTANT NOTE:

Until the configuration file is finalized, config files may not be
compatable across releases.  If the streamTranscoder crashes on startup
then it's most likely a cause of incompatable config files...So, if you have this
problem, simply delete the transcoder.cfg from your application directory and re-run.

## ANOTHER IMPORTANT NOTE:


Vorbis input is still not working properly...it will work for the first song, but screws up
when a new vorbis bitstream is started...I will fix this, (eventually) but for now if you 
have problems transcoding from Vorbis to anything then please keep this in mind.  

Note that transcoding *TO* vorbis (from Mp3) does work, it's only on the *FROM* side that it does
not.


## What is it ?

The streamTranscoder is a multi-platform utility which can be used to transcode media streams from one format to another, and from one server type to another.

streamTranscoder will read in streams of type MP3 and Vorbis from most servers (Icecast, Icecast2, Shoutcast) and convert it into  various formats and send to various streaming servers...

The following is currently supported:

Input	MP3	Icecast
Input	MP3	Shoutcast
Input	Vorbis	Icecast2
Input	MP3	Icecast2

Output	MP3	Icecast
Output	MP3	Shoutcast
Output	MP3	Icecast2
Output	Vorbis	Icecast2
Output	WMA	WMA (Win32 only)


In addition to being able to convert streaming formats, streamTranscoder
has the ability to resample and also convert from stereo to mono and vice 
versa.  So, it can be used to transcode a 160kbps/44100/Stereo MP3 Shoutcast
stream into a Quality 0/32000/Mono Icecast2 Vorbis streams.

there are alot of combinations, and many way to mix and match, so please
be creative.. :)

Building
------------------------

On unix, only the command line program (streamTranscoder_CL) is built, to build
other components run the following :

NCurses UI

    % make streamTranscoder_NUI

WX Windows UI

    % make streamTranscoder_WX


## Configuration

The configuration of the streamTranscoder is all contained within a single configuration file (transcoder.cfg). This file is created the first time the program is run, and should be edited manually before being used.  Each parameter contains a comment which describes the purpose
of the configuration option.

## Platforms

The streamTranscoder is available on many different platforms, both UI form and non UI form.  Currently, the following is available :

**streamTranscoder_console** - Console win32 command line

**streamTranscoder console** - Console unix command line

**streamTranscoder text UI** - Unix ncurses UI

**streamTranscoder UI**      - Unix wxWindows GUI

**streamTranscoder UI**      - win32 wxWindows GUI


## Encoding with LAME


MP3 encoding is supported using LAME only. Unfortunalely, legally I am unable to distribute LAME with this program.  In order to encode to MP3 you must find a copy of lame_enc.dll Dimitry Kutsanov runs a site which may help (http://mitiok.cjb.net)

