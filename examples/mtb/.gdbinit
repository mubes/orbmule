# For this example, have 'blackmagic' running already. No command line params
# are needed.
!orbtrace -p vtref,3.3 -e vtref,on
file ofiles/mtb.elf
target extended-remote localhost:2000
set mem inaccessible-by-default off
monitor swdp_scan
attach 1
load

############### Everything below is for MTB ########

source /usr/local/bin/orbcode/gdbtrace.init