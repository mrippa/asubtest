TOP=../..

include $(TOP)/configure/CONFIG
#----------------------------------------
#  ADD MACRO DEFINITIONS AFTER THIS LINE
#=============================

#=============================
# Build the IOC application

USR_CFLAGS   += -DTELESCOPE


#PROD_IOC_RTEMS = asubtest-mk-IOC
PROD_IOC = asubtest-mk-IOC
# asubtest-mk-IOC.dbd will be created and installed
DBD += asubtest-mk-IOC.dbd

# asubtest-mk-IOC.dbd will be made up from these files:
asubtest-mk-IOC_DBD += base.dbd
asubtest-mk-IOC_DBD += iocAdmin.dbd    
#asubtest-mk-IOC_DBD += geminiRecords.dbd        
asubtest-mk-IOC_DBD += aSubRecord.dbd 
#asubtest-mk-IOC_DBD += pvload.dbd   
#asubtest-mk-IOC_DBD += loadRecord.dbd
#asubtest-mk-IOC_DBD += pmaclib.dbd
#asubtest-mk-IOC_DBD += bancomm.dbd 
#asubtest-mk-IOC_DBD += xy240.dbd 
#asubtest-mk-IOC_DBD += xy566.dbd 
#asubtest-mk-IOC_DBD += timelib.dbd 
#asubtest-mk-IOC_DBD += slalib.dbd
#asubtest-mk-IOC_DBD += timeProbe.dbd 
#asubtest-mk-IOC_DBD += listAlarmRecords.dbd
asubtest-mk-IOC_DBD += menuScan.dbd 
asubtest-mk-IOC_DBD += initTrace.dbd 
asubtest-mk-IOC_DBD += asubtest.dbd 

# Include dbd files from all support modules:
#asubtest-mk-IOC_DBD += xxx.dbd

# Add all the support libraries used by this IOC
# asubtest-mk-IOC_LIBS += xxx
#asubtest-mk-IOC_LIBS += seq pv seqDev
asubtest-mk-IOC_LIBS += devIocStats        
#asubtest-mk-IOC_LIBS += geminiRecords        
#asubtest-mk-IOC_LIBS += pvload             
#asubtest-mk-IOC_LIBS += bancomm
#asubtest-mk-IOC_LIBS += xy240
#asubtest-mk-IOC_LIBS += xy566
#asubtest-mk-IOC_LIBS += pmaclib
#asubtest-mk-IOC_LIBS += timelib
#asubtest-mk-IOC_LIBS += slalib
#asubtest-mk-IOC_LIBS += timeProbe
#asubtest-mk-IOC_LIBS += seq pv
#asubtest-mk-IOC_LIBS += listAlarmRecords

#asubtest-mk-IOC_SRCS += caLogDataDriver.c
#asubtest-mk-IOC_SRCS += caput.c
#asubtest-mk-IOC_SRCS += follow.c
asubtest-mk-IOC_SRCS += asubtest.c
#asubtest-mk-IOC_SRCS += getTime.c
#asubtest-mk-IOC_SRCS += logData.c
#asubtest-mk-IOC_SRCS += miscRoutines.c
#asubtest-mk-IOC_SRCS += simRoutines.c
#asubtest-mk-IOC_SRCS += tcsRandom.c
#asubtest-mk-IOC_SRCS += tcsServo.c
asubtest-mk-IOC_SRCS += initTrace.c
#asubtest-mk-IOC_SRCS += testFollow.c
#asubtest-mk-IOC_SRCS += testLan.c
#asubtest-mk-IOC_SRCS += trajectory.c
asubtest-mk-IOC_SRCS_RTEMS += rtems_config.c

# asubtest-mk-IOC_registerRecordDeviceDriver.cpp derives from asubtest-mk-IOC.dbd
asubtest-mk-IOC_SRCS += asubtest-mk-IOC_registerRecordDeviceDriver.cpp

# Build the main IOC entry point on workstation OSs.
asubtest-mk-IOC_SRCS += asubtest-mk-IOCMain.cpp

# Finally link to the EPICS Base libraries
asubtest-mk-IOC_LIBS += $(EPICS_BASE_IOC_LIBS)

#===========================

include $(TOP)/configure/RULES
#----------------------------------------
#  ADD RULES AFTER THIS LINE

