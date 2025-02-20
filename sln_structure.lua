dependencies = { basePath = "./deps" }

function dependencies.load()
	dir = path.join(dependencies.basePath, "premake/*.lua")
	deps = os.matchfiles(dir)
	for i, dep in pairs(deps) do
		dep = dep:gsub(".lua", "")
		require(dep)
	end
end

function dependencies.imports()
	for i, proj in pairs(dependencies) do
		if type(i) == 'number' then
			proj.import()
		end
	end
end

function dependencies.projects()
	for i, proj in pairs(dependencies) do
		if type(i) == 'number' then
			proj.project()
		end
	end
end

dependencies.load()

workspace "iw1x-client"
configurations { "Debug", "Release" }
platforms "Win32"
architecture "x86"
startproject "client"
location "./build"
objdir "%{wks.location}/obj"
targetdir "%{wks.location}/bin/%{cfg.platform}/%{cfg.buildcfg}"
language "C++"
cppdialect "C++20"
systemversion "latest"
symbols "On"
staticruntime "On"
editandcontinue "Off"
warnings "Extra"
characterset "ASCII"

flags
{
	"NoIncrementalLink",
	"NoMinimalRebuild",
	"MultiProcessorCompile",
	"No64BitChecks"
}

-- Config: Debug
filter "configurations:Debug"
optimize "Debug"
defines { "DEBUG" }

-- Config: Release
filter "configurations:Release"
optimize "Speed"
linktimeoptimization "On"
linkoptions
{
	"/OPT:NOREF", -- Prevents crash when using /GL
}
defines { "NDEBUG" }
fatalwarnings { "All" }

-- Project: client
project "client"
kind "WindowedApp"
targetname "iw1x"
pchheader "std_include.hpp"
pchsource "src/client/std_include.cpp"
linkoptions { "/DYNAMICBASE:NO", "/SAFESEH:NO", "/LARGEADDRESSAWARE", "/LAST:._text", "/PDBCompress" }
files { "./src/client/**.hpp", "./src/client/**.cpp" }
includedirs {"./src/client", "./src/utils", "%{prj.location}/src"}
resincludedirs { "$(ProjectDir)src" }
links { "utils" }
dependencies.imports()

-- Project: utils
project "utils"
kind "StaticLib"
files { "./src/utils/**.hpp", "./src/utils/**.cpp" }
includedirs { "./src/utils", "%{prj.location}/src" }
resincludedirs { "$(ProjectDir)src" }
dependencies.imports()

group "Dependencies"
dependencies.projects()