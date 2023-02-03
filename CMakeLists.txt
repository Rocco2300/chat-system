cmake_minimum_required(VERSION 3.22)
project(chat-system)

add_executable(chat_client client/main.c)
add_executable(chat_server server/main.c)

add_compile_options(-pthread)