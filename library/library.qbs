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
        "inc/upscale_rpc/array.hpp",
        "inc/upscale_rpc/basic.hpp",
        "inc/upscale_rpc/basic_types.hpp",
        "inc/upscale_rpc/interface.hpp",
        "inc/upscale_rpc/request.hpp",
        "inc/upscale_rpc/request_cancel.hpp",
        "inc/upscale_rpc/response.hpp",
        "inc/upscale_rpc/utils.hpp",
        "doc/protocol.txt",
        "src/upscale_rpc/basic.cpp",
        "src/upscale_rpc/request.cpp",
        "src/upscale_rpc/request_cancel.cpp",
        "src/upscale_rpc/response.cpp",
        "src/upscale_rpc/utils.cpp",
    ]

    Group {     // Properties for the produced executable
        fileTagsFilter: "application"
        qbs.install: true
        qbs.installDir: "bin"
    }
}
