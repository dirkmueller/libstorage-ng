/*
 * Copyright (c) [2004-2009] Novell, Inc.
 * Copyright (c) 2017 SUSE LLC
 *
 * All Rights Reserved.
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of version 2 of the GNU General Public License as published
 * by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, contact Novell, Inc.
 *
 * To contact Novell about this file by physical or electronic mail, you may
 * find current contact information at www.novell.com.
 */


#include <regex>
#include <boost/algorithm/string.hpp>

#include "storage/Utils/SystemCmd.h"
#include "storage/Utils/LoggerImpl.h"
#include "storage/Utils/StorageDefines.h"
#include "storage/Utils/StorageTmpl.h"
#include "storage/SystemInfo/CmdMultipath.h"


namespace storage
{
    using namespace std;


    CmdMultipath::CmdMultipath(bool test)
    {
	string cmd = MULTIPATHBIN " -d -v 2+";
	if (!test)
	    cmd += " -ll";

	SystemCmd c(cmd);
	if (c.retcode() != 0 || c.stdout().empty())
	    return;

	regex lun("[0-9]+:[0-9]+:[0-9]+:[0-9]+", regex_constants::extended);

	const vector<string>& lines = c.stdout();
	vector<string>::const_iterator it1 = lines.begin();

	while (it1 != lines.end())
	{
	    Entry entry;

	    string line = *it1;
	    y2mil("mp line:" << line);

	    if (boost::starts_with(line, "create:"))
		line = extractNthWord(1, line, true);

	    string name = extractNthWord(0, line);
	    y2mil("mp name:" << name);

	    bool has_alias = boost::starts_with(extractNthWord(1, line), "(");

	    list<string> tmp = splitString(extractNthWord(has_alias ? 3 : 2, line, true), ",");
	    if (tmp.size() >= 2)
	    {
		list<string>::const_iterator it2 = tmp.begin();
		entry.vendor = boost::trim_copy(*it2++, locale::classic());
		entry.model = boost::trim_copy(*it2++, locale::classic());
	    }

	    ++it1;

	    if (it1 != lines.end())
		++it1;

	    while (it1 != lines.end())
	    {
		if (it1->empty() || isalnum((*it1)[0]))
		    break;

		if (regex_search(*it1, lun))
		{
		    string tmp = it1->substr(5);
		    y2mil("mp element:" << tmp);
		    string dev = "/dev/" + extractNthWord(1, tmp);
		    if (find(entry.devices.begin(), entry.devices.end(), dev) == entry.devices.end())
			entry.devices.push_back(dev);
		}

		++it1;
	    }

	    data[name] = entry;
	}

	for (const_iterator it = data.begin(); it != data.end(); ++it)
	    y2mil("data[" << it->first << "] -> vendor:" << it->second.vendor <<
		  " model:" << it->second.model << " devices:" << it->second.devices);
    }


    vector<string>
    CmdMultipath::get_entries() const
    {
	vector<string> ret;
	for (const_iterator it = begin(); it != end(); ++it)
	    ret.push_back(it->first);
	return ret;
    }


    const CmdMultipath::Entry&
    CmdMultipath::get_entry(const string& name) const
    {
	const_iterator it = data.find(name);
	if (it == data.end())
	    ST_THROW(Exception("entry not found"));

	return it->second;
    }


    bool
    CmdMultipath::looks_like_real_multipath() const
    {
	for (const_iterator it = begin(); it != end(); ++it)
	    if (it->second.devices.size() > 1)
		return true;

	return false;
    }


    std::ostream&
    operator<<(std::ostream& s, const CmdMultipath& cmd_multipath)
    {
	for (const pair<string, CmdMultipath::Entry>& entry : cmd_multipath)
	    s << "data[" << entry.first << "] -> " << entry.second << '\n';

	return s;
    }


    std::ostream&
    operator<<(std::ostream& s, const CmdMultipath::Entry& entry)
    {
	s << "vendor:" << entry.vendor << " model:" << entry.model << " devices:" << entry.devices;

	return s;
    }
}
