/*

Copyright (c) 2005-2018, University of Oxford.
All rights reserved.

University of Oxford means the Chancellor, Masters and Scholars of the
University of Oxford, having an administrative office at Wellington
Square, Oxford OX1 2JD, UK.

This file is part of Chaste.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:
 * Redistributions of source code must retain the above copyright notice,
   this list of conditions and the following disclaimer.
 * Redistributions in binary form must reproduce the above copyright notice,
   this list of conditions and the following disclaimer in the documentation
   and/or other materials provided with the distribution.
 * Neither the name of the University of Oxford nor the names of its
   contributors may be used to endorse or promote products derived from this
   software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE
GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT
OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

*/

#ifndef _TESTCIPAQNETCALCULATOR_HPP_
#define _TESTCIPAQNETCALCULATOR_HPP_

// System includes
#include <iostream>

// Chaste includes
#include "CheckpointArchiveTypes.hpp"
#include "FileFinder.hpp"
#include "SteadyStateRunner.hpp"
#include "Warnings.hpp"

// ApPredict includes
#include "CipaQNetCalculator.hpp"
#include "SetupModel.hpp"

// Final Chaste include for making parallel work...
#include "FakePetscSetup.hpp"

class TestCipaQNetCalculator : public CxxTest::TestSuite
{
public:
    void TestExceptions()
    {
        boost::shared_ptr<AbstractCvodeCell> p_model;
        TS_ASSERT_THROWS_THIS(CipaQNetCalculator calc(p_model),
                              "Model used in constructor of CipaQNetCalculator needs to be an Ohara-Rudy-CiPA-v1 model.");
    }

    void TestWarnings()
    {
        const unsigned ord_cipa_index = 8u;
        const double hertz = 1.0;
        SetupModel setup(hertz, ord_cipa_index);
        boost::shared_ptr<AbstractCvodeCell> p_model = setup.GetModel();
        CipaQNetCalculator calculator(p_model);

        TS_ASSERT_EQUALS(Warnings::Instance()->GetNumWarnings(), 1u);
        TS_ASSERT_EQUALS(Warnings::Instance()->GetNextWarningMessage(),
                         "qNet should be calculated at 0.5Hz (pacing cycle length of 2000ms), your stimulus is set to 1000ms.");
    }

    void TestQNetCalculation()
    {
        const unsigned ord_cipa_index = 8u;
        const double hertz = 0.5;
        SetupModel setup(hertz, ord_cipa_index);
        boost::shared_ptr<AbstractCvodeCell> p_model = setup.GetModel();

        // Run this once, and copy state variable archive into ApPredict test data folder.
        //        {
        //            // Used to generate an archive with 0.5Hz variables.
        //            SteadyStateRunner runner(p_model);
        //            runner.RunToSteadyState();
        //
        //            OutputFileHandler handler("archive", false);
        //            std::string archive_filename;
        //            archive_filename = handler.GetOutputDirectoryFullPath() + "ord_cipa_0.5Hz_state_vars.arch";
        //
        //            std::ofstream ofs(archive_filename.c_str());
        //            boost::archive::text_oarchive output_arch(ofs);
        //
        //            const std::vector<double> state_vars = MakeStdVec(p_model->GetStateVariables());
        //            output_arch << state_vars;
        //        }

        // Load archive of 0.5Hz steady state variables.
        {
            FileFinder archive_file("projects/ApPredict/test/data/ord_cipa_0.5Hz_state_vars.arch", RelativeTo::ChasteSourceRoot);
            std::string archive_filename = archive_file.GetAbsolutePath();
            std::ifstream ifs(archive_filename.c_str(), std::ios::binary);
            boost::archive::text_iarchive input_arch(ifs);

            std::vector<double> state_vars;
            input_arch >> state_vars;
            p_model->SetStateVariables(state_vars);
        }

        // Run qNet calculation.
        CipaQNetCalculator calculator(p_model);
        double q_net = calculator.ComputeQNet();

        TS_ASSERT_DELTA(q_net, 0.06967, 1e-5);
    }
};

#endif //_TESTCIPAQNETCALCULATOR_HPP_
