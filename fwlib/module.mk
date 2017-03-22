
SRC += fwlib/fw_format.c
SRC += fwlib/fw_conv.c
SRC += fwlib/fw_string.c

CINCS += fwlib

FWLIB_API_OUTPUT := fwlib/fwlib.h

FWLIB_API_HEADERS :=
FWLIB_API_HEADERS += fwlib/fw_conv.h
FWLIB_API_HEADERS += fwlib/fw_wstream.h
FWLIB_API_HEADERS += fwlib/fw_mem_wstream.h
FWLIB_API_HEADERS += fwlib/fw_string.h
FWLIB_API_HEADERS += fwlib/fw_format.h

AUTOGENS += $(FWLIB_API_OUTPUT)

$(FWLIB_API_OUTPUT): $(FWLIB_API_HEADERS)
	@echo 'Generating fwlib API header' $@
	$(CMDP) $(PY) $(TOOLS_DIR)/apigen/apigen.py --guard _FWLIB_H_ -o $@ $^
