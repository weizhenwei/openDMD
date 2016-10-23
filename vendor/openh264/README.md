# prerequisite:
install nasm 

# download openh264:
    git clone https://github.com/cisco/openh264.git

# build:
    cd openh264
    git checkout origin/openh264v1.6 -b openh264v1.6
    make


# install
    modify Makefile's PREFIX to: ../linux-x86_64
    make install


