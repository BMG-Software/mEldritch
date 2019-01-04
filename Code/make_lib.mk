# make include file to build a program

OBJS=$(CXXFILES:%.cpp=$(BUILD_PATH)/%.o)
OBJS+=$(CFILES:%.c=$(BUILD_PATH)/%.o)
OBJS+=$(OBJCFILES:%.mm=$(BUILD_PATH)/%.o)

$(BUILD_PATH)/%.o: %.c
	@mkdir -p $(shell dirname $@)
	$(CC) $(CXXFLAGS) $(SYSCXXFLAGS) -c -o $@ $<

$(BUILD_PATH)/%.o: %.cpp
	@mkdir -p $(shell dirname $@)
	$(CXX) $(CXXFLAGS) $(SYSCXXFLAGS) -c -o $@ $<

$(BUILD_PATH)/%.o: %.mm
	@mkdir -p $(shell dirname $@)
	$(CXX) $(CXXFLAGS) $(SYSCXXFLAGS) -c -o $@ $<

$(LIB): $(OBJS)
	$(AR) crs $(LIB) $(OBJS)

.PHONY: all clean

all: $(LIB)

clean:
	rm -rf $(BUILDS) $(DEPDIR)

### automatic dependency generation
$(DEP_PATH)/%.dep: %.cpp; @mkdir -p $(shell dirname $@) && $(MAKEDEPEND)
-include $(CXXFILES:%.cpp=$(DEP_PATH)/%.dep)
