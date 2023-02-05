cmake_minimum_required(VERSION 3.22)
project(chat-system)

add_executable(chat_client client_main.c)
add_executable(chat_server server_main.c)

add_compile_options(-pthread)