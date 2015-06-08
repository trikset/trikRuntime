#!/bin/sh
tclsh vera++/generatePaths.tcl
vera++ --error -p allRules --root vera++ <vera++/params
