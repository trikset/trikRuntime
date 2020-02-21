#!/bin/bash
set -xueo pipefail
tclsh vera++/generatePaths.tcl
vera++ --error -p allRules --root vera++ <vera++/params
git diff --diff-filter=d --name-only ${TRAVIS_COMMIT_RANGE:-upstream/master HEAD} | \
  grep -E '\.(cpp|h|hpp)$' | vera++ --error -p strict --root vera++
