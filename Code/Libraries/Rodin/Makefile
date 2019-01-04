# Makefile in Rodin

LIB=$(LIB_RODIN)

CXXFILES=src/Actions/wbactionrodinblackboardwrite.cpp src/BTNodes/rodinbtnodeblackboardwrite.cpp \
	src/BTNodes/rodinbtnodecastresult.cpp src/BTNodes/rodinbtnodecomposite.cpp \
	src/BTNodes/rodinbtnodecompositesingular.cpp src/BTNodes/rodinbtnodeconditionpe.cpp \
	src/BTNodes/rodinbtnodedecorator.cpp src/BTNodes/rodinbtnodelog.cpp \
	src/BTNodes/rodinbtnodelookup.cpp src/BTNodes/rodinbtnodeloop.cpp \
	src/BTNodes/rodinbtnodenull.cpp src/BTNodes/rodinbtnodeparallel.cpp \
	src/BTNodes/rodinbtnodeplayactions.cpp src/BTNodes/rodinbtnodeselector.cpp \
	src/BTNodes/rodinbtnodesendevent.cpp src/BTNodes/rodinbtnodesequence.cpp \
	src/BTNodes/rodinbtnodesleep.cpp src/BTNodes/rodinbtnodetimeout.cpp \
	src/BTNodes/rodinbtnodeuseresource.cpp src/BTNodes/rodinbtnodeusestatmod.cpp \
	src/BTNodes/rodinbtnodewait.cpp src/BTNodes/rodinbtnodewaitforevent.cpp \
	src/Components/wbcomprodinbehaviortree.cpp src/Components/wbcomprodinblackboard.cpp \
	src/Components/wbcomprodinknowledge.cpp src/Components/wbcomprodinresourcemap.cpp \
	src/PEs/wbperodinblackboardget.cpp src/PEs/wbperodingetlastknownlocation.cpp \
	src/PEs/wbperodingetlastknowntime.cpp src/rodinbtnode.cpp src/rodinbtnodefactory.cpp

INCLUDES=$(INC_CORE) $(INC_MATH) $(INC_WORKBENCH)

OBJS=$(CXXFILES:%.cpp=$(BUILD_PATH)/%.o)

include $(TOP)/make_lib.mk
