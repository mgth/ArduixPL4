/*
  ArduHA - ArduixPL - xPL library for Arduino(tm)
  Copyright (c) 2012/2014 Mathieu GRENET.  All right reserved.

  This file is part of ArduHA / ArduixPL.

    ArduixPL is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    ArduixPL is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with ArduixPL.  If not, see <http://www.gnu.org/licenses/>.

	  Modified 2014-3-14 by Mathieu GRENET 
	  mailto:mathieu@mgth.fr
	  http://www.mgth.fr
*/
#include "sensor.h"

void Calibration2ndOrder_Shifted::input(int nx)
{
	long lnx = (long)nx + (1 << (_shift - 1));
	int delta = (long)_nc + (((lnx*lnx / _inv_a) + (lnx << _shift) / _inv_b) >> _shift); //132

	out.write(nx + delta);
}

void Calibration2ndOrder::input(const double& value)
{
	double delta = _ax2 * value * value;
	delta += _bx + value;
	delta += _c;
	out.write(value + delta);
}
