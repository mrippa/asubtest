TOP=../..
include $(TOP)/configure/CONFIG

#----------------------------------------
#  ADD MACRO DEFINITIONS AFTER THIS LINE

# Search for .db files in $SYS_DB_PATHS, the list 
# of paths output by "convertGemRelease.pl dbPaths"
vpath %.db $(SYS_DB_PATHS)

#----------------------------------------------------
# Create and install (or just install) into <top>/db
# databases, templates, substitutions like this
# DB += xxx.db
#DB += time.db
#DB += testFollow_aSub.db
DB += iocAdminRTEMS.db
DB += asub2.db
DB += asub3.db
DB += asub4.db
DB += userHost2.substitutions
DB += userHost3.substitutions
DB += userHost4.substitutions
DB += tcs.substitutions
DB += mcs.substitutions
DB += userHostlinux.substitutions

include $(TOP)/configure/RULES
#----------------------------------------
#  ADD RULES AFTER THIS LINE
$(COMMON_DIR)/%.db: %.db
	cp $< $@

CLEANS += *.dat *.bak
ifndef BASE_3_15
clean::
	$(RM) *.dat *.bak
endif

