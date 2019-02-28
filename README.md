# Measure ESP32 internal temperature with the ULP

ULP wakes up in intervals to measure the temperature with internal sensor while main cores are in deep sleep.
If the data buffer is full, the ULP wakes up the main cores to process the data.
Main cores will connect to wifi, post data to a webserver and go to sleep again.
WLAN, Webserver, interval and buffer size can be configured in sdkconfig (make menuconfig).
Also, dont forget to set CONFIG_ULP_COPROC_RESERVE_MEM for the ULP program and the buffer.

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

## Example data

testing a MH-ET live mini kit with a wemos 18650 (3000mAh) adapter

* Waking up every 5s runs for 8h, drawing 375mA on average
* Waking up every 5min runs for 42h, drawing 70mA on average
* Waking up every 5h runs for ?h, drawing ?mA on average (ongoing)

JoBa1
