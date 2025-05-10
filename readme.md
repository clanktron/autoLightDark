# autoLightDark

Daemon that auto sets system-wide light-dark mode on gtk systems
based on the time of day.

---

## Dependencies
* `glib2-devel`

## Build
```sh
make
```

* estimates latitude/longitude based on timezone
* uses the [sunrise equation](https://en.wikipedia.org/wiki/Sunrise_equation) to calculate sunrise and sunset
* Uses `gio` to toggle gtk color-scheme
