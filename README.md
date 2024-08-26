# Sparkfun Alphanumeric Display ESPHome Component

ESPHome component for the Sparkfun Qwiic Alphanumeric Display.

Product: https://www.sparkfun.com/products/18565

Driver Datasheet: https://cdn.sparkfun.com/assets/c/8/7/2/5/VK16K33Datasheet.pdf

LED Datasheet: https://cdn.sparkfun.com/assets/d/c/5/c/d/JMF-4473BB3-059-P6.8.pdf

Existing Arduino Library: https://registry.platformio.org/libraries/sparkfun/SparkFun%20Qwiic%20Alphanumeric%20Display%20Arduino%20Library

Note: This was intended to be a thin wrapper around the above library to implement it as an ESPHome display, but it turned into a bit of a rewrite as I wanted to remove the display-chaining and simplify the `char->font->memory` transformation.


---


```
external_components:
  - source: github://wadesherman/esphome_qwiic_alphanumeric@main
    components:
      - ht16k33

display:
  - platform: ht16k33
    brightness: 3
    lambda: |-
      it.print("Milk");
```

Configuration variables:

- __brightness__ (Optional, integer): 0-15
- __address__ (Optional, integer): Manually specify the i2c address of the display. Defaults to `0x70`.
- __update_interval__ (Optional, Time): The interval to update the display. Defaults to `1s`.


---

### Lambda

Public methods available to the lamda:
-  size_t __print__(const char *str)

    Prints a string to the display. Only the first 4 characters of the string are displayed.
    ```
    it.print("Demo");
    ```

- void __set_brightness__(uint8_t brightness)

    Sets the display brightness.  Brightness is an int between 0-15.
    ```
    it.set_brightness(15);
    ```

- bool __colon_on__(bool update_now);

    Turns the colon on.
    ```
    it.colon_on(true);
    ```
- bool __colon_off__(bool update_now);

    Turns the colon off.
    ```
    it.colon_off(true);
    ```

- bool __decimal_on__(bool update_now);

    Turns the decimal point on.
    ```
    it.decimal_on(true);
    ```

- bool __decimal_off__(bool update_now);

    Turns the decimal point off.
    ```
    it.decimal_off(true);
    ```

- bool __update_display__();

    Refreshes the display.
    ```
    it.update_display();
    ```
---
## Internal Bits

### LED Segments
```
----A----
|\  |  /|
F I J K B
|  \|/  |
|-G---H-|
|  /|\  |
E N M L C
|/  |  \|
----D----
```

### Memory Format

The display data is an array of 16 bytes.  Each bit corresponds to the on/off state of a single LED in the display. Each bit is labeled `<segment><digit>`.

```
h4	h3	h2	h1	a4	a3	a2	a1
-	-	-	-	-	-	-	colon
i4	i3	i2	i1	b4	b3	b2	b1
-	-	-	-	-	-	-	decimal
j4	j3	j2	j1	c4	c3	c2	c1
-	-	-	-	-	-	-	-
k4	k3	k2	k1	d4	d3	d2	d1
-	-	-	-	-	-	-	-
l4	l3	l2	l1	e4	e3	e2	e1
-	-	-	-	-	-	-	-
m4	m3	m2	m1	f4	f3	f2	f1
-	-	-	-	-	-	-	-
n4	n3	n2	n1	g4	g3	g2	g1
-	-	-	-	-	-	-	-
-	-	-	-	-	-	-	-
-	-	-	-	-	-	-	-
```

### Font Encoding

The font encodes chars as uint16_t where each bit represents a display segment being on or off.
```
  -nmlkjih-gfedcba
0b0000000000000000
```

---

### TODOs
- implement the global blink rate setting.
- implement scrolling for long strings like the Adafruit flavor of this component by ssieb: https://github.com/ssieb/esphome_components/tree/master/components/ht16k33_alpha
- implement blinking for the colon only. (like a clock display)
- implement the display-chaining.  Preferrably should use one component instance per display.

---

