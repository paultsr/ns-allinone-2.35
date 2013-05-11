# Copyright (C) 2013 PAUL SHAIJU, THRISSUR, KERALA, INDIA.
#
# This script is free software; you can redistribute it and/or modify
# it under the terms of the GNU Lesser General Public License as
# published by the Free Software Foundation; either version 3 of the
# License, or (at your option) any later version.
#
# This library is distributed in the hope that it will be useful, but
# WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
# Lesser General Public License for more details.
#
# You should have received a copy of the GNU Lesser General Public
# License along with this library; if not, write to the Free Software
# Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307,
# USA.
#
# Author: Paul S <intouch.paulson@gmail.com>
# Website : http://paultsr.in
# Email-id : paultsr@gmail.com

# LD_LIBRARY_PATH
OTCL_LIB=/usr/local/ns-allinone-2.35/otcl-1.14
NS2_LIB=/usr/local/ns-allinone-2.35/lib
#X11_LIB=/usr/X11R6/lib
USR_LOCAL_LIB=/usr/local/lib
#export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:$OTCL_LIB:$NS2_LIB:$X11_LIB:$USR_LOCAL_LIB
export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:$OTCL_LIB:$NS2_LIB:$USR_LOCAL_LIB
 
# TCL_LIBRARY
TCL_LIB=/usr/local/ns-allinone-2.35/tcl8.5.10/library
USR_LIB=/usr/lib
export TCL_LIBRARY=$TCL_LIB:$USR_LIB
 
# PATH
XGRAPH=/usr/local/ns-allinone-2.35/bin:/usr/local/ns-allinone-2.35/tcl8.5.10/unix:/usr/local/ns-allinone-2.35/tk8.5.10/unix
NS=/usr/local/ns-allinone-2.35/ns-2.35/
NAM=/usr/local/ns-allinone-2.34/nam-1.15/
export PATH=$PATH:$XGRAPH:$NS:$NAM

