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
// File      : ctrl.h
// Created   : 05 Apr 2019 for apec_bless_noc
// Author(s) : Michael Vonbun
// E-mail(s) : michael.vonbun@tum.de
// 

#ifndef __APEC_BLESS_NOC_CTRL_H_
#define __APEC_BLESS_NOC_CTRL_H_

// stl
#include <vector> // std::vector

// omnet
#include <omnetpp.h>
using namespace omnetpp;

/** Simulation Control Module.
 *
 *  Mainly used to check for convergence and to stop the simulation when all
 *  modules converged.
 */
class Ctrl : public cSimpleModule
{
public:
    Ctrl();
    virtual ~Ctrl();

protected:
    /** @name Omnet++ interface. */
    //@{
    virtual void initialize();
    virtual void finish() override;
    virtual void handleMessage(cMessage *msg);
    //@}

private:
    /** @name Static network parameters. */
    //@{
    int m_num_nodes; ///< Number of nodes in the network.
    //@}

    /** @name Data members. */
    //@{
    simtime_t m_conv_th;                      ///< Convergence threshold.
    std::vector<bool> m_module_has_converged; ///< Convergence state per module.
    std::vector<simtime_t> m_conv_value_last; ///< Per module last check value.
    time_t m_start_time;                      ///< Start time of simulation.
    bool m_simulation_has_converged;          ///< Convergence state at exit.
    bool m_simulation_ended_overflow;         ///< Bufferoverflow state at exit.
    //@}

    /** @name Methods. */
    //@{
    bool have_all_converged(); ///< True if all network modules have converged.
    //@}
};

#endif
