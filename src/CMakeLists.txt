#Copyright PA Knowledge Ltd 2021
#MIT License. For licence terms see LICENCE.md file.

add_subdirectory(client)
add_subdirectory(server)
add_subdirectory(diodeheader)
add_subdirectory(SislTools)


add_executable(tester
        diodetester/EnterpriseDiodeTesterMain.cpp
        )

target_link_libraries(tester
        CLIENT_LIBRARY
        SERVER_LIBRARY
        HEADER_LIBRARY
        ${Boost_LIBRARIES}
        SISL_TOOLS_LIBRARY
        pthread
        stdc++fs
        )


add_executable(client
        client/ClientMain.cpp
        )

target_link_libraries(client
        CLIENT_LIBRARY
        HEADER_LIBRARY
        ${Boost_LIBRARIES}
        SISL_TOOLS_LIBRARY
        pthread
        )


add_executable(server
        server/ServerMain.cpp)

target_link_libraries(server
        SERVER_LIBRARY
        HEADER_LIBRARY
        ${Boost_LIBRARIES}
        SISL_TOOLS_LIBRARY
        pthread
        stdc++fs
        )