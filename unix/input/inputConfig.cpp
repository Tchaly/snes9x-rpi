// snes9x-rpi is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 3, or (at your option)
// any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; see the file COPYING.  If not, write to
// the Free Software Foundation, Inc., 51 Franklin Street, Fifth
// Floor, Boston, MA 02110-1301, USA.
//
//
// snes9x-rpi is based on snes9x:
// Snes9x - Portable Super Nintendo Entertainment System (TM) emulator.
//
// (c) Copyright 1996 - 2001 Gary Henderson (gary.henderson@ntlworld.com) and
//                           Jerremy Koot (jkoot@snes9x.com)
//
// Super FX C emulator code 
// (c) Copyright 1997 - 1999 Ivar (ivar@snes9x.com) and
//                           Gary Henderson.
// Super FX assembler emulator code (c) Copyright 1998 zsKnight and _Demo_.
//
// DSP1 emulator code (c) Copyright 1998 Ivar, _Demo_ and Gary Henderson.
// C4 asm and some C emulation code (c) Copyright 2000 zsKnight and _Demo_.
// C4 C code (c) Copyright 2001 Gary Henderson (gary.henderson@ntlworld.com).
//
// DOS port code contains the works of other authors. See headers in
// individual files.
//
// Snes9x homepage: http://www.snes9x.com
//
// Copyright Cédric Chépied 2013

#include <iostream>
#include <fstream>
#include <boost/archive/xml_oarchive.hpp>
#include <boost/archive/xml_iarchive.hpp>

#include "inputConfig.hpp"


InputConfig::InputConfig(const std::string &file) throw (ExitException)
{
	std::ifstream f(file.c_str());
	if (!f.is_open() || f.bad())
		throw ExitException();

	try
	{
		boost::archive::xml_iarchive xml(f);
		xml >> boost::serialization::make_nvp("InputConfig", *this);
	}
	catch (...)
	{
		throw ExitException();
	}

	if (!verify())
		throw ExitException();
}


InputConfig::InputConfig(bool unused)
{
	(void) unused;

	globalMap[KEY_ACCEL] = SDLK_BACKSPACE;
	globalMap[KEY_QUIT] = SDLK_ESCAPE;

	KeyboardMapping m;
	m.setSDLKey(KEY_A, SDLK_z);
	m.setSDLKey(KEY_B, SDLK_x);
	m.setSDLKey(KEY_X, SDLK_a);
	m.setSDLKey(KEY_Y, SDLK_s);
	m.setSDLKey(KEY_L, SDLK_q);
	m.setSDLKey(KEY_R, SDLK_w);
	m.setSDLKey(KEY_START, SDLK_RETURN);
	m.setSDLKey(KEY_SELECT, SDLK_LCTRL);
	m.setSDLKey(KEY_RIGHT, SDLK_RIGHT);
	m.setSDLKey(KEY_LEFT, SDLK_LEFT);
	m.setSDLKey(KEY_UP, SDLK_UP);
	m.setSDLKey(KEY_DOWN, SDLK_DOWN);
	kbdMaps.push_back(m);

	AvailableJoystick j;
	j[JB_A] = 0;
	j[JB_B] = 1;
	j[JB_X] = 2;
	j[JB_Y] = 3;
	j[JB_L] = 4;
	j[JB_R] = 5;
	j[JB_START] = 6;
	j[JB_SELECT] = 7;
	j[JB_ACCEL] = 8;
	j[JB_QUIT] = 9;

	joysticks.push_back(j);
}


void InputConfig::save(const std::string &file) throw (SnesException)
{
	std::ofstream f(file.c_str());
	if (!f.is_open() || f.bad())
		throw SnesException("Unable to open file for writting");

	try
	{
		boost::archive::xml_oarchive xml(f);
		xml << boost::serialization::make_nvp("InputConfig", *this);
	}
	catch (...)
	{
		throw SnesException("Unable to save config");
	}
}


KeyboardMapping* InputConfig::getKbdMap(unsigned index) throw (SnesException)
{
	if (index >= kbdMaps.size())
		throw SnesException("Invalid index");

	return &(kbdMaps[index]);
}

AvailableJoystick* InputConfig::getJoystick(std::string name)
{
	std::vector<AvailableJoystick>::iterator it;
	it = std::find(joysticks.begin(), joysticks.end(), name);
	if (it == joysticks.end())
	{
		std::cerr<<"Could not find joystick mapping for"<<name;
		std::cerr<<", use default"<<std::endl;
		return &(joysticks.front());
	}

	std::cout<<"Mapping for "<<name<<" found!"<<std::endl;
	return &(*it);
}

AvailableJoystick* InputConfig::addJoystick(std::string name)
{
	joysticks.push_back(AvailableJoystick(name));
	return &(joysticks.back());
}


KeyboardMapping* InputConfig::addKbdMap(void)
{
	kbdMaps.push_back(KeyboardMapping());
	return &(kbdMaps.back());
}
