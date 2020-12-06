# Makefile for luaqrencode library for Lua

LIBNAME= luaqrencode

LUAEXE= lua

ROCKSPEC= $(shell find . -name $(LIBNAME)-*-*.rockspec)

all: install

install:
	luarocks make $(ROCKSPEC)

test: 
	$(LUAEXE) test/test.lua

.PHONY: all test install
