cmake_minimum_required(VERSION 3.7)
project(rpi-gpio-matrix-keyboard)

add_executable(gpio_keyboard gpio_keyboard.c)
set(CMAKE_C_FLAGS "-O3")

install(FILES gpio_keyboard.service DESTINATION /etc/systemd/system/)
install(TARGETS gpio_keyboard DESTINATION /usr/local/bin)
