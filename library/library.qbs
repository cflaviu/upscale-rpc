import qbs

StaticLibrary {
    name: "upscale-rpc"
    Depends { name: 'cpp' }
    consoleApplication: true
    cpp.cxxLanguageVersion: "c++20"
    cpp.enableRtti: false
    cpp.includePaths: ["inc", "inc_dep"]
    //cpp.cxxFlags: "-fsanitize=address"
    //cpp.staticLibraries: "asan"
    files: [
        "inc/upscale_rpc/basic_types.hpp",
        "inc/upscale_rpc/message.hpp",
        "inc/upscale_rpc/request.hpp",
        "inc/upscale_rpc/response.hpp",
        "doc/protocol.txt",
    ]

    Group {     // Properties for the produced executable
        fileTagsFilter: "application"
        qbs.install: true
        qbs.installDir: "bin"
    }
}
