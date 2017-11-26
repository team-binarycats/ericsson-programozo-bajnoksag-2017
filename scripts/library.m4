include(`forloop.m4')dnl
include(`join.m4')dnl
define(`NL', `
')dnl
define(`repeat', `forloop(`i', `1', `$1', join(NL, shift($@))NL)dnl')dnl
define(`l', `ifelse(`$#',`0',`LEFT',`repeat(`$1',LEFT)')')dnl
define(`r', `ifelse(`$#',`0',`RIGHT',`repeat(`$1',RIGHT)')')dnl
define(`u', `ifelse(`$#',`0',`UP',`repeat(`$1',UP)')')dnl
define(`d', `ifelse(`$#',`0',`DOWN',`repeat(`$1',DOWN)')')dnl
