#$(LINUX_ONLY)#!${INSTALL}/bin/${ARCH}/crcs-cp-ioc
## Example RTEMS startup script

#cd ${INSTALL}
cd /gem_sw/work/R3.14.12.6/ioc/example
# epicsEnvSet("EPICS_TIMEZONE", "HST::600:0:0")
#zoneset("UTC0")
#zoneset("CST3")
#zoneset("HST10")
epicsEnvSet("ENGINEER","Matt Rippa")
epicsEnvSet("LOCATION","HBF")
epicsEnvSet("IOCSH_PS1","example-mk-ioc> ")
## You may have to change example to something else
## everywhere it appears in this file

#< envPaths

## Register all support components
dbLoadDatabase("dbd/example.dbd")
example_registerRecordDeviceDriver(pdbbase)

bc635TimeSetTpPrio (60)
# - Master ? Simulate ? intPerSecond, intPerTick, timeOffset (microsec)
BCconfigure (0, 0, 80, 1, 0)
 
## Load record instances
#dbLoadTemplate("db/user.substitutions")
#dbLoadRecords("db/dbSubExample.db", "user=mrippa")
dbLoadRecords("db/iocAdminRTEMS.db", "IOC=mrippa:iocStats")
dbLoadRecords("db/exampleTop.db", "top=mrippa:")
	
## Set this to see messages from mySub
#var mySubDebug 1

## Run this to trace the stages of iocInit
traceIocInit

iocInit

## Start any sequence programs
seq &timeSeq "sys=mrippa:"
