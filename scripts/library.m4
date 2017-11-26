include(`forloop.m4')dnl
include(`join.m4')dnl
define(`NL', `
')dnl
define(`repeat', `forloop(`i', `1', `$1', join(NL, shift($@))NL)')dnl
