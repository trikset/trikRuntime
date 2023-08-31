#!/usr/bin/tclsh

# Copyright 2013-2014 Vladimir Nazarenko, Cybertech Labs Ltd.
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#   http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

proc getFiles { folder } {
  set listOfFiles ""

  foreach excludeFolder [list *PythonQt* *PyTrikControl* *qslog* *thirdparty* *mlx90640-library*] {
      if {[string match $excludeFolder $folder] == 1} {
          return ""
      }
  }
  
  foreach sourceFile [glob -nocomplain -type f -directory $folder *] {
    if [regexp -nocase {.*[.]cpp$|.*[.]h$} $sourceFile ] {
      set listOfFiles [concat $listOfFiles $sourceFile]
    }
  }

  foreach subfolder [glob -nocomplain -type {d r} -directory $folder *] {    
    set listOfFiles [concat $listOfFiles [getFiles $subfolder]]
  }

  return $listOfFiles                                          
}

set parameters [getFiles [pwd]]
set fo [open "scripts/vera++/params" "w" ]
foreach par $parameters {
  puts $fo $par
}

close $fo
