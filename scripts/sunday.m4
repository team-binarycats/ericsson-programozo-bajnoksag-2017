include(`library.m4')dnl
] Next level is 0, your points: 0
repeat(24, RIGHT)
repeat(77, DOWN)
repeat(25, RIGHT)
repeat(34, UP)
repeat(34, DOWN)
repeat(77, UP)
repeat(25, RIGHT)
repeat(36, DOWN)
repeat(36, UP)
repeat(56, DOWN)
repeat(20, UP)
repeat(40, DOWN)
DOWN # Uncaught
] Next level is 1, your points: 3
repeat(24, RIGHT)
repeat(77, DOWN)
repeat(25, RIGHT)
repeat(26, UP)
repeat(26, DOWN)
repeat(53, UP)
repeat(27, DOWN)
repeat(33, UP)
repeat(6, DOWN)
repeat(24, UP)
repeat(25, RIGHT)
repeat(27, DOWN)
repeat(27, UP)
repeat(68, DOWN)
repeat(41, UP)
repeat(41, DOWN)
UP
repeat(6, DOWN)
repeat(5, UP)
repeat(9, DOWN)
repeat(24, RIGHT)
repeat(59, UP)
repeat(42, LEFT)
repeat(18, RIGHT)
repeat(21, LEFT)
repeat(3, RIGHT)
repeat(31, LEFT)
LEFT # Uncaught
] Next level is 2, your points: 6
DOWN
repeat(24, RIGHT)
repeat(17, DOWN)
repeat(17, UP)
repeat(42, DOWN)
repeat(25, UP)
repeat(41, DOWN)
repeat(16, UP)
repeat(35, DOWN)
repeat(25, RIGHT)
repeat(32, UP)
repeat(32, DOWN)
repeat(50, UP)
repeat(18, DOWN)
repeat(45, UP)
repeat(25, RIGHT)
repeat(22, DOWN)
repeat(22, UP)
repeat(46, DOWN)
repeat(24, UP)
repeat(32, DOWN)
repeat(8, UP)
repeat(31, DOWN)
repeat(24, RIGHT)
repeat(59, UP)
repeat(17, LEFT)
repeat(17, RIGHT)
repeat(89, LEFT)
repeat(15, RIGHT)
repeat(22, LEFT)
LEFT # Uncaught
] Next level is 3, your points: 16
DOWN
RIGHT
RIGHT
divide_r(4)
u(19)
repeat(24, LEFT)
repeat(24, LEFT,RIGHT,LEFT)
u(19)
repeat(48, RIGHT)
u(19)
repeat(24, LEFT) # We finish earlyer
] Level3 finished, next should be level4
# This level is done by keyboard
DOWN #NOOP
] Not sent any command at tick 1.
repeat(10, DOWN)
repeat(13, RIGHT)
repeat(11, UP)
repeat(13, DOWN)
LEFT
repeat(6, UP)
repeat(7, DOWN)
RIGHT
repeat(4, UP)
repeat(26, DOWN)
repeat(13, LEFT)
repeat(28, RIGHT)
repeat(9, UP)
repeat(15, LEFT)
repeat(26, RIGHT)
repeat(11, LEFT)
repeat(14, RIGHT)
repeat(5, LEFT)
repeat(10, RIGHT)
repeat(7, LEFT)
repeat(11, RIGHT)
repeat(7, LEFT)
repeat(10, RIGHT)
repeat(10, LEFT)
repeat(21, RIGHT)
repeat(10, LEFT)
repeat(3, RIGHT)
repeat(5, LEFT)
repeat(46, RIGHT)
repeat(25, LEFT)
repeat(27, UP)
repeat(6, DOWN)
repeat(3, LEFT)
repeat(4, UP)
repeat(4, LEFT)
repeat(2, UP)
repeat(13, LEFT)
repeat(50, DOWN)
repeat(6, UP)
repeat(11, DOWN)
repeat(15, UP)
repeat(34, DOWN)
repeat(36, RIGHT)
repeat(10, LEFT)
repeat(10, RIGHT)
repeat(2, DOWN)
repeat(5, UP)
repeat(48, DOWN)
repeat(24, UP)
repeat(8, RIGHT)
repeat(9, LEFT)
repeat(6, RIGHT)
repeat(3, LEFT)
repeat(21, RIGHT)
repeat(15, LEFT)
repeat(54, RIGHT)
repeat(22, LEFT)
repeat(6, DOWN)
repeat(7, UP)
repeat(25, DOWN)
repeat(12, UP)
repeat(40, LEFT)
] Next level is 5, your points: 22
DOWN
RIGHT
divide_r(3)
# The rest is done by keyboard
RIGHT
repeat(16, LEFT)
repeat(27, DOWN)
repeat(20, LEFT)
repeat(20, RIGHT)
repeat(5, DOWN)
repeat(5, UP)
repeat(6, DOWN)
repeat(3, RIGHT)
repeat(3, LEFT)
repeat(3, UP)
repeat(16, RIGHT)
repeat(69, LEFT)
repeat(5, RIGHT)
repeat(7, LEFT)
repeat(4, RIGHT)
repeat(6, LEFT)
repeat(4, RIGHT)
repeat(8, LEFT)
repeat(5, RIGHT)
repeat(25, LEFT)
] Next level is 6, your points: 24
DOWN
RIGHT
r(eval((100-4)/4))
repeat(eval(80-4), DOWN,UP,DOWN)
# Finished manually
DOWN #NOOP
] Not sent any command at tick 255.
DOWN
repeat(42, UP)
repeat(12, RIGHT)
repeat(12, LEFT)
repeat(16, RIGHT)
repeat(11, DOWN)
repeat(16, LEFT)
repeat(39, RIGHT)
repeat(10, UP)
repeat(34, RIGHT)
repeat(34, LEFT)
repeat(41, DOWN)
repeat(23, UP)
repeat(34, RIGHT)
repeat(17, LEFT)
repeat(2, DOWN)
repeat(5, UP)
repeat(10, DOWN)
repeat(6, UP)
repeat(13, DOWN)
repeat(7, UP)
repeat(10, DOWN)
repeat(6, UP)
repeat(12, DOWN)
repeat(11, UP)
repeat(13, LEFT)
* Unit died :(
LEFT
repeat(14, RIGHT)
repeat(2, DOWN)
repeat(2, UP)
repeat(4, DOWN)
repeat(4, UP)
repeat(5, DOWN)
repeat(4, UP)
repeat(6, DOWN)
repeat(5, UP)
repeat(8, DOWN)
repeat(5, UP)
repeat(11, DOWN)
repeat(12, LEFT)
LEFT # Uncaught
] Next level is 7, your points: 24
> Next level
DOWN
RIGHT
#RIGHT # Why isn't it required??
#divide_r(4) # Expanded below
r(eval((100-4)/4))
repeat(eval(80-4+1), DOWN,UP,DOWN)
r(eval((100-4)/4))
repeat(3, DOWN,UP) # stale
repeat(eval(80-4+1), :i,UP,DOWN,UP)
r(eval((100-4)/4))
repeat(eval(80-4+1), DOWN,UP,DOWN)
r(eval((100-4)/4))
# End of expansion
# The rest is generated using keyboard
> Not tested
RIGHT #NOOP
] Not sent any command at tick 798.
* Setting up for level 7
repeat(10, LEFT)
repeat(11, UP)
repeat(15, LEFT)
repeat(26, UP)
repeat(74, LEFT)
RIGHT
repeat(13, UP)
repeat(24, RIGHT)
* Unit died :(
RIGHT
repeat(8, DOWN)
LEFT
repeat(5, RIGHT)
repeat(4, LEFT)
repeat(48, RIGHT)
repeat(14, DOWN)
repeat(17, LEFT)
] You run out of ticks: 1070. You reached 7. level, and got 66 points. Status: Sent moves, 0 to LEFT
*terminate called after throwing an instance of 'kj::ExceptionImpl'
*what():  kj/io.c++:48: failed: expected n >= minBytes; Premature EOF
*stack: 0x7ff4138897f8 0x7ff41388b44b 0x7ff41388b4ec 0x4065c2 0x407f67 0x4090ec 0x40977c 0x407d4f 0x407f0a 0x408bbc 0x407085 0x40665e 0x407f67 0x4090ec 0x40977c 0x407d4f 0x407f0a 0x408bbc 0x407085 0x40665e 0x407f67 0x4090ec 0x40977c 0x407d4f 0x407f0a 0x408bbc 0x407085 0x40665e 0x407f67 0x4090ec 0x40977c