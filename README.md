# Measure ESP32 internal temperature with the ULP

Measure temperature with internal sensor in ULP.
If the data buffer is full, wake up the main cores to process the data.

## Build - Flash - Monitor
* Prepare shell to use ESP-IDF (see my [Blink-ULP repo](https://github.com/joba-1/Blink-ULP/blob/master/README.md) for details)
```
. ~/esp32/env.sh
```

* Fetch and build this example, flash it and open serial console (adapt ESPPORT and ESPBAUD to your setup. Can be configured in sdkconfig file)
```
REPO="Temp-ULP"
BASE="/tmp/$REPO-$$"
mkdir -p "$BASE" && \
cd "$BASE" && \
git clone "https://github.com/joba-1/$REPO.git" && \
cd "$REPO" && \
make menuconfig && \
make -j8 flash monitor ESPPORT=/dev/ttyUSB0 ESPBAUD=115200

```

Exit the monitor with [Ctrl]-] (which is [Strg]-[Alt-Gr]-] on german keyboard)

JoBa1
