/*
 o----------------------------------------------------------------------------o
 |
 | MAD frontend
 |
 | Methodical Accelerator Design - Copyright CERN 2015
 | Support: http://cern.ch/mad  - mad at cern.ch
 | Authors: L. Deniau, laurent.deniau at cern.ch
 | Contrib: -
 |
 o----------------------------------------------------------------------------o
 | You can redistribute this file and/or modify it under the terms of the GNU
 | General Public License GPLv3 (or later), as published by the Free Software
 | Foundation. This file is distributed in the hope that it will be useful, but
 | WITHOUT ANY WARRANTY OF ANY KIND. See http://gnu.org/licenses for details.
 o----------------------------------------------------------------------------o
	
	Purpose:
	- Frontend (main) of the MAD application.

	Comment:
	- MAD is embedding the LuaJIT library and frontend developped by Mike Pall
		modified for the purpose of MAD. See COPYRIGHT.luajit in the lib/patches
		directory.
 o----------------------------------------------------------------------------o
 */

/*
** LuaJIT frontend. Runs commands, scripts, read-eval-print (REPL) etc.
** Copyright (C) 2005-2016 Mike Pall. See Copyright Notice in luajit.h
**
** Major portions taken verbatim or adapted from the Lua interpreter.
** Copyright (C) 1994-2008 Lua.org, PUC-Rio. See Copyright Notice in lua.h
*/

#define _GNU_SOURCE 1
#define _XOPEN_UNIX 1
#define _XOPEN_VERSION 700
#define _DARWIN_BETTER_REALPATH 1

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define luajit_c

#include "lua.h"
#include "lauxlib.h"
#include "lualib.h"
#include "luajit.h"

#include "lj_arch.h"

#if LJ_TARGET_POSIX
#include <unistd.h>
#define lua_stdin_is_tty()	isatty(0)
#elif LJ_TARGET_WINDOWS
#include <io.h>
#ifdef __BORLANDC__
#define lua_stdin_is_tty()	isatty(_fileno(stdin))
#else
#define lua_stdin_is_tty()	_isatty(_fileno(stdin))
#endif
#else
#define lua_stdin_is_tty()	1
#endif

#if !LJ_TARGET_CONSOLE
#include <signal.h>
#endif

static lua_State *globalL = NULL;
static const char *progname = "mad";

#if defined(__MINGW32__) || defined(__MINGW64__)
#include <sys/stat.h>

#undef     lua_stdin_is_tty
static int lua_stdin_is_tty (void)
{
  struct stat stats;
  fstat(0, &stats);
  return S_ISFIFO(stats.st_mode) || isatty(0);
}

/* Missing declaration in Mingw */
int setenv(const char *name, const char *value, int overwrite);
#endif

/* --- MAD (start) -----------------------------------------------------------*/

/* Assume Posix: MacOSX, Linux, Mingw32/64 or Cygwin */
#include <unistd.h>
#include <limits.h>
#include <time.h>
#include "mad_log.h"

#ifndef MAD_VERSION
#define MAD_VERSION "0.0.0"
#endif

int mad_trace_level    = 0;
int mad_trace_location = 0;

/* Forward declarations. */
static int dofile   (lua_State *L, const char *name);
static int dostring (lua_State *L, const char *s, const char *name);

static void print_version (void)
{
	const char *ver = MAD_VERSION " (" LJ_OS_NAME " " MKSTR(LJ_ARCH_BITS) ")";
	const char *msg = 
	"    ____  __   ______    ______     |   Methodical Accelerator Design\n"
	"     /  \\/  \\   /  _  \\   /  _  \\   |   release: %s\n"
	"    /  __   /  /  /_/ /  /  /_/ /   |   support: http://cern.ch/mad\n"
	"   /__/  /_/  /__/ /_/  /_____ /    |   licence: GPLv3 (C) CERN 2016\n"
	"                                    |   started: %s\n"
	"\n";

	time_t t = time(NULL);
	struct tm *tm = localtime(&t);
	char buf[80];

	strftime(buf, sizeof buf, "%Y-%m-%d %H:%M:%S", tm);
	printf(msg, ver, buf);
}

static const char*
logmsg (lua_State *L, const char *fname, const char *fmt, va_list va)
{
	if (mad_trace_location) {
		int n = 2;
		luaL_where(L, 1);
		if (fname) lua_pushfstring(L, "%s: ", fname), ++n;
		lua_pushvfstring(L, fmt, va);
		lua_concat(L, n);
	} else
		lua_pushvfstring(L, fmt, va);

	return lua_tostring(L, -1);
}

LUALIB_API int luaL_warn (lua_State *L, const char *fmt, ...)
{
	va_list va;
	va_start(va, fmt);
	fprintf(stderr, "warning: %s\n", logmsg(L, NULL, fmt, va));
	va_end(va);
	fflush(stderr);
	return 0;
}

LUALIB_API int luaL_trace (lua_State *L, const char *fmt, ...)
{
	if (mad_trace_level) {
		va_list va;
		va_start(va, fmt);
		fprintf(stderr, "trace: %s\n", logmsg(L, NULL, fmt, va));
		va_end(va);
		fflush(stderr);
	}
	return 0;
}

void (mad_error) (const char *fname, const char *fmt, ...)
{
	char msg[256];
	int n = 0;
	va_list va;
	va_start(va, fmt);
	if (fname) n = snprintf(msg, 255, "%s:", fname);
	vsnprintf(msg+n, 255-n, fmt, va);
	va_end(va);
	msg[255] = '\0';
	luaL_error(globalL, msg);
	exit(EXIT_FAILURE); /* never reached */
}

void (mad_warn) (const char *fname, const char *fmt, ...)
{
	va_list va;
	va_start(va, fmt);
	fprintf(stderr, "warning: %s\n", logmsg(globalL, fname, fmt, va));
	va_end(va);
	fflush(stderr);
}

void (mad_trace) (const char *fname, const char *fmt, ...)
{
	if (mad_trace_level) {
		va_list va;
		va_start(va, fmt);
		fprintf(stderr, "trace: %s\n", logmsg(globalL, fname, fmt, va));
		va_end(va);
		fflush(stderr);
	}
}

static int mad_luawarn (lua_State *L) {
	return luaL_warn(L, "%s", luaL_checkstring(L, 1));
}

static int mad_luatrace (lua_State *L) {
	if (mad_trace_level)
		return luaL_trace(L, "%s", luaL_checkstring(L, 1));
	else
		return 0;
}

/* Windows: not declared by any mean but provided by libgettextlib */
extern char *realpath (const char *restrict fname, char *restrict rname);

#if LUAJIT_OS == LUAJIT_OS_WINDOWS
static char* winpath (char *buf)
{
	char *p = buf;
	while ((p = strchr(p, '/'))) *p++ = '\\';
	return buf;
}

static char* winexe (char *buf)
{
	int len = strlen(buf);
	if (len > 0 && (len < 4 || strcmp(buf+len-4, ".exe")))
		strcpy(buf+len, ".exe");
	return buf;
}
#endif

static void setpaths (lua_State *L, int no_env)
{
	lua_register(L, "warn" , mad_luawarn );  
	lua_register(L, "trace", mad_luatrace);

	char prog_name[PATH_MAX+1] = "";
	char prog_path[PATH_MAX+1] = "";
	char curr_path[PATH_MAX+1] = "";
	char home_path[PATH_MAX+1] = "";
	char nam[PATH_MAX+4+1], buf[PATH_MAX+4+1];
	char *path, *p, nul = 0, psep = ':', dsep = '/';

	/* get curr_path [=getenv(unix:"PWD" or win:"CD")+cannonize] */
	if (!(path = getcwd(curr_path, sizeof curr_path))) *curr_path = nul;
	if (!path && !(path = getenv("PATH"))) path = &nul;

	/* retrieve separators */
	p = strchr(path, '/'); if (!p) p = strchr(path, '\\');
	if (p) dsep = *p, psep = *p == '\\' ? ';' : ':';

  /* get home_path */
#if LUAJIT_OS == LUAJIT_OS_WINDOWS
	if ((path = getenv("HOMEDRIVE"))) strcpy(home_path, path);
	if ((path = getenv("HOMEPATH" ))) strcat(home_path, path);
	winpath(home_path);
#else
	if ((path = getenv("HOME"     ))) strcpy(home_path, path);
#endif

	/* get prog_name */
	strcpy(nam, progname);
#if LUAJIT_OS == LUAJIT_OS_WINDOWS
	winexe(winpath(prog_path));  /* canonize */
#endif
	p = strrchr(nam, dsep);
	strcpy(prog_name, p ? p+1 : nam);

	/* get prog_path */
  if (realpath(nam, prog_path)) goto found; /* absolute path */
	if (snprintf(buf, sizeof buf, "%s%c%s", curr_path, dsep, nam) > 0 &&
		  realpath(buf, prog_path)) goto found; /* relative path */
	if ((path = getenv("PATH")) && *path) {   /* $PATH path */
		for(;;) {
			p = strchr(path, psep);
			if (p) *p = nul;
			if (snprintf(buf, sizeof buf, "%s%c%s", path, dsep, prog_name) > 0 &&
					realpath(buf, prog_path)) { if (p) *p = psep; goto found; } 
			if (p) *p = psep, path = p+1; else break;
		}
	}
  *prog_path = nul;

found:
#if LUAJIT_OS == LUAJIT_OS_WINDOWS
	winexe(winpath(prog_path));  /* canonize */
#endif
  if ((p=strrchr(prog_path, dsep)) && !strcmp(p+1, prog_name)) *p = nul;

	/* add trailing dir separator */
	if (*curr_path && (p=curr_path+strlen(curr_path))[-1] != dsep)
		p[0] = dsep, p[1] = nul;
	if (*home_path && (p=home_path+strlen(home_path))[-1] != dsep)
		p[0] = dsep, p[1] = nul;
	if (*prog_path && (p=prog_path+strlen(prog_path))[-1] != dsep)
		p[0] = dsep, p[1] = nul;

	/* set global table 'path' */
	lua_createtable(L, 0, 5);
  lua_pushstring(L, "currpath");
  lua_pushstring(L, curr_path);
	lua_rawset(L, -3);
  lua_pushstring(L, "homepath");
  lua_pushstring(L, home_path);
	lua_rawset(L, -3);
  lua_pushstring(L, "progpath");
  lua_pushstring(L, prog_path);
	lua_rawset(L, -3);
  lua_pushstring(L, "progname");
  lua_pushstring(L, prog_name);
	lua_rawset(L, -3);
	buf[0] = dsep, buf[1] = nul;
  lua_pushstring(L, "dirsep");
  lua_pushstring(L, buf);
	lua_rawset(L, -3);
	lua_setglobal(L, "path");

	if (no_env) return;

	const char *mpath = getenv("MAD_PATH"), *cpath  = getenv("MAD_CPATH");
	const char *lpath = getenv("LUA_PATH"), *lcpath = getenv("LUA_CPATH");
	int len, mlen, clen, marg, carg;

	trace(2, "LUA_PATH='%s'", lpath ? lpath : "");
	trace(2, "LUA_CPATH='%s'", lcpath ? lcpath : "");

	if (mpath) marg = 0, mlen = strlen(mpath);
	else
		mpath = "./?.mad;./?.lua;" 												// CURR_PATH
						/* MAD modules (user's home) */
						"%s.mad/?.mad;" 													// HOME_PATH	(1)
						"%s.mad/?.lua;" 													// HOME_PATH	(1)
						/* MAD modules (relative) */
						"%s?.mad;"																// MAD_PATH		(1)
						"%s?.lua;"																// MAD_PATH		(1)
#if LUAJIT_OS == LUAJIT_OS_WINDOWS
						/* MAD modules (relative) */
						"%s../mad/?.mad;"													// MAD_PATH		(1)
						"%s../mad/?.lua;"													// MAD_PATH		(1)
						/* From Lua unofficial uFAQ (relative) */
						"%s../Lua/5.1/?.lua;"											// MAD_PATH		(1)
						"%s../Lua/5.1/?/init.lua;"								// MAD_PATH		(1)
						"%s../Lua/5.1/lua/?.lua;"									// MAD_PATH		(1)
						"%s../Lua/5.1/lua/?/init.lua;"						// MAD_PATH		(1)
						/* From Lua unofficial uFAQ (absolute) */
						"C:/Program Files/Lua/5.1/?.lua;"
						"C:/Program Files/Lua/5.1/?/init.lua;"
						"C:/Program Files/Lua/5.1/lua/?.lua;"
						"C:/Program Files/Lua/5.1/lua/?/init.lua;",
		marg = 4+6,
#else
						/* MAD modules (relative) */
						"%s../share/mad/?.mad;"										// MAD_PATH		(1)
						"%s../share/mad/?.lua;"										// MAD_PATH		(1)
						/* From Lua unofficial uFAQ (relative) */
						"%s../share/lua/5.1/?.lua;"								// MAD_PATH		(1)
						"%s../share/lua/5.1/?/init.lua;"					// MAD_PATH		(1)
						"%s../lib/lua/5.1/?.lua;"									// MAD_PATH		(1)
						"%s../lib/lua/5.1/?/init.lua;"						// MAD_PATH		(1)
						/* From Lua unofficial uFAQ (/opt/local absolute) */
						"/opt/local/share/lua/5.1/?.lua;"
						"/opt/local/share/lua/5.1/?/init.lua;"
						"/opt/local/lib/lua/5.1/?.lua;"
						"/opt/local/lib/lua/5.1/?/init.lua;"
						/* From Lua unofficial uFAQ (/usr/local absolute) */
						"/usr/local/share/lua/5.1/?.lua;"
						"/usr/local/share/lua/5.1/?/init.lua;"
						"/usr/local/lib/lua/5.1/?.lua;"
						"/usr/local/lib/lua/5.1/?/init.lua;",
		marg = 4+6,
#endif
		mlen = strlen(mpath)-(2*marg)
					 + 2*strlen(home_path) + (marg-2)*strlen(prog_path);

	if (cpath) carg = 0, clen = strlen(cpath);
	else
		cpath = 
#if LUAJIT_OS == LUAJIT_OS_WINDOWS
						"./?.dll;./?.so;"													// CURR_PATH
						/* MAD modules (user's home) */
						"%s.mad/?.dll;"														// HOME_PATH	(1)
						"%s.mad/?.so;"														// HOME_PATH	(1)
						/* MAD modules (relative) */
						"%s?.dll;"																// MAD_PATH		(1)
						"%s?.so;"																	// MAD_PATH		(1)
						"%s../lib/mad/?.dll;"											// MAD_PATH		(1)
						"%s../lib/mad/?.so;" 											// MAD_PATH		(1)
						/* From Lua unofficial uFAQ (relative) */
						"%s../lib/lua/5.1/?.dll;"									// MAD_PATH		(1)
						"%s../lib/lua/5.1/?.so;"									// MAD_PATH		(1)
						"C:/Program Files/Lua/5.1/?.dll;"
						"C:/Program Files/Lua/5.1/?.so;"
						"C:/Program Files/Lua/5.1/clibs/?.dll;"
						"C:/Program Files/Lua/5.1/clibs/?.so;",
		carg = 8,
#elif LUAJIT_OS == LUAJIT_OS_OSX
						"./?.dylib;./?.so;"												// CURR_PATH
						/* MAD modules (user's home) */
						"%s.mad/?.dylib;"													// HOME_PATH	(1)
						"%s.mad/?.so;"														// HOME_PATH	(1)
						/* MAD modules (relative) */
						"%s?.dylib;"															// MAD_PATH		(1)
						"%s?.so;"																	// MAD_PATH		(1)
						"%s../lib/mad/?.dylib;"										// MAD_PATH		(1)
						"%s../lib/mad/?.so;"											// MAD_PATH		(1)
						/* From Lua unofficial uFAQ (relative) */
						"%s../lib/lua/5.1/?.dylib;"								// MAD_PATH		(1)
						"%s../lib/lua/5.1/?.so;"									// MAD_PATH		(1)
						/* From Lua unofficial uFAQ (/opt/local absolute) */
						"/opt/local/lib/lua/5.1/?.dylib;"
						"/opt/local/lib/lua/5.1/?.so;"
						/* From Lua unofficial uFAQ (/usr/local absolute) */
						"/usr/local/lib/lua/5.1/?.dylib;"
						"/usr/local/lib/lua/5.1/?.so;",
		carg = 8,
#else
						"./?.so;"																	// CURR_PATH
						/* MAD modules (user's home) */
						"%s.mad/?.so;"														// HOME_PATH	(1)
						/* MAD modules (relative) */
						"%s?.so;"																	// MAD_PATH		(1)
						"%s../lib/mad/?.so;"											// MAD_PATH		(1)
						/* From Lua unofficial uFAQ (relative) */
						"%s../lib/lua/5.1/?.so;"									// MAD_PATH		(1)
						/* From Lua unofficial uFAQ (/opt/local absolute) */
						"/opt/local/lib/lua/5.1/?.so;"
						/* From Lua unofficial uFAQ (/usr/local absolute) */
						"/usr/local/lib/lua/5.1/?.so;",
		carg = 4,
#endif
		clen = strlen(cpath)-(2*carg)
					 + 2*strlen(home_path) + (carg-2)*strlen(prog_path);

	/* LUA_PATH = $MAD_PATH;$LUA_PATH */
	char env[(mlen>clen?mlen:clen) + strlen(lpath ? lpath : "") + 1];
	ensure(marg == 10, "invalid number of path argument");
	len = snprintf(env, sizeof env, mpath, /* next args discarded without %s */
					 				home_path, home_path, prog_path, prog_path, prog_path,
					 				prog_path, prog_path, prog_path, prog_path, prog_path);
	if (lpath) strcat(env, lpath); else if (len>0) env[len-1] = nul;
#if LUAJIT_OS == LUAJIT_OS_WINDOWS
	winpath(env); /* canonize */
#endif

	trace(2, "set LUA_PATH='%s'", env);
	setenv("LUA_PATH", env, 1);
	
	/* LUA_CPATH = $MAD_CPATH;$LUA_CPATH */
	ensure(carg == 8 || carg == 4, "invalid number of path argument");
  if (carg == 4)
		len = snprintf(env, sizeof env, cpath, /* next args discarded without %s */
						 			 home_path, prog_path, prog_path, prog_path);
	else
		len = snprintf(env, sizeof env, cpath, /* next args discarded without %s */
						 			 home_path, home_path, prog_path, prog_path,
						 			 prog_path, prog_path, prog_path, prog_path);
	if (lcpath) strcat(env, lcpath); else if (len>0) env[len-1] = nul;
#if LUAJIT_OS == LUAJIT_OS_WINDOWS
	winpath(env); /* canonize */
#endif

	trace(2, "set LUA_CPATH='%s'", env);
	setenv("LUA_CPATH", env, 1);
}

static int handle_madinit (lua_State *L)
{
	const char *init = getenv("MAD_INIT");
	if (init == NULL)	return 0;  /* status OK */
	else if (init[0] == '@') return dofile(L, init+1);
	else return dostring(L, init, "=" "MAD_INIT");
}

/* --- MAD (end) -------------------------------------------------------------*/

#if !LJ_TARGET_CONSOLE
static void lstop(lua_State *L, lua_Debug *ar)
{
	(void)ar;  /* unused arg. */
	lua_sethook(L, NULL, 0, 0);
	/* Avoid luaL_error -- a C hook doesn't add an extra frame. */
	luaL_where(L, 0);
	lua_pushfstring(L, "%sinterrupted!", lua_tostring(L, -1));
	lua_error(L);
}

static void laction(int i)
{
	signal(i, SIG_DFL); /* if another SIGINT happens before lstop,
			 terminate process (default action) */
	lua_sethook(globalL, lstop, LUA_MASKCALL | LUA_MASKRET | LUA_MASKCOUNT, 1);
}
#endif

static void print_usage(void)
{
	fputs("usage: ", stderr);
	fputs(progname, stderr);
	fputs(" [options]... [script [args]...].\n"
	"Available options are:\n"
	"  -e chunk  Execute string " LUA_QL("chunk") ".\n"
	"  -l name   Require library " LUA_QL("name") ".\n"
	"  -b ...    Save or list bytecode.\n"
	"  -j cmd    Perform JIT control command.\n"
	"  -O[opt]   Control JIT optimizations.\n"
	"  -i        Enter interactive mode after executing " LUA_QL("script") ".\n"
	"  -q        Do not show version information.\n"
	"  -t[num]   Set initial trace level for debugging.\n"
	"  -E        Ignore environment variables.\n"
	"  --        Stop handling options.\n"
	"  -         Execute stdin and stop handling options.\n", stderr);
	fflush(stderr);
}

static void l_message(const char *pname, const char *msg)
{
	if (pname) { fputs(pname, stderr); fputc(':', stderr); fputc(' ', stderr); }
	fputs(msg, stderr); fputc('\n', stderr);
	fflush(stderr);
}

static int report(lua_State *L, int status)
{
	if (status && !lua_isnil(L, -1)) {
		const char *msg = lua_tostring(L, -1);
		if (msg == NULL) msg = "(error object is not a string)";
		l_message(progname, msg);
		lua_pop(L, 1);
	}
	return status;
}

static int traceback(lua_State *L)
{
	if (!lua_isstring(L, 1)) { /* Non-string error object? Try metamethod. */
		if (lua_isnoneornil(L, 1) ||
				!luaL_callmeta(L, 1, "__tostring") ||
				!lua_isstring(L, -1))
			return 1;  /* Return non-string error object. */
		lua_remove(L, 1);  /* Replace object by result of __tostring metamethod. */
	}
	luaL_traceback(L, L, lua_tostring(L, 1), 1);
	return 1;
}

static int docall(lua_State *L, int narg, int clear)
{
	int status;
	int base = lua_gettop(L) - narg;  /* function index */
	lua_pushcfunction(L, traceback);  /* push traceback function */
	lua_insert(L, base);  /* put it under chunk and args */
#if !LJ_TARGET_CONSOLE
	signal(SIGINT, laction);
#endif
	status = lua_pcall(L, narg, (clear ? 0 : LUA_MULTRET), base);
#if !LJ_TARGET_CONSOLE
	signal(SIGINT, SIG_DFL);
#endif
	lua_remove(L, base);  /* remove traceback function */
	/* force a complete garbage collection in case of errors */
	if (status != 0) lua_gc(L, LUA_GCCOLLECT, 0);
	return status;
}

static void print_jit_status(lua_State *L)
{
	int n;
	const char *s;
	lua_getfield(L, LUA_REGISTRYINDEX, "_LOADED");
	lua_getfield(L, -1, "jit");  /* Get jit.* module table. */
	lua_remove(L, -2);
	lua_getfield(L, -1, "status");
	lua_remove(L, -2);
	n = lua_gettop(L);
	lua_call(L, 0, LUA_MULTRET);
	fputs(lua_toboolean(L, n) ? "JIT: ON" : "JIT: OFF", stdout);
	for (n++; (s = lua_tostring(L, n)); n++) {
		putc(' ', stdout);
		fputs(s, stdout);
	}
	putc('\n', stdout);
}

static int setargs(lua_State *L, char **argv, int n)
{
	int narg;
	int i;
	int argc = 0;
	while (argv[argc]) argc++;  /* count total number of arguments */
	narg = argc - (n + 1);  /* number of arguments to the script */
	luaL_checkstack(L, narg + 3, "too many arguments to script");
	for (i = n+1; i < argc; i++)
		lua_pushstring(L, argv[i]);
	lua_createtable(L, narg, n + 1);
	for (i = 0; i < argc; i++) {
		lua_pushstring(L, argv[i]);
		lua_rawseti(L, -2, i - n);
	}
	lua_setglobal(L, "arg");
	return narg;
}

static int dofile(lua_State *L, const char *name)
{
	int status = luaL_loadfile(L, name) || docall(L, 0, 1);
	return report(L, status);
}

static int dostring(lua_State *L, const char *s, const char *name)
{
	int status = luaL_loadbuffer(L, s, strlen(s), name) || docall(L, 0, 1);
	return report(L, status);
}

static int dolibrary(lua_State *L, const char *name)
{
	lua_getglobal(L, "require");
	lua_pushstring(L, name);
	return report(L, docall(L, 1, 1));
}

static void write_prompt(lua_State *L, int firstline)
{
	const char *p;
	lua_getfield(L, LUA_GLOBALSINDEX, firstline ? "_PROMPT" : "_PROMPT2");
	p = lua_tostring(L, -1);
	if (p == NULL) p = firstline ? LUA_PROMPT : LUA_PROMPT2;
	fputs(p, stdout);
	fflush(stdout);
	lua_pop(L, 1);  /* remove global */
}

static int incomplete(lua_State *L, int status)
{
	if (status == LUA_ERRSYNTAX) {
		size_t lmsg;
		const char *msg = lua_tolstring(L, -1, &lmsg);
		const char *tp = msg + lmsg - (sizeof(LUA_QL("<eof>")) - 1);
		if (strstr(msg, LUA_QL("<eof>")) == tp) {
			lua_pop(L, 1);
			return 1;
		}
	}
	return 0;  /* else... */
}

static int pushline(lua_State *L, int firstline)
{
	char buf[LUA_MAXINPUT];
	write_prompt(L, firstline);
	if (fgets(buf, LUA_MAXINPUT, stdin)) {
		size_t len = strlen(buf);
		if (len > 0 && buf[len-1] == '\n') buf[len-1] = '\0';
		if (firstline && buf[0] == '=')
			lua_pushfstring(L, "return %s", buf+1);
		else
			lua_pushstring(L, buf);
		return 1;
	}
	return 0;
}

static int loadline(lua_State *L)
{
	int status;
	lua_settop(L, 0);
	if (!pushline(L, 1)) return -1;  /* no input */
	for (;;) {  /* repeat until gets a complete line */
		status = luaL_loadbuffer(L, lua_tostring(L, 1), lua_strlen(L, 1), "=stdin");
		if (!incomplete(L, status)) break;  /* cannot try to add lines? */
		if (!pushline(L, 0)) return -1; /* no more input? */
		lua_pushliteral(L, "\n");  /* add a new line... */
		lua_insert(L, -2);  /* ...between the two lines */
		lua_concat(L, 3);  /* join them */
	}
	lua_remove(L, 1);  /* remove line */
	return status;
}

static void dotty(lua_State *L)
{
	int status;
	const char *oldprogname = progname;
	progname = NULL;
	while ((status = loadline(L)) != -1) {
		if (status == 0) status = docall(L, 0, 0);
		report(L, status);
		if (status == 0 && lua_gettop(L) > 0) {  /* any result to print? */
			lua_getglobal(L, "print");
			lua_insert(L, 1);
			if (lua_pcall(L, lua_gettop(L)-1, 0, 0) != 0)
				l_message(progname,
									lua_pushfstring(L, "error calling " LUA_QL("print") " (%s)",
									lua_tostring(L, -1)));
		}
	}
	lua_settop(L, 0);  /* clear stack */
	fputs("\n", stdout);
	fflush(stdout);
	progname = oldprogname;
}

static int handle_script(lua_State *L, char **argv, int n, int narg)
{
	int status;
	const char *fname;
	fname = argv[n];
	if (strcmp(fname, "-") == 0 && strcmp(argv[n-1], "--") != 0)
		fname = NULL;  /* stdin */
	status = luaL_loadfile(L, fname);
	lua_insert(L, -(narg+1));
	if (status == 0) status = docall(L, narg, 0);
	else lua_pop(L, narg);
	return report(L, status);
}

/* Load add-on module. */
static int loadjitmodule(lua_State *L)
{
	lua_getglobal(L, "require");
	lua_pushliteral(L, "jit.");
	lua_pushvalue(L, -3);
	lua_concat(L, 2);
	if (lua_pcall(L, 1, 1, 0)) {
		const char *msg = lua_tostring(L, -1);
		if (msg && !strncmp(msg, "module ", 7))	goto nomodule;
		return report(L, 1);
	}
	lua_getfield(L, -1, "start");
	if (lua_isnil(L, -1)) {
	nomodule:
		l_message(progname,
			"unknown luaJIT command or jit.* modules not installed");
		return 1;
	}
	lua_remove(L, -2);  /* Drop module table. */
	return 0;
}

/* Run command with options. */
static int runcmdopt(lua_State *L, const char *opt)
{
	int narg = 0;
	if (opt && *opt) {
		for (;;) {  /* Split arguments. */
			const char *p = strchr(opt, ',');
			narg++;
			if (!p) break;
			if (p == opt) lua_pushnil(L);
			else lua_pushlstring(L, opt, (size_t)(p - opt));
			opt = p + 1;
		}
		if (*opt)	lua_pushstring(L, opt);
		else			lua_pushnil(L);
	}
	return report(L, lua_pcall(L, narg, 0, 0));
}

/* JIT engine control command: try jit library first or load add-on module. */
static int dojitcmd(lua_State *L, const char *cmd)
{
	const char *opt = strchr(cmd, '=');
	lua_pushlstring(L, cmd, opt ? (size_t)(opt - cmd) : strlen(cmd));
	lua_getfield(L, LUA_REGISTRYINDEX, "_LOADED");
	lua_getfield(L, -1, "jit");  /* Get jit.* module table. */
	lua_remove(L, -2);
	lua_pushvalue(L, -2);
	lua_gettable(L, -2);  /* Lookup library function. */
	if (!lua_isfunction(L, -1)) {
		lua_pop(L, 2);  /* Drop non-function and jit.* table, keep module name. */
		if (loadjitmodule(L)) return 1;
	} else
		lua_remove(L, -2);  /* Drop jit.* table. */
	lua_remove(L, -2);  /* Drop module name. */
	return runcmdopt(L, opt ? opt+1 : opt);
}

/* Optimization flags. */
static int dojitopt(lua_State *L, const char *opt)
{
	lua_getfield(L, LUA_REGISTRYINDEX, "_LOADED");
	lua_getfield(L, -1, "jit.opt");  /* Get jit.opt.* module table. */
	lua_remove(L, -2);
	lua_getfield(L, -1, "start");
	lua_remove(L, -2);
	return runcmdopt(L, opt);
}

/* Save or list bytecode. */
static int dobytecode(lua_State *L, char **argv)
{
	int narg = 0;
	lua_pushliteral(L, "bcsave");
	if (loadjitmodule(L)) return 1;
	if (argv[0][2]) {
		narg++;
		argv[0][1] = '-';
		lua_pushstring(L, argv[0]+1);
	}
	for (argv++; *argv != NULL; narg++, argv++)
		lua_pushstring(L, *argv);
	return report(L, lua_pcall(L, narg, 0, 0));
}

/* check that argument has no extra characters at the end */
#define notail(x)	{if ((x)[2] != '\0') return -1;}

#define FLAGS_INTERACTIVE	1
#define FLAGS_VERSION			2
#define FLAGS_EXEC				4
#define FLAGS_OPTION			8
#define FLAGS_NOENV				16

static int collectargs(char **argv, int *flags)
{
	int i;
	*flags |= FLAGS_VERSION;
	for (i = 1; argv[i] != NULL; i++) {
		if (argv[i][0] != '-')  /* Not an option? */
			return i;
		switch (argv[i][1]) {  /* Check option. */
		case '-':
			notail(argv[i]);
			return (argv[i+1] != NULL ? i+1 : 0);
		case '\0':
			return i;
		case 'i':
			notail(argv[i]);
			*flags |= FLAGS_INTERACTIVE;
			break;
		case 'q':
			notail(argv[i]);
			*flags &= ~FLAGS_VERSION;
			break;
		case 'e':
			*flags |= FLAGS_EXEC;
		case 'j':  /* LuaJIT extension */
		case 'l':
			*flags |= FLAGS_OPTION;
			if (argv[i][2] == '\0' && argv[++i] == NULL) return -1;
			break;
		case 'O': break;  /* LuaJIT extension */
		case 'b':  /* LuaJIT extension */
			if ((*flags &= ~FLAGS_VERSION)) return -1;
			*flags |= FLAGS_EXEC;
			return 0;
		case 'E':
			*flags |= FLAGS_NOENV;
			break;
		case 'T': /* MAD initial trace level */
			mad_trace_location = 1;
		case 't': /* MAD initial trace level */
			mad_trace_level = argv[i][2] ? strtol(argv[i]+2, NULL, 0) : 1;
			break;
		default: return -1;  /* invalid option */
		}
	}
	return 0;
}

static int runargs(lua_State *L, char **argv, int n)
{
	int i;
	for (i = 1; i < n; i++) {
		if (argv[i] == NULL) continue;
		lua_assert(argv[i][0] == '-');
		switch (argv[i][1]) {  /* option */
		case 'e': {
			const char *chunk = argv[i] + 2;
			if (*chunk == '\0') chunk = argv[++i];
			lua_assert(chunk != NULL);
			if (dostring(L, chunk, "=(command line)") != 0)	return 1;
		} break;
		case 'l': {
			const char *filename = argv[i] + 2;
			if (*filename == '\0') filename = argv[++i];
			lua_assert(filename != NULL);
			if (dolibrary(L, filename))	return 1;  /* stop if file fails */
		} break;
		case 'j': {  /* LuaJIT extension */
			const char *cmd = argv[i] + 2;
			if (*cmd == '\0') cmd = argv[++i];
			lua_assert(cmd != NULL);
			if (dojitcmd(L, cmd))	return 1;
		} break;
		case 'O':  /* LuaJIT extension */
			if (dojitopt(L, argv[i] + 2)) return 1;
			break;
		case 'b':  /* LuaJIT extension */
			return dobytecode(L, argv+i);
		default: break;
		}
	}
	return 0;
}

static int handle_luainit(lua_State *L)
{
#if LJ_TARGET_CONSOLE
  const char *init = NULL;
#else
  const char *init = getenv(LUA_INIT);
#endif
	if (init == NULL)	return 0;  /* status OK */
	else if (init[0] == '@') return dofile(L, init+1);
	else return dostring(L, init, "=" LUA_INIT);
}

static struct Smain {
	char **argv;
	int argc;
	int status;
} smain;

static int pmain(lua_State *L)
{
	struct Smain *s = &smain;
	char **argv = s->argv;
	int script, narg;
	int flags = 0;
	globalL = L;
	if (argv[0] && argv[0][0]) progname = argv[0];
	LUAJIT_VERSION_SYM();  /* linker-enforced version check */
	script = collectargs(argv, &flags);
	if (script < 0) {  /* invalid args? */
		print_usage();
		s->status = 1;
		return 0;
	}
	setpaths(L, flags & FLAGS_NOENV);
	narg = setargs(L, argv, (script > 0) ? script : s->argc); /* set arg */
	if ((flags & FLAGS_NOENV)) {
		lua_pushboolean(L, 1);
		lua_setfield(L, LUA_REGISTRYINDEX, "LUA_NOENV");
	}
	lua_gc(L, LUA_GCSTOP, 0);  /* stop collector during initialization */
	luaL_openlibs(L);  /* open libraries */
	lua_gc(L, LUA_GCRESTART, -1);
	if (!(flags & FLAGS_NOENV)) {
		s->status = handle_luainit(L);
		if (s->status != 0) return 0;
		s->status = handle_madinit(L);
		if (s->status != 0) return 0;
	}
	if ((flags & FLAGS_VERSION)) print_version();
	s->status = runargs(L, argv, (script > 0) ? script : s->argc);
	if (s->status != 0) return 0;
	if (script) {
		s->status = handle_script(L, argv, script, narg);
		if (s->status != 0) return 0;
	}
	if ((flags & FLAGS_INTERACTIVE)) {
		(void)print_jit_status;
		dotty(L);
	} else if (script == 0 && !(flags & FLAGS_EXEC)) {
		if (lua_stdin_is_tty()) {
			(void)print_jit_status;
			dotty(L);
		} else {
			dofile(L, NULL);  /* executes stdin as a file */
		}
	}
	return 0;
}

int main(int argc, char **argv)
{
	int status;
	lua_State *L = lua_open();  /* create state */
	if (L == NULL) {
		l_message(argv[0], "cannot create state: not enough memory");
		return EXIT_FAILURE;
	}
	smain.argc = argc;
	smain.argv = argv;
	status = lua_cpcall(L, pmain, NULL);
	report(L, status);
	lua_close(L);
	return (status || smain.status) ? EXIT_FAILURE : EXIT_SUCCESS;
}

