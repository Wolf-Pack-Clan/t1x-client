submodules = { basePath = "../submodules" }

function submodules.load()
	dir = path.join("submodules", "*.lua")
	deps = os.matchfiles(dir)
	for i, dep in pairs(deps) do
		dep = dep:gsub(".lua", "")
		require(dep)
	end
end

function submodules.imports()
	for i, proj in pairs(submodules) do
		if type(i) == 'number' then
			proj.import()
		end
	end
end

function submodules.projects()
	for i, proj in pairs(submodules) do
		if type(i) == 'number' then
			proj.project()
		end
	end
end

submodules.load()

-- Solution
workspace "iw1x-client"
configurations { "Debug", "Release" }
platforms "Win32"
architecture "x86"
startproject "client"
location "../build"
objdir "%{wks.location}/obj"
targetdir "%{wks.location}/bin/%{cfg.buildcfg}"
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
defines { "NDEBUG", "RELEASE" }
fatalwarnings { "All" }

-- Project: client
project "client"
kind "WindowedApp"
targetname "iw1x"
pchheader "pch.h"
pchsource "../src/client/pch.cpp"
includedirs { "../src/client", "../src/utils" }
files { "../src/client/**.h", "../src/client/**.cpp", "../src/client/**.rc" }
links { "utils" }
linkoptions
{ 
	"/DYNAMICBASE:NO",
	"/SAFESEH:NO", -- Prevents crash on Win10 when loading map
	"/LAST:._text"
}
submodules.imports()

-- Project: utils
project "utils"
kind "StaticLib"
files { "../src/utils/**.h", "../src/utils/**.cpp" }
vpaths
{
	["Header Files/*"] = { "../src/utils/**.h" },
	["Source Files/*"] = { "../src/utils/**.cpp" }
}
submodules.imports()

-- Group: submodules
group "submodules"
submodules.projects()