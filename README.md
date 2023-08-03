# Deluge bootloader

When the Deluge is powered up, this bootloader is responsible for loading the firmware into RAM, and additionally for the firmware-update functionality.

This code was based on a sample project provided by Renesas.

## How the code works
The functionality of this code is a bit complex. Initially, the bootloader begins executing directly from the external flash memory IC. This portion of the bootloader then loads the next part of this bootloader into RAM for faster execution. Then a third part is loaded into RAM and executed. Only then, the bootloader performs its main function of loading or updating the Deluge's main firmware, or even updating the bootloader itself.

Because of all of this, the exact positions of the various program sections within the linked binary are important. See the linker script - compiler_specific/GNU_e2studio-QSPI_SpSb.ld

This linker script could probably be improved to make it more foolproof, to ensure this all works regardless of other factors. Currently, a custom linkage order is specified in e2 Studio, without which I couldn't get the project working after updating it from an ancient version of e2 Studio. I haven't investigated where exactly that problem lay. All-in-all, it's possible that significant additions to the codebase may cause unforeseen problems as additional sections get inserted into the binary, and these will be fiddly to troubleshoot.

## Safety
You should only work on this codebase if you have the means to re-flash the Deluge's flash IC manually when you inevitably end up testing a non-functional build of the bootloader which can't update itself. For this, you will need a DelugeProbe, a J-link "PLUS" or higher, or as a last resort a SPI programmer can be used but it's very fiddly.

But other than the need to manually reflash, experimenting with this bootloader cannot cause any physical harm to your Deluge.

## Building
Like the main Deluge firmware, this codebase is provided as an e2 studio project. Other methods of building it are probably possible. As a starting point, please see the building instructions for the main [Deluge firmware](https://github.com/SynthstromAudible/DelugeFirmware).

Unlike the main Deluge firmware, this bootloader code needs to be built using the now legacy [GNUARM-NONE v14.02 toolchain](https://llvm-gcc-renesas.com/rz/rz-download-toolchains/).

Build it under the "Release" configuration. The "HardwareDebug" one isn't set up, and I've never actually tried running this code using in-circuit debugging - just by flashing the binary.

I (Rohan) have tried to instead get it running on GNU ARM Embedded with GCC 9.2, but have given up for now as then this bootloader inexplicably freezes when trying to update the Deluge's firmware. Hours of troubleshooting left me just as perplexed as when I started. The problem didn't seem to be related to the linkage complexities outlined above, and also trying updating FatFS to the latest version didn't fix things. Perhaps you can find the problem?
