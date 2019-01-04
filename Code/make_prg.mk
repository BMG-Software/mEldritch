# make include file to build a program

OBJS=$(CXXFILES:%.cpp=$(BUILD_PATH)/%.o)
OBJS+=$(CFILES:%.c=$(BUILD_PATH)/%.o)
OBJS+=$(OBJCFILES:%.mm=$(BUILD_PATH)/%.o)

TARGET=$(BUILD_PATH)/$(PROGRAM)

$(BUILD_PATH)/%.o: %.cpp
	@mkdir -p $(shell dirname $@)
	$(CXX) $(CXXFLAGS) $(SYSCXXFLAGS) -c -o $@ $<

$(BUILD_PATH)/%.o: %.mm
	@mkdir -p $(shell dirname $@)
	$(CXX) $(CXXFLAGS) $(SYSCXXFLAGS) -c -o $@ $<

$(TARGET): $(OBJS) $(LIBRARIES)
	mkdir -p $(BUILD_PATH)
	$(CXX) $(LDFLAGS) $(SYSLDFLAGS) -o $@ $(OBJS) $(LIBRARIES) $(EXTRALIBS) $(SYSLIBS)
ifneq ($(BUILD),debug)
	strip $@
endif

all: $(TARGET)

install: all
	for B in $(BUILDS); do \
		for P in $(PLATFORMS); do \
			if [ -f $$B/$$P/$(PROGRAM) ]; then \
				mkdir -p $(INST_PATH)/$$P/$$B$(INST_EXTRA); \
				cp $$B/$$P/$(PROGRAM) $(INST_PATH)/$$P/$$B$(INST_EXTRA); \
			fi; \
		done; \
	done

clean:
	rm -rf $(BUILDS) $(DEPDIR)

### automatic dependency generation
$(DEP_PATH)/%.dep: %.cpp; @mkdir -p $(shell dirname $@) && $(MAKEDEPEND)
-include $(CXXFILES:%.cpp=$(DEP_PATH)/%.dep)
