/*
                                    __
                                   / _|
  __ _ _   _ _ __ ___  _ __ __ _  | |_ ___  ___ ___
 / _` | | | | '__/ _ \| '__/ _` | |  _/ _ \/ __/ __|
| (_| | |_| | | | (_) | | | (_| | | || (_) \__ \__ \
 \__,_|\__,_|_|  \___/|_|  \__,_| |_| \___/|___/___/

Copyright (C) 2018-2019 Aurora Free Open Source Software.
Copyright (C) 2018-2019 Luís Ferreira <luis@aurorafoss.org>

This file is part of the Aurora Free Open Source Software. This
organization promote free and open source software that you can
redistribute and/or modify under the terms of the GNU Lesser General
Public License Version 3 as published by the Free Software Foundation or
(at your option) any later version approved by the Aurora Free Open Source
Software Organization. The license is available in the package root path
as 'LICENSE' file. Please review the following information to ensure the
GNU Lesser General Public License version 3 requirements will be met:
https://www.gnu.org/licenses/lgpl.html .

Alternatively, this file may be used under the terms of the GNU General
Public License version 3 or later as published by the Free Software
Foundation. Please review the following information to ensure the GNU
General Public License requirements will be met:
https://www.gnu.org/licenses/gpl-3.0.html.

NOTE: All products, services or anything associated to trademarks and
service marks used or referenced on this file are the property of their
respective companies/owners or its subsidiaries. Other names and brands
may be claimed as the property of others.

For more info about intellectual property visit: aurorafoss.org or
directly send an email to: contact (at) aurorafoss.org .
*/

module aurorafw.core.opt;

public import aurorafw.stdx.getopt : defaultRuntimeArgs;
import std.algorithm : startsWith, canFind, sort, any;
import aurorafw.stdx.string : isAlpha;
import std.array : split, array;
import std.range.primitives : empty;
import std.exception;
import std.string : indexOf;
import std.format : format;
import std.variant;
import core.runtime : Runtime;
import std.traits : fullyQualifiedName;
import std.typecons;

version (unittest) import aurorafw.unit.assertion;

@safe pure
class OptionHandlerException : Exception
{
	mixin basicExceptionCtors;
}

static assert(is(typeof(new OptionHandlerException("message"))));
static assert(is(typeof(new OptionHandlerException("message", Exception.init))));

@safe pure
struct OptionHandler
{
	struct Option
	{
		string[] opts;
		string help;
	}

	struct Argument
	{
		string name;
		ArgumentSize size;
		string value = null;
	}

	enum ArgumentSize
	{
		Long,
		Short
	}

	@safe pure
	public this(string[] args)
	{
		foreach (string arg; args[1 .. $])
		{
			if (arg.startsWith("--"))
			{
				string splitted = arg.split("--")[1];
				if (splitted.length == 0)
					break;

				ptrdiff_t hasValue = splitted.indexOf('=');
				if (hasValue != -1)
				{
					string newSplitted = splitted[0 .. hasValue];
					if (newSplitted.isAlpha)
						this.args ~= Argument(newSplitted, ArgumentSize.Long, splitted[hasValue + 1 .. $]);
				}
				else if (splitted.isAlpha)
					this.args ~= Argument(splitted, ArgumentSize.Long);
			}
			else if (arg.startsWith("-"))
			{
				string splitted = arg.split("-")[1];
				ptrdiff_t hasValue = splitted.indexOf('=');
				if (hasValue != -1)
				{
					string newSplitted = splitted[0 .. hasValue];
					if (newSplitted.isAlpha)
						this.args ~= Argument(newSplitted, ArgumentSize.Short, splitted[hasValue + 1 .. $]);
				}
				else if (splitted.isAlpha && splitted.length != 0)
					this.args ~= Argument(splitted, ArgumentSize.Short);
			}
		}
	}

	@safe pure
	public T[] read(T)(string opts, string help, bool required = false)
	{
		read(opts, help);

		T[] ret;
		bool value = false;
		foreach (arg; args)
			if (opts.canFind(arg.name))
			{
				value = true;
				string retstr = arg.value;
				if (retstr !is null)
				{
					import std.conv : to;

					ret ~= to!T(retstr);
				}
			}

		if (required && (!value || (value && ret.empty)))
			throw new OptionHandlerException("Required option %s with %s type".format(opts, fullyQualifiedName!T));

		return ret;
	}

	@safe pure
	public Nullable!Argument read(string opts, string help, ref bool value, bool required = false)
	{
		Option opte = read(opts, help);
		value = false;
		foreach (opt; opte.opts)
		{
			foreach (arg; args)
				if (arg.name == opt)
				{
					value = true;
					return arg.nullable;
				}
		}
		if (value == false && required == true)
			throw new OptionHandlerException("Required option " ~ opts);

		return Nullable!Argument();
	}

	@safe pure
	private Option read(string opts, string help)
	{
		Option opte;
		opte.opts = opts.split("|").sort!((a, b) => a.length < b.length).array;
		opte.help = help;

		if (this.opts.any!(o => o.opts == opte.opts))
			throw new OptionHandlerException("Trying to read the same option name twice");

		this.opts ~= opte;

		return opte;
	}

	@safe pure nothrow
	public bool helpWanted()
	{
		foreach (arg; args)
		{
			if (arg.name == "help")
				return true;
		}
		return false;
	}

	@safe pure
	public string printableHelp(string programName)
	{
		string ret;

		ret ~= "Usage:\n\t%s -- <options>\n\nOptions:\n".format(programName);

		foreach (opt; opts)
		{
			ret ~= opt.opts[0];
			if (opt.opts.length == 2)
			{
				ret ~= " " ~ opt.opts[1];
			}
			ret ~= "\t" ~ opt.help ~ "\n";
		}
		return ret;
	}

	@safe pure
	@property public Argument[] arguments()
	{
		return this.args.dup;
	}

	@property @safe pure
	public Option[] options()
	{
		return this.opts.dup;
	}

	private Argument[] args;
	private Option[] opts;
}

@safe pure
@("Option Handler: help")
unittest
{
	bool dummy;

	{
		OptionHandler optHandler = OptionHandler(["prog", "--help"]);
		optHandler.read("dummy", "some", dummy);
		optHandler.read("duuu|d", "none", dummy);

		auto pHelp = "Usage:\n\tprogram -- <options>\n\nOptions:\n";
		pHelp ~= "dummy\tsome\n";
		pHelp ~= "d duuu\tnone\n";

		assertEquals(optHandler.printableHelp("program"), pHelp);
		assertTrue(optHandler.helpWanted);

		optHandler = OptionHandler(["prog"]);
		assertFalse(optHandler.helpWanted);
	}
}

@safe pure
@("Option Handler: check arguments and options")
unittest
{
	OptionHandler opts = OptionHandler(["prog", "--foo", "-f", "--bar=foobar", "--", "tunaisgood"]);

	OptionHandler.Argument[] args = [
		OptionHandler.Argument("foo", OptionHandler.ArgumentSize.Long),
		OptionHandler.Argument("f", OptionHandler.ArgumentSize.Short),
		OptionHandler.Argument("bar", OptionHandler.ArgumentSize.Long, "foobar")
	];

	assertEquals(args, opts.arguments);
	assertEquals([], opts.options);
}

@safe pure
@("Option Handler: check read")
unittest
{
	bool isFoo, isFoobar;

	{
		OptionHandler optHandler = OptionHandler(["prog", "--foo", "-f"]);

		optHandler.read("foo", "information", isFoo, true);
		optHandler.read("foobar|f", "quick", isFoobar);

		auto opts = [
			OptionHandler.Option(["foo"], "information"),
			OptionHandler.Option(["f", "foobar"], "quick")
		];

		assertEquals(opts, optHandler.options);

		assertThrown(optHandler.read!int("tuna", "tunaisgood", true));
	}

	assertTrue(isFoo);
	assertTrue(isFoobar);
}

@safe pure
@("Option Handler: check values")
unittest
{
	int[] foo;

	{
		OptionHandler optHandler = OptionHandler(["prog", "--foo=4", "-f=7"]);

		foo = optHandler.read!int("foo|f", "information", true);
	}

	assertFalse(foo.empty);
	assertEquals(4, foo[0]);
	assertEquals(7, foo[1]);
}

@safe pure
@("Option Handler: required exception")
unittest
{
	bool isFoo, isBar;

	{
		OptionHandler optHandler = OptionHandler(["prog"]);
		optHandler.read("bar", "information", isBar);

		assertThrown!OptionHandlerException(optHandler.read("foo", "more information", isFoo, true));
	}
}

@safe pure
@("Option Handler: twice")
unittest
{
	bool dummy;

	{
		OptionHandler optHandler = OptionHandler(["prog"]);
		optHandler.read("dummy", "some", dummy);

		assertThrown!OptionHandlerException(optHandler.read("dummy", "same", dummy));
	}
}
