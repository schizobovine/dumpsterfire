dumpsterfire
================
Exactly what it says on the tin.

Audio Conversion
================

Using sox(1) which should be available in any reasonable Linux distro:

```sh
sox ${INPUT}.wav -b 8 -c 1 -e unsigned-integer -r 16k ${OUTPUT}.wav
```
