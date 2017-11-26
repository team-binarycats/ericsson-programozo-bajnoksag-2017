include(`forloop.m4')dnl
include(`join.m4')dnl
define(`NL', `
')dnl
define(`repeat', `forloop(`i', `1', `$1', join(NL, shift($@))NL)dnl')dnl
define(`l', `ifelse(`$#',`0',`LEFT',`repeat(`$1',LEFT)')')dnl
define(`r', `ifelse(`$#',`0',`RIGHT',`repeat(`$1',RIGHT)')')dnl
define(`u', `ifelse(`$#',`0',`UP',`repeat(`$1',UP)')')dnl
define(`d', `ifelse(`$#',`0',`DOWN',`repeat(`$1',DOWN)')')dnl
define(`divide_r', `repeat(eval($1-`1'), `: Column i',		`r(eval(`(100-4)/$1'))',`repeat(eval(`80-4+1'), ifelse(eval(i%`2'),`0', `UP,DOWN,UP',`DOWN,UP,DOWN'))')
r(eval(`(100-4)/$1'))')dnl
define(`divide_u', `repeat(eval($1-`1'), `: Row eval(`$1'-i+1)',`u(eval(`(80-4)/$1'))',`repeat(eval(`100-4+1'), ifelse(eval(i%`2'),`0', `RIGHT,LEFT,RIGHT',`LEFT,RIGHT,LEFT'))')
r(eval(`(80-4)/$1'))')dnl
