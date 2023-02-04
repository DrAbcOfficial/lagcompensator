MODNAME = lagcompensator
SRCFILES = dllapi.cpp engine_api.cpp sdk_util.cpp h_export.cpp meta_api.cpp share_obj.cpp
INCLUDEDIRS+=-I$(SDKSRC)/../CDetour
EXTRA_CFLAGS += -DPLATFORM_POSIX
