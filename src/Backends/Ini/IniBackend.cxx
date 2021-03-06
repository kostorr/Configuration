// Copyright CERN and copyright holders of ALICE O2. This software is
// distributed under the terms of the GNU General Public License v3 (GPL
// Version 3), copied verbatim in the file "COPYING".
//
// See http://alice-o2.web.cern.ch/license for full licensing information.
//
// In applying this license CERN does not waive the privileges and immunities
// granted to it by virtue of its status as an Intergovernmental Organization
// or submit itself to any jurisdiction.

/// \file IniBackend.cxx
/// \brief Configuration interface for files. Port of Configuration.h & Configuration.cxx
///
/// \author Sylvain Chapeland, CERN
/// \author Pascal Boeschoten, CERN

#include "IniBackend.h"
#include <vector>
#include <boost/property_tree/ini_parser.hpp>
#include <boost/algorithm/string/predicate.hpp>

namespace o2
{
namespace configuration
{
namespace backends
{

/// Load the configuration from given path
/// \param path  Path to configuration data.
///              Example: file:/configDir/example.cfg
///              Accepted prefix:
///                 file:     for a file accessible from the filesystem
///              Accepted suffix, to define file format (TODO: separate doc for file formats):
///                 .ini, .cfg    see example.cfg
/// \exception   Throws a <std::string> exception on error.
void loadConfigFile(const std::string& filePath, boost::property_tree::ptree& pt)
{
  if (filePath.length() == 0) { throw std::runtime_error("Invalid argument"); }

  // INI file
  for (auto suffix : {".ini", ".cfg"}) {
    if (boost::algorithm::ends_with(filePath, suffix)) {
      try {
        boost::property_tree::ini_parser::read_ini(filePath, pt);
      }
      catch (const boost::property_tree::ini_parser::ini_parser_error& perr) {
        std::stringstream ss;
        if (perr.line()) {
          ss << perr.message() << " in " << perr.filename() << " line " << perr.line();
        } else {
          ss << perr.message() << " " << perr.filename();
        }
        throw ss.str();
      }
      return;
    }
  }
  throw std::runtime_error("Invalid type in file name");
}

IniBackend::IniBackend(const std::string& filePath)
{
  loadConfigFile(filePath, mPropertyTree);
}

void IniBackend::putString(const std::string&, const std::string&)
{
  throw std::runtime_error("IniBackend does not support putting values");
}

boost::optional<std::string> IniBackend::getString(const std::string& path)
{
  // To use a custom separator instead of the default '.', we need to construct the path_type object explicitly
  return mPropertyTree.get_optional<std::string>(decltype(mPropertyTree)::path_type(addPrefix(path), getSeparator()));
}

boost::property_tree::ptree IniBackend::getRecursive(const std::string&/* path*/)
{
  return mPropertyTree;
}

} // namespace configuration
} // namespace backends
} // namespace o2
