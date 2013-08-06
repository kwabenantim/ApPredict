/*

Copyright (c) 2005-2013, University of Oxford.
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

#include "AbstractDataStructure.hpp"
#include "Exception.hpp"

void AbstractDataStructure::LoadDataFromFile(std::string fileName)
{
    std::ifstream indata; // indata is like cin
    indata.open(fileName.c_str()); // opens the file
    if(!indata.good())
    { // file couldn't be opened
       EXCEPTION("Couldn't open data file: " + fileName);
    }

    bool first_line = true;

    while (indata.good())
    {
       std::string this_line;
       getline(indata, this_line);

       if (this_line=="" || this_line=="\r")
       {
           if (indata.eof())
           {    // If the blank line is the last line carry on OK.
               break;
           }
           else
           {
               EXCEPTION("No data found on this line");
           }
       }
       std::stringstream line(this_line);

       if (first_line)
       {
           first_line = false;
           // Try and read a header line if present
           if(LoadHeaderLine(line))
           {
               continue;
           }
       }
       // Load a standard data line.
       LoadALine(line);
    }

    if (!indata.eof())
    {
       EXCEPTION("A file reading error occurred");
    }
}

bool AbstractDataStructure::LoadHeaderLine(std::stringstream& rLine)
{
    return false;
}



