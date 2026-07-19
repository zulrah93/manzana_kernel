qemu-system-aarch64 \
                -M virt \
                -cpu cortex-a72 \
                -device ramfb \
                -device qemu-xhci \
                -device usb-kbd \
                -device usb-tablet \
                -drive if=pflash,unit=0,format=raw,file=edk2-ovmf-bins/ovmf-code-aarch64.fd,readonly=on \
                -cdrom template-aarch64.iso \
                -m 2G