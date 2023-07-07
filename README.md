# SproutExample

Sample project for the *Sprout* eurorack modular.

# Presets

| Color | Name | Parameters | Description |
| --- | --- | --- | --- |
| Green | Delay | [time: delay time], [sun: feedback], [wet: dry/wet] | Simple digital delay |
| Blue | FDN Reverb | [time: dencity], [sun: diffusion], [wet: dry/wet] | Feedback delay network reverb |
| Yellow | LP/HP Crossfade Filter | [time: LP <-> HP crossfade], [sun: cutoff frequency], [wet: resonance] | DJ style Lowpass <-> Highpass crossfade filter |
 
# How to compile

Clone this project on your local pc.

```
cd (Your Project Dir)
git clone --recursive git@github.com:centrevillage/SproutExample.git
```
(Don't forget to add `--recursive` option)

Get the latest compiler from this:

[Arm GNU Toolchain](https://developer.arm.com/downloads/-/arm-gnu-toolchain-downloads
)

Install this compiler on your OS.

Then, export `ARM_GCC_TOOLCHAIN` environment variable.

```
export ARM_TOOLKIT_ROOT=(Your Arm GNU Toolchain Path)
```

(If you use MacOS, probably like `/Applications/ArmGNUToolchain/(version name)/arm-none-eabi` )


Move the current directory to the `firmware` directory of the project.

```
cd (Your Project Dir)/DaisyExample/firmware
```

Build libraries.

```
./rebuild_libs
```

Build src.

```
make
```

If the compile is success, the `build/Sprout.bin` file is generated.

# How to upload the program

*Sprout* is using the Daisy Bootloader.

[Getting Started - Daisy Bootloader](https://electro-smith.github.io/libDaisy/md_doc_2md_2__a7___getting-_started-_daisy-_bootloader.html) 

So, you can use the DFU or the SD Card boot.

> DFU
>
> If the bootloader's LED is pulsing in the grace period, and the Daisy is connected to your computer via USB, you can run make program-dfu. The APP_TYPE will automatically adjust the DFU command to write to the correct address. Note that you'll have to change the app type back to BOOT_NONE or remove it to flash non-bootloader programs.

> SD Card
> 
> If your Daisy is connected to a micro SD card slot, you can just drag and drop the .bin file generated in your project's build folder onto the card. If you then plug it into the Daisy and restart it, the bootloader will automatically flash it to the QSPI chip.

# How to add your custom effect

Move the current directory to the `firmware/src/fx_model`.

```
cd (Your Project Dir)/DaisyExample/firmware/src/fx_model
```



Copy the effect template.

```
cp fx_model_template.hpp fx_model_your_effect.hpp
```
(Here, the custom model name is `FxModelYourEffect`)


Edit the effect class.

- Change a class name to `FxModelYourEffect` from `FxModelTemplate`
- Edit the `param_ranges` variable to customize the parameter range.
- Implement the methods (`init`, `deinit`, `updateParam`, `process`, `tapTempo`, `gatein`)

Edit `firmware/src/fx_model_select.hpp` file.

```
#pragma once

#include "fx_model/fx_model_delay.hpp"
#include "fx_model/fx_model_reverb.hpp"
#include "fx_model/fx_model_filter.hpp"

// Add effect file
#include "fx_model/fx_model_your_effect.hpp"

// Add effect class
#define SPR_FX_MODEL_SELECT SPR_FX_MODEL_LIST(FxModelDelay, FxModelReverb, FxModelFilter, FxModelYourEffect)
```

If you want to distribute your custom effect, edit the `firmware/src/config.hpp`

```
#pragma once
...
// firmware vender id (replace your vendor id)
#define SPR_VENDOR_ID 0xFFFFFFFF
// firmware id (SMPL = sample firmware)
#define SPR_FIRMWARE_ID 0x534d504c
// firmware version ( major (msb) | minor (lsb))
#define SPR_FIRMWARE_VER (0UL << 16U | 1UL)
```

Finally, you can `make` & upload this program.

```
cd (Your Project Dir)/DaisyExample/firmware
make
make program-dfu
```

If you make a custom effect, please let me know!
