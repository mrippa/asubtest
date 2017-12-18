#!/home/mrippa/work/test/ioc/asubtest/mk/bin/linux-x86_64/asubtest-mk-IOC

#cd /home/mrippa/work/test/ioc/asubtest/mk
cd /gem_sw/work/R3.14.12.6/ioc/asubtest/mk
# epicsEnvSet("EPICS_TIMEZONE", "HST::600:0:0")
#zoneset("UTC0")
#zoneset("CST3")
#zoneset("HST10")
epicsEnvSet("ENGINEER","Matt Rippa")
epicsEnvSet("LOCATION","HBF")
epicsEnvSet("IOCSH_PS1","asubtest-mk-IOC> ")
## You may have to change example to something else
## everywhere it appears in this file

#< envPaths

## Register all support components
dbLoadDatabase("dbd/asubtest-mk-IOC.dbd")
asubtest_mk_IOC_registerRecordDeviceDriver(pdbbase)

bc635TimeSetTpPrio (-1)
# - Master ? Simulate ? intPerSecond, intPerTick, timeOffset (microsec)
#BCconfigure (0, 0, 80, 1, 0)

## Load record instances
#dbLoadTemplate("db/user.substitutions")
#dbLoadRecords("db/dbSubExample.db", "user=mrippa")
dbLoadRecords("db/iocAdminRTEMS.db", "IOC=mrippa:iocStats")
dbLoadRecords("db/testFollow_aSub.db", "top=mc1:")
dbLoadRecords("db/asub.db")
	
## Set this to see messages from mySub
#var mySubDebug 1

## Run this to trace the stages of iocInit
#traceIocInit

iocInit

## Start any sequence programs
#seq(sncxxx, "user=mrippa")
