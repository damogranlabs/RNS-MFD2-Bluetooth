##
## This file is part of the libsigrokdecode project.
##
## Copyright (C) 2012 Uwe Hermann <uwe@hermann-uwe.de>
##
## This program is free software; you can redistribute it and/or modify
## it under the terms of the GNU General Public License as published by
## the Free Software Foundation; either version 2 of the License, or
## (at your option) any later version.
##
## This program is distributed in the hope that it will be useful,
## but WITHOUT ANY WARRANTY; without even the implied warranty of
## MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
## GNU General Public License for more details.
##
## You should have received a copy of the GNU General Public License
## along with this program; if not, see <http://www.gnu.org/licenses/>.
##

'''
RNS MFD2 VW radio -> CD Changer data protocol decoder.
Data is interpretted as: 
<start sequence: long HIGH pulse, long LOW pulse><data: logic 0: short high and short low pulse; logic 1: short high pulse, long low pulse>
Each byte (always 4 in a block) is intepreted as LSB first.
'''
from .pd import Decoder
