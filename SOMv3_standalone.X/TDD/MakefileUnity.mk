#Set this to @ to keep the makefile quiet
SILENCE = @

#---- Outputs ----#
COMPONENT_NAME = SandBox_Unity

#--- Inputs ----#
PROJECT_HOME_DIR = ../
UNITY_HOME = ./
CPP_PLATFORM = Gcc
UNITY_BUILD_HOME = /build

UNITY_CFLAGS += -DUNITY_OUTPUT_CHAR=UnityOutputCharSpy_OutputChar

SRC_DIRS = \
	$(PROJECT_HOME_DIR)\
	$(PROJECT_HOME_DIR)/TDD\
	

TEST_SRC_DIRS = \
	$(PROJECT_HOME_DIR)/TDD\
	
INCLUDE_DIRS =\
  .\
  $(UNITY_HOME)\
  $(PROJECT_HOME_DIR)\

include $(UNITY_BUILD_HOME)/MakefileWorker.mk
