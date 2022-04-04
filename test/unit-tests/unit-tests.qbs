import qbs

CppApplication {
    Depends { name: "upscale-rpc" }
    name: "upscale-rpc-unit-tests"
    consoleApplication: true
    cpp.cxxLanguageVersion: "c++20"
    cpp.enableRtti: false
    cpp.includePaths: ["inc", "inc_dep"]
    cpp.staticLibraries: [
        "/usr/local/lib/libCatch2Main.a",
        "/usr/local/lib/libCatch2.a",
    ]
    //cpp.cxxFlags: "-fsanitize=address"
    //cpp.staticLibraries: "asan"
    files: [
        "src/header_test.cpp",
        "src/request_cancel_tests.cpp",
        "src/request_tests.cpp",
        "src/response_tests.cpp",
    ]

    Group {     // Properties for the produced executable
        fileTagsFilter: "application"
        qbs.install: true
        qbs.installDir: "bin"
    }
}
