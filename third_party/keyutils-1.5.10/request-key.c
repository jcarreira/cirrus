/* request-key.c: hand a key request off to the appropriate process
 *
 * Copyright (C) 2005 Red Hat, Inc. All Rights Reserved.
 * Written by David Howells (dhowells@redhat.com)
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version
 * 2 of the License, or (at your option) any later version.
 *
 * /sbin/request-key <op> <key> <uid> <gid> <threadring> <processring> <sessionring> [<info>]
 *
 * Searches the specified session ring for a key indicating the command to run:
 *	type:	"user"
 *	desc:	"request-key:<op>"
 *	data:	command name, eg: "/home/dhowells/request-key-create.sh"
 */

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <signal.h>
#include <syslog.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <ctype.h>
#include <sys/select.h>
#include <sys/wait.h>
#include "keyutils.h"


static int xdebug;
static int xnolog;
static char *xkey;
static char *xuid;
static char *xgid;
static char *xthread_keyring;
static char *xprocess_keyring;
static char *xsession_keyring;
static char conffile[256];
static int confline;
static int norecurse;

static void lookup_action(char *op,
			  key_serial_t key,
			  char *ktype,
			  char *kdesc,
			  char *callout_info)
	__attribute__((noreturn));

static void execute_program(char *op,
			    key_serial_t key,
			    char *ktype,
			    char *kdesc,
			    char *callout_info,
			    char *cmdline)
	__attribute__((noreturn));

static void pipe_to_program(char *op,
			    key_serial_t key,
			    char *ktype,
			    char *kdesc,
			    char *callout_info,
			    char *prog,
			    char **argv)
	__attribute__((noreturn));

static int match(const char *pattern, int plen, const char *datum, int dlen);

static void debug(const char *fmt, ...) __attribute__((format(printf, 1, 2)));
static void debug(const char *fmt, ...)
{
	va_list va;

	if (xdebug) {
		va_start(va, fmt);
		vfprintf(stderr, fmt, va);
		va_end(va);

		if (!xnolog) {
			openlog("request-key", 0, LOG_AUTHPRIV);

			va_start(va, fmt);
			vsyslog(LOG_DEBUG, fmt, va);
			va_end(va);

			closelog();
		}
	}
}

static void error(const char *fmt, ...) __attribute__((noreturn, format(printf, 1, 2)));
static void error(const char *fmt, ...)
{
	va_list va;

	if (xdebug) {
		va_start(va, fmt);
		vfprintf(stderr, fmt, va);
		va_end(va);
	}

	if (!xnolog) {
		openlog("request-key", 0, LOG_AUTHPRIV);

		va_start(va, fmt);
		vsyslog(LOG_ERR, fmt, va);
		va_end(va);

		closelog();
	}

	exit(1);
}

#define file_error(FMT, ...)  error("%s: "FMT, conffile, ## __VA_ARGS__)
#define line_error(FMT, ...)  error("%s:%d: "FMT, conffile, confline, ## __VA_ARGS__)

static void oops(int x)
{
	error("Died on signal %d", x);
}

/*****************************************************************************/
/*
 *
 */
int main(int argc, char *argv[])
{
	key_serial_t key;
	char *ktype, *kdesc, *buf, *callout_info;
	int ret, ntype, dpos, n, fd;

	if (argc == 2 && strcmp(argv[1], "--version") == 0) {
		printf("request-key from %s (Built %s)\n",
		       keyutils_version_string, keyutils_build_string);
		return 0;
	}

	signal(SIGSEGV, oops);
	signal(SIGBUS, oops);
	signal(SIGPIPE, SIG_IGN);

	for (;;) {
		if (argc > 1 && strcmp(argv[1], "-d") == 0) {
			xdebug++;
			argv++;
			argc--;
		}
		else if (argc > 1 && strcmp(argv[1], "-n") == 0) {
			xnolog = 1;
			argv++;
			argc--;
		}
		else
			break;
	}

	if (argc != 8 && argc != 9)
		error("Unexpected argument count: %d\n", argc);

	fd = open("/dev/null", O_RDWR);
	if (fd < 0)
		error("open");
	if (fd > 2) {
		close(fd);
	}
	else if (fd < 2) {
		ret = dup(fd);
		if (ret < 0)
			error("dup failed: %m\n");

		if (ret < 2 && dup(fd) < 0)
			error("dup failed: %m\n");
	}

	xkey = argv[2];
	xuid = argv[3];
	xgid = argv[4];
	xthread_keyring = argv[5];
	xprocess_keyring = argv[6];
	xsession_keyring = argv[7];

	key = atoi(xkey);

	/* assume authority over the key
	 * - older kernel doesn't support this function
	 */
	ret = keyctl_assume_authority(key);
	if (ret < 0 && !(argc == 9 || errno == EOPNOTSUPP))
		error("Failed to assume authority over key %d (%m)\n", key);

	/* ask the kernel to describe the key to us */
	if (xdebug < 2) {
		ret = keyctl_describe_alloc(key, &buf);
		if (ret < 0)
			goto inaccessible;
	}
	else {
		buf = strdup("user;0;0;1f0000;debug:1234");
	}

	/* extract the type and description from the key */
	debug("Key descriptor: \"%s\"\n", buf);
	ntype = -1;
	dpos = -1;

	n = sscanf(buf, "%*[^;]%n;%*d;%*d;%x;%n", &ntype, &n, &dpos);
	if (n != 1)
		error("Failed to parse key description\n");

	ktype = buf;
	ktype[ntype] = 0;
	kdesc = buf + dpos;

	debug("Key type: %s\n", ktype);
	debug("Key desc: %s\n", kdesc);

	/* get hold of the callout info */
	callout_info = argv[8];

	if (!callout_info) {
		void *tmp;

		if (keyctl_read_alloc(KEY_SPEC_REQKEY_AUTH_KEY, &tmp) < 0)
			error("Failed to retrieve callout info (%m)\n");

		callout_info = tmp;
	}

	debug("CALLOUT: '%s'\n", callout_info);

	/* determine the action to perform */
	lookup_action(argv[1],		/* op */
		      key,		/* ID of key under construction */
		      ktype,		/* key type */
		      kdesc,		/* key description */
		      callout_info	/* call out information */
		      );

inaccessible:
	error("Key %d is inaccessible (%m)\n", key);

} /* end main() */

/*****************************************************************************/
/*
 * determine the action to perform
 */
static void lookup_action(char *op,
			  key_serial_t key,
			  char *ktype,
			  char *kdesc,
			  char *callout_info)
{
	char buf[4096 + 2], *p, *q;
	FILE *conf;
	int len, oplen, ktlen, kdlen, cilen;

	oplen = strlen(op);
	ktlen = strlen(ktype);
	kdlen = strlen(kdesc);
	cilen = strlen(callout_info);

	/* search the config file for a command to run */
	if (strlen(ktype) <= sizeof(conffile) - 30) {
		if (xdebug < 2)
			snprintf(conffile, sizeof(conffile) - 1,
				 "/etc/request-key.d/%s.conf", ktype);
		else
			snprintf(conffile, sizeof(conffile) - 1,
				 "request-key.d/%s.conf", ktype);
		conf = fopen(conffile, "r");
		if (conf)
			goto opened_conf_file;
		if (errno != ENOENT)
			error("Cannot open %s: %m\n", conffile);
	}

	if (xdebug < 2)
		snprintf(conffile, sizeof(conffile) - 1, "/etc/request-key.conf");
	else
		snprintf(conffile, sizeof(conffile) - 1, "request-key.conf");
	conf = fopen(conffile, "r");
	if (!conf)
		error("Cannot open %s: %m\n", conffile);

opened_conf_file:
	debug("Opened config file '%s'\n", conffile);

	for (confline = 1;; confline++) {
		/* read the file line-by-line */
		if (!fgets(buf, sizeof(buf), conf)) {
			if (feof(conf))
				error("Cannot find command to construct key %d\n", key);
			file_error("error %m\n");
		}

		len = strlen(buf);
		if (len >= sizeof(buf) - 2)
			line_error("Line too long\n");

		/* ignore blank lines and comments */
		if (len == 1 || buf[0] == '#' || isspace(buf[0]))
			continue;

		buf[--len] = 0;
		p = buf;

		/* attempt to match the op */
		q = p;
		while (*p && !isspace(*p)) p++;
		if (!*p)
			goto syntax_error;
		*p = 0;

		if (!match(q, p - q, op, oplen))
			continue;

		p++;

		/* attempt to match the type */
		while (isspace(*p)) p++;
		if (!*p)
			goto syntax_error;

		q = p;
		while (*p && !isspace(*p)) p++;
		if (!*p)
			goto syntax_error;
		*p = 0;

		if (!match(q, p - q, ktype, ktlen))
			continue;

		p++;

		/* attempt to match the description */
		while (isspace(*p)) p++;
		if (!*p)
			goto syntax_error;

		q = p;
		while (*p && !isspace(*p)) p++;
		if (!*p)
			goto syntax_error;
		*p = 0;

		if (!match(q, p - q, kdesc, kdlen))
			continue;

		p++;

		/* attempt to match the callout info */
		while (isspace(*p)) p++;
		if (!*p)
			goto syntax_error;

		q = p;
		while (*p && !isspace(*p)) p++;
		if (!*p)
			goto syntax_error;
		*p = 0;

		if (!match(q, p - q, callout_info, cilen))
			continue;

		p++;

		debug("%s:%d: Line matches\n", conffile, confline);

		/* we've got an action */
		while (isspace(*p)) p++;
		if (!*p)
			goto syntax_error;

		fclose(conf);

		execute_program(op, key, ktype, kdesc, callout_info, p);
	}

	file_error("No matching action\n");

syntax_error:
	line_error("Syntax error\n");

} /* end lookup_action() */

/*****************************************************************************/
/*
 * attempt to match a datum to a pattern
 * - one asterisk is allowed anywhere in the pattern to indicate a wildcard
 * - returns true if matched, false if not
 */
static int match(const char *pattern, int plen, const char *datum, int dlen)
{
	const char *asterisk;
	int n;

	debug("match(%*.*s,%*.*s)\n", plen, plen, pattern, dlen, dlen, datum);

	asterisk = memchr(pattern, '*', plen);
	if (!asterisk) {
		/* exact match only if no wildcard */
		if (plen == dlen && memcmp(pattern, datum, dlen) == 0)
			goto yes;
		goto no;
	}

	/* the datum mustn't be shorter than the pattern without the asterisk */
	if (dlen < plen - 1)
		goto no;

	n = asterisk - pattern;
	if (n == 0) {
		/* wildcard at beginning of pattern */
		pattern++;
		if (!*pattern)
			goto yes; /* "*" matches everything */

		/* match the end of the datum */
		plen--;
		if (memcmp(pattern, datum + (dlen - plen), plen) == 0)
			goto yes;
		goto no;
	}

	/* need to match beginning of datum for "abc*" and "abc*def" */
	if (memcmp(pattern, datum, n) != 0)
		goto no;

	if (!asterisk[1])
		goto yes; /* "abc*" matches */

	/* match the end of the datum */
	asterisk++;
	n = plen - n - 1;
	if (memcmp(pattern, datum + (dlen - n), n) == 0)
		goto yes;

no:
	debug(" = no\n");
	return 0;

yes:
	debug(" = yes\n");
	return 1;

} /* end match() */

/*****************************************************************************/
/*
 * execute a program to deal with a key
 */
static void execute_program(char *op,
			    key_serial_t key,
			    char *ktype,
			    char *kdesc,
			    char *callout_info,
			    char *cmdline)
{
	char *argv[256];
	char *prog, *p, *q;
	int argc, pipeit;

	debug("execute_program('%s','%s')\n", callout_info, cmdline);

	/* if the commandline begins with a bar, then we pipe the callout data into it and read
	 * back the payload data
	 */
	pipeit = 0;

	if (cmdline[0] == '|') {
		pipeit = 1;
		cmdline++;
	}

	/* extract the path to the program to run */
	prog = p = cmdline;
	while (*p && !isspace(*p)) p++;
//	if (!*p)
//		line_error("No command path\n");
//	*p++ = 0;
	if (*p)
		*p++ = 0;

	argv[0] = strrchr(prog, '/') + 1;

	/* extract the arguments */
	for (argc = 1; p; argc++) {
		while (isspace(*p)) p++;
		if (!*p)
			break;

		if (argc >= 254)
			line_error("Too many arguments\n");
		argv[argc] = q = p;

		while (*p && !isspace(*p)) p++;

		if (*p)
			*p++ = 0;
		else
			p = NULL;

		debug("argv[%d]: '%s'\n", argc, argv[argc]);

		if (*q != '%')
			continue;

		/* it's a macro */
		q++;
		if (!*q)
			line_error("Missing macro name\n");

		if (*q == '%') {
			/* it's actually an anti-macro escape "%%..." -> "%..." */
			argv[argc]++;
			continue;
		}

		/* single character macros */
		if (!q[1]) {
			switch (*q) {
			case 'o': argv[argc] = op;			continue;
			case 'k': argv[argc] = xkey;			continue;
			case 't': argv[argc] = ktype;			continue;
			case 'd': argv[argc] = kdesc;			continue;
			case 'c': argv[argc] = callout_info;		continue;
			case 'u': argv[argc] = xuid;			continue;
			case 'g': argv[argc] = xgid;			continue;
			case 'T': argv[argc] = xthread_keyring;		continue;
			case 'P': argv[argc] = xprocess_keyring;	continue;
			case 'S': argv[argc] = xsession_keyring;	continue;
			default:
				line_error("Unsupported macro\n");
			}
		}

		/* keysub macro */
		if (*q == '{') {
			key_serial_t keysub;
			void *tmp;
			char *ksdesc, *end, *subdata;
			int ret, loop;

			/* extract type and description */
			q++;
			ksdesc = strchr(q, ':');
			if (!ksdesc)
				line_error("Keysub macro lacks ':'\n");
			*ksdesc++ = 0;
			end = strchr(ksdesc, '}');
			if (!end)
				line_error("Unterminated keysub macro\n");

			*end++ = 0;
			if (*end)
				line_error("Keysub macro has trailing rubbish\n");

			debug("Keysub: %s key \"%s\"\n", q, ksdesc);

			if (!q[0])
				line_error("Keysub type empty\n");

			if (!ksdesc[0])
				line_error("Keysub description empty\n");

			/* look up the key in the requestor's keyrings, but fail immediately if the
			 * key is not found rather than invoking /sbin/request-key again
			 */
			keysub = request_key(q, ksdesc, NULL, 0);
			if (keysub < 0)
				line_error("Keysub key not found: %m\n");

			ret = keyctl_read_alloc(keysub, &tmp);
			if (ret < 0)
				line_error("Can't read keysub %d data: %m\n", keysub);
			subdata = tmp;

			for (loop = 0; loop < ret; loop++)
				if (!isprint(subdata[loop]))
					error("keysub %d data not printable ('%02hhx')\n",
					      keysub, subdata[loop]);

			argv[argc] = subdata;
			continue;
		}
	}

	if (argc == 0)
		line_error("No arguments\n");

	argv[argc] = NULL;

	if (xdebug) {
		char **ap;

		debug("%s %s\n", pipeit ? "PipeThru" : "Run", prog);
		for (ap = argv; *ap; ap++)
			debug("- argv[%td] = \"%s\"\n", ap - argv, *ap);
	}

	/* become the same UID/GID as the key requesting process */
	//setgid(atoi(xuid));
	//setuid(atoi(xgid));

	/* if the last argument is a single bar, we spawn off the program dangling on the end of
	 * three pipes and read the key material from the program, otherwise we just exec
	 */
	if (pipeit)
		pipe_to_program(op, key, ktype, kdesc, callout_info, prog, argv);

	/* attempt to execute the command */
	execv(prog, argv);

	line_error("Failed to execute '%s': %m\n", prog);

} /* end execute_program() */

/*****************************************************************************/
/*
 * pipe the callout information to the specified program and retrieve the payload data over another
 * pipe
 */
static void pipe_to_program(char *op,
			    key_serial_t key,
			    char *ktype,
			    char *kdesc,
			    char *callout_info,
			    char *prog,
			    char **argv)
{
	char errbuf[512], payload[32768 + 1], *pp, *pc, *pe;
	int ipi[2], opi[2], epi[2], childpid;
	int ifl, ofl, efl, npay, ninfo, espace, tmp;

	debug("pipe_to_program(%s -> %s)", callout_info, prog);

	if (pipe(ipi) < 0 || pipe(opi) < 0 || pipe(epi) < 0)
		error("pipe failed: %m");

	childpid = fork();
	if (childpid == -1)
		error("fork failed: %m");

	if (childpid == 0) {
		/* child process */
		if (dup2(ipi[0], 0) < 0 ||
		    dup2(opi[1], 1) < 0 ||
		    dup2(epi[1], 2) < 0)
			error("dup2 failed: %m");
		close(ipi[0]);
		close(ipi[1]);
		close(opi[0]);
		close(opi[1]);
		close(epi[0]);
		close(epi[1]);

		execv(prog, argv);
		line_error("Failed to execute '%s': %m\n", prog);
	}

	/* parent process */
	close(ipi[0]);
	close(opi[1]);
	close(epi[1]);

#define TOSTDIN ipi[1]
#define FROMSTDOUT opi[0]
#define FROMSTDERR epi[0]

	ifl = fcntl(TOSTDIN, F_GETFL);
	ofl = fcntl(FROMSTDOUT, F_GETFL);
	efl = fcntl(FROMSTDERR, F_GETFL);
	if (ifl < 0 || ofl < 0 || efl < 0)
		error("fcntl/F_GETFL failed: %m");

	ifl |= O_NONBLOCK;
	ofl |= O_NONBLOCK;
	efl |= O_NONBLOCK;

	if (fcntl(TOSTDIN, F_SETFL, ifl) < 0 ||
	    fcntl(FROMSTDOUT, F_SETFL, ofl) < 0 ||
	    fcntl(FROMSTDERR, F_SETFL, efl) < 0)
		error("fcntl/F_SETFL failed: %m");

	ninfo = strlen(callout_info);
	pc = callout_info;

	npay = sizeof(payload);
	pp = payload;

	espace = sizeof(errbuf);
	pe = errbuf;

	do {
		fd_set rfds, wfds;

		FD_ZERO(&rfds);
		FD_ZERO(&wfds);

		if (TOSTDIN != -1) {
			if (ninfo > 0) {
				FD_SET(TOSTDIN, &wfds);
			}
			else {
				close(TOSTDIN);
				TOSTDIN = -1;
				continue;
			}
		}

		if (FROMSTDOUT != -1)
			FD_SET(FROMSTDOUT, &rfds);

		if (FROMSTDERR != -1)
			FD_SET(FROMSTDERR, &rfds);

		tmp = TOSTDIN > FROMSTDOUT ? TOSTDIN : FROMSTDOUT;
		tmp = tmp > FROMSTDERR ? tmp : FROMSTDERR;
		tmp++;

		debug("select r=%d,%d w=%d m=%d\n", FROMSTDOUT, FROMSTDERR, TOSTDIN, tmp);

		tmp = select(tmp, &rfds, &wfds, NULL, NULL);
		if (tmp < 0)
			error("select failed: %m\n");

		if (TOSTDIN != -1 && FD_ISSET(TOSTDIN, &wfds)) {
			tmp = write(TOSTDIN, pc, ninfo);
			if (tmp < 0) {
				if (errno != EPIPE)
					error("write failed: %m\n");

				debug("EPIPE");
				ninfo = 0;
			}
			else {
				debug("wrote %d\n", tmp);

				pc += tmp;
				ninfo -= tmp;
			}
		}

		if (FROMSTDOUT != -1 && FD_ISSET(FROMSTDOUT, &rfds)) {
			tmp = read(FROMSTDOUT, pp, npay);
			if (tmp < 0)
				error("read failed: %m\n");

			debug("read %d\n", tmp);

			if (tmp == 0) {
				close(FROMSTDOUT);
				FROMSTDOUT = -1;
			}
			else {
				pp += tmp;
				npay -= tmp;

				if (npay == 0)
					error("Too much data read from query program\n");
			}
		}

		if (FROMSTDERR != -1 && FD_ISSET(FROMSTDERR, &rfds)) {
			char *nl;

			tmp = read(FROMSTDERR, pe, espace);
			if (tmp < 0)
				error("read failed: %m\n");

			debug("read err %d\n", tmp);

			if (tmp == 0) {
				close(FROMSTDERR);
				FROMSTDERR = -1;
				continue;
			}

			pe += tmp;
			espace -= tmp;

			while ((nl = memchr(errbuf, '\n', pe - errbuf))) {
				int n, rest;

				nl++;
				n = nl - errbuf;

				if (xdebug)
					fprintf(stderr, "Child: %*.*s", n, n, errbuf);

				if (!xnolog) {
					openlog("request-key", 0, LOG_AUTHPRIV);
					syslog(LOG_ERR, "Child: %*.*s", n, n, errbuf);
					closelog();
				}

				rest = pe - nl;
				if (rest > 0) {
					memmove(errbuf, nl, rest);
					pe -= n;
					espace += n;
				}
				else {
					pe = errbuf;
					espace = sizeof(errbuf);
				}
			}

			if (espace == 0) {
				int n = sizeof(errbuf);

				if (xdebug)
					fprintf(stderr, "Child: %*.*s", n, n, errbuf);

				if (!xnolog) {
					openlog("request-key", 0, LOG_AUTHPRIV);
					syslog(LOG_ERR, "Child: %*.*s", n, n, errbuf);
					closelog();
				}

				pe = errbuf;
				espace = sizeof(errbuf);
			}
		}

	} while (TOSTDIN != -1 || FROMSTDOUT != -1 || FROMSTDERR != -1);

	/* wait for the program to exit */
	if (waitpid(childpid, &tmp, 0) != childpid)
		error("wait for child failed: %m\n");

	/* if the process exited non-zero or died on a signal, then we call back in to ourself to
	 * decide on negation
	 * - this is not exactly beautiful but the quickest way of having configurable negation
	 *   settings
	 */
	if (WIFEXITED(tmp) && WEXITSTATUS(tmp) != 0) {
		if (norecurse)
			error("child exited %d\n", WEXITSTATUS(tmp));

		norecurse = 1;
		debug("child exited %d\n", WEXITSTATUS(tmp));
		lookup_action("negate", key, ktype, kdesc, callout_info);
	}

	if (WIFSIGNALED(tmp)) {
		if (norecurse)
			error("child died on signal %d\n", WTERMSIG(tmp));

		norecurse = 1;
		debug("child died on signal %d\n", WTERMSIG(tmp));
		lookup_action("negate", key, ktype, kdesc, callout_info);
	}

	/* attempt to instantiate the key */
	debug("instantiate with %td bytes\n", pp - payload);

	if (keyctl_instantiate(key, payload, pp - payload, 0) < 0)
		error("instantiate key failed: %m\n");

	debug("instantiation successful\n");
	exit(0);

} /* end pipe_to_program() */
