INST_PATH=../$(INSTDIR)/$(PLATFORM)/$(BUILD)
TOOL_PATH=../$(INST_PATH)$(INST_EXTRA_DIR)
MOD_PATH=Mods/mEldritch

all: # $(TOOL_PATH)/FilePacker
	@mkdir -p $(MOD_PATH)
	@mkdir -p ModTmp/Config
	@mkdir -p ModTmp/Fonts
	@mkdir -p ModTmp/Textures/Fonts
	# build font
	cd Raw/Fonts && ../$(TOOL_PATH)/mFontGenerator freesans10.font ../../ModTmp/Fonts/freesans10.fnp ../../ModTmp/Textures/Fonts/freesans10.tga Textures/Fonts
	# pack archive
	sed -e"s/%VERSION%/$(VERSION)/" < Raw/Config/syncer.syncerconfig > ModTmp/Config/syncer.syncerconfig
	cd Raw && find Config -name "*.config" -exec $(TOOL_PATH)/mFilePacker {} ../mod_meldritch.cpk -c \;
	cd ModTmp && find * -type f -exec $(TOOL_PATH)/mFilePacker {} ../mod_meldritch.cpk -c \;
	cd Raw && find Textures -type f -exec $(TOOL_PATH)/mFilePacker {} ../mod_meldritch.cpk -c \;
	cd Raw && $(TOOL_PATH)/mFilePacker Rooms/Library/library-start-n.eldritchroom ../mod_meldritch.cpk -c
	cd Raw && $(TOOL_PATH)/mFilePacker Rooms/Library/library-tutorial-n.eldritchroom ../mod_meldritch.cpk -c
	mv mod_meldritch.cpk $(MOD_PATH)
	# build mod.config
	sed -e"s/%VERSION%/$(VERSION)/" < Raw/mod.config > $(MOD_PATH)/mod.config
	echo "NumConfigFiles = &" >> $(MOD_PATH)/mod.config
	echo "@ConfigFile" >> $(MOD_PATH)/mod.config
	cd Raw && find Config -type f -exec echo "@@& = \"{}\"" >> ../$(MOD_PATH)/mod.config \;
	echo "NumPackages = &" >> $(MOD_PATH)/mod.config
	echo "@Package" >> $(MOD_PATH)/mod.config
	echo "@@& = \"mod_meldritch.cpk\"" >> $(MOD_PATH)/mod.config
	@rm -rf ModTmp

install: all
	cp -r Mods readme_meldritch.txt $(INST_PATH)
	@mkdir -p $(INST_PATH)$(INST_EXTRA_DIR)/Raw
	cp -r Raw/Config Raw/Fonts Raw/Rooms $(INST_PATH)$(INST_EXTRA_DIR)/Raw
	cp -r Examples Tools Raw/tools.cfg $(INST_PATH)$(INST_EXTRA_DIR)

clean:
	rm -rf ModTmp $(MOD_PATH)
