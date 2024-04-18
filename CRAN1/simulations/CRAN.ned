//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
// 
// You should have received a copy of the GNU Lesser General Public License
// along with this program.  If not, see http://www.gnu.org/licenses/.
// 

package cran1.simulations;

import cran1.AS;
import cran1.BBU;
import cran1.Cell;
import cran1.RRH;


network CRAN
{
    parameters:
        bool debugMode = default(false);
        int N = default(1); // number of cells of CRAN
    submodules:
        as: AS {
            @display("p=27,97");
        }
        bbu: BBU {
            @display("p=163,97");
        }
        rrh[N]: RRH {
            @display("p=297,97");
        }
        cell[N]: Cell {
            @display("p=388,97");
        }
    connections:
        as.out --> bbu.in;
        for i=0..(N-1){
            bbu.out++ --> rrh[i].in;
            rrh[i].out --> cell[i].in;
        }
}
