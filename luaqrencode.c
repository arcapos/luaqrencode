/*
 * Copyright (c) 2020 - 2021 Micro Systems Marc Balmer, CH-5073 Gipf-Oberfrick
 *
 * Permission is hereby granted, free of charge, to any person obtaining
 * a copy of this software and associated documentation files (the
 * "Software"), to deal in the Software without restriction, including
 * without limitation the rights to use, copy, modify, merge, publish,
 * distribute, sublicense, and/or sell copies of the Software, and to
 * permit persons to whom the Software is furnished to do so, subject to
 * the following conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 * IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
 * CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
 * TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
 * SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
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

/* How we return QR code data might change in a future release */
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
		{ "__gc",		luaqrcode_free },
		{ "__close",		luaqrcode_free },
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

		lua_pushliteral(L, "__metatable");
		lua_pushliteral(L, "must not access this metatable");
		lua_settable(L, -3);
	}
	lua_pop(L, 1);


	luaL_newlib(L, luaqrencode);
	lua_pushliteral(L, "_COPYRIGHT");
	lua_pushliteral(L, "Copyright (C) 2020 - 2021 by "
	    "micro systems marc balmer");
	lua_settable(L, -3);
	lua_pushliteral(L, "_DESCRIPTION");
	lua_pushliteral(L, "QR code generation functions for Lua");
	lua_settable(L, -3);
	lua_pushliteral(L, "_VERSION");
	lua_pushliteral(L, "qrencode 1.0.1");
	lua_settable(L, -3);

	return 1;
}
