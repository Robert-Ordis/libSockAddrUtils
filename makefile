####EXT_PATHS####
DEF_MKFILES		= ./make_config/*.mk
EXT_MKFILES		=

####OUTPUT INFO####
PROJECT_NAME	= sockaddr_utils
TARGET_AOUT		= testProgram
TARGET_ALIB		= libsockaddr_utils.a
TARGET_SLIB		=

####Source files####

SRC_LIBPART		=\
				 sockaddr_utils.c\

SRC_PROGRAM		=\
				 main.c\

#### default compilers ####
CC	= gcc
AR	= ar

####build options####
CFLAGS			= -Wall -O2
DEFS			= 
CROSS_PREFIX	= 

####dependency options####
#ALIB_EXTERNAL	:= $(ALIB_EXTERNAL) libExternal.a
#SLIB_EXTERNAL	:= $(SLIB_EXTERNAL) -lexternal_lib
#SLIB_EXTPATH	:= $(SLIB_EXTPATH) -L /dirpath/contains/.so_file/
#HDRS_EXTPATH	:= $(HDRS_EXTPATH) -I /dirpath/contains/header_root/

ALIB_EXTERNAL	=
SLIB_INTERNAL	= 
SLIB_EXTERNAL	=
SLIB_EXTPATH	=
HDRS_EXTPATH	=

#################
## PARAMETERS WRITTEN BELOW MUST "NOT" BE OVERWRITTEN !!
#################

####BUILDING OBJ(*.o) and DEP(*.depend) INFO####
#OBJ_DIR and DEP_DIR is the most important AUTO-GENERATED directory.
#DO NOT RE-WRITE THIS PARAMETER!!
OBJ_DIR			= ./.obj
DEP_DIR			= ./.dep

OBJ_LIBPART	:= $(SRC_LIBPART:%.c=$(OBJ_DIR)/%.o)
DEP_LIBPART := $(SRC_LIBPART:%.c=$(DEP_DIR)/%.depend)

DEP_PROGRAM := $(OBJ_PROGRAM:%.o=%.d)
OBJ_PROGRAM	:= $(SRC_PROGRAM:%.c=$(OBJ_DIR)/%.o)
DEP_PROGRAM := $(SRC_PROGRAM:%.c=$(DEP_DIR)/%.depend)


#### FIXED PATH INFO ####
INCDIR_MYSELF	=./include/
INSTALL_DIR		=./install

####include .mk files as config####
include $(DEF_MKFILES) $(EXT_MKFILES)

####SETUP BUILD COMMANDS####
HDRS_EXTPATH	:= -I$(INCDIR_MYSELF) $(HDRS_EXTPATH)
CC		:=	$(CROSS_PREFIX)$(CC) $(HDRS_EXTPATH) $(CFLAGS) $(DEFS)
AR		:=	$(CROSS_PREFIX)$(AR)

LFLAGS	=	$(ALIB_EXTERNAL) $(SLIB_EXTPATH) $(SLIB_EXTERNAL) $(SLIB_INTERNAL)

OBJ		=	$(OBJ_LIBPART) $(OBJ_PROGRAM)
DEP		=	$(DEP_LIBPART) $(DEP_PROGRAM)

#ひとまずビルド。
all:	$(TARGET_ALIB)	$(TARGET_AOUT)

#ライブラリ用のビルドなので、target.aの生成を念頭に入れる。
$(TARGET_ALIB)	: $(OBJ_LIBPART)
	$(AR) rcs $(TARGET_ALIB) $(OBJ_LIBPART)
ifneq ($(strip $(TARGET_SLIB)),)
	$(CC) -shared -o $(TARGET_SLIB) $(OBJ_LIBPART)
endif
	mkdir -p $(INSTALL_DIR)/lib/
	cp -R -f -p $(TARGET_ALIB) $(INSTALL_DIR)/lib/
	cp -R -f -p $(INCDIR_MYSELF) $(INSTALL_DIR)
ifneq ($(strip $(TARGET_SLIB)),)
	cp -R -f -p $(TARGET_SLIB) $(INSTALL_DIR)/lib/
endif

#テスタービルド
ifneq ($(strip $(TARGET_AOUT)), )
$(TARGET_AOUT)	:$(TARGET_ALIB) $(OBJ_PROGRAM)
	$(CC) -o $(TARGET_AOUT) $(OBJ_PROGRAM) $(TARGET_ALIB) $(LFLAGS)
endif

#インストール。外部で指定したインストール先があるなら、そこに突っ込む。
install:	FORCE
ifneq ($(strip $(INSTALL_EXT)),)
	cp -R -f -p $(INSTALL_DIR)/* $(INSTALL_EXT)/
endif

#みんないなくなる。
clean:
	rm -rf $(TARGET_ALIB) $(TARGET_AOUT) $(TARGET_SLIB) $(OBJ_DIR) $(DEP_DIR) $(INSTALL_DIR)/*

#オブジェクトファイルを「所定の場所に」作成する
#ついでに依存関係のファイルを出す。
$(OBJ_DIR)/%.o : %.c
	mkdir -p $(dir $(DEP_DIR)/$(<:%.c=%.depend))
	$(CC) -MM $< -MT $@ > $(DEP_DIR)/$(<:%.c=%.depend)
	mkdir -p $(dir $@)
	$(CC) -c $< -o $@ -fPIC


#普通のビルド。
#.c.o:
#ifneq ($(strip $(TARGET_SLIB)),)
#	$(CC) -c -MMD $< -fPIC
#else
#	$(CC) -c -MMD $<
#endif

FORCE:

-include $(DEP)
