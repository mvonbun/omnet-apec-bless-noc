// Omnet++ model of an apec bufferless noc.
// Copyright (C) 2019  by the author(s)
// 
// This program is free software: you can redistribute it and/or modify it under
// the terms of the GNU General Public License as published by the Free Software
// Foundation, either version 3 of the License, or (at your option) any later
// version.
// 
// This program is distributed in the hope that it will be useful, but WITHOUT
// ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
// FOR A PARTICULAR PURPOSE.  See the GNU General Public License for mor
// details.
// 
// You should have received a copy of the GNU General Public License along with
// this program.  If not, see <https://www.gnu.org/licenses/>.
//
// File      : add_module_statistic.cc
// Created   : 07 Jun 2017 for apec_bless_noc
// Author(s) : Michael Vonbun
// E-mail(s) : michael.vonbun@tum.de
// 

#include <add_module_statistic.h>
#include <cstring>

simsignal_t util::addModuleStatistic(omnetpp::cSimpleModule *module_ptr,
                                     const char *signal_name,
                                     const char *statistic_name,
                                     const char *ned_name)
{
    // register signal with module using signal_name
    simsignal_t signal = module_ptr->registerSignal(signal_name);

    // add module result recorder based on statistic template
    cProperty *statistic_template =
        module_ptr->getProperties()->get("statisticTemplate", ned_name);
    getEnvir()->addResultRecorders(module_ptr, signal, statistic_name,
                                   statistic_template);

    // return signal id
    return signal;
}

simsignal_t util::addModuleStatistic(cSimpleModule *module_ptr,
                                     const char *prefix, const int module_id,
                                     const char *postfix,
                                     const char *ned_statistic_name)
{
    char signal_name[100];
    char statistic_name[100];
    char *ned_name;

    // base names
    sprintf(signal_name, "%s%02d", prefix, module_id);
    sprintf(statistic_name, "%s%02d", prefix, module_id);

    // append postfix
    if (postfix) {
        strcat(signal_name, postfix);
        strcat(statistic_name, postfix);
    }

    // set ned name
    if (ned_statistic_name) {
        ned_name = const_cast<char *>(ned_statistic_name);
    } else {
        ned_name = const_cast<char *>(prefix);
        if (postfix) {
            strcat(ned_name, postfix);
        }
    }

    // call implementation
    return util::addModuleStatistic(module_ptr, signal_name, statistic_name,
                                    ned_name);
}
