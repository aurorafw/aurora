/*****************************************************************************
**                                    / _|
**   __ _ _   _ _ __ ___  _ __ __ _  | |_ ___  ___ ___
**  / _` | | | | '__/ _ \| '__/ _` | |  _/ _ \/ __/ __|
** | (_| | |_| | | | (_) | | | (_| | | || (_) \__ \__ \
**  \__,_|\__,_|_|  \___/|_|  \__,_| |_| \___/|___/___/
**
** Copyright (C) 2018 Aurora Free Open Source Software.
**
** This file is part of the Aurora Free Open Source Software. This
** organization promote free and open source software that you can
** redistribute and/or modify under the terms of the GNU Lesser General
** Public License Version 3 as published by the Free Software Foundation or
** (at your option) any later version approved by the Aurora Free Open Source
** Software Organization. The license is available in the package root path
** as 'LICENSE' file. Please review the following information to ensure the
** GNU Lesser General Public License version 3 requirements will be met:
** https://www.gnu.org/licenses/lgpl.html .
**
** Alternatively, this file may be used under the terms of the GNU General
** Public License version 3 or later as published by the Free Software
** Foundation. Please review the following information to ensure the GNU
** General Public License requirements will be met:
** http://www.gnu.org/licenses/gpl-3.0.html.
**
** NOTE: All products, services or anything associated to trademarks and
** service marks used or referenced on this file are the property of their
** respective companies/owners or its subsidiaries. Other names and brands
** may be claimed as the property of others.
**
** For more info about intellectual property visit: aurorafoss.org or
** directly send an email to: contact (at) aurorafoss.org .
*****************************************************************************/

#include <AuroraFW/Core/Application.h>
#include <AuroraFW/CLI/Log.h>
#include <AuroraFW/Core/DebugManager.h>

#include <AuroraFW/STDL/STL/IOStream.h>

namespace AuroraFW {
	Application::Application(int argc, char *argv[], void (*mainFunction)(Application*))
	{
		args = AFW_NEW std::vector<std::string>(argv + 1, argv + argc);
		for (std::vector<std::string>::iterator i = args->begin(); i != args->end(); ++i) {
			if(*i == "--afw-debug")
				DebugManager::enable();
		}
		DebugManager::Log("creating new application");
		DebugManager::Log("application is created.");
		(*mainFunction)(this);
	}
	Application::~Application()
	{
		delete args;
		DebugManager::Log("application is destroyed.");
	}
	void Application::ExitSuccess()
	{
		DebugManager::Log("application return success code: ", EXIT_SUCCESS);
		exit(EXIT_SUCCESS);
	}
	void Application::ExitFail()
	{
		DebugManager::Log("application return error code: ", EXIT_FAILURE);
		exit(EXIT_FAILURE);
	}
}
