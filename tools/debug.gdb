target remote localhost:3333

set remote hardware-breakpoint-limit 6
set remote hardware-watchpoint-limit 4

#b main.c:20
#commands 1
#  p I
#  continue
#end

cont
