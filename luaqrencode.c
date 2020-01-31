/*
 * Copyright (c) 2020 Micro Systems Marc Balmer, CH-5073 Gipf-Oberfrick
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the distribution.
 *     * Neither the name of Micro Systems Marc Balmer nor the
 *       names of its contributors may be used to endorse or promote products
 *       derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL MICRO SYSTEMS MARC BALMER BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

/* QR code generation functions for Lua */

#include <qrencode.h>

#include <lua.h>
#include <lauxlib.h>

#include "luaqrencode.h"

static int qr_encoding[] = {
	QR_MODE_NUM,
	QR_MODE_AN,
	QR_MODE_8,
	QR_MODE_KANJI,
	QR_MODE_ECI,
	QR_MODE_FNC1FIRST,
	QR_MODE_FNC1SECOND
};

static const char *qr_encoding_opt[] = {
	"numeric",
	"alphanumeric",
	"8bit",
	"kanji",
	"eci",
	"fnc1first",
	"fnc1second",
	NULL
};

static int qr_eclevel[] = {
	QR_ECLEVEL_L,
	QR_ECLEVEL_M,
	QR_ECLEVEL_Q,
	QR_ECLEVEL_H
};

static const char *qr_eclevel_opt[] = {
	"l",
	"m",
	"q",
	"h",
	NULL
};

static int
luaqrencode_apiversion(lua_State *L)
{
	int major, minor, micro;

	QRcode_APIVersion(&major, &minor, &micro);
	lua_newtable(L);
	lua_pushinteger(L, major);
	lua_setfield(L, -2, "major");
	lua_pushinteger(L, minor);
	lua_setfield(L, -2, "minor");
	lua_pushinteger(L, micro);
	lua_setfield(L, -2, "micro");
	return 1;
}

static int
luaqrencode_apiversionstring(lua_State *L)
{
	lua_pushstring(L, QRcode_APIVersionString());
	return 1;
}

/* How we return QR code data migth change in a future release */
static int
luaqrencode_encodedata(lua_State *L)
{
	QRcode *qrcode;
	const unsigned char *data;
	int n;
	size_t len;

	data = luaL_checklstring(L, 1, &len);
	qrcode = QRcode_encodeData(len, data, 0,
	    qr_eclevel[luaL_checkoption(L, 2, "l", qr_eclevel_opt)]);
	len = qrcode->width * qrcode->width;
	for (n = 0; n < len; n++)
		qrcode->data[n] = qrcode->data[n] & 0x01 ? '#' : ' ';
	lua_pushinteger(L, qrcode->version);
	lua_pushinteger(L, qrcode->width);
	lua_pushlstring(L, qrcode->data, len);
	return 3;
}

static int
luaqrcode_free(lua_State *L)
{
	QRcode **qrcode;

	qrcode = luaL_checkudata(L, 1, QRCODE_METATABLE);
	if (*qrcode) {
		QRcode_free(*qrcode);
		*qrcode = NULL;
	}
	return 0;
}

int
luaopen_qrencode(lua_State *L)
{
	int n;
	struct luaL_Reg luaqrencode[] = {
		{ "APIVersion",		luaqrencode_apiversion },
		{ "APIVersionString",	luaqrencode_apiversionstring },
		{ "encodeData",		luaqrencode_encodedata },
		{ NULL, NULL }
	};
	struct luaL_Reg qrcode_methods[] = {
		{ NULL, NULL }
	};
	if (luaL_newmetatable(L, QRCODE_METATABLE)) {
		luaL_setfuncs(L, qrcode_methods, 0);

		lua_pushliteral(L, "__index");
		lua_pushvalue(L, -2);
		lua_settable(L, -3);

		lua_pushliteral(L, "__gc");
		lua_pushcfunction(L, luaqrcode_free);
		lua_settable(L, -3);

		lua_pushliteral(L, "__metatable");
		lua_pushliteral(L, "must not access this metatable");
		lua_settable(L, -3);
	}
	lua_pop(L, 1);


	luaL_newlib(L, luaqrencode);
	lua_pushliteral(L, "_COPYRIGHT");
	lua_pushliteral(L, "Copyright (C) 2020 by "
	    "micro systems marc balmer");
	lua_settable(L, -3);
	lua_pushliteral(L, "_DESCRIPTION");
	lua_pushliteral(L, "QR code generation functions for Lua");
	lua_settable(L, -3);
	lua_pushliteral(L, "_VERSION");
	lua_pushliteral(L, "qrencode 1.0.0");
	lua_settable(L, -3);

	return 1;
}
