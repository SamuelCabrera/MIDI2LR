/*
  ==============================================================================

  CommandTableModel.cpp

This file is part of MIDI2LR. Copyright 2015-2016 by Rory Jaffe.

MIDI2LR is free software: you can redistribute it and/or modify it under the
terms of the GNU General Public License as published by the Free Software
Foundation, either version 3 of the License, or (at your option) any later version.

MIDI2LR is distributed in the hope that it will be useful, but WITHOUT ANY
WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A
PARTICULAR PURPOSE.  See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with
MIDI2LR.  If not, see <http://www.gnu.org/licenses/>.
  ==============================================================================
*/
#include "CommandTableModel.h"
#include "LRCommands.h"

/**********************************************************************************************//**
 * @fn  CommandTableModel::CommandTableModel()
 *
 * @brief   Default constructor.
 *
 *
 * @date    3/22/2016
 **************************************************************************************************/

CommandTableModel::CommandTableModel() : _rows(0), m_commandMap(nullptr)
{
}

/**********************************************************************************************//**
 * @fn  int CommandTableModel::getNumRows()
 *
 * @brief   Gets number rows.
 *
 *
 * @date    3/22/2016
 *
 * @return  The number rows.
 **************************************************************************************************/

int CommandTableModel::getNumRows()
{
	return _rows;
}

/**********************************************************************************************//**
 * @fn  void CommandTableModel::paintRowBackground(Graphics &g, int , int , int , bool rowIsSelected)
 *
 * @brief   Paints the row background.
 *
 *
 * @date    3/22/2016
 *
 * @param [in,out]  g       The Graphics to process.
 * @param   parameter2      The second parameter.
 * @param   parameter3      The third parameter.
 * @param   parameter4      The fourth parameter.
 * @param   rowIsSelected   true if row is selected.
 **************************************************************************************************/

void CommandTableModel::paintRowBackground(Graphics &g, int /*rowNumber*/, int /*width*/, int /*height*/, bool rowIsSelected)
{
	if (rowIsSelected)
		g.fillAll(Colours::lightblue);
}

/**********************************************************************************************//**
 * @fn  void CommandTableModel::paintCell(Graphics &g, int rowNumber, int columnId, int width, int height, bool )
 *
 * @brief   Paints the cell.
 *
 *
 * @date    3/22/2016
 *
 * @param [in,out]  g   The Graphics to process.
 * @param   rowNumber   The row number.
 * @param   columnId    Identifier for the column.
 * @param   width       The width.
 * @param   height      The height.
 * @param   parameter6  true to parameter 6.
 **************************************************************************************************/

void CommandTableModel::paintCell(Graphics &g, int rowNumber, int columnId, int width, int height, bool /*rowIsSelected*/)
{
	g.setColour(Colours::black);
	g.setFont(12.0f);

	if (columnId == 1) // write the MIDI message in the MIDI command column
	{
		if (_commands[rowNumber].isCC)
			g.drawText(String::formatted("%d | CC: %d", _commands[rowNumber].channel,
				_commands[rowNumber].controller), 0, 0, width, height, Justification::centred);
		else
			g.drawText(String::formatted("%d | Note: %d", _commands[rowNumber].channel,
				_commands[rowNumber].pitch), 0, 0, width, height, Justification::centred);
	}
}

/**********************************************************************************************//**
 * @fn  Component *CommandTableModel::refreshComponentForCell(int rowNumber, int columnId, bool , Component *existingComponentToUpdate)
 *
 * @brief   Refresh component for cell.
 *
 *
 * @date    3/22/2016
 *
 * @param   rowNumber                           The row number.
 * @param   columnId                            Identifier for the column.
 * @param   parameter3                          true to parameter 3.
 * @param [in,out]  existingComponentToUpdate   If non-null, the existing component to update.
 *
 * @return  null if it fails, else a pointer to a Component.
 **************************************************************************************************/

Component *CommandTableModel::refreshComponentForCell(int rowNumber, int columnId, bool /*isRowSelected*/, Component *existingComponentToUpdate)
{
	if (columnId == 2) // LR command column
	{
		// because Juce recycles these components when scrolling, we need to reset their properties		
		CommandMenu* commandSelect =  dynamic_cast<CommandMenu *>(existingComponentToUpdate);

		// create a new command menu
		if (commandSelect == nullptr)
		{
			commandSelect = new CommandMenu(_commands[rowNumber]);
			commandSelect->Init(m_commandMap);
		}
		else
			commandSelect->setMsg(_commands[rowNumber]);

		if (m_commandMap)
		{
			// add 1 because 0 is reserved for no selection
			commandSelect->setSelectedItem(LRCommandList::getIndexOfCommand(m_commandMap->getCommandforMessage(_commands[rowNumber])) + 1);
		}
		
		return commandSelect;
	}
	else
		return nullptr;
}

/**********************************************************************************************//**
 * @fn  void CommandTableModel::addRow(int midi_channel, int midi_data, bool isCC)
 *
 * @brief   Adds a row.
 *
 *
 * @date    3/22/2016
 *
 * @param   midi_channel    The MIDI channel.
 * @param   midi_data       Information describing the MIDI.
 * @param   isCC            true if this object is Cc.
 **************************************************************************************************/

void CommandTableModel::addRow(int midi_channel, int midi_data, bool isCC)
{
	MIDI_Message msg(midi_channel, midi_data, isCC);
	if (m_commandMap)
	{
		if (!m_commandMap->messageExistsInMap(msg))
		{
			_commands.push_back(msg);
			m_commandMap->addCommandforMessage(0, msg); // add an entry for 'no command'
			_rows++;
		}
	}
}

/**********************************************************************************************//**
 * @fn  int CommandTableModel::getRowForMessage(int midi_channel, int midi_data, bool isCC) const
 *
 * @brief   return value -1 means can not find.
 *
 *
 * @date    3/22/2016
 *
 * @param   midi_channel    The MIDI channel.
 * @param   midi_data       Information describing the MIDI.
 * @param   isCC            true if this object is Cc.
 *
 * @return  The row for message.
 **************************************************************************************************/

int CommandTableModel::getRowForMessage(int midi_channel, int midi_data, bool isCC) const
{
	for (auto idx = 0; idx < _rows; idx++)
	{
		if (_commands[idx].channel == midi_channel && _commands[idx].controller == midi_data
			&& _commands[idx].isCC == isCC)
			return idx;
	}
	//could not find 
	return -1;
}

/**********************************************************************************************//**
 * @fn  void CommandTableModel::removeRow(int row)
 *
 * @brief   Removes the row described by row.
 *
 *
 * @date    3/22/2016
 *
 * @param   row The row.
 **************************************************************************************************/

void CommandTableModel::removeRow(int row)
{
	MIDI_Message msg = _commands[row];
	_commands.erase(_commands.begin() + row);
	if (m_commandMap)
	{
		m_commandMap->removeMessage(msg);
	}
	_rows--;
}

/**********************************************************************************************//**
 * @fn  void CommandTableModel::removeAllRows()
 *
 * @brief   Removes all rows.
 *
 *
 * @date    3/22/2016
 **************************************************************************************************/

void CommandTableModel::removeAllRows()
{
	_commands.clear();

	if (m_commandMap)
	{
		m_commandMap->clearMap();
	}
	
	_rows = 0;
}

/**********************************************************************************************//**
 * @fn  void CommandTableModel::buildFromXml(XmlElement *root)
 *
 * @brief   Builds from XML.
 *
 *
 * @date    3/22/2016
 *
 * @param [in,out]  root    If non-null, the root.
 **************************************************************************************************/

void CommandTableModel::buildFromXml(XmlElement *root)
{
	if (root->getTagName().compare("settings") != 0)
		return;

	removeAllRows();

	XmlElement* setting = root->getFirstChildElement();
	while ((setting) && (m_commandMap))
	{
		if (setting->hasAttribute("controller"))
		{
			MIDI_Message cc(setting->getIntAttribute("channel"), setting->getIntAttribute("controller"), true);
			addRow(cc.channel, cc.controller, true);

			// older versions of MIDI2LR stored the index of the string, so we should attempt to parse this as well
			if (setting->getIntAttribute("command", -1) != -1)
			{
				m_commandMap->addCommandforMessage(setting->getIntAttribute("command"), cc);
			}
			else
			{
				m_commandMap->addCommandforMessage(setting->getStringAttribute("command_string"), cc);
			}
		}
		else if (setting->hasAttribute("note"))
		{
			MIDI_Message note(setting->getIntAttribute("channel"), setting->getIntAttribute("note"), false);
			addRow(note.channel, note.pitch, false);

			// older versions of MIDI2LR stored the index of the string, so we should attempt to parse this as well
			if (setting->getIntAttribute("command", -1) != -1)
			{
				m_commandMap->addCommandforMessage(setting->getIntAttribute("command"), note);
			}
			else
			{
				m_commandMap->addCommandforMessage(setting->getStringAttribute("command_string"), note);
			}
		}
		setting = setting->getNextElement();
	}
}

/**********************************************************************************************//**
 * @fn  void CommandTableModel::Init(CommandMap *mapCommand)
 *
 * @brief   Inits the given map command.
 *
 *
 * @date    3/22/2016
 *
 * @param [in,out]  mapCommand  If non-null, the map command.
 **************************************************************************************************/

void CommandTableModel::Init(CommandMap *mapCommand)
{
	//copy the pointer
	m_commandMap = mapCommand;
}