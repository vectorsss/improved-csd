/*  
 * Created by Fernando Silva on 2/08/17.
 *
 * Copyright (C) 2017-current-year, Fernando Silva, all rights reserved.
 *
 * 
 * Author's contact: Fernando Silva  <fernando.silva@udc.es>
 * Databases Lab, University of A Coruña. Campus de Elviña s/n. Spain
 *
 * DESCRIPTION
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 *
 */

#include <fstream>
#include <algorithm>

int main(int argc, char* argv[]) {

    if (argc != 5) {
        printf("Usage: %s <inputFile> <outputFile> <addStartSymbol> <addEndSymbol>\n",
               argv[0]);
        exit(-1);
    }

    bool addStartSymbol = atoi(argv[3]);
    bool addEndSymbol = atoi(argv[4]);

    // Open input file
    std::ifstream inputFile(argv[1]);
    if (!inputFile.good()) {
        printf("File %s unable to open\n", argv[1]);
        exit(-1);
    }

    // Create output file
    std::ofstream outputFile(argv[2]);
    if (!outputFile.good()) {
        printf("File %s unable to open\n", argv[2]);
        exit(-1);
    }

    // Each row contains one string
    std::string str;
    std::vector<std::string> entries;
    while (std::getline(inputFile, str)) {
        if (str.length() == 0) {
            continue;
        }
        entries.push_back(str);
    }

    sort(entries.begin(), entries.end());                                       // Sort entries
    entries.erase(std::unique(entries.begin(), entries.end()), entries.end());  // Remove duplicates


    // Store all string in a file
    std::string outStr;
    for (uint i = 0; i < entries.size(); i++) {
        outStr = "";

        if (addStartSymbol) {
            outStr = '$';
        }

        outStr += entries[i];

        if (addEndSymbol) {
            outStr += '$';
        }

        outStr += '\0';
        outputFile << outStr;
    }

    inputFile.close();
    outputFile.close();
}