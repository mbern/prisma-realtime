/*
 * prisma-realtime analyzes and visualizes monophonic musical instrument tones
 * Copyright (C) 2014  Michael Bernhard
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */

#include "configselector.h"

#include "configuration.h"

#include <qstring.h>
#include <qstringlist.h>
#include <qdir.h>
#include <q3listbox.h>

#include "config/node_macro.h"
#include "config/application_node.h"
#include "config/plugins_node.h"
#include "config/config_node.h"

ConfigSelector::ConfigSelector()
: ConfigSelectorBase(NULL, NULL, TRUE, Qt::WType_Modal), configs(new QStringList())
{
	QDir dir;
	Configuration * c = NULL;

	dir.setNameFilter("*.xml");
	for(unsigned int i = 0; i < dir.count(); i++)
	{
		if(!QFileInfo(dir,dir[i]).isWritable())
			continue;

		c = new Configuration();
		if(c->load(dir[i]))
		{
			configs->append(dir[i]);
			LB_configs->insertItem(c->getConfig()->getDescription());
		}
		delete c;
		c = NULL;
	}
	
	if(configs->count() == 0)
		qFatal("No config files found");

	LB_configs->setSelectionMode(Q3ListBox::Single);
	LB_configs->setSelected(0,true);
}

ConfigSelector::~ConfigSelector()
{
	if(configs != NULL)
		delete configs;
}

const QString & ConfigSelector::getConfig() const
{
	unsigned int i;
	for(i = 0; i < LB_configs->count(); i++)
	{
		if(LB_configs->isSelected(i))
			break;
	}
	return configs->at(i);
}

int ConfigSelector::getNumConfigs()
{
	return configs->count();
}
