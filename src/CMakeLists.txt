#Copyright PA Knowledge Ltd 2021
#MIT License. For licence terms see LICENCE.md file.

add_subdirectory(client)
add_subdirectory(diodeheader)
add_subdirectory(rewrapper)
add_subdirectory(server)
add_subdirectory(SislTools)


add_executable(tester
        diodetester/EnterpriseDiodeTesterMain.cpp
        )

target_link_libraries(tester
        CLIENT_LIBRARY
        SERVER_LIBRARY
        HEADER_LIBRARY
        REWRAPPER_LIBRARY
        SISL_TOOLS_LIBRARY
        ${Boost_LIBRARIES}
        pthread
        stdc++fs
        spdlog::spdlog
        )


add_executable(client
        client/ClientMain.cpp
        )

target_link_libraries(client
        CLIENT_LIBRARY
        HEADER_LIBRARY
        SISL_TOOLS_LIBRARY
        ${Boost_LIBRARIES}
        pthread
        stdc++fs
        spdlog::spdlog
        )


add_executable(server
        server/ServerMain.cpp)

target_link_libraries(server
        SERVER_LIBRARY
        HEADER_LIBRARY
        REWRAPPER_LIBRARY
        SISL_TOOLS_LIBRARY
        ${Boost_LIBRARIES}
        pthread
        stdc++fs
        spdlog::spdlog
        )