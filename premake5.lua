workspace "dscrd"
   configurations { "debug", "release", "update_headers" }

gateway_enabled = 1

if gateway_enabled then
  defines {
    "HAVE_GATEWAY"
  }
end


filter "configurations:debug or debug"
  defines { "DEBUG" }
  symbols "On"

filter "configurations:debug or release"
  buildoptions { "-fPIC" }
  includedirs { "rest-client-c/lib", "lib/"  }
  libdirs { "rest-client-c/lib" }


filter "configurations:release"
  buildoptions { "-O2" }

project "update_headers"
   kind "Utility"
   language "C"

   filter "configurations:update_headers"
     prebuildcommands { 
      "scripts/gen_dscrd_h.sh",
      "scripts/gen_dscrd_i_h.sh",
      "false"
      }

-- rest-client-c

project "rest-client-c"
  kind "StaticLib"
  language "C"
  targetdir "."
  pic "on"
  prebuildcommands {
  "git submodule update",
  "echo \"\" > rest-client-c/lib/config.h"
  }

  files { "rest-client-c/lib/object.c",
          "rest-client-c/lib/rest_client.c"}
  links {"pthread"}

-- dscrd (shared)

project "dscrd"
   kind "SharedLib"
   language "C"
   targetdir "."

   files {
       "lib/*.c",
       "lib/jts/*.c",
       "lib/base64/*.c",
       "lib/con/*.c"
   }

   defines {  }
   links { "curl", "jansson", "magic", "websockets", "pthread", "rest-client-c"}

-- dscrd (static without rest-client-c)

project "dscrd_static_norest"
   kind "StaticLib"
   language "C"
   targetdir "."

   files {
       "lib/*.c",
       "lib/jts/*.c",
       "lib/base64/*.c",
       "lib/con/*.c"
   }
   defines {  }
   links { "curl", "jansson", "magic", "websockets", "pthread"}

-- dscrd_static (with rest-client-c)

project "dscrd_static"
   kind "StaticLib"
   language "C"
   targetdir "."
   postbuildcommands {
   "ar crsT libdscrd_static.a libdscrd_static_norest.a librest-client-c.a"
   }
   links {"dscrd_static_norest", "rest-client-c"}


-- dscrd_daemon

project "dscrd_daemon"
  kind "ConsoleApp"
  language "C"
  targetdir "./dscrd_daemon"
  files { "dscrd_daemon/dscrd_daemon.c"}
  links {"jansson"}

-- Examples

project "gateway_simple"
   kind "ConsoleApp"
   language "C"
   targetdir "./examples"
   files { "examples/src/gateway_simple/*.c"}
   links { "curl", "jansson", "magic", "websockets", "pthread", "dscrd_static"}

project "gateway_callback"
   kind "ConsoleApp"
   language "C"
   targetdir "./examples"
   files { "examples/src/gateway_callback/*.c"}
   links { "curl", "jansson", "magic", "websockets", "pthread", "dscrd_static"}


project "webhook_create"
   kind "ConsoleApp"
   language "C"
   targetdir "./examples"
   files { "examples/src/webhook_create/*.c"}
   links { "curl", "jansson", "magic", "websockets", "pthread", "dscrd_static"}

project "webhook_exec"
   kind "ConsoleApp"
   language "C"
   targetdir "./examples"
   files { "examples/src/webhook_exec/*.c"}
   links { "curl", "jansson", "magic", "websockets", "pthread", "dscrd_static"}

project "webhook_simple"
   kind "ConsoleApp"
   language "C"
   targetdir "./examples"
   files { "examples/src/webhook_simple/*.c"}
   links { "curl", "jansson", "magic", "websockets", "pthread", "dscrd_static"}

project "jts_showcase"
   kind "ConsoleApp"
   language "C"
   targetdir "./examples"
   files { "examples/src/jts_showcase/*.c", "lib/jts/*.c"}
   links { "jansson"}

project "jts_test"
   kind "ConsoleApp"
   language "C"
   targetdir "./examples"
   files { "examples/src/jts_test/*.c", "lib/jts/*.c"}
   links {"jansson"}

project "multithread"
   kind "ConsoleApp"
   language "C"
   targetdir "./examples"
   files { "examples/src/multithread/*.c"}
   links { "curl", "jansson", "magic", "websockets", "pthread", "dscrd_static"}