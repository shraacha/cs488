#!lua

includeDirList = {
    "../shared",
    "../shared/include",
    "../shared/gl3w",
    "../shared/imgui",
    "include"
}

libDirectories = { 
    "../lib",
    "lib"
}


if os.get() == "macosx" then
    linkLibs = {
        "cs488-framework",
        "imgui",
        "glfw3",
        "lua",
		"lodepng"
    }
end

if os.get() == "linux" then
    linkLibs = {
        "cs488-framework",
        "lua",
        "lodepng",
        "stdc++",
        "dl",
        "pthread"
    }
end

if os.get() == "macosx" then
    linkOptionList = { "-framework IOKit", "-framework Cocoa", "-framework CoreVideo", "-framework OpenGL" }
end

buildOptions = {"-std=c++17"}

solution "CS488-Projects"
    configurations { "Debug", "Release" }

    project "A5"
        kind "ConsoleApp"
        language "C++"
        location "build"
        objdir "build"
        targetdir "."
        buildoptions (buildOptions)
        libdirs (libDirectories)
        links (linkLibs)
        linkoptions (linkOptionList)
        includedirs (includeDirList)
        files { "src/*.cpp", "include/*.hpp" }

    configuration "Debug"
        defines { "DEBUG" }
        flags { "Symbols" }

    configuration "Release"
        defines { "NDEBUG" }
        flags { "Optimize" }

    project "Tests"
        kind "ConsoleApp"
        language "C++"
        location "build"
        targetdir "."
        objdir "build"
        files { "tests/**.cpp", "tests/**.h" }
        includedirs { "include", "third_party/googletest/googletest/include" }

        links { "googletest" }
        libdirs { "lib", "third_party/googletest/build/lib" }

    configuration "Debug"
        defines { "DEBUG" }
        flags { "Symbols" }

    configuration "Release"
        defines { "NDEBUG" }
        flags { "Optimize" }

    project "googletest"
        kind "StaticLib"
        language "C++"
        location "build"
        objdir "build"
        targetdir "lib"
        files { "third_party/googletest/googletest/src/**.cc" }
        includedirs { "third_party/googletest/googletest/include", "third_party/googletest/googletest/" }
