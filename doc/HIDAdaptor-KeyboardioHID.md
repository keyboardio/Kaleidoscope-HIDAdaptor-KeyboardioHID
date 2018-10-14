Kaleidoscope-HIDAdaptor-KeyboardioHID
=====================================

A small library that implements the `kaleidoscope::hid` namespace, using
[KeyboardioHID](https://github.com/keyboardio/KeyboardioHID), with all bells and
whistles.

## Settable knobs

There are a number of features one can disable via a compiler flag (set either
via the Arduino IDE, or by using the `LOCAL_CFLAGS` setting of
`kaleidoscope-builder`), to reduce the size of the final firmware, if certain
features aren't needed. These knobs are:

### `KALEIDOSCOPE_HIDADAPTOR_DISABLE_KEYBOARD_BOOT_PROTOCOL`

> Disables the support for the Boot HID protocol (also known as 6KRO). This
> significantly reduces the firmware size, but the keyboard will not function
> under certain situations, where the host does not support N-key roll over.
> Such as older BIOSes, FreeBSD, or BitLocker prompts.
>
> Another side-effect will be that the keyboard will no longer be able to wake
> up a sleeping system, unless using the [HostPowerManagement][host-power]
> plugin.
>
> Disabling this also frees up an USB endpoint.

 [HostPowerManagement]: https://github.com/keyboardio/Kaleidoscope-HostPowerManagement

### `KALEIDOSCOPE_HIDADAPTOR_DISABLE_ABSOLUTE_MOUSE`

> Disables the absolute mouse functionality. The functions will still be
> present, but they will do nothing.
>
> Absolute mouse is most used for implementing the mouse warping feature of
> [MouseKeys][mousekeys], but can be used outside of that, too.
>
> Disabling this also frees up an USB endpoint.

 [MouseKeys]: https://github.com/keyboardio/Kaleidoscope-MouseKeys

### `KALEIDOSCOPE_HIDADAPTOR_DISABLE_MOUSE`

> Disables *all* mouse functionality, absolute mouse included.
>
> Disabling this frees up an USB endpoint.
